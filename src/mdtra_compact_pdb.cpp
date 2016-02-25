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
//	Implementation of MDTRA_Compact_PDB_File

#include "mdtra_main.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb_format.h"
#include "mdtra_compact_pdb.h"
#include "mdtra_cpuid.h"
#include "mdtra_math.h"
#include "mdtra_utils.h"
#include "mdtra_select.h"
#include "mdtra_sse.h"

#define CPDB_FLAG_PROTEIN_BACKBONE	(1<<10)
#define CPDB_FLAG_NUCLEIC_BACKBONE	(1<<11)
#define CPDB_FLAG_NEW_RESIDUE		(1<<12)

//-------------------------------------------------------------------

MDTRA_Compact_PDB_File :: MDTRA_Compact_PDB_File()
{
	m_iNumAtoms = 0;
	m_iNumAlignedAtoms = 0;
	m_iNumBackboneAtoms = 0;
	m_pAtoms = NULL;
}

MDTRA_Compact_PDB_File :: ~MDTRA_Compact_PDB_File()
{
	unload();
}

void MDTRA_Compact_PDB_File :: unload( void )
{
	if (m_pAtoms) {
		UTIL_AlignedFree(m_pAtoms);
		m_pAtoms = NULL;
	}
	m_iNumAtoms = 0;
	m_iNumAlignedAtoms = 0;
	m_iNumBackboneAtoms = 0;
}

void MDTRA_Compact_PDB_File :: reset( void )
{
	m_iNumAtoms = 0;
	m_iNumAlignedAtoms = 0;
	m_iNumBackboneAtoms = 0;
}

void MDTRA_Compact_PDB_File :: set_flags( void )
{
	m_iNumBackboneAtoms = 0;

	//collect residue flags
	int residueFlags = 0;
	int last_i = 0;
	MDTRA_Compact_PDB_Atom *pLast = NULL;
	MDTRA_Compact_PDB_Atom *pAtom = m_pAtoms;

	for (int i = 0; i < m_iNumAtoms; i++, pAtom++) {
		if (pAtom->atomFlags & CPDB_FLAG_NEW_RESIDUE) {
			if (pLast) {
				//previous residue finished
				//now we have its complete flags
				//set the backbone flags for its atoms
				for (int j = last_i; j < i; j++, pLast++) {
					if ((residueFlags & PDB_FLAG_NUCLEIC) && (pLast->atomFlags & CPDB_FLAG_NUCLEIC_BACKBONE)) {
						pLast->atomFlags |= PDB_FLAG_BACKBONE;
						m_iNumBackboneAtoms++;
					} else if (!(residueFlags & PDB_FLAG_WATER) && (pLast->atomFlags & CPDB_FLAG_PROTEIN_BACKBONE)) {
						pLast->atomFlags |= PDB_FLAG_BACKBONE;
						m_iNumBackboneAtoms++;
					}
				}
			}
			residueFlags = 0;
			pLast = pAtom;
			last_i = i;
		}
		residueFlags |= pAtom->atomFlags;
	}

	if (pLast) {
		//last residue finished
		//now we have its complete flags
		//set the backbone flags for its atoms
		for (int j = last_i; j < m_iNumAtoms; j++, pLast++) {
			if ((residueFlags & PDB_FLAG_NUCLEIC) && (pLast->atomFlags & CPDB_FLAG_NUCLEIC_BACKBONE)) {
				pLast->atomFlags |= PDB_FLAG_BACKBONE;
				m_iNumBackboneAtoms++;
			} else if (!(residueFlags & PDB_FLAG_WATER) && (pLast->atomFlags & CPDB_FLAG_PROTEIN_BACKBONE)) {
				pLast->atomFlags |= PDB_FLAG_BACKBONE;
				m_iNumBackboneAtoms++;
			}
		}
	}
}

