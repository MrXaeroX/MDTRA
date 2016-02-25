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

// Purpose:
//	Implementation of SAS calculation algorithm (based on Shrake-Rupley)

#include "mdtra_main.h"
#include "mdtra_math.h"
#include "mdtra_cuda.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb.h"
#include "mdtra_SAS.h"

#define SAS_DEFAULT_PROBE_RADIUS		1.4f
#define SAS_DEFAULT_GEO_SUBDIVISIONS	2
#define SAS_DEFAULT_EXCLUDE_MASK		PDB_FLAG_WATER

typedef struct stMDTRA_SASParms {
	float	probeRadius;
	int		geoSubdivisions;
	int		excludeMask;
} MDTRA_SASParms;

static MDTRA_SASParms sasParms = {
	SAS_DEFAULT_PROBE_RADIUS,
	SAS_DEFAULT_GEO_SUBDIVISIONS,
	SAS_DEFAULT_EXCLUDE_MASK
};

typedef struct stMDTRA_SASAtomInfo {
	const char* symbol;
	float vdwRadius;
} MDTRA_SASAtomInfo;

static MDTRA_SASAtomInfo s_SASAtomInfo[] = {
	{ "??", 1.50f },
	{ "H", 1.09f },
	{ "C", 1.70f },
	{ "N", 1.55f },
	{ "O", 1.52f },
	{ "F", 1.47f },
	{ "P", 1.80f },
	{ "S", 1.80f },
	{ "CL", 1.75f },
	{ "SE", 2.0f },
	{ "BR", 1.85f },
	{ "I", 1.98f },
	{ "ZN", 1.33f },
	{ "MG", 1.73f },
	{ "FE", 1.52f }
};

static float SAS_VdW4Sym( const char* symbol )
{
	if ( symbol && strlen(symbol) ) {
		int n = sizeof(s_SASAtomInfo)/sizeof(s_SASAtomInfo[0]);
		for ( int i = 1; i < n; i++ ) {
			if ( !_stricmp( s_SASAtomInfo[i].symbol, symbol ) )
				return s_SASAtomInfo[i].vdwRadius;
		}
	}
	return s_SASAtomInfo[0].vdwRadius;
}

static int SAS_lv( const float* v, int& numverts, float* pverts )
{
	int c = numverts;
	for ( int i = 0; i < c; i++ ) {
		if ( (fabsf( pverts[i*3+0] - v[0] ) < 0.00001f) &&
			 (fabsf( pverts[i*3+1] - v[1] ) < 0.00001f) &&
			 (fabsf( pverts[i*3+2] - v[2] ) < 0.00001f))
			 return i;
	}
	memcpy( pverts + c*3, v, sizeof(float)*3 );
	numverts++;
	return c;
}

static void SAS_Sub4( int& numverts, int& numtris, float* pverts, int* ptris )
{
	float va[3], vb[3], vc[3];

	for ( int i = 0; i < numtris; i++ ) {
		int i1 = ptris[i*3+0];
		int i2 = ptris[i*3+1];
		int i3 = ptris[i*3+2];
		float* v1 = pverts + i1*3;
		float* v2 = pverts + i2*3;
		float* v3 = pverts + i3*3;
		Vec3_Add( va, v1, v2 );
		Vec3_Add( vb, v1, v3 );
		Vec3_Add( vc, v2, v3 );
		Vec3_Nrm( va, va );
		Vec3_Nrm( vb, vb );
		Vec3_Nrm( vc, vc );
		int ia = SAS_lv( va, numverts, pverts );
		int ib = SAS_lv( vb, numverts, pverts );
		int ic = SAS_lv( vc, numverts, pverts );

		//replace triangle i
		//add 3 triangles to the end
		ptris[i*3+0] = i1;
		ptris[i*3+1] = ia;
		ptris[i*3+2] = ib;
		ptris[numtris*3+i*9+0] = ia;
		ptris[numtris*3+i*9+1] = i2;
		ptris[numtris*3+i*9+2] = ic;
		ptris[numtris*3+i*9+3] = ic;
		ptris[numtris*3+i*9+4] = i3;
		ptris[numtris*3+i*9+5] = ib;
		ptris[numtris*3+i*9+6] = ia;
		ptris[numtris*3+i*9+7] = ic;
		ptris[numtris*3+i*9+8] = ib;
	}
	numtris += numtris*3;
}

