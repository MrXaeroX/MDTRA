/***************************************************************************
* Copyright (C) 2011-2017 Alexander V. Popov.
* 
* This file is part of Molecular Dynamics Trajectory 
* Reader & Analyzer (MDTRA) source code.
* 
* MDTRA source code is free software; you can redistribute it and/or 
* modify it under the terms of the GNU General Public License as 
* published by the Free Software Foundation; either version 2 of 
* the License, or (at your option) any later version.
* 
* MDTRA source code is distributed in the hope that it will be 
* useful, but WITHOUT ANY WARRANTY; without even the implied 
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
* See the GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software 
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
***************************************************************************/
#include "mdtra_main.h"
#include "mdtra_math.h"
#include "mdtra_cuda.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb.h"

#include <cuda.h>
#include <cuda_runtime_api.h>
#include <math_constants.h>

// within ideal capability, run 512 threads per block, 256 otherwise
#define CUDA_IDEAL_COMPUTE_CAPABILITY	1.2f

// this value depends on max. accuracy
// 642 dots for accuracy level = 3
#define CUDA_SAS_MAX_DOTS				642
#define CUDA_SAS_MAX_DOTS_ALIGN			768

#define CUDA_SAS_ATOM_SIZE				(sizeof(float)*4 + sizeof(int))

__constant__ float cudaSASDots[CUDA_SAS_MAX_DOTS*3];
__constant__ int cudaSASNumDots;

static void* cudaHostMemory = NULL;
static void* cudaDeviceAtoms = NULL;
static float* cudaDeviceResult = NULL;
static int cudaMaxAtoms = 0;
static int cudaMaxThreads = 0;
static int sasExcludeMask = 0;
static bool s_cudaError = false;

__device__ float calc_SAS_surface( float radius, int buried )
{
	float areaTotal = 4.0f * CUDART_PI_F * radius * radius;
	float areaRatio = areaTotal * ((float)buried / cudaSASNumDots);
	return areaTotal - areaRatio;
}

__device__ float calc_OCC_surface( float radius, int buried )
{
	float areaTotal = 4.0f * CUDART_PI_F * radius * radius;
	return areaTotal * ((float)buried / cudaSASNumDots);
}

__device__ int process_SAS_atom_pair( float x0, float y0, float z0, float r0, float x1, float y1, float z1, float r1, int* dotinfo )
{
	float diff[3];
	float delta;
	float origin[3];

	// check for sphere intersection
	diff[0] = x1 - x0;
	diff[1] = y1 - y0;
	diff[2] = z1 - z0;
	delta = diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2];

	if ( delta >= (r0+r1)*(r0+r1) )
		return 1;

	int accessible = 0;
	float r1sq = r1 * r1;

	// check each dot against this neighbour
	const float* pDotCoords = cudaSASDots;
	for ( int k = 0; k < cudaSASNumDots; k++, pDotCoords += 3 ) {
		// this dot has already been discarded
		if ( dotinfo[k] != 0 )
			continue;

		// get dot origin relative to the neighbour
		origin[0] = pDotCoords[0] * r0 + diff[0];
		origin[1] = pDotCoords[1] * r0 + diff[1];
		origin[2] = pDotCoords[2] * r0 + diff[2];
		delta = origin[0]*origin[0] + origin[1]*origin[1] + origin[2]*origin[2];

		// check if it is inside the sphere
		if ( delta < r1sq ) {
			//buried
			dotinfo[k] = 1;
			continue;
		}

		// accessible
		accessible = 1;
	}

	return accessible;
}