bool MDTRA_Compact_PDB_File :: read_atom( int threadnum, unsigned int format, const char *linebuf, MDTRA_Compact_PDB_Atom *pOut, int &residue_c )
{
	char title[6];
	char trimmed_title[6];
	char residue[5];
	char trimmed_residue[5];
	int residue_n;
	char* pchr = NULL;

	memset( title, 0, sizeof(title) );
	memset( residue, 0, sizeof(residue) );

	if (!g_PDBFormatManager.parse( threadnum, format, linebuf, 
								   &pOut->serialnumber, &residue_n, 
								   title, residue,
								   NULL, pOut->original_xyz, NULL )) {
		return false;
	}

	pOut->original_xyz[3] = 0.0f;
	memcpy( pOut->modified_xyz, pOut->original_xyz, sizeof(pOut->original_xyz) );

	//get trimmed title and residue
	char *p = title;
	while (*p && isspace(*p)) p++;
	strncpy_s( trimmed_title, p, 5 );
	p = trimmed_title + strlen(trimmed_title) - 1;
	while (*p && isspace(*p)) { *p = 0; p--; }

	p = residue;
	while (*p && isspace(*p)) p++;
	strncpy_s( trimmed_residue, p, 4 );
	p = trimmed_residue + strlen(trimmed_residue) - 1;
	while (*p && isspace(*p)) { *p = 0; p--; }

	//set flags
	pOut->atomFlags = 0;

	if (residue_c != residue_n) {
		residue_c = residue_n;
		pOut->atomFlags = CPDB_FLAG_NEW_RESIDUE;
	}

	if (!_stricmp( trimmed_residue, "HOH" ) ||
		!_stricmp( trimmed_residue, "H2O" ) ||
		!_stricmp( trimmed_residue, "WAT" )) {
		pOut->atomFlags |= PDB_FLAG_WATER;
	} else if (!_stricmp( trimmed_residue, "NA+" ) ||
		!_stricmp( trimmed_residue, "CL-" )) {
		pOut->atomFlags |= PDB_FLAG_ION;
	} else if ( strchr( trimmed_title, '\'') ) {
		pOut->atomFlags |= PDB_FLAG_NUCLEIC;
	} else if (( pchr = strchr( trimmed_title, '*') )) {
		pOut->atomFlags |= PDB_FLAG_NUCLEIC;
		*pchr = '\'';
	}

	if (!_stricmp( trimmed_title, "P" ) ||
		!_stricmp( trimmed_title, "O1P" ) ||
		!_stricmp( trimmed_title, "O2P" ) ||
		!_stricmp( trimmed_title, "C5'" ) ||
		!_stricmp( trimmed_title, "C4'" ) ||
		!_stricmp( trimmed_title, "O4'" ) ||
		!_stricmp( trimmed_title, "C3'" ) ||
		!_stricmp( trimmed_title, "O3'" ) ||
		!_stricmp( trimmed_title, "O2'" ) ||
		!_stricmp( trimmed_title, "C2'" ) ||
		!_stricmp( trimmed_title, "C1'" )) {
		pOut->atomFlags |= CPDB_FLAG_NUCLEIC_BACKBONE;
	}
	else if (!_stricmp( trimmed_title, "C" ) ||
			 !_stricmp( trimmed_title, "O" ) ||
			 !_stricmp( trimmed_title, "N" ) ||
			 !_stricmp( trimmed_title, "CA" )) {
		pOut->atomFlags |= CPDB_FLAG_PROTEIN_BACKBONE;
	}

	return true;
}