static float* SAS_Geo( int iterations, dword& numdots )
{
	const float f1 = 0.525731112f;
	const float f2 = 0.850650808f;

	const float v[12][3] = {
		{ -f1,   0,  f2 },
		{  f1,   0,  f2 },
		{ -f1,   0, -f2 },
		{  f1,   0, -f2 },
		{   0,  f2,  f1 },
		{   0,  f2, -f1 },
		{   0, -f2,  f1 },
		{   0, -f2, -f1 },
		{  f2,  f1,   0 },
		{ -f2,  f1,   0 },
		{  f2, -f1,   0 },
		{ -f2, -f1,   0 }
	};
	const int tri[20][3] = {
		{ 1, 4, 0 },
		{ 4, 9, 0 },
		{ 4, 5, 9 },
		{ 8, 5, 4 },
		{ 1, 8, 4 },
		{ 1, 10, 8 },
		{ 10, 3, 8 },
		{ 8, 3, 5 },
		{ 3, 2, 5 },
		{ 3, 7, 2 },
		{ 3, 10, 7 },
		{ 10, 6, 7 },
		{ 6, 11, 7 },
		{ 6, 0, 11 },
		{ 6, 1, 0 },
		{ 10, 1, 6 },
		{ 11, 0, 9 },
		{ 2, 11, 9 },
		{ 5, 2, 9 },
		{ 11, 2, 7 }
	};

	int max_tri = (int)(20 * pow( 4.0f, iterations ));
	int max_v = max_tri / 2 + 2;
	int* ptri = new int[max_tri*3*2];
	float* pv = new float[max_v*3*2];

	// initialize
	int num_tri = 20;
	int num_v = 12;
	memcpy( pv, &v[0][0], 12*3*sizeof(float) );
	memcpy( ptri, &tri[0][0], 20*3*sizeof(int) );

	// subdivide
	for ( int i = 0; i < iterations; i++ ) {
		SAS_Sub4( num_v, num_tri, pv, ptri );
		assert( num_tri == (int)(20 * pow( 4.0f, i+1 )) );
		assert( num_v == (num_tri / 2 + 2) );
	}

	assert( num_tri <= max_tri );
	assert( num_v == max_v );
		
#if 0
	FILE* fp;
	if ( !fopen_s( &fp, "SAS_Geo.pdb", "w" ) ) {
		fprintf(fp, "REMARK SAS Geosphere (iterations = %i)\n", iterations );
		fprintf(fp, "REMARK Memory usage: %.1f kb\n", (max_v*3*2*sizeof(float))/1024.0f );
		for ( int i = 0; i < num_v; i++ ) {
			fprintf(fp, "ATOM  %5i %4s %3s A%4i  %8.3f%8.3f%8.3f\n", i+1, "S", "GEO", 1, pv[i*3+0]*20, pv[i*3+1]*20, pv[i*3+2]*20 );
		}
		for ( int i = 0; i < num_tri; i++ ) {
			for ( int j = 0; j < 3; j++ ) {
				fprintf(fp, "CONECT%5i%5i\n", ptri[i*3+j]+1, ptri[i*3+((j+1)%3)]+1 );
			}
		}
		fprintf(fp, "END\n" );
		fclose(fp);
	}
#endif

	delete [] ptri;

	numdots = (dword)num_v;
	return pv;
}

//====================================================================
static float* s_pSASDots = NULL;
static dword s_iSASNumDots = 0;
static dword* s_pSASDotMask = NULL;
static dword s_iSASDotMaskSize = 0;

#define MDTRA_SAS_DOTMASK_CHECK(x,y)	(x[(y) >> 5] &  ( 1 << ( (y) & 31 ) ))
#define MDTRA_SAS_DOTMASK_SET(x,y)		 x[(y) >> 5] |= ( 1 << ( (y) & 31 ) )

void MDTRA_InitSAS( void )
{
	if ( !s_pSASDots ) {
		// create atomic SAS geosphere
		s_pSASDots = SAS_Geo( sasParms.geoSubdivisions, s_iSASNumDots );
		s_iSASDotMaskSize = (s_iSASNumDots >> 5) + 1;
		s_pSASDotMask = new dword[s_iSASDotMaskSize*2*MDTRA_MAX_THREADS];
	}

#if defined(MDTRA_ALLOW_CUDA)
	if ( g_bAllowCUDA )
		MDTRA_CUDA_InitSAS( s_pSASDots, s_iSASNumDots, sasParms.excludeMask );
#endif
}