__device__ int process_OCC_atom_pair( float x0, float y0, float z0, float r0, float x1, float y1, float z1, float r1, int occluder, int* dotinfo1, int* dotinfo2 )
{
	float diff[3];
	float delta;
	float origin[3];

	// check for sphere intersection
	diff[0] = x1 - x0;
	diff[1] = y1 - y0;
	diff[2] = z1 - z0;
	delta = diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2];

	if ( delta >= (r0+r1)*(r0+r1) )
		return 1;

	int accessible = 0;
	float r1sq = r1 * r1;

	// check each dot against this neighbour
	const float* pDotCoords = cudaSASDots;
	for ( int k = 0; k < cudaSASNumDots; k++, pDotCoords += 3 ) {
		// this dot has already been discarded
		if ( dotinfo2[k] != 0 )
			continue;
		if ( occluder && ( dotinfo1[k] != 0 ) )
			continue;

		// get dot origin relative to the neighbour
		origin[0] = pDotCoords[0] * r0 + diff[0];
		origin[1] = pDotCoords[1] * r0 + diff[1];
		origin[2] = pDotCoords[2] * r0 + diff[2];
		delta = origin[0]*origin[0] + origin[1]*origin[1] + origin[2]*origin[2];

		// check if it is inside the sphere
		if ( delta < r1sq ) {
			//buried
			dotinfo1[k] = 1;
			if ( !occluder ) {
				dotinfo2[k] = 1;
				continue;
			}
		}

		// accessible
		accessible = 1;
	}

	return accessible;
}

__global__ void kernel_SAS( void* pAtoms, int iStride, float* outData ) 
{
	__shared__ int dotinfo[CUDA_SAS_MAX_DOTS_ALIGN];

	int firstAtomIndex = blockIdx.x;
	int secondAtomIndex = threadIdx.x;
	int numAtoms = gridDim.x;

	float* pAtX = (float*)pAtoms;
	float* pAtY = pAtX + iStride;
	float* pAtZ = pAtY + iStride;
	float* pAtR = pAtZ + iStride;
	int*   pAtFlags = (int*)(pAtR + iStride);

	// check if we take this atom into account
	if ( pAtFlags[firstAtomIndex] == 0 ) {
		// nothing to calculate at this block
		if ( threadIdx.x == 0 ) outData[firstAtomIndex] = 0;
		return;
	}

	// clear shared memory
	int currentDot = threadIdx.x;
	while ( currentDot < cudaSASNumDots ) {
		dotinfo[currentDot] = 0;
		currentDot += blockDim.x;
	}
	__syncthreads();

	// get first atom's xyzr
	float x0 = pAtX[firstAtomIndex];
	float y0 = pAtY[firstAtomIndex];
	float z0 = pAtZ[firstAtomIndex];
	float r0 = pAtR[firstAtomIndex];

	// fill shared memory
	while ( secondAtomIndex < numAtoms ) {
		if ( firstAtomIndex != secondAtomIndex ) {
			float x1 = pAtX[secondAtomIndex];
			float y1 = pAtY[secondAtomIndex];
			float z1 = pAtZ[secondAtomIndex];
			float r1 = pAtR[secondAtomIndex];
			if ( !process_SAS_atom_pair( x0, y0, z0, r0, x1, y1, z1, r1, dotinfo ) )
				break;
		}
		secondAtomIndex += blockDim.x;
	}
	__syncthreads();

	// reduction
	int i = blockDim.x;
	while ( i != 0 ) {
		if ( cudaSASNumDots > i ) {
			if ( threadIdx.x < i && threadIdx.x < cudaSASNumDots-i )
				dotinfo[threadIdx.x] += dotinfo[threadIdx.x+i];
			__syncthreads();
		}
		i >>= 1;
	}

	// return surface for atom
	if ( threadIdx.x == 0 )
		outData[firstAtomIndex] = calc_SAS_surface( pAtR[firstAtomIndex], dotinfo[0] );
}