bool MDTRA_Compact_PDB_File :: load( int threadnum, unsigned int format, const char *filename, int streamFlags )
{
	FILE *fp = NULL;
	char linebuf[82];
	int line_c = 0;
	int residue_c = 0;

	if (fopen_s( &fp, filename, "r" )) {
		return false;
	}

	reset();

	int fIgnoreHetatm = streamFlags & STREAM_FLAG_IGNORE_HETATM;
	int fIgnoreSolvent = streamFlags & STREAM_FLAG_IGNORE_SOLVENT;

	//count ATOM lines
	while ( !feof(fp) ) {
		linebuf[0] = 0;
		if (!fgets( linebuf, 82, fp )) break;
		if (!_strnicmp( linebuf, "ATOM  ", 6 ) || ( !fIgnoreHetatm && !_strnicmp( linebuf, "HETATM", 6 ) )) line_c++;
	}

	//allocate atoms
	m_pAtoms = (MDTRA_Compact_PDB_Atom*)UTIL_AlignedMalloc(line_c * sizeof(MDTRA_Compact_PDB_Atom));
	if (!m_pAtoms) {
		fclose( fp );
		reset();
		return false;
	}
	memset( m_pAtoms, 0, line_c * sizeof(MDTRA_Compact_PDB_Atom) );

	//load ATOM lines
	rewind(fp);
	while ( !feof(fp) ) {
		linebuf[0] = 0;
		if (!fgets( linebuf, 82, fp )) break;
		if (!_strnicmp( linebuf, "ATOM  ", 6 ) || 
			( !fIgnoreHetatm && !_strnicmp( linebuf, "HETATM", 6 ) )) {
			//parse atom
			if (!read_atom( threadnum, format, linebuf, m_pAtoms + m_iNumAtoms, residue_c )) {
				fclose( fp );
				reset();
				return false;
			}
			if ( fIgnoreSolvent && (m_pAtoms[m_iNumAtoms].atomFlags & (PDB_FLAG_WATER|PDB_FLAG_ION) ))
				continue;
			m_iNumAtoms++;
		}
	}
	fclose( fp );

	set_flags();

	return true;
}

const MDTRA_Compact_PDB_Atom* MDTRA_Compact_PDB_File :: fetchAtomBySerialNumber( int serialnumber ) const
{
	//fast case
	if (serialnumber > 0 && serialnumber <= m_iNumAtoms && m_pAtoms[serialnumber-1].serialnumber == serialnumber)
		return &m_pAtoms[serialnumber-1];

	for (int i = 0; i < m_iNumAtoms; i++) {
		if (m_pAtoms[i].serialnumber == serialnumber)
			return &m_pAtoms[i];
	}
	return NULL;
}

void MDTRA_Compact_PDB_File :: move_to_centroid( void )
{
	XMM_FLOAT centroid_origin[4];
	*(int*)&centroid_origin[0] = 0;
	*(int*)&centroid_origin[1] = 0;
	*(int*)&centroid_origin[2] = 0;
	*(int*)&centroid_origin[3] = 0;

	XMM_FLOAT inv_num_atoms = 1.0f / m_iNumBackboneAtoms;

#if defined(MDTRA_ALLOW_SSE)
	if (!g_bAllowSSE) {
#endif
		for (int i = 0; i < m_iNumAtoms; i++) {
			if (!(m_pAtoms[i].atomFlags & PDB_FLAG_BACKBONE))
				continue;
			centroid_origin[0] += m_pAtoms[i].original_xyz[0];
			centroid_origin[1] += m_pAtoms[i].original_xyz[1];
			centroid_origin[2] += m_pAtoms[i].original_xyz[2];
		}

		centroid_origin[0] *= inv_num_atoms;
		centroid_origin[1] *= inv_num_atoms;
		centroid_origin[2] *= inv_num_atoms;

		for (int i = 0; i < m_iNumAtoms; i++) {
			m_pAtoms[i].modified_xyz[0] = m_pAtoms[i].original_xyz[0] - centroid_origin[0];
			m_pAtoms[i].modified_xyz[1] = m_pAtoms[i].original_xyz[1] - centroid_origin[1];
			m_pAtoms[i].modified_xyz[2] = m_pAtoms[i].original_xyz[2] - centroid_origin[2];
		}
#if defined(MDTRA_ALLOW_SSE)
	} else {
		int sizeof_pdb_atom = sizeof(MDTRA_Compact_PDB_Atom);
		XMM_FLOAT inv_num_atoms_vec[4];
	
		*(int*)&inv_num_atoms_vec[0] = *(int*)&inv_num_atoms;
		*(int*)&inv_num_atoms_vec[1] = *(int*)&inv_num_atoms;
		*(int*)&inv_num_atoms_vec[2] = *(int*)&inv_num_atoms;
		*(int*)&inv_num_atoms_vec[3] = 0;

		SSE_PDB_MOVE_TO_CENTROID(MDTRA_Compact_PDB_Atom, original_xyz, modified_xyz, PDB_FLAG_BACKBONE);
	}
#endif
}