void MDTRA_ShutdownSAS( void )
{
	// free memory
	if ( s_pSASDots ) {
		delete [] s_pSASDots;
		s_pSASDots = NULL;
	}
	if ( s_pSASDotMask ) {
		delete [] s_pSASDotMask;
		s_pSASDotMask = NULL;
	}

#if defined(MDTRA_ALLOW_CUDA)
	MDTRA_CUDA_ShutdownSAS();
#endif
}

void MDTRA_SetSASParms( float probeRadius, int subdivisions, bool excludeWater )
{
	sasParms.probeRadius = probeRadius;
	sasParms.excludeMask = 0;
	if ( excludeWater ) sasParms.excludeMask |= PDB_FLAG_WATER;

	if (subdivisions < 0)
		subdivisions = 0;
	if (subdivisions > 3)
		subdivisions = 3;

	if ( sasParms.geoSubdivisions != subdivisions ) {
		sasParms.geoSubdivisions = subdivisions;
		MDTRA_ShutdownSAS(); //force memory to be reallocated on next SAS calculation
	}
}

void MDTRA_GetSASParms( float& probeRadius, int& subdivisions, bool& excludeWater )
{
	probeRadius = sasParms.probeRadius;
	subdivisions = sasParms.geoSubdivisions;
	excludeWater = (( sasParms.excludeMask & PDB_FLAG_WATER ) != 0);
}

float MDTRA_GetVdWRadius( const char* symbol )
{
	return SAS_VdW4Sym( symbol );
}

float MDTRA_GetSASRadius( float vdwr )
{
	return vdwr + sasParms.probeRadius;
}

float MDTRA_CalculateSAS( int threadnum, const MDTRA_PDB_Atom *pAtoms, int numAtoms )
{
	float flSAS = 0.0f;
	const MDTRA_PDB_Atom *pAt0 = pAtoms;
	dword* pDotMask = s_pSASDotMask + threadnum*2*s_iSASDotMaskSize;
	dword iDotsBuried;

	for ( int i = 0; i < numAtoms; i++, pAt0++ ) {
		// check if we take this atom into account
		if ( !(pAt0->atomFlags & PDB_FLAG_SAS))
			continue;
		if ( pAt0->atomFlags & sasParms.excludeMask )
			continue;

		// clear dotmask (assume all dots are accessible)
		memset( pDotMask, 0, s_iSASDotMaskSize*sizeof(dword) );
		iDotsBuried = 0;

		// test against all neighbours
		const MDTRA_PDB_Atom *pAt1 = pAtoms;
		for ( int j = 0; j < numAtoms; j++, pAt1++ ) {
			// ignore self
			if ( i == j )
				continue;
			if ( pAt1->atomFlags & sasParms.excludeMask )
				continue;

			// check for sphere intersection
			float diff[3];
			float delta;
			Vec3_Sub( diff, pAt0->original_xyz, pAt1->original_xyz );
			Vec3_LenSq( delta, diff );
			if ( delta >= (pAt0->sasRadius + pAt1->sasRadius)*(pAt0->sasRadius + pAt1->sasRadius) )
				continue;

			float r1sq = pAt1->sasRadius * pAt1->sasRadius;

			// check each dot against this neighbour
			const float* pDotCoords = s_pSASDots;
			for ( dword k = 0; k < s_iSASNumDots; k++, pDotCoords += 3 ) {
				// this dot has already been discarded
				if ( MDTRA_SAS_DOTMASK_CHECK( pDotMask, k ) )
					continue;

				// get dot origin relative to the neighbour
				float origin[3];
				Vec3_Scale( origin, pDotCoords, pAt0->sasRadius );
				Vec3_Add( origin, origin, diff );

				// check if it is inside the sphere
				Vec3_LenSq( delta, origin );
				if ( delta < r1sq ) {
					//buried
					MDTRA_SAS_DOTMASK_SET( pDotMask, k );
					iDotsBuried++;
					continue;
				}
			}

			// if no accessible dots found, break the loop
			if ( iDotsBuried == s_iSASNumDots )
				break;
		}

		if ( iDotsBuried == s_iSASNumDots )
			continue;

		assert( iDotsBuried <= s_iSASNumDots );

		// get total area
		float areaTotal = 4.0f * M_PI_F * pAt0->sasRadius * pAt0->sasRadius;

		// add surface part to total SAS
		flSAS += areaTotal * ( 1.0f - ((float)iDotsBuried / s_iSASNumDots) );
	}

	return flSAS;
}