__global__ void kernel_OCC( void* pAtoms, int iStride, float* outData ) 
{
	__shared__ int dotinfo1[CUDA_SAS_MAX_DOTS_ALIGN];
	__shared__ int dotinfo2[CUDA_SAS_MAX_DOTS_ALIGN];

	int firstAtomIndex = blockIdx.x;
	int secondAtomIndex = threadIdx.x;
	int numAtoms = gridDim.x;

	float* pAtX = (float*)pAtoms;
	float* pAtY = pAtX + iStride;
	float* pAtZ = pAtY + iStride;
	float* pAtR = pAtZ + iStride;
	int*   pAtFlags = (int*)(pAtR + iStride);

	// check if we take this atom into account
	if ( !(pAtFlags[firstAtomIndex] & PDB_FLAG_SAS) ||
		  (pAtFlags[firstAtomIndex] & PDB_FLAG_OCCLUDER) ) {
		if ( threadIdx.x == 0 ) outData[firstAtomIndex] = 0;
		return;
	}

	// clear shared memory
	int currentDot = threadIdx.x;
	while ( currentDot < cudaSASNumDots ) {
		dotinfo1[currentDot] = 0;
		dotinfo2[currentDot] = 0;
		currentDot += blockDim.x;
	}
	__syncthreads();

	// get first atom's xyzr
	float x0 = pAtX[firstAtomIndex];
	float y0 = pAtY[firstAtomIndex];
	float z0 = pAtZ[firstAtomIndex];
	float r0 = pAtR[firstAtomIndex];

	// fill shared memory
	while ( secondAtomIndex < numAtoms ) {
		if ( firstAtomIndex != secondAtomIndex ) {
			float x1 = pAtX[secondAtomIndex];
			float y1 = pAtY[secondAtomIndex];
			float z1 = pAtZ[secondAtomIndex];
			float r1 = pAtR[secondAtomIndex];
			int occluder = pAtFlags[secondAtomIndex] & PDB_FLAG_OCCLUDER;
			if ( !process_OCC_atom_pair( x0, y0, z0, r0, x1, y1, z1, r1, occluder, dotinfo1, dotinfo2 ) )
				break;
		}
		secondAtomIndex += blockDim.x;
	}
	__syncthreads();
	 
	// reduction
	int i = blockDim.x;
	while ( i != 0 ) {
		if ( cudaSASNumDots > i ) {
			if ( threadIdx.x < i && threadIdx.x < cudaSASNumDots-i ) {
				dotinfo1[threadIdx.x] += dotinfo1[threadIdx.x+i];
				dotinfo2[threadIdx.x] += dotinfo2[threadIdx.x+i];
			}
			__syncthreads();
		}
		i >>= 1;
	}

	// return surface for atom
	if ( threadIdx.x == 0 )
		outData[firstAtomIndex] = calc_OCC_surface( pAtR[firstAtomIndex], dotinfo1[0] - dotinfo2[0] );
}

__host__ void alloc_memory( int numAtoms )
{
	size_t atoms_size = numAtoms * CUDA_SAS_ATOM_SIZE * CountThreads();
	size_t result_size = numAtoms * sizeof(float) * CountThreads();

	MDTRA_CUDA_SAFE_CALL( cudaMallocHost( (void**)&cudaHostMemory, atoms_size ) );
	MDTRA_CUDA_SAFE_CALL( cudaMalloc( (void**)&cudaDeviceAtoms, atoms_size ) );
	MDTRA_CUDA_SAFE_CALL( cudaMalloc( (void**)&cudaDeviceResult, result_size ) );
}

__host__ void free_memory( void )
{
	if ( cudaHostMemory ) {
		MDTRA_CUDA_SAFE_CALL( cudaFreeHost( cudaHostMemory ) );
		cudaHostMemory = NULL;
	}
	if ( cudaDeviceAtoms ) {
		MDTRA_CUDA_SAFE_CALL( cudaFree( cudaDeviceAtoms ) );
		cudaDeviceAtoms = NULL;
	}
	if ( cudaDeviceResult ) {
		MDTRA_CUDA_SAFE_CALL( cudaFree( cudaDeviceResult ) );
		cudaDeviceResult = NULL;
	}

	cudaMaxAtoms = 0;
}

__host__ void ensure_memory_capacity( int numAtoms )
{
	if ( numAtoms > cudaMaxAtoms || CountThreads() > cudaMaxThreads) {
		ThreadLock();
		free_memory();
		if ( !cudaMaxAtoms ) cudaMaxAtoms = MDTRA_CUDA_ATOMS_INIT;
		while ( numAtoms > cudaMaxAtoms ) cudaMaxAtoms += MDTRA_CUDA_ATOMS_GROW;
		alloc_memory( cudaMaxAtoms );
		cudaMaxThreads = CountThreads();
		ThreadUnlock();
	}
}