#define JACOBI_MAXSWEEP					50
#define JACOBI_ROTATE(a,i,j,k,l)		{ g=a[j*4+i]; h=a[l*4+k]; a[j*4+i]=g-s*(h+g*tau); a[l*4+k]=h+s*(g-h*tau); }

bool MDTRA_Compact_PDB_File :: jacobi( float *matrix, float *d, float *v )
{
    float tresh, theta, tau, t, sm, s, h, g, c;
	float b[3];
	float z[3];

	memset( v, 0, sizeof(*v) * 12 );
	memset( z, 0, sizeof(z) );

    for( int ip = 0; ip < 3; ip++ ) {
		v[ip*4+ip] = 1.0f;
		b[ip] = d[ip] = matrix[ip*4+ip];
	}

	for ( int i = 1; i <= JACOBI_MAXSWEEP; i++ ) {
		sm = 0.0;

		for ( int ip = 0; ip < 2; ip++ ) {
			for ( int iq = ip+1; iq < 3; iq++ ) 
				sm += fabsf( matrix[iq*4+ip] );
		}

		if ( sm == 0.0f ) 
			return true;

		tresh = (i < 4) ? (0.2f * sm/9.0f) : 0.0f;

		for ( int ip = 0; ip < 2; ip++ ) {
			for( int iq = ip+1; iq < 3; iq++ ) {
				g = 100.0f * fabs(matrix[iq*4+ip]);
				if ((i > 4) && fabsf(d[ip]+g) == fabsf(d[ip]) 
						    && fabsf(d[iq]+g) == fabsf(d[iq]))
				{
				   matrix[iq*4+ip] = 0.0f;
				}
				else if (fabs(matrix[iq*4+ip]) > tresh) {
					h = d[iq] - d[ip];
					
					if ((fabsf(h)+g)==fabsf(h)) {
						t = matrix[iq*4+ip]/h;
					} else {
						theta = 0.5f*h/matrix[iq*4+ip];
						t = 1.0f/(fabsf(theta)+sqrt(1.0f+theta*theta));
						if (theta<0.0f) t = -t;
					}

					c = 1.0f/sqrtf(1.0f+t*t); 
					s = t*c; 
					tau = s/(1.0f + c); 
					h = t*matrix[iq*4+ip];

					z[ip] -= h; 
					z[iq] += h; 
					d[ip] -= h; 
					d[iq] += h;
					matrix[iq*4+ip] = 0.0f;

					for ( int j = 0; j <= ip-1; j++ )	JACOBI_ROTATE(matrix,j,ip,j,iq);
					for ( int j = ip+1; j <= iq-1; j++ )JACOBI_ROTATE(matrix,ip,j,j,iq);
					for ( int j = iq+1; j < 3; j++ )	JACOBI_ROTATE(matrix,ip,j,iq,j);
					for ( int j = 0; j < 3; j++ )		JACOBI_ROTATE(v,j,ip,j,iq);
				}
			}
		}

		for ( int ip = 0; ip < 3; ip++ ) {
			b[ip] += z[ip]; 
			d[ip] = b[ip]; 
			z[ip] = 0.0f;
		}
    }

	printf("ERROR: jacobi: Too many iterations\n");
	return false;
}

void MDTRA_Compact_PDB_File :: eigsrt( float *d, float *v )
{	
	for ( int i = 0; i < 2; i++ ) {
		int k = i;
		float p = d[k];

		for( int j = i+1; j < 3; j++ ) {
			if ( d[j] >= p ) p = d[k=j];
		}
		
		if ( k != i ) {
			d[k] = d[i]; 
			d[i] = p;
			for( int j = 0; j < 3; j++ ) {
				float temp = v[k*4+j];
				v[k*4+j] = v[i*4+j];
				v[i*4+j] = temp;
			}
		}
	}
}