float MDTRA_CalculateOcclusion( int threadnum, const MDTRA_PDB_Atom *pAtoms, int numAtoms )
{
	float flOCC = 0.0f;
	const MDTRA_PDB_Atom *pAt0 = pAtoms;
	dword* pDotMask = s_pSASDotMask + threadnum*2*s_iSASDotMaskSize;
	dword* pDotMask2 = s_pSASDotMask + (threadnum*2+1)*s_iSASDotMaskSize;
	dword iDotsBuried, iDotsBuried2;

#if 0
	FILE *fp;
	if ( fopen_s( &fp, "Occlusion.pdb", "w" ) ) 
		__asm int 3
	fprintf(fp, "REMARK Occlusion\n");
	int gc = 0;
#endif

	for ( int i = 0; i < numAtoms; i++, pAt0++ ) {
		// check if we take this atom into account
		if ( !(pAt0->atomFlags & PDB_FLAG_SAS) || (pAt0->atomFlags & (PDB_FLAG_OCCLUDER|sasParms.excludeMask)))
			continue;

		// clear dotmasks (assume all dots are accessible)
		memset( pDotMask, 0, s_iSASDotMaskSize*sizeof(dword) );
		memset( pDotMask2, 0, s_iSASDotMaskSize*sizeof(dword) );
		iDotsBuried = iDotsBuried2 = 0;

		// test against all neighbours
		const MDTRA_PDB_Atom *pAt1 = pAtoms;
		for ( int j = 0; j < numAtoms; j++, pAt1++ ) {
			// ignore self
			if ( i == j )
				continue;
			if ( pAt1->atomFlags & sasParms.excludeMask )
				continue;

			// check for sphere intersection
			float diff[3];
			float delta;
			Vec3_Sub( diff, pAt0->original_xyz, pAt1->original_xyz );
			Vec3_LenSq( delta, diff );
			if ( delta >= (pAt0->sasRadius + pAt1->sasRadius)*(pAt0->sasRadius + pAt1->sasRadius) )
				continue;

			float r1sq = pAt1->sasRadius * pAt1->sasRadius;

			// check each dot against this neighbour
			const float* pDotCoords = s_pSASDots;
			for ( dword k = 0; k < s_iSASNumDots; k++, pDotCoords += 3 ) {
				// this dot has already been discarded
				if ( MDTRA_SAS_DOTMASK_CHECK( pDotMask2, k ) )
					continue;
				if ((pAt1->atomFlags & PDB_FLAG_OCCLUDER) && MDTRA_SAS_DOTMASK_CHECK( pDotMask, k ))
					continue;

				// get dot origin relative to the neighbour
				float origin[3];
				Vec3_Scale( origin, pDotCoords, pAt0->sasRadius );
				Vec3_Add( origin, origin, diff );

				// check if it is inside the sphere
				Vec3_LenSq( delta, origin );
				if ( delta < r1sq ) {
					if ( !MDTRA_SAS_DOTMASK_CHECK( pDotMask, k ) ) {
						MDTRA_SAS_DOTMASK_SET( pDotMask, k );
						iDotsBuried++;
					}
					if (!(pAt1->atomFlags & PDB_FLAG_OCCLUDER)) {
						MDTRA_SAS_DOTMASK_SET( pDotMask2, k );
						iDotsBuried2++;
						continue;
					}
				}
			}

			// if no accessible dots found, break the loop
			if ( iDotsBuried2 == s_iSASNumDots )
				break;
		}

		if ( iDotsBuried2 == s_iSASNumDots )
			continue;

#if 0
		for ( dword k = 0; k < s_iSASNumDots; k++ ) {
			float origin[3];
			Vec3_Scale( origin, (s_pSASDots + k*3), pAt0->sasRadius );
			Vec3_Add( origin, pAt0->original_xyz, origin );
			Vec3_Scale( origin, origin, 20 );
			if (!MDTRA_SAS_DOTMASK_CHECK( pDotMask, k )) {
				fprintf(fp, "ATOM%7i %4s %3s A%4i  %8.2f%8.2f%8.2f\n", 
					++gc, "H", pAt0->residue, pAt0->residuenumber, origin[0], origin[1], origin[2] );
			} else if (!MDTRA_SAS_DOTMASK_CHECK( pDotMask2, k )) {
				fprintf(fp, "ATOM%7i %4s %3s A%4i  %8.2f%8.2f%8.2f\n", 
					++gc, "O", pAt0->residue, pAt0->residuenumber, origin[0], origin[1], origin[2] );
			}
		}
#endif

		assert( iDotsBuried <= s_iSASNumDots );
		assert( iDotsBuried2 <= s_iSASNumDots );

		// get total area
		float areaTotal = 4.0f * M_PI_F * pAt0->sasRadius * pAt0->sasRadius;

		// add surface part to total SAS
		flOCC += areaTotal * ( (float)(iDotsBuried - iDotsBuried2) / s_iSASNumDots );
	}

#if 0
	fclose( fp );
#endif

	return flOCC;
}