__host__ void MDTRA_CUDA_InitSAS( const float* pDots, int numDots, dword excludeMask )
{
	s_cudaError = false;

	if ( cudaHostMemory )
		return;

	assert( numDots < (512*2) ); //for kernel reduction
	assert( numDots <= CUDA_SAS_MAX_DOTS );

	sasExcludeMask = excludeMask;

	MDTRA_CUDA_SAFE_CALL( cudaMemcpyToSymbol( "cudaSASDots", pDots, sizeof(float)*3*numDots ) );
	MDTRA_CUDA_SAFE_CALL( cudaMemcpyToSymbol( "cudaSASNumDots", &numDots, sizeof(numDots) ) );

	ensure_memory_capacity( MDTRA_CUDA_ATOMS_INIT );
}

__host__ void MDTRA_CUDA_ShutdownSAS( void )
{
	free_memory();
}

__host__ float MDTRA_CUDA_CalculateSAS( int threadnum, const MDTRA_PDB_Atom *pAtoms, int numAtoms )
{
	if ( s_cudaError )
		return 0.0f;

	// check memory
	ensure_memory_capacity( numAtoms );

	// get device pointers
	void* cudaThreadDeviceAtoms = (float*)((char*)cudaDeviceAtoms + numAtoms * CUDA_SAS_ATOM_SIZE * threadnum);
	float* cudaThreadDeviceResult = (float*)((char*)cudaDeviceResult + numAtoms * sizeof(float) * threadnum);

	// build atoms on the host
	const MDTRA_PDB_Atom* pAt = pAtoms;
	float* cudaThreadHostMemory = (float*)((char*)cudaHostMemory + numAtoms * CUDA_SAS_ATOM_SIZE * threadnum);
	float* hostX = cudaThreadHostMemory;
	float* hostY = hostX + numAtoms;
	float* hostZ = hostY + numAtoms;
	float* hostR = hostZ + numAtoms;
	int* hostFlags = (int*)(hostR + numAtoms);
	int realNumAtoms = 0;

	for ( int i = 0; i < numAtoms; i++, pAt++ ) {
		if ( pAt->atomFlags & sasExcludeMask )
			continue;
		*hostX = pAt->original_xyz[0];
		*hostY = pAt->original_xyz[1];
		*hostZ = pAt->original_xyz[2];
		*hostR = pAt->sasRadius;
		*hostFlags = (pAt->atomFlags & PDB_FLAG_SAS);
		realNumAtoms++;
		hostX++, hostY++, hostZ++, hostR++, hostFlags++;
	}

	// copy atoms to device
	assert( realNumAtoms <= MDTRA_CUDA_ATOMS_MAX );
	MDTRA_CUDA_SAFE_CALL( cudaMemcpy( cudaThreadDeviceAtoms, cudaThreadHostMemory, numAtoms*CUDA_SAS_ATOM_SIZE, cudaMemcpyHostToDevice ) );

	// run kernel
	int kernelSize = ( g_flComputeVersionCUDA >= CUDA_IDEAL_COMPUTE_CAPABILITY ) ? 512 : 256;
	kernel_SAS<<<realNumAtoms,kernelSize>>>( cudaThreadDeviceAtoms, numAtoms, cudaThreadDeviceResult );

	// get result
	MDTRA_CUDA_SAFE_CALL( cudaMemcpy( cudaThreadHostMemory, cudaThreadDeviceResult, realNumAtoms*sizeof(float), cudaMemcpyDeviceToHost ) );

	// final reduction
	for ( int i = 1; i < realNumAtoms; i++ )
		cudaThreadHostMemory[0] += cudaThreadHostMemory[i];

	return cudaThreadHostMemory[0];
}