void MDTRA_Compact_PDB_File :: align_kabsch( const MDTRA_Compact_PDB_File *pOther )
{
	if (pOther->m_iNumAtoms != m_iNumAtoms)
		return;
	if (pOther == this)
		return;
	if (m_iNumBackboneAtoms < 2)
		return;

#if defined(MDTRA_ALLOW_SSE)
	int sizeof_pdb_atom = sizeof(MDTRA_Compact_PDB_Atom);
#endif

	XMM_FLOAT rMatrix[12];
	XMM_FLOAT rtrMatrix[12];

	memset( rMatrix, 0, sizeof(rMatrix) );

#if defined(MDTRA_ALLOW_SSE)
	if (!g_bAllowSSE) {
#endif
		//Build R-matrix
		for (int k = 0; k < m_iNumAtoms; k++) {
			if (!(m_pAtoms[k].atomFlags & PDB_FLAG_BACKBONE))
				continue;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					rMatrix[i*4+j] += m_pAtoms[k].modified_xyz[i] * pOther->m_pAtoms[k].modified_xyz[j];
				}
			}
		}

		//Build RtR-matrix
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				rtrMatrix[i*4+j] = rMatrix[0*4+i]*rMatrix[0*4+j] + rMatrix[1*4+i]*rMatrix[1*4+j] + rMatrix[2*4+i]*rMatrix[2*4+j];
			}
		}
#if defined(MDTRA_ALLOW_SSE)
	} else {
		SSE_PDB_CALC_RTR_MATRIX(MDTRA_Compact_PDB_Atom, modified_xyz, PDB_FLAG_BACKBONE);
	}
#endif

	//Get eigenvalues
	XMM_FLOAT eVal[4];
	XMM_FLOAT eVec[12];

	if (!jacobi( rtrMatrix, eVal, eVec ))
		return;
	eigsrt( eVal, eVec );

#if defined(MDTRA_ALLOW_SSE)
	if (!g_bAllowSSE) {
#endif
		//eVec2 = eVec0 x eVec1
		eVec[2*4+0] = eVec[0*4+1]*eVec[1*4+2] - eVec[0*4+2]*eVec[1*4+1];
		eVec[2*4+1] = eVec[0*4+2]*eVec[1*4+0] - eVec[0*4+0]*eVec[1*4+2];
		eVec[2*4+2] = eVec[0*4+0]*eVec[1*4+1] - eVec[0*4+1]*eVec[1*4+0];
#if defined(MDTRA_ALLOW_SSE)
	} else {
		SSE_PDB_CALC_EVEC2();
	}
#endif

	//Calculate B-vectors
	XMM_FLOAT bVec[12];

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			bVec[j*4+i] = rMatrix[i*4+0]*eVec[j*4+0] + rMatrix[i*4+1]*eVec[j*4+1] + rMatrix[i*4+2]*eVec[j*4+2];
			if(j < 2) bVec[j*4+i] /= sqrtf(fabs(eVal[j]));
		}
	}

#if defined(MDTRA_ALLOW_SSE) && defined(WIN32)
	if (!g_bAllowSSE) {
#endif
		float bVecTemp[3];

		//bVecTemp = bVec0 x bVec1
		bVecTemp[0] = bVec[0*4+1]*bVec[1*4+2] - bVec[0*4+2]*bVec[1*4+1];
		bVecTemp[1] = bVec[0*4+2]*bVec[1*4+0] - bVec[0*4+0]*bVec[1*4+2];
		bVecTemp[2] = bVec[0*4+0]*bVec[1*4+1] - bVec[0*4+1]*bVec[1*4+0];

		//adjust rotation
		float s = ((bVecTemp[0] * bVec[2*4+0] + bVecTemp[1] * bVec[2*4+1] + bVecTemp[2] * bVec[2*4+2]) < 0.0f) ? (-1.0f) : (1.0f);
		bVec[2*4+0] = s * bVecTemp[0];
		bVec[2*4+1] = s * bVecTemp[1];
		bVec[2*4+2] = s * bVecTemp[2];

		//Calculate rotation matrix
		float uMatrix[9];
		memset( uMatrix, 0, sizeof(uMatrix) );

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				uMatrix[i*3+j] = bVec[0*4+i]*eVec[0*4+j] + bVec[1*4+i]*eVec[1*4+j] + bVec[2*4+i]*eVec[2*4+j];
			}
		}

		//Transform coords
		float oldCoord[3];
		for (int i = 0; i < m_iNumAtoms; i++) {
			oldCoord[0] = m_pAtoms[i].modified_xyz[0];
			oldCoord[1] = m_pAtoms[i].modified_xyz[1];
			oldCoord[2] = m_pAtoms[i].modified_xyz[2];
			m_pAtoms[i].modified_xyz[0] = oldCoord[0] * uMatrix[0*3+0] + oldCoord[1] * uMatrix[1*3+0] + oldCoord[2] * uMatrix[2*3+0];
			m_pAtoms[i].modified_xyz[1] = oldCoord[0] * uMatrix[0*3+1] + oldCoord[1] * uMatrix[1*3+1] + oldCoord[2] * uMatrix[2*3+1];
			m_pAtoms[i].modified_xyz[2] = oldCoord[0] * uMatrix[0*3+2] + oldCoord[1] * uMatrix[1*3+2] + oldCoord[2] * uMatrix[2*3+2];
		}