void MDTRA_CalculateSASPerResidue( int threadnum, const MDTRA_PDB_Atom *pAtoms, int numAtoms, float* pResidueSAS )
{
	float* pflSAS = NULL;
	int iRSN = -1;
	const MDTRA_PDB_Atom *pAt0 = pAtoms;
	dword* pDotMask = s_pSASDotMask + threadnum*2*s_iSASDotMaskSize;
	dword iDotsBuried;

	for ( int i = 0; i < numAtoms; i++, pAt0++ ) {
		// check if we take this atom into account
		if ( !(pAt0->atomFlags & PDB_FLAG_SAS) )
			continue;
		if ( pAt0->atomFlags & sasParms.excludeMask )
			continue;

		// check if changing residue
		if ( pAt0->residuenumber != iRSN ) {
			iRSN = pAt0->residuenumber;
			pflSAS = pResidueSAS + pAt0->residuenumber - 1;
		}

		// clear dotmask (assume all dots are accessible)
		memset( pDotMask, 0, s_iSASDotMaskSize*sizeof(dword) );
		iDotsBuried = 0;

		bool hasAccessible = true;

		// test against all neighbours
		const MDTRA_PDB_Atom *pAt1 = pAtoms;
		for ( int j = 0; j < numAtoms; j++, pAt1++ ) {
			// ignore self
			if ( i == j )
				continue;
			if ( pAt1->atomFlags & sasParms.excludeMask )
				continue;

			// check for sphere intersection
			float diff[3];
			float delta;
			Vec3_Sub( diff, pAt0->original_xyz, pAt1->original_xyz );
			Vec3_LenSq( delta, diff );
			if ( delta >= (pAt0->sasRadius + pAt1->sasRadius)*(pAt0->sasRadius + pAt1->sasRadius) )
				continue;

			int accessible = 0;
			float r1sq = pAt1->sasRadius * pAt1->sasRadius;

			// check each dot against this neighbour
			const float* pDotCoords = s_pSASDots;
			for ( dword k = 0; k < s_iSASNumDots; k++, pDotCoords += 3 ) {
				// this dot has already been discarded
				if ( MDTRA_SAS_DOTMASK_CHECK( pDotMask, k ) )
					continue;

				// get dot origin relative to the neighbour
				float origin[3];
				Vec3_Scale( origin, pDotCoords, pAt0->sasRadius );
				Vec3_Add( origin, origin, diff );

				// check if it is inside the sphere
				Vec3_LenSq( delta, origin );
				if ( delta < r1sq ) {
					//buried
					MDTRA_SAS_DOTMASK_SET( pDotMask, k );
					iDotsBuried++;
					continue;
				}

				// accessible
				accessible++;
			}

			// if no accessible dots found, break the loop
			if ( !accessible ) {
				hasAccessible = false;
				break;
			}
		}

		if ( !hasAccessible )
			continue;

		assert( iDotsBuried <= s_iSASNumDots );
	
		// get total area
		float areaTotal = 4.0f * M_PI_F * pAt0->sasRadius * pAt0->sasRadius;

		// add surface part to total SAS
		*pflSAS += areaTotal * ( 1.0f - ((float)iDotsBuried / s_iSASNumDots) );
	}
}

