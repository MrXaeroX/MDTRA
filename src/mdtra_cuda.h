/***************************************************************************
* Copyright (C) 2011-2016 Alexander V. Popov.
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
#ifndef MDTRA_CUDA_H
#define MDTRA_CUDA_H

#define MDTRA_CUDA_ATOMS_MAX	65535	// absolute maximum number of atoms to process on GPU
#define MDTRA_CUDA_ATOMS_INIT	10000	// initial size of atom buffer
#define MDTRA_CUDA_ATOMS_GROW	10000	// reallocation increment for atom buffer

extern bool		g_bSupportsCUDA;
extern bool		g_bAllowCUDA;
extern float	g_flComputeVersionCUDA;

extern void MDTRA_CUDA_CheckSupport( void );
extern void MDTRA_CUDA_InitDevice( int deviceNumber );

#if defined(MDTRA_ALLOW_CUDA)
extern void MDTRA_CUDA_ErrorMessage( const char* s, const char* f, int l );
extern bool MDTRA_CUDA_CanCalculateSAS( int numAtoms );
extern void MDTRA_CUDA_InitSAS( const float* pDots, int numDots, dword excludeMask );
extern void MDTRA_CUDA_ShutdownSAS( void );
extern float MDTRA_CUDA_CalculateSAS( int threadnum, const struct stMDTRA_PDB_Atom *pAtoms, int numAtoms );
extern void MDTRA_CUDA_CalculateSASPerResidue( int threadnum, const struct stMDTRA_PDB_Atom *pAtoms, int numAtoms, float* pOutData );
extern float MDTRA_CUDA_CalculateOcclusion( int threadnum, const struct stMDTRA_PDB_Atom *pAtoms, int numAtoms );
extern void MDTRA_CUDA_CalculateOcclusionPerResidue( int threadnum, const struct stMDTRA_PDB_Atom *pAtoms, int numAtoms, float* pOutData );

#if defined(_DEBUG)
#define MDTRA_CUDA_SAFE_CALL(x)		{ cudaError_t cerr = (x); if ( cerr != cudaSuccess ) { const char* szerr = cudaGetErrorString(cerr); s_cudaError = true; \
									  _asm { int 3 } \
									  MDTRA_CUDA_ErrorMessage( szerr, __FILE__, __LINE__ ); }}
#else
#define MDTRA_CUDA_SAFE_CALL(x)		{ cudaError_t cerr = (x); if ( cerr != cudaSuccess ) { s_cudaError = true; MDTRA_CUDA_ErrorMessage( cudaGetErrorString(cerr), __FILE__, __LINE__ ); }}
#endif
#endif

#endif //MDTRA_CUDA_H