#if defined(MDTRA_ALLOW_SSE) && defined(WIN32)
	} else {
		XMM_FLOAT flt_zero[4];
		*(int*)&flt_zero[0] = 0;
		*(int*)&flt_zero[1] = 0;
		*(int*)&flt_zero[2] = 0;

		_asm 
		{
			movaps	xmm0, xmmword ptr[bVec+0]
			movaps	xmm1, xmmword ptr[bVec+16]
			movaps	xmm4, xmmword ptr[bVec+32]
			movaps	xmm2, xmm0
			movaps	xmm3, xmm1
			shufps	xmm0, xmm0, 11001001b
			shufps	xmm1, xmm1, 11010010b
			mulps	xmm0, xmm1
			shufps	xmm2, xmm2, 11010010b
			shufps	xmm3, xmm3, 11001001b
			mulps	xmm2, xmm3
			subps	xmm0, xmm2

			mulps	xmm4, xmm0
			movaps	xmm5, xmm4
			shufps	xmm5, xmm5, 11100001b
			addss	xmm4, xmm5
			shufps	xmm5, xmm5, 11100110b
			addss	xmm4, xmm5

			comiss	xmm4, flt_zero
			jnz		dont_negate
			movaps	xmm2, xmm0
			movaps	xmm0, xmmword ptr[flt_zero]
			subps	xmm0, xmm2

	dont_negate:
			movaps	xmmword ptr[bVec+32], xmm0

			mov		esi, DWORD PTR[this]
			mov		ecx, DWORD PTR[esi].m_iNumAtoms
			mov		edx, DWORD PTR[esi].m_pAtoms
			xor		eax, eax

			movaps	xmm3, xmmword ptr[eVec+0]
			movaps	xmm4, xmmword ptr[eVec+16]
			movaps	xmm5, xmmword ptr[eVec+32]

			movaps	xmm0, xmmword ptr[bVec+0]
			shufps	xmm0, xmm0, 00000000b
			mulps	xmm0, xmm3
			movaps	xmm7, xmmword ptr[bVec+16]
			shufps	xmm7, xmm7, 00000000b
			movaps	xmm6, xmm4
			mulps	xmm6, xmm7
			addps	xmm0, xmm6
			movaps	xmm7, xmmword ptr[bVec+32]
			shufps	xmm7, xmm7, 00000000b
			movaps	xmm6, xmm5
			mulps	xmm6, xmm7
			addps	xmm0, xmm6

			movaps	xmm1, xmmword ptr[bVec+0]
			shufps	xmm1, xmm1, 01010101b
			mulps	xmm1, xmm3
			movaps	xmm7, xmmword ptr[bVec+16]
			shufps	xmm7, xmm7, 01010101b
			movaps	xmm6, xmm4
			mulps	xmm6, xmm7
			addps	xmm1, xmm6
			movaps	xmm7, xmmword ptr[bVec+32]
			shufps	xmm7, xmm7, 01010101b
			movaps	xmm6, xmm5
			mulps	xmm6, xmm7
			addps	xmm1, xmm6

			movaps	xmm2, xmmword ptr[bVec+0]
			shufps	xmm2, xmm2, 10101010b
			mulps	xmm2, xmm3
			movaps	xmm7, xmmword ptr[bVec+16]
			shufps	xmm7, xmm7, 10101010b
			movaps	xmm6, xmm4
			mulps	xmm6, xmm7
			addps	xmm2, xmm6
			movaps	xmm7, xmmword ptr[bVec+32]
			shufps	xmm7, xmm7, 10101010b
			movaps	xmm6, xmm5
			mulps	xmm6, xmm7
			addps	xmm2, xmm6

	transform_more:
			movaps	xmm3, xmmword ptr[edx+eax].modified_xyz
			movaps	xmm4, xmm3
			movaps	xmm5, xmm3
			shufps	xmm3, xmm3, 00000000b
			shufps	xmm4, xmm4, 01010101b
			shufps	xmm5, xmm5, 10101010b
			mulps	xmm3, xmm0
			mulps	xmm4, xmm1
			mulps	xmm5, xmm2
			addps	xmm3, xmm4
			addps	xmm3, xmm5
			movaps	xmmword ptr[edx+eax].modified_xyz, xmm3
			add		eax, sizeof_pdb_atom
			loop	transform_more
		}
	}