__host__ void MDTRA_CUDA_CalculateSASPerResidue( int threadnum, const MDTRA_PDB_Atom *pAtoms, int numAtoms, float* pOutData )
{
	if ( s_cudaError )
		return;

	// check memory
	ensure_memory_capacity( numAtoms );

	// get device pointers
	void* cudaThreadDeviceAtoms = (float*)((char*)cudaDeviceAtoms + numAtoms * CUDA_SAS_ATOM_SIZE * threadnum);
	float* cudaThreadDeviceResult = (float*)((char*)cudaDeviceResult + numAtoms * sizeof(float) * threadnum);

	// build atoms on the host
	const MDTRA_PDB_Atom* pAt = pAtoms;
	float* cudaThreadHostMemory = (float*)((char*)cudaHostMemory + numAtoms * CUDA_SAS_ATOM_SIZE * threadnum);
	float* hostX = cudaThreadHostMemory;
	float* hostY = hostX + numAtoms;
	float* hostZ = hostY + numAtoms;
	float* hostR = hostZ + numAtoms;
	int* hostFlags = (int*)(hostR + numAtoms);
	int realNumAtoms = 0;

	for ( int i = 0; i < numAtoms; i++, pAt++ ) {
		if ( pAt->atomFlags & sasExcludeMask )
			continue;
		*hostX = pAt->original_xyz[0];
		*hostY = pAt->original_xyz[1];
		*hostZ = pAt->original_xyz[2];
		*hostR = pAt->sasRadius;
		*hostFlags = (pAt->atomFlags & PDB_FLAG_SAS);
		realNumAtoms++;
		hostX++, hostY++, hostZ++, hostR++, hostFlags++;
	}

	// copy atoms to device
	assert( realNumAtoms <= MDTRA_CUDA_ATOMS_MAX );
	MDTRA_CUDA_SAFE_CALL( cudaMemcpy( cudaThreadDeviceAtoms, cudaThreadHostMemory, numAtoms*CUDA_SAS_ATOM_SIZE, cudaMemcpyHostToDevice ) );

	// run kernel
	int kernelSize = ( g_flComputeVersionCUDA >= CUDA_IDEAL_COMPUTE_CAPABILITY ) ? 512 : 256;
	kernel_SAS<<<realNumAtoms,kernelSize>>>( cudaThreadDeviceAtoms, numAtoms, cudaThreadDeviceResult );

	// get result
	MDTRA_CUDA_SAFE_CALL( cudaMemcpy( cudaThreadHostMemory, cudaThreadDeviceResult, realNumAtoms*sizeof(float), cudaMemcpyDeviceToHost ) );

	// sum values per atom to residue surface buffer
	pAt = pAtoms;
	int iRSN = -1;
	int iRealIndex = 0;
	float* pflSAS = NULL;

	for ( int i = 0; i < numAtoms; i++, pAt++ ) {
		if ( pAt->atomFlags & sasExcludeMask )
			continue;

		// check if changing residue
		if ( pAt->residuenumber != iRSN ) {
			iRSN = pAt->residuenumber;
			pflSAS = pOutData + pAt->residuenumber - 1;
		}

		*pflSAS += cudaThreadHostMemory[iRealIndex++];
	}
}

__host__ float MDTRA_CUDA_CalculateOcclusion( int threadnum, const MDTRA_PDB_Atom *pAtoms, int numAtoms )
{
	if ( s_cudaError )
		return 0.0f;

	// check memory
	ensure_memory_capacity( numAtoms );

	// get device pointers
	void* cudaThreadDeviceAtoms = (float*)((char*)cudaDeviceAtoms + numAtoms * CUDA_SAS_ATOM_SIZE * threadnum);
	float* cudaThreadDeviceResult = (float*)((char*)cudaDeviceResult + numAtoms * sizeof(float) * threadnum);

	// build atoms on the host
	const MDTRA_PDB_Atom* pAt = pAtoms;
	float* cudaThreadHostMemory = (float*)((char*)cudaHostMemory + numAtoms * CUDA_SAS_ATOM_SIZE * threadnum);
	float* hostX = cudaThreadHostMemory;
	float* hostY = hostX + numAtoms;
	float* hostZ = hostY + numAtoms;
	float* hostR = hostZ + numAtoms;
	int* hostFlags = (int*)(hostR + numAtoms);
	int realNumAtoms = 0;

	for ( int i = 0; i < numAtoms; i++, pAt++ ) {
		if ( pAt->atomFlags & sasExcludeMask )
			continue;
		*hostX = pAt->original_xyz[0];
		*hostY = pAt->original_xyz[1];
		*hostZ = pAt->original_xyz[2];
		*hostR = pAt->sasRadius;
		*hostFlags = pAt->atomFlags;
		realNumAtoms++;
		hostX++, hostY++, hostZ++, hostR++, hostFlags++;
	}

	// copy atoms to device
	assert( realNumAtoms <= MDTRA_CUDA_ATOMS_MAX );
	MDTRA_CUDA_SAFE_CALL( cudaMemcpy( cudaThreadDeviceAtoms, cudaThreadHostMemory, numAtoms*CUDA_SAS_ATOM_SIZE, cudaMemcpyHostToDevice ) );

	// run kernel
	int kernelSize = ( g_flComputeVersionCUDA >= CUDA_IDEAL_COMPUTE_CAPABILITY ) ? 512 : 256;
	kernel_OCC<<<realNumAtoms,kernelSize>>>( cudaThreadDeviceAtoms, numAtoms, cudaThreadDeviceResult );

	// get result
	MDTRA_CUDA_SAFE_CALL( cudaMemcpy( cudaThreadHostMemory, cudaThreadDeviceResult, realNumAtoms*sizeof(float), cudaMemcpyDeviceToHost ) );

	// final reduction
	for ( int i = 1; i < realNumAtoms; i++ )
		cudaThreadHostMemory[0] += cudaThreadHostMemory[i];

	return cudaThreadHostMemory[0];
}