void MDTRA_CalculateOcclusionPerResidue( int threadnum, const MDTRA_PDB_Atom *pAtoms, int numAtoms, float* pResidueOCC )
{
	float* pflOCC = NULL;
	int iRSN = -1;
	const MDTRA_PDB_Atom *pAt0 = pAtoms;
	dword* pDotMask = s_pSASDotMask + threadnum*2*s_iSASDotMaskSize;
	dword* pDotMask2 = s_pSASDotMask + (threadnum*2+1)*s_iSASDotMaskSize;
	dword iDotsBuried, iDotsBuried2;

	for ( int i = 0; i < numAtoms; i++, pAt0++ ) {
		// check if we take this atom into account
		if ( !(pAt0->atomFlags & PDB_FLAG_SAS) || (pAt0->atomFlags & (PDB_FLAG_OCCLUDER|sasParms.excludeMask)))
			continue;

		// check if changing residue
		if ( pAt0->residuenumber != iRSN ) {
			iRSN = pAt0->residuenumber;
			pflOCC = pResidueOCC + pAt0->residuenumber - 1;
		}

		// clear dotmask (assume all dots are accessible)
		memset( pDotMask, 0, s_iSASDotMaskSize*sizeof(dword) );
		memset( pDotMask2, 0, s_iSASDotMaskSize*sizeof(dword) );
		iDotsBuried = iDotsBuried2 = 0;

		// test against all neighbours
		const MDTRA_PDB_Atom *pAt1 = pAtoms;
		for ( int j = 0; j < numAtoms; j++, pAt1++ ) {
			// ignore self
			if ( i == j )
				continue;
			if ( pAt1->atomFlags & sasParms.excludeMask )
				continue;

			// check for sphere intersection
			float diff[3];
			float delta;
			Vec3_Sub( diff, pAt0->original_xyz, pAt1->original_xyz );
			Vec3_LenSq( delta, diff );
			if ( delta >= (pAt0->sasRadius + pAt1->sasRadius)*(pAt0->sasRadius + pAt1->sasRadius) )
				continue;

			float r1sq = pAt1->sasRadius * pAt1->sasRadius;

			// check each dot against this neighbour
			const float* pDotCoords = s_pSASDots;
			for ( dword k = 0; k < s_iSASNumDots; k++, pDotCoords += 3 ) {
				// this dot has already been discarded
				if ( MDTRA_SAS_DOTMASK_CHECK( pDotMask2, k ) )
					continue;
				if ((pAt1->atomFlags & PDB_FLAG_OCCLUDER) && MDTRA_SAS_DOTMASK_CHECK( pDotMask, k ))
					continue;

				// get dot origin relative to the neighbour
				float origin[3];
				Vec3_Scale( origin, pDotCoords, pAt0->sasRadius );
				Vec3_Add( origin, origin, diff );

				// check if it is inside the sphere
				Vec3_LenSq( delta, origin );
				if ( delta < r1sq ) {
					if ( !MDTRA_SAS_DOTMASK_CHECK( pDotMask, k ) ) {
						MDTRA_SAS_DOTMASK_SET( pDotMask, k );
						iDotsBuried++;
					}
					if (!(pAt1->atomFlags & PDB_FLAG_OCCLUDER)) {
						MDTRA_SAS_DOTMASK_SET( pDotMask2, k );
						iDotsBuried2++;
						continue;
					}
				}
			}

			// if no accessible dots found, break the loop
			if ( iDotsBuried2 == s_iSASNumDots )
				break;
		}

		if ( iDotsBuried2 == s_iSASNumDots )
			continue;

		assert( iDotsBuried <= s_iSASNumDots );
		assert( iDotsBuried2 <= s_iSASNumDots );

		// get total area
		float areaTotal = 4.0f * M_PI_F * pAt0->sasRadius * pAt0->sasRadius;

		// add surface part to total SAS
		*pflOCC += areaTotal * ( (float)(iDotsBuried - iDotsBuried2) / s_iSASNumDots );
	}
}