#endif
}

void MDTRA_Compact_PDB_File :: set_rmsd_flag( int iSelectionCount, const int *iSelectionData )
{
	MDTRA_Compact_PDB_Atom *pAt = m_pAtoms;
	const int *pSel = iSelectionData;

	for (int i = 0; i < m_iNumAtoms; i++, pAt++) {
		pAt->atomFlags &= ~PDB_FLAG_RMSD;
	}

	m_iNumAlignedAtoms = 0;

	for (int j = 0; j < iSelectionCount; j++, pSel++) {
		if (m_pAtoms[*pSel].atomFlags & PDB_FLAG_RMSD)
			continue;
		m_pAtoms[*pSel].atomFlags |= PDB_FLAG_RMSD;
		m_iNumAlignedAtoms++;
	}
}

float MDTRA_Compact_PDB_File :: get_rmsd( const MDTRA_Compact_PDB_File *pOther ) const
{
	if (pOther == this)
		return 0.0f;

	XMM_FLOAT flRMSD;
	*(int*)&flRMSD = 0;

	if (pOther->m_iNumAtoms != m_iNumAtoms)
		return -1.0f;

	XMM_FLOAT inv_num_atoms = 1.0f / m_iNumAlignedAtoms;

#if defined(MDTRA_ALLOW_SSE)
	if (!g_bAllowSSE) {
#endif
		for (int i = 0; i < m_iNumAtoms; i++) {
			if (!(m_pAtoms[i].atomFlags & PDB_FLAG_RMSD))
				continue;

			flRMSD += ( (m_pAtoms[i].modified_xyz[0] - pOther->m_pAtoms[i].modified_xyz[0])*(m_pAtoms[i].modified_xyz[0] - pOther->m_pAtoms[i].modified_xyz[0]) +
						(m_pAtoms[i].modified_xyz[1] - pOther->m_pAtoms[i].modified_xyz[1])*(m_pAtoms[i].modified_xyz[1] - pOther->m_pAtoms[i].modified_xyz[1]) +
						(m_pAtoms[i].modified_xyz[2] - pOther->m_pAtoms[i].modified_xyz[2])*(m_pAtoms[i].modified_xyz[2] - pOther->m_pAtoms[i].modified_xyz[2]) );
		}
		return sqrtf( flRMSD * inv_num_atoms );
#if defined(MDTRA_ALLOW_SSE)
	} else {
		int sizeof_pdb_atom = sizeof(MDTRA_Compact_PDB_Atom);
		SSE_PDB_CALC_RMSD(MDTRA_Compact_PDB_Atom, modified_xyz, PDB_FLAG_RMSD);
		return flRMSD;
	}
#endif
}

float MDTRA_Compact_PDB_File :: get_distance( int atIndex1, int atIndex2 ) const
{
	const MDTRA_Compact_PDB_Atom* at1 = fetchAtomBySerialNumber( atIndex1 );
	const MDTRA_Compact_PDB_Atom* at2 = fetchAtomBySerialNumber( atIndex2 );

	if (!at1 || !at2)
		return 0.0f;

	float vecDist[3];
	float len;

	Vec3_Sub( vecDist, at1->original_xyz, at2->original_xyz );
	Vec3_Len( len, vecDist );

	return len;
}