__host__ void MDTRA_CUDA_CalculateOcclusionPerResidue( int threadnum, const MDTRA_PDB_Atom *pAtoms, int numAtoms, float* pOutData )
{
	if ( s_cudaError )
		return;

	// check memory
	ensure_memory_capacity( numAtoms );

	// get device pointers
	void* cudaThreadDeviceAtoms = (float*)((char*)cudaDeviceAtoms + numAtoms * CUDA_SAS_ATOM_SIZE * threadnum);
	float* cudaThreadDeviceResult = (float*)((char*)cudaDeviceResult + numAtoms * sizeof(float) * threadnum);

	// build atoms on the host
	const MDTRA_PDB_Atom* pAt = pAtoms;
	float* cudaThreadHostMemory = (float*)((char*)cudaHostMemory + numAtoms * CUDA_SAS_ATOM_SIZE * threadnum);
	float* hostX = cudaThreadHostMemory;
	float* hostY = hostX + numAtoms;
	float* hostZ = hostY + numAtoms;
	float* hostR = hostZ + numAtoms;
	int* hostFlags = (int*)(hostR + numAtoms);
	int realNumAtoms = 0;

	for ( int i = 0; i < numAtoms; i++, pAt++ ) {
		if ( pAt->atomFlags & sasExcludeMask )
			continue;
		*hostX = pAt->original_xyz[0];
		*hostY = pAt->original_xyz[1];
		*hostZ = pAt->original_xyz[2];
		*hostR = pAt->sasRadius;
		*hostFlags = pAt->atomFlags;
		realNumAtoms++;
		hostX++, hostY++, hostZ++, hostR++, hostFlags++;
	}

	// copy atoms to device
	assert( realNumAtoms <= MDTRA_CUDA_ATOMS_MAX );
	MDTRA_CUDA_SAFE_CALL( cudaMemcpy( cudaThreadDeviceAtoms, cudaThreadHostMemory, numAtoms*CUDA_SAS_ATOM_SIZE, cudaMemcpyHostToDevice ) );

	// run kernel
	int kernelSize = ( g_flComputeVersionCUDA >= CUDA_IDEAL_COMPUTE_CAPABILITY ) ? 512 : 256;
	kernel_OCC<<<realNumAtoms,kernelSize>>>( cudaThreadDeviceAtoms, numAtoms, cudaThreadDeviceResult );

	// get result
	MDTRA_CUDA_SAFE_CALL( cudaMemcpy( cudaThreadHostMemory, cudaThreadDeviceResult, realNumAtoms*sizeof(float), cudaMemcpyDeviceToHost ) );

	// sum values per atom to residue surface buffer
	pAt = pAtoms;
	int iRSN = -1;
	int iRealIndex = 0;
	float* pflSAS = NULL;

	for ( int i = 0; i < numAtoms; i++, pAt++ ) {
		if ( pAt->atomFlags & sasExcludeMask )
			continue;

		// check if changing residue
		if ( pAt->residuenumber != iRSN ) {
			iRSN = pAt->residuenumber;
			pflSAS = pOutData + pAt->residuenumber - 1;
		}

		*pflSAS += cudaThreadHostMemory[iRealIndex++];
	}
}