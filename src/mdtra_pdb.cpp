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

// Purpose:
//	Implementation of MDTRA_PDB_File

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb_format.h"
#include "mdtra_pdb.h"
#include "mdtra_cpuid.h"
#include "mdtra_cuda.h"
#include "mdtra_math.h"
#include "mdtra_utils.h"
#include "mdtra_select.h"
#include "mdtra_sse.h"
#include "mdtra_SAS.h"
#include "mdtra_hbSearch.h"

//-------------------------------------------------------------------------
// These Standard Reference PDB Frames were taken from 3DNA parameter files
// By Xiang-Jun Lu <xiangjun@rutchem.rutgers.edu>, 2000

#define MAX_SRF_ATOMS	9

static MDTRA_SRFAtom s_SRFAtoms_A[] = 
{
{ "N9", { -1.291f, 4.498f, 0.000f } },
{ "C8", {  0.024f, 4.897f, 0.000f } },
{ "N7", {  0.877f, 3.902f, 0.000f } },
{ "C5", {  0.071f, 2.771f, 0.000f } },
{ "C6", {  0.369f, 1.398f, 0.000f } },
{ "N1", { -0.668f, 0.532f, 0.000f } },
{ "C2", { -1.912f, 1.023f, 0.000f } },
{ "N3", { -2.320f, 2.290f, 0.000f } },
{ "C4", { -1.267f, 3.124f, 0.000f } },
};

static MDTRA_SRFAtom s_SRFAtoms_G[] = 
{
{ "N9", { -1.289f, 4.551f, 0.000f } },
{ "C8", {  0.023f, 4.962f, 0.000f } },
{ "N7", {  0.870f, 3.969f, 0.000f } },
{ "C5", {  0.071f, 2.833f, 0.000f } },
{ "C6", {  0.424f, 1.460f, 0.000f } },
{ "N1", { -0.700f, 0.641f, 0.000f } },
{ "C2", { -1.999f, 1.087f, 0.000f } },
{ "N3", { -2.342f, 2.364f, 0.001f } },
{ "C4", { -1.265f, 3.177f, 0.000f } },
};

static MDTRA_SRFAtom s_SRFAtoms_C[] = 
{
{ "N1", { -1.285f, 4.542f, 0.000f } },
{ "C2", { -1.472f, 3.158f, 0.000f } },
{ "N3", { -0.391f, 2.344f, 0.000f } },
{ "C4", {  0.837f, 2.868f, 0.000f } },
{ "C5", {  1.056f, 4.275f, 0.000f } },
{ "C6", { -0.023f, 5.068f, 0.000f } },
};

static MDTRA_SRFAtom s_SRFAtoms_T[] = 
{
{ "N1", { -1.284f, 4.500f, 0.000f } },
{ "C2", { -1.462f, 3.135f, 0.000f } },
{ "N3", { -0.298f, 2.407f, 0.000f } },
{ "C4", {  0.994f, 2.897f, 0.000f } },
{ "C5", {  1.106f, 4.338f, 0.000f } },
{ "C6", { -0.024f, 5.057f, 0.000f } },
};

static MDTRA_SRFAtom s_SRFAtoms_U[] = 
{
{ "N1", { -1.284f, 4.500f, 0.000f } },
{ "C2", { -1.462f, 3.131f, 0.000f } },
{ "N3", { -0.302f, 2.397f, 0.000f } },
{ "C4", {  0.989f, 2.884f, 0.000f } },
{ "C5", {  1.089f, 4.311f, 0.000f } },
{ "C6", { -0.024f, 5.053f, 0.000f } },
};

static MDTRA_SRFDef s_SRFDef[] = 
{
{ "DA", sizeof(s_SRFAtoms_A)/sizeof(s_SRFAtoms_A[0]), s_SRFAtoms_A },
{ "DG", sizeof(s_SRFAtoms_G)/sizeof(s_SRFAtoms_G[0]), s_SRFAtoms_G },
{ "DC", sizeof(s_SRFAtoms_C)/sizeof(s_SRFAtoms_C[0]), s_SRFAtoms_C },
{ "DT", sizeof(s_SRFAtoms_T)/sizeof(s_SRFAtoms_T[0]), s_SRFAtoms_T },
{ "DU", sizeof(s_SRFAtoms_U)/sizeof(s_SRFAtoms_U[0]), s_SRFAtoms_U },
};

//-------------------------------------------------------------------

MDTRA_PDB_File :: MDTRA_PDB_File()
{
	m_iNumAtoms = 0;
	m_iNumLastFlaggedAtoms = 0;
	m_iNumBackboneAtoms = 0;
	m_iMaxAtoms = 0;
	m_iNumResidues = 0;
	m_iMaxResidues = 0;
	m_pAtoms = NULL;
	m_pResidues = NULL;
	m_pTempFloats = NULL;
	memset( m_vecCentroidOrigin, 0, sizeof(m_vecCentroidOrigin) );
}

MDTRA_PDB_File :: MDTRA_PDB_File( int threadnum, unsigned int format, const char *filename, int streamFlags )
{
	m_iNumAtoms = 0;
	m_iNumLastFlaggedAtoms = 0;
	m_iNumBackboneAtoms = 0;
	m_iMaxAtoms = 0;
	m_iNumResidues = 0;
	m_iMaxResidues = 0;
	m_pAtoms = NULL;
	m_pResidues = NULL;
	m_pTempFloats = NULL;
	memset( m_vecCentroidOrigin, 0, sizeof(m_vecCentroidOrigin) );
	load( threadnum, format, filename, streamFlags );
}

MDTRA_PDB_File :: ~MDTRA_PDB_File()
{
	unload();
}

void MDTRA_PDB_File :: unload( void )
{
	if (m_pAtoms) {
		UTIL_AlignedFree(m_pAtoms);
		m_pAtoms = NULL;
	}
	if (m_pResidues) {
		UTIL_AlignedFree(m_pResidues);
		m_pResidues = NULL;
	}
	if (m_pTempFloats) {
		UTIL_AlignedFree(m_pTempFloats);
		m_pTempFloats = NULL;
	}
	m_iNumAtoms = 0;
	m_iNumLastFlaggedAtoms = 0;
	m_iNumBackboneAtoms = 0;
	m_iMaxAtoms = 0;
	m_iNumResidues = 0;
	m_iMaxResidues = 0;
	memset( m_vecCentroidOrigin, 0, sizeof(m_vecCentroidOrigin) );
}

void MDTRA_PDB_File :: reset( void )
{
	m_iNumAtoms = 0;
	m_iNumLastFlaggedAtoms = 0;
	m_iNumBackboneAtoms = 0;
	m_iNumResidues = 0;
	m_iLastChainIndex = 0;
	m_bChainTerminator = false;
	memset( m_vecCentroidOrigin, 0, sizeof(m_vecCentroidOrigin) );
}

void MDTRA_PDB_File :: alloc_floats( int count )
{
	if (m_pTempFloats) {
		UTIL_AlignedFree(m_pTempFloats);
		m_pTempFloats = NULL;
	}
	m_pTempFloats = (float*)UTIL_AlignedMalloc(count * sizeof(float));
}

void MDTRA_PDB_File :: free_floats( void )
{
	if (m_pTempFloats) {
		UTIL_AlignedFree(m_pTempFloats);
		m_pTempFloats = NULL;
	}
}

bool MDTRA_PDB_File :: ensure_atom_buffer_size( void )
{
	const int iAtomBufferGrow = 2048;
	if (m_iMaxAtoms >= m_iNumAtoms + 1)
		return true;
	m_iMaxAtoms += iAtomBufferGrow;
	if (!m_pAtoms) {
		m_pAtoms = (MDTRA_PDB_Atom*)UTIL_AlignedMalloc(m_iMaxAtoms * sizeof(MDTRA_PDB_Atom));
		memset( m_pAtoms, 0, m_iMaxAtoms * sizeof(MDTRA_PDB_Atom) );
	} else {
		m_pAtoms = (MDTRA_PDB_Atom*)UTIL_AlignedRealloc(m_pAtoms, m_iMaxAtoms*sizeof(MDTRA_PDB_Atom), (m_iMaxAtoms-iAtomBufferGrow)*sizeof(MDTRA_PDB_Atom));
		memset( m_pAtoms + m_iMaxAtoms - iAtomBufferGrow, 0, iAtomBufferGrow * sizeof(MDTRA_PDB_Atom) );
	}

	return (m_pAtoms != NULL);
}

void MDTRA_PDB_File :: set_flags( void )
{
	int currentResidue = -1;
	int numResidues = 0;

	m_iNumBackboneAtoms = 0;

	for (int i = 0; i < m_iNumAtoms; i++) {
		if (m_pAtoms[i].residuenumber != currentResidue) {
			currentResidue = m_pAtoms[i].residuenumber;
			if (currentResidue >= numResidues)
				numResidues = currentResidue+1;
		}
	}

	if (!numResidues)
		return;

	if (numResidues > m_iMaxResidues) {
		if (m_pResidues) UTIL_AlignedFree( m_pResidues );
		m_iMaxResidues = numResidues;
		m_pResidues = (int*)UTIL_AlignedMalloc( m_iMaxResidues * sizeof(int) );
	}

	memset( m_pResidues, 0, sizeof(int)*numResidues );

	//We discriminate protein and nucleic by ' or * in atom titles
	//Water is a special case: HOH H2O WAT
	for (int i = 0; i < m_iNumAtoms; i++) {
		if (!_stricmp( m_pAtoms[i].trimmed_residue, "HOH" ) ||
			!_stricmp( m_pAtoms[i].trimmed_residue, "H2O" ) ||
			!_stricmp( m_pAtoms[i].trimmed_residue, "WAT" )) {
			m_pResidues[m_pAtoms[i].residuenumber] |= PDB_FLAG_WATER;
		} else if (!_stricmp( m_pAtoms[i].trimmed_residue, "NA+" ) ||
			!_stricmp( m_pAtoms[i].trimmed_residue, "CL-" )) {
			m_pResidues[m_pAtoms[i].residuenumber] |= PDB_FLAG_ION;
		} else if ( strchr( m_pAtoms[i].trimmed_title, '\'') ||
					strchr( m_pAtoms[i].trimmed_title, '*')) {
			m_pResidues[m_pAtoms[i].residuenumber] |= PDB_FLAG_NUCLEIC;
		}
	}

	for (int i = 0; i < m_iNumAtoms; i++) {
		set_atom_flags(m_pResidues[m_pAtoms[i].residuenumber], &m_pAtoms[i]);
	}
}

void MDTRA_PDB_File :: set_atom_flags( int residueFlags, MDTRA_PDB_Atom *pOut )
{
	if (!residueFlags)
		pOut->atomFlags |= PDB_FLAG_PROTEIN;
	else
		pOut->atomFlags |= residueFlags;

	if (!_strnicmp( pOut->trimmed_title, "H", 1 ))
		pOut->atomFlags |= PDB_FLAG_HYDROGEN;

	if (pOut->atomFlags & PDB_FLAG_NUCLEIC) {
		if (!_stricmp( pOut->trimmed_title, "P" ) ||
			!_stricmp( pOut->trimmed_title, "O1P" ) ||
			!_stricmp( pOut->trimmed_title, "O2P" ) ||
			!_stricmp( pOut->trimmed_title, "C5'" ) ||
			!_stricmp( pOut->trimmed_title, "C4'" ) ||
			!_stricmp( pOut->trimmed_title, "O4'" ) ||
			!_stricmp( pOut->trimmed_title, "C3'" ) ||
			!_stricmp( pOut->trimmed_title, "O3'" ) ||
			!_stricmp( pOut->trimmed_title, "O2'" ) ||
			!_stricmp( pOut->trimmed_title, "C2'" ) ||
			!_stricmp( pOut->trimmed_title, "C1'" ) ||
			!_stricmp( pOut->trimmed_title, "C5*" ) ||
			!_stricmp( pOut->trimmed_title, "C4*" ) ||
			!_stricmp( pOut->trimmed_title, "O4*" ) ||
			!_stricmp( pOut->trimmed_title, "C3*" ) ||
			!_stricmp( pOut->trimmed_title, "O3*" ) ||
			!_stricmp( pOut->trimmed_title, "O2*" ) ||
			!_stricmp( pOut->trimmed_title, "C2*" ) ||
			!_stricmp( pOut->trimmed_title, "C1*" )) {
			pOut->atomFlags |= PDB_FLAG_BACKBONE;
			m_iNumBackboneAtoms++;
		}
	} else if (pOut->atomFlags & PDB_FLAG_PROTEIN) {
		if (!_stricmp( pOut->trimmed_title, "C" ) ||
			!_stricmp( pOut->trimmed_title, "O" ) ||
			!_stricmp( pOut->trimmed_title, "N" ) ||
			!_stricmp( pOut->trimmed_title, "CA" )) {
			pOut->atomFlags |= PDB_FLAG_BACKBONE;
			m_iNumBackboneAtoms++;
		}
	}
}

bool MDTRA_PDB_File :: read_atom( int threadnum, unsigned int format, const char *linebuf, MDTRA_PDB_Atom *pOut )
{
	if (!g_PDBFormatManager.parse( threadnum, format, linebuf, 
								   &pOut->serialnumber, &pOut->residuenumber, 
								   pOut->title, pOut->residue,
								   &pOut->chain, pOut->xyz, pOut->force )) {
		return false;
	}

	if (pOut->chain >= 'A' && pOut->chain <= 'Z')
		pOut->chainIndex = pOut->chain - 'A';
	else if (pOut->chain >= 'a' && pOut->chain <= 'z')
		pOut->chainIndex = pOut->chain - 'a';
	else if (pOut->chain >= '1' && pOut->chain <= '9')
		pOut->chainIndex = pOut->chain - '1';
	else
		pOut->chainIndex = pOut->chain;

	if ( m_bChainTerminator ) {
		m_iLastChainIndex++;
		m_bChainTerminator = false;
	}

	if ( pOut->chainIndex < m_iLastChainIndex )
		pOut->chainIndex = m_iLastChainIndex;

	pOut->xyz[3] = 0.0f;
	memcpy( pOut->xyz2, pOut->xyz, sizeof(pOut->xyz) );
	memcpy( pOut->original_xyz, pOut->xyz, sizeof(pOut->xyz) );

	//get trimmed title and residue
	char *p = pOut->title;
	while (*p && isspace(*p)) p++;
	strncpy_s( pOut->trimmed_title, p, 5 );
	p = pOut->trimmed_title + strlen(pOut->trimmed_title) - 1;
	while (*p && isspace(*p)) { *p = 0; p--; }

	p = pOut->residue;
	while (*p && isspace(*p)) p++;
	strncpy_s( pOut->trimmed_residue, p, 4 );
	p = pOut->trimmed_residue + strlen(pOut->trimmed_residue) - 1;
	while (*p && isspace(*p)) { *p = 0; p--; }

	//get symbol
	strcpy_s(pOut->symbol, 3, UTIL_SymbolForAtomTitle(pOut->trimmed_title));
	pOut->vdwRadius = MDTRA_GetVdWRadius( pOut->symbol );
	pOut->sasRadius = MDTRA_GetSASRadius( pOut->vdwRadius );

	//clear flags
	pOut->atomFlags = 0;

	return true;
}

bool MDTRA_PDB_File :: load( int threadnum, unsigned int format, const char *filename, int streamFlags )
{
	FILE *fp = NULL;
	if (fopen_s( &fp, filename, "r" )) {
		return false;
	}

	reset();

	char linebuf[82];
	int lastresnum = -1;
	m_iFirstResidue = -1;

	int fIgnoreHetatm = streamFlags & STREAM_FLAG_IGNORE_HETATM;
	int fIgnoreSolvent = streamFlags & STREAM_FLAG_IGNORE_SOLVENT;

	while ( !feof(fp) ) {
		linebuf[0] = 0;
		if (!fgets( linebuf, 82, fp )) break;
		if (!_strnicmp( linebuf, "ATOM  ", 6 ) || 
			( !fIgnoreHetatm && !_strnicmp( linebuf, "HETATM", 6 ) ) ) {
			//parse atom
			if (!ensure_atom_buffer_size()) {
				fclose( fp );
				reset();
				return false;
			}
			if (!read_atom( threadnum, format, linebuf, m_pAtoms + m_iNumAtoms )) {
				fclose( fp );
				reset();
				return false;
			}
			if ( fIgnoreSolvent ) {
				if (!_stricmp( m_pAtoms[m_iNumAtoms].trimmed_residue, "HOH" ) ||
					!_stricmp( m_pAtoms[m_iNumAtoms].trimmed_residue, "H2O" ) ||
					!_stricmp( m_pAtoms[m_iNumAtoms].trimmed_residue, "WAT" ) ||
					!_stricmp( m_pAtoms[m_iNumAtoms].trimmed_residue, "CL-" ) ||
					!_stricmp( m_pAtoms[m_iNumAtoms].trimmed_residue, "NA+" )) {
					continue;
				}
			}
			if (lastresnum != m_pAtoms[m_iNumAtoms].residuenumber) {
				lastresnum = m_pAtoms[m_iNumAtoms].residuenumber;
				if ( m_iFirstResidue < 0 )
					m_iFirstResidue = lastresnum;
				m_iNumResidues++;
			}
			m_pAtoms[m_iNumAtoms].residueserial = m_iNumResidues;
			m_iNumAtoms++;
		} else if (!_strnicmp( linebuf, "TER", 3 )) {
			m_bChainTerminator = true;
		} else {
			printf( linebuf );
		}
	}
	fclose( fp );
	set_flags();
	return true;
}

bool MDTRA_PDB_File :: load( const MDTRA_PDB_File* pOther )
{
	reset();

	m_iNumAtoms = pOther->getAtomCount();
	m_iNumResidues = pOther->getResidueCount();

	m_iMaxAtoms = m_iNumAtoms;
	m_pAtoms = (MDTRA_PDB_Atom*)UTIL_AlignedMalloc(m_iMaxAtoms * sizeof(MDTRA_PDB_Atom));
	memcpy( m_pAtoms, pOther->m_pAtoms, sizeof(MDTRA_PDB_Atom)*m_iNumAtoms);

	m_iFirstResidue = pOther->m_iFirstResidue;
	m_iMaxResidues = m_iNumResidues;
	m_pResidues = (int*)UTIL_AlignedMalloc( m_iMaxResidues * sizeof(int) );
	memcpy( m_pResidues, pOther->m_pResidues, sizeof(int)*m_iNumResidues);

	m_iNumLastFlaggedAtoms = pOther->m_iNumLastFlaggedAtoms;
	m_iNumBackboneAtoms = pOther->m_iNumBackboneAtoms;
	memcpy( m_vecCentroidOrigin, pOther->m_vecCentroidOrigin, sizeof(m_vecCentroidOrigin) );

	return true;
}

static const char *ftos83_local_independent( float f )
{
	static char m_floatBuffer[8][16];
	static int ind = 0;

	int ind2 = ind;
	ind = (ind+1) % 8;
	sprintf_s( m_floatBuffer[ind2], 16, "%8.3f%c", f, 0 );
	char *p = m_floatBuffer[ind2];
	while ( *p ) {
		if (*p == ',') { 
			*p = '.';
			break;
		}
		p++;
	}
	return m_floatBuffer[ind2];
}

bool MDTRA_PDB_File :: save( const char *filename )
{
	FILE *fp = NULL;
	if (fopen_s( &fp, filename, "w" )) {
		return false;
	}

	for (int i = 0; i < m_iNumAtoms; i++) {
		fprintf(fp, "ATOM  %5i %5s%4s%c%4i    %8s%8s%8s\n", 
					m_pAtoms[i].serialnumber,
					m_pAtoms[i].title,
					m_pAtoms[i].residue,
					m_pAtoms[i].chain,
					m_pAtoms[i].residuenumber,
					ftos83_local_independent(m_pAtoms[i].xyz[0]),
					ftos83_local_independent(m_pAtoms[i].xyz[1]),
					ftos83_local_independent(m_pAtoms[i].xyz[2]));
	}

	fclose(fp);
	return true;
}

const MDTRA_PDB_Atom* MDTRA_PDB_File :: fetchAtomByIndex( int index ) const
{
	if (index < 0 || index >= m_iNumAtoms)
		return NULL;

	return &m_pAtoms[index];
}

const MDTRA_PDB_Atom* MDTRA_PDB_File :: fetchAtomBySerialNumber( int serialnumber ) const
{
	if ( !serialnumber )
		return NULL;

	//fast case
	if (serialnumber > 0 && serialnumber <= m_iNumAtoms && m_pAtoms[serialnumber-1].serialnumber == serialnumber)
		return &m_pAtoms[serialnumber-1];

	for (int i = 0; i < m_iNumAtoms; i++) {
		if (m_pAtoms[i].serialnumber == serialnumber)
			return &m_pAtoms[i];
	}
	return NULL;
}

const MDTRA_PDB_Atom* MDTRA_PDB_File :: fetchAtomByResidueSerial( int residueserial ) const
{
	for (int i = 0; i < m_iNumAtoms; i++) {
		if (m_pAtoms[i].residueserial == residueserial)
			return &m_pAtoms[i];
	}
	return NULL;
}

const MDTRA_PDB_Atom* MDTRA_PDB_File :: fetchAtomByDesc( int chain, int resnum, const char *atname ) const
{
	for (int i = 0; i < m_iNumAtoms; i++) {
		if ((chain == -1 || m_pAtoms[i].chainIndex == chain) &&
			(m_pAtoms[i].residuenumber == resnum) &&
			!_stricmp(m_pAtoms[i].trimmed_title, atname))
			return &m_pAtoms[i];
	}
	return NULL;
}

int MDTRA_PDB_File :: fetchAtomIndexBySerialNumber( int serialnumber ) const
{
	//fast case
	if (serialnumber > 0 && serialnumber <= m_iNumAtoms && m_pAtoms[serialnumber-1].serialnumber == serialnumber)
		return serialnumber-1;

	for (int i = 0; i < m_iNumAtoms; i++) {
		if (m_pAtoms[i].serialnumber == serialnumber)
			return i;
	}

	return -1;
}

int MDTRA_PDB_File :: fetchAtomIndexByDesc( int chain, int resnum, const char *atname ) const
{
	for (int i = 0; i < m_iNumAtoms; i++) {
		if ((m_pAtoms[i].chainIndex == chain) &&
			(m_pAtoms[i].residuenumber == resnum) &&
			!_stricmp(m_pAtoms[i].trimmed_title, atname))
			return i;
	}
	return -1;
}

void MDTRA_PDB_File :: clear_coords( void )
{
	for (int i = 0; i < m_iNumAtoms; i++) {
		m_pAtoms[i].xyz[0] = 0;
		m_pAtoms[i].xyz[1] = 0;
		m_pAtoms[i].xyz[2] = 0;
	}
}

void MDTRA_PDB_File :: average_coords( const MDTRA_PDB_File *pOther, float scale )
{
	if (pOther->m_iNumAtoms != m_iNumAtoms)
		return;

	for (int i = 0; i < m_iNumAtoms; i++) {
		m_pAtoms[i].xyz[0] += pOther->m_pAtoms[i].xyz[0] * scale;
		m_pAtoms[i].xyz[1] += pOther->m_pAtoms[i].xyz[1] * scale;
		m_pAtoms[i].xyz[2] += pOther->m_pAtoms[i].xyz[2] * scale;
	}
}

void MDTRA_PDB_File :: finalize_coords( void )
{
	for (int i = 0; i < m_iNumAtoms; i++) {
		memcpy( m_pAtoms[i].xyz2, m_pAtoms[i].xyz, sizeof(XMM_FLOAT)*3 );
		memcpy( m_pAtoms[i].original_xyz, m_pAtoms[i].xyz, sizeof(XMM_FLOAT)*3 );
	}
}

void MDTRA_PDB_File :: move_to_centroid( void )
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
			centroid_origin[0] += m_pAtoms[i].xyz[0];
			centroid_origin[1] += m_pAtoms[i].xyz[1];
			centroid_origin[2] += m_pAtoms[i].xyz[2];
		}

		centroid_origin[0] *= inv_num_atoms;
		centroid_origin[1] *= inv_num_atoms;
		centroid_origin[2] *= inv_num_atoms;

		for (int i = 0; i < m_iNumAtoms; i++) {
			m_pAtoms[i].xyz[0] -= centroid_origin[0];
			m_pAtoms[i].xyz[1] -= centroid_origin[1];
			m_pAtoms[i].xyz[2] -= centroid_origin[2];
		}
#if defined(MDTRA_ALLOW_SSE)
	} else {
		int sizeof_pdb_atom = sizeof(MDTRA_PDB_Atom);
		XMM_FLOAT inv_num_atoms_vec[4];
	
		*(int*)&inv_num_atoms_vec[0] = *(int*)&inv_num_atoms;
		*(int*)&inv_num_atoms_vec[1] = *(int*)&inv_num_atoms;
		*(int*)&inv_num_atoms_vec[2] = *(int*)&inv_num_atoms;
		*(int*)&inv_num_atoms_vec[3] = 0;

		SSE_PDB_MOVE_TO_CENTROID(MDTRA_PDB_Atom, xyz, xyz, PDB_FLAG_BACKBONE);
	}
#endif

	*(int*)&m_vecCentroidOrigin[0] = *(int*)&centroid_origin[0];
	*(int*)&m_vecCentroidOrigin[1] = *(int*)&centroid_origin[1];
	*(int*)&m_vecCentroidOrigin[2] = *(int*)&centroid_origin[2];
}

void MDTRA_PDB_File :: move_to_centroid2( void )
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
			m_pAtoms[i].xyz2[0] = m_pAtoms[i].original_xyz[0] - centroid_origin[0];
			m_pAtoms[i].xyz2[1] = m_pAtoms[i].original_xyz[1] - centroid_origin[1];
			m_pAtoms[i].xyz2[2] = m_pAtoms[i].original_xyz[2] - centroid_origin[2];
		}
#if defined(MDTRA_ALLOW_SSE)
	} else {
		int sizeof_pdb_atom = sizeof(MDTRA_PDB_Atom);
		XMM_FLOAT inv_num_atoms_vec[4];
	
		*(int*)&inv_num_atoms_vec[0] = *(int*)&inv_num_atoms;
		*(int*)&inv_num_atoms_vec[1] = *(int*)&inv_num_atoms;
		*(int*)&inv_num_atoms_vec[2] = *(int*)&inv_num_atoms;
		*(int*)&inv_num_atoms_vec[3] = 0;

		SSE_PDB_MOVE_TO_CENTROID(MDTRA_PDB_Atom, original_xyz, xyz2, PDB_FLAG_BACKBONE);
	}
#endif

	*(int*)&m_vecCentroidOrigin[0] = *(int*)&centroid_origin[0];
	*(int*)&m_vecCentroidOrigin[1] = *(int*)&centroid_origin[1];
	*(int*)&m_vecCentroidOrigin[2] = *(int*)&centroid_origin[2];
}

#define JACOBI_MAXSWEEP					50
#define JACOBI_ROTATE(a,i,j,k,l)		{ g=a[j*4+i]; h=a[l*4+k]; a[j*4+i]=g-s*(h+g*tau); a[l*4+k]=h+s*(g-h*tau); }

bool MDTRA_PDB_File :: jacobi3( float *matrix, float *d, float *v ) const
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

#if defined(LINUX)
	printf("ERROR: jacobi3: Too many iterations\n");
#elif defined(WIN32) && defined(DEBUG)
	OutputDebugString("ERROR: jacobi3: Too many iterations\n");
#endif

	return false;
}

bool MDTRA_PDB_File :: jacobi4( float *matrix, float *d, float *v ) const
{
    float tresh, theta, tau, t, sm, s, h, g, c;
	float b[4];
	float z[4];

	memset( v, 0, sizeof(*v) * 16 );
	memset( z, 0, sizeof(z) );

    for( int ip = 0; ip < 4; ip++ ) {
		v[ip*4+ip] = 1.0f;
		b[ip] = d[ip] = matrix[ip*4+ip];
	}

	for ( int i = 1; i <= JACOBI_MAXSWEEP; i++ ) {
		sm = 0.0;

		for ( int ip = 0; ip < 3; ip++ ) {
			for ( int iq = ip+1; iq < 4; iq++ ) 
				sm += fabsf( matrix[iq*4+ip] );
		}

		if ( sm == 0.0f ) 
			return true;

		tresh = (i < 4) ? (0.2f * sm/9.0f) : 0.0f;

		for ( int ip = 0; ip < 3; ip++ ) {
			for( int iq = ip+1; iq < 4; iq++ ) {
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
					for ( int j = iq+1; j < 4; j++ )	JACOBI_ROTATE(matrix,ip,j,iq,j);
					for ( int j = 0; j < 4; j++ )		JACOBI_ROTATE(v,j,ip,j,iq);
				}
			}
		}

		for ( int ip = 0; ip < 4; ip++ ) {
			b[ip] += z[ip]; 
			d[ip] = b[ip]; 
			z[ip] = 0.0f;
		}
    }

#if defined(LINUX)
	printf("ERROR: jacobi4: Too many iterations\n");
#elif defined(WIN32) && defined(DEBUG)
	OutputDebugString("ERROR: jacobi4: Too many iterations\n");
#endif

	return false;
}

void MDTRA_PDB_File :: eigsrt3( float *d, float *v ) const
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

void MDTRA_PDB_File :: eigsrt4( float *d, float *v ) const
{	
	for ( int i = 0; i < 3; i++ ) {
		int k = i;
		float p = d[k];

		for( int j = i+1; j < 4; j++ ) {
			if ( d[j] >= p ) p = d[k=j];
		}
		
		if ( k != i ) {
			d[k] = d[i]; 
			d[i] = p;
			for( int j = 0; j < 4; j++ ) {
				float temp = v[k*4+j];
				v[k*4+j] = v[i*4+j];
				v[i*4+j] = temp;
			}
		}
	}
}

void MDTRA_PDB_File :: align_kabsch( const MDTRA_PDB_File *pOther )
{
	if (m_iNumBackboneAtoms < 2)
		return;

#if defined(MDTRA_ALLOW_SSE)
	int sizeof_pdb_atom = sizeof(MDTRA_PDB_Atom);
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
					rMatrix[i*4+j] += m_pAtoms[k].xyz[i] * pOther->m_pAtoms[k].xyz[j];
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
		SSE_PDB_CALC_RTR_MATRIX(MDTRA_PDB_Atom, xyz, PDB_FLAG_BACKBONE);
	}
#endif

	//Get eigenvalues
	XMM_FLOAT eVal[4];
	XMM_FLOAT eVec[12];

	if (!jacobi3( rtrMatrix, eVal, eVec ))
		return;
	eigsrt3( eVal, eVec );

/*
	// SSE version is NOT optimal
#if defined(MDTRA_ALLOW_SSE)
	if (!g_bAllowSSE) {
#endif*/
		//eVec2 = eVec0 x eVec1
		eVec[2*4+0] = eVec[0*4+1]*eVec[1*4+2] - eVec[0*4+2]*eVec[1*4+1];
		eVec[2*4+1] = eVec[0*4+2]*eVec[1*4+0] - eVec[0*4+0]*eVec[1*4+2];
		eVec[2*4+2] = eVec[0*4+0]*eVec[1*4+1] - eVec[0*4+1]*eVec[1*4+0];
/*#if defined(MDTRA_ALLOW_SSE)
	} else {
		SSE_PDB_CALC_EVEC2();
	}
#endif*/

	//Calculate B-vectors
	XMM_FLOAT bVec[12];

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			bVec[j*4+i] = rMatrix[i*4+0]*eVec[j*4+0] + rMatrix[i*4+1]*eVec[j*4+1] + rMatrix[i*4+2]*eVec[j*4+2];
			if(j < 2) bVec[j*4+i] /= sqrtf(eVal[j]);
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
			oldCoord[0] = m_pAtoms[i].xyz[0];
			oldCoord[1] = m_pAtoms[i].xyz[1];
			oldCoord[2] = m_pAtoms[i].xyz[2];
			m_pAtoms[i].xyz[0] = oldCoord[0] * uMatrix[0*3+0] + oldCoord[1] * uMatrix[1*3+0] + oldCoord[2] * uMatrix[2*3+0];
			m_pAtoms[i].xyz[1] = oldCoord[0] * uMatrix[0*3+1] + oldCoord[1] * uMatrix[1*3+1] + oldCoord[2] * uMatrix[2*3+1];
			m_pAtoms[i].xyz[2] = oldCoord[0] * uMatrix[0*3+2] + oldCoord[1] * uMatrix[1*3+2] + oldCoord[2] * uMatrix[2*3+2];
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
			movaps	xmm3, xmmword ptr[edx+eax].xyz
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
			movaps	xmmword ptr[edx+eax].xyz, xmm3
			add		eax, sizeof_pdb_atom
			loop	transform_more
		}
	}
#endif
}

void MDTRA_PDB_File :: align_kabsch2( const MDTRA_PDB_File *pOther )
{
	if (pOther->m_iNumAtoms != m_iNumAtoms)
		return;

	if (m_iNumBackboneAtoms < 2)
		return;

#if defined(MDTRA_ALLOW_SSE)
	int sizeof_pdb_atom = sizeof(MDTRA_PDB_Atom);
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
					rMatrix[i*4+j] += m_pAtoms[k].xyz2[i] * pOther->m_pAtoms[k].xyz2[j];
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
		SSE_PDB_CALC_RTR_MATRIX(MDTRA_PDB_Atom, xyz2, PDB_FLAG_BACKBONE);
	}
#endif

	//Get eigenvalues
	XMM_FLOAT eVal[4];
	XMM_FLOAT eVec[12];

	if (!jacobi3( rtrMatrix, eVal, eVec ))
		return;
	eigsrt3( eVal, eVec );

/*
	// SSE version is NOT optimal
#if defined(MDTRA_ALLOW_SSE)
	if (!g_bAllowSSE) {
#endif*/
		//eVec2 = eVec0 x eVec1
		eVec[2*4+0] = eVec[0*4+1]*eVec[1*4+2] - eVec[0*4+2]*eVec[1*4+1];
		eVec[2*4+1] = eVec[0*4+2]*eVec[1*4+0] - eVec[0*4+0]*eVec[1*4+2];
		eVec[2*4+2] = eVec[0*4+0]*eVec[1*4+1] - eVec[0*4+1]*eVec[1*4+0];
/*#if defined(MDTRA_ALLOW_SSE)
	} else {
		SSE_PDB_CALC_EVEC2();
	}
#endif*/

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
			oldCoord[0] = m_pAtoms[i].xyz2[0];
			oldCoord[1] = m_pAtoms[i].xyz2[1];
			oldCoord[2] = m_pAtoms[i].xyz2[2];
			m_pAtoms[i].xyz2[0] = oldCoord[0] * uMatrix[0*3+0] + oldCoord[1] * uMatrix[1*3+0] + oldCoord[2] * uMatrix[2*3+0];
			m_pAtoms[i].xyz2[1] = oldCoord[0] * uMatrix[0*3+1] + oldCoord[1] * uMatrix[1*3+1] + oldCoord[2] * uMatrix[2*3+1];
			m_pAtoms[i].xyz2[2] = oldCoord[0] * uMatrix[0*3+2] + oldCoord[1] * uMatrix[1*3+2] + oldCoord[2] * uMatrix[2*3+2];
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
			movaps	xmm3, xmmword ptr[edx+eax].xyz2
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
			movaps	xmmword ptr[edx+eax].xyz2, xmm3
			add		eax, sizeof_pdb_atom
			loop	transform_more
		}
	}
#endif
}

float MDTRA_PDB_File :: get_rmsd( const MDTRA_PDB_File *pOther ) const
{
	if (pOther == this)
		return 0.0f;

	XMM_FLOAT flRMSD;
	*(int*)&flRMSD = 0;

	if (pOther->m_iNumAtoms != m_iNumAtoms)
		return -1.0f;

	XMM_FLOAT inv_num_atoms = 1.0f / m_iNumBackboneAtoms;

#if defined(MDTRA_ALLOW_SSE)
	if (!g_bAllowSSE) {
#endif
		for (int i = 0; i < m_iNumAtoms; i++) {
			if (!(m_pAtoms[i].atomFlags & PDB_FLAG_BACKBONE))
				continue;

			flRMSD += ( (m_pAtoms[i].xyz[0] - pOther->m_pAtoms[i].xyz[0])*(m_pAtoms[i].xyz[0] - pOther->m_pAtoms[i].xyz[0]) +
						(m_pAtoms[i].xyz[1] - pOther->m_pAtoms[i].xyz[1])*(m_pAtoms[i].xyz[1] - pOther->m_pAtoms[i].xyz[1]) +
						(m_pAtoms[i].xyz[2] - pOther->m_pAtoms[i].xyz[2])*(m_pAtoms[i].xyz[2] - pOther->m_pAtoms[i].xyz[2]) );
		}
		flRMSD = sqrtf( flRMSD * inv_num_atoms );
		return flRMSD;
#if defined(MDTRA_ALLOW_SSE)
	} else {
		int sizeof_pdb_atom = sizeof(MDTRA_PDB_Atom);
		SSE_PDB_CALC_RMSD(MDTRA_PDB_Atom, xyz, PDB_FLAG_BACKBONE);
		return flRMSD;
	}
#endif
}

void MDTRA_PDB_File :: get_rmsd_of_residues( const MDTRA_PDB_File *pOther, float *pOutData ) const
{
	//TODO: SSE version

	if (pOther == this) {
		memset( pOutData, 0, m_iNumResidues*sizeof(float) );
		return;
	}

	if (pOther->m_iNumAtoms != m_iNumAtoms)
		return;

	XMM_FLOAT flRMSD;
	int iNumRMSDAtoms;

	for (int j = 0; j < m_iNumResidues; j++, pOutData++) {
		*(int*)&flRMSD = 0;
		iNumRMSDAtoms = 0;
		for (int i = 0; i < m_iNumAtoms; i++) {
			if (m_pAtoms[i].residueserial > j)
				break;
			if (m_pAtoms[i].residueserial < j)
				continue;
			if (!(m_pAtoms[i].atomFlags & PDB_FLAG_BACKBONE))
				continue;

			iNumRMSDAtoms++;
			flRMSD += ( (m_pAtoms[i].xyz[0] - pOther->m_pAtoms[i].xyz[0])*(m_pAtoms[i].xyz[0] - pOther->m_pAtoms[i].xyz[0]) +
						(m_pAtoms[i].xyz[1] - pOther->m_pAtoms[i].xyz[1])*(m_pAtoms[i].xyz[1] - pOther->m_pAtoms[i].xyz[1]) +
						(m_pAtoms[i].xyz[2] - pOther->m_pAtoms[i].xyz[2])*(m_pAtoms[i].xyz[2] - pOther->m_pAtoms[i].xyz[2]) );
		}

		if (iNumRMSDAtoms > 0) {
			XMM_FLOAT inv_num_atoms = 1.0f / iNumRMSDAtoms;
			*pOutData = sqrtf( flRMSD * inv_num_atoms );
		}
	}
}

float MDTRA_PDB_File :: get_rmsd2( const MDTRA_PDB_File *pOther ) const
{
	if (pOther == this)
		return 0.0f;

	XMM_FLOAT flRMSD;
	*(int*)&flRMSD = 0;

	if (pOther->m_iNumAtoms != m_iNumAtoms)
		return -1.0f;

	XMM_FLOAT inv_num_atoms = 1.0f / m_iNumLastFlaggedAtoms;

#if defined(MDTRA_ALLOW_SSE)
	if (!g_bAllowSSE) {
#endif
		for (int i = 0; i < m_iNumAtoms; i++) {
			if (!(m_pAtoms[i].atomFlags & PDB_FLAG_RMSD))
				continue;

			flRMSD += ( (m_pAtoms[i].xyz2[0] - pOther->m_pAtoms[i].xyz2[0])*(m_pAtoms[i].xyz2[0] - pOther->m_pAtoms[i].xyz2[0]) +
						(m_pAtoms[i].xyz2[1] - pOther->m_pAtoms[i].xyz2[1])*(m_pAtoms[i].xyz2[1] - pOther->m_pAtoms[i].xyz2[1]) +
						(m_pAtoms[i].xyz2[2] - pOther->m_pAtoms[i].xyz2[2])*(m_pAtoms[i].xyz2[2] - pOther->m_pAtoms[i].xyz2[2]) );

		}
		return sqrtf( flRMSD * inv_num_atoms );
#if defined(MDTRA_ALLOW_SSE)
	} else {
		int sizeof_pdb_atom = sizeof(MDTRA_PDB_Atom);
		SSE_PDB_CALC_RMSD(MDTRA_PDB_Atom, xyz2, PDB_FLAG_RMSD);
		return flRMSD;
	}
#endif
}

void MDTRA_PDB_File :: get_rmsd2_of_residues( const MDTRA_PDB_File *pOther, float *pOutData ) const
{
	//TODO: SSE version

	if (pOther == this) {
		memset( pOutData, 0, m_iNumResidues*sizeof(float) );
		return;
	}

	if (pOther->m_iNumAtoms != m_iNumAtoms)
		return;

	XMM_FLOAT flRMSD;
	int iNumRMSDAtoms;

	for (int j = 0; j < m_iNumResidues; j++, pOutData++) {
		*(int*)&flRMSD = 0;
		iNumRMSDAtoms = 0;
		for (int i = 0; i < m_iNumAtoms; i++) {
			if (m_pAtoms[i].residueserial > j)
				break;
			if (m_pAtoms[i].residueserial < j)
				continue;
			if (!(m_pAtoms[i].atomFlags & PDB_FLAG_RMSD))
				continue;

			iNumRMSDAtoms++;
			flRMSD += ( (m_pAtoms[i].xyz2[0] - pOther->m_pAtoms[i].xyz2[0])*(m_pAtoms[i].xyz2[0] - pOther->m_pAtoms[i].xyz2[0]) +
						(m_pAtoms[i].xyz2[1] - pOther->m_pAtoms[i].xyz2[1])*(m_pAtoms[i].xyz2[1] - pOther->m_pAtoms[i].xyz2[1]) +
						(m_pAtoms[i].xyz2[2] - pOther->m_pAtoms[i].xyz2[2])*(m_pAtoms[i].xyz2[2] - pOther->m_pAtoms[i].xyz2[2]) );
		}

		if (iNumRMSDAtoms > 0) {
			XMM_FLOAT inv_num_atoms = 1.0f / iNumRMSDAtoms;
			*pOutData = sqrtf( flRMSD * inv_num_atoms );
		}
	}
}

void MDTRA_PDB_File :: get_rmsf_of_atoms( const MDTRA_PDB_File *pOther, float *pOutData ) const
{
	if (pOther == this) {
		memset( pOutData, 0, m_iNumResidues*sizeof(float) );
		return;
	}

	if (pOther->m_iNumAtoms != m_iNumAtoms)
		return;

	for (int i = 0; i < m_iNumAtoms; i++, pOutData++) {
		if (!(m_pAtoms[i].atomFlags & PDB_FLAG_BACKBONE))
			continue;
		*pOutData = ( (m_pAtoms[i].xyz[0] - pOther->m_pAtoms[i].xyz[0])*(m_pAtoms[i].xyz[0] - pOther->m_pAtoms[i].xyz[0]) +
					  (m_pAtoms[i].xyz[1] - pOther->m_pAtoms[i].xyz[1])*(m_pAtoms[i].xyz[1] - pOther->m_pAtoms[i].xyz[1]) +
					  (m_pAtoms[i].xyz[2] - pOther->m_pAtoms[i].xyz[2])*(m_pAtoms[i].xyz[2] - pOther->m_pAtoms[i].xyz[2]) );
	}
}

void MDTRA_PDB_File :: get_rmsf2_of_atoms( const MDTRA_PDB_File *pOther, float *pOutData ) const
{
	if (pOther == this) {
		memset( pOutData, 0, m_iNumResidues*sizeof(float) );
		return;
	}

	if (pOther->m_iNumAtoms != m_iNumAtoms)
		return;

	for (int i = 0; i < m_iNumAtoms; i++, pOutData++) {
		if (!(m_pAtoms[i].atomFlags & PDB_FLAG_RMSF))
			continue;
		*pOutData = ( (m_pAtoms[i].xyz2[0] - pOther->m_pAtoms[i].xyz2[0])*(m_pAtoms[i].xyz2[0] - pOther->m_pAtoms[i].xyz2[0]) +
					  (m_pAtoms[i].xyz2[1] - pOther->m_pAtoms[i].xyz2[1])*(m_pAtoms[i].xyz2[1] - pOther->m_pAtoms[i].xyz2[1]) +
					  (m_pAtoms[i].xyz2[2] - pOther->m_pAtoms[i].xyz2[2])*(m_pAtoms[i].xyz2[2] - pOther->m_pAtoms[i].xyz2[2]) );
	}
}

float MDTRA_PDB_File :: get_radius_of_gyration( void ) const
{
	XMM_FLOAT flRoG;
	*(int*)&flRoG = 0;

	XMM_FLOAT inv_num_atoms = 1.0f / m_iNumAtoms;

	for (int i = 0; i < m_iNumAtoms; i++) {
		flRoG += ( (m_pAtoms[i].xyz[0])*(m_pAtoms[i].xyz[0]) +
				   (m_pAtoms[i].xyz[1])*(m_pAtoms[i].xyz[1]) +
				   (m_pAtoms[i].xyz[2])*(m_pAtoms[i].xyz[2]) );
	}
	flRoG = sqrtf( flRoG * inv_num_atoms );
	return flRoG;
}

void MDTRA_PDB_File :: set_flag( int iSelectionCount, const int *iSelectionData, int flag )
{
	MDTRA_PDB_Atom *pAt = m_pAtoms;
	const int *pSel = iSelectionData;

	if ( !iSelectionCount || !iSelectionData ) {
		for (int i = 0; i < m_iNumAtoms; i++, pAt++)
			pAt->atomFlags |= flag;
		m_iNumLastFlaggedAtoms = m_iNumAtoms;
		return;
	}

	for (int i = 0; i < m_iNumAtoms; i++, pAt++) {
		pAt->atomFlags &= ~flag;
	}

	m_iNumLastFlaggedAtoms = 0;

	for (int j = 0; j < iSelectionCount; j++, pSel++) {
		if (m_pAtoms[*pSel].atomFlags & flag)
			continue;
		m_pAtoms[*pSel].atomFlags |= flag;
		m_iNumLastFlaggedAtoms++;
	}
}

float MDTRA_PDB_File :: get_distance( int atIndex1, int atIndex2 ) const
{
	const MDTRA_PDB_Atom* at1 = fetchAtomBySerialNumber( atIndex1 );
	const MDTRA_PDB_Atom* at2 = fetchAtomBySerialNumber( atIndex2 );

	if (!at1 || !at2)
		return 0.0f;

	float vecDist[3];
	float len;
	
	Vec3_Sub( vecDist, at1->xyz, at2->xyz );
	Vec3_Len( len, vecDist );

	return len;
}

float MDTRA_PDB_File :: get_selection_pair_distance( int atIndex1, int atIndex2 ) const
{
	const MDTRA_PDB_Atom* at1 = fetchAtomByIndex( atIndex1 );
	const MDTRA_PDB_Atom* at2 = fetchAtomByIndex( atIndex2 );

	if (!at1 || !at2)
		return 0.0f;

	float vecDist[3];
	float len;

	Vec3_Sub( vecDist, at1->xyz, at2->xyz );
	Vec3_Len( len, vecDist );

	return len;
}

bool MDTRA_PDB_File :: is_selection_pair_of_same_residue( int atIndex1, int atIndex2 ) const
{
	const MDTRA_PDB_Atom* at1 = fetchAtomByIndex( atIndex1 );
	const MDTRA_PDB_Atom* at2 = fetchAtomByIndex( atIndex2 );

	if (at1 && at2 && (at1->residueserial == at2->residueserial))
		return true;

	return false;
}

float MDTRA_PDB_File :: get_angle( int atIndex1, int atIndex2, int atIndex3 ) const
{
	const MDTRA_PDB_Atom* at1 = fetchAtomBySerialNumber( atIndex1 );
	const MDTRA_PDB_Atom* at2 = fetchAtomBySerialNumber( atIndex2 );
	const MDTRA_PDB_Atom* at3 = fetchAtomBySerialNumber( atIndex3 );

	if (!at1 || !at2 || !at3)
		return 0.0f;

	float vecDir1[3];
	float vecDir2[3];
	float flDot;

	Vec3_Sub( vecDir1, at1->xyz, at2->xyz );
	Vec3_Sub( vecDir2, at3->xyz, at2->xyz );
	Vec3_Nrm( vecDir1, vecDir1 );
	Vec3_Nrm( vecDir2, vecDir2 );
	Vec3_Dot( flDot, vecDir1, vecDir2 );

	if ( flDot < -1.0f )
		flDot = -1.0f;
	else if ( flDot > 1.0f )
		flDot = 1.0f;

	return acosf( flDot );
}

float MDTRA_PDB_File :: get_angle_between_sections( int atIndex1, int atIndex2, int atIndex3, int atIndex4 ) const
{
	const MDTRA_PDB_Atom* at1 = fetchAtomBySerialNumber( atIndex1 );
	const MDTRA_PDB_Atom* at2 = fetchAtomBySerialNumber( atIndex2 );
	const MDTRA_PDB_Atom* at3 = fetchAtomBySerialNumber( atIndex3 );
	const MDTRA_PDB_Atom* at4 = fetchAtomBySerialNumber( atIndex4 );

	if (!at1 || !at2 || !at3 || !at4)
		return 0.0f;

	float vecDir1[3];
	float vecDir2[3];
	float flDot;

	Vec3_Sub( vecDir1, at1->xyz, at2->xyz );
	Vec3_Sub( vecDir2, at3->xyz, at4->xyz );
	Vec3_Nrm( vecDir1, vecDir1 );
	Vec3_Nrm( vecDir2, vecDir2 );
	Vec3_Dot( flDot, vecDir1, vecDir2 );

	if ( flDot < -1.0f )
		flDot = -1.0f;
	else if ( flDot > 1.0f )
		flDot = 1.0f;

	return acosf( flDot );
}

float MDTRA_PDB_File :: get_torsion( int atIndex1, int atIndex2, int atIndex3, int atIndex4, bool bSigned ) const
{
	const MDTRA_PDB_Atom* at1 = fetchAtomBySerialNumber( atIndex1 );
	const MDTRA_PDB_Atom* at2 = fetchAtomBySerialNumber( atIndex2 );
	const MDTRA_PDB_Atom* at3 = fetchAtomBySerialNumber( atIndex3 );
	const MDTRA_PDB_Atom* at4 = fetchAtomBySerialNumber( atIndex4 );

	if (!at1 || !at2 || !at3 || !at4)
		return 0.0f;

	float v1[3];
	float v2[3];
	float v3[3];
	float cr1[3];
	float cr2[3];
	float t1, t2, ang;

	Vec3_Sub( v1, at2->xyz, at1->xyz );
	Vec3_Sub( v2, at3->xyz, at2->xyz );
	Vec3_Sub( v3, at4->xyz, at3->xyz );
	Vec3_Cross( cr1, v1, v2 );
	Vec3_Cross( cr2, v2, v3 );
	Vec3_Len( t1, v2 );
	Vec3_Scale( v1, v1, t1 );
	Vec3_Dot( t1, v1, cr2 );
	Vec3_Dot( t2, cr1, cr2 );

	ang = atan2( t1, t2 );

	if ( !bSigned && ( ang < 0 ) )
		ang += M_PI_F * 2.0f;

	return ang;
}

float MDTRA_PDB_File :: get_dihedral( int atIndex1, int atIndex2, int atIndex3, int atIndex4, bool absValue ) const
{
	const MDTRA_PDB_Atom* at1 = fetchAtomBySerialNumber( atIndex1 );
	const MDTRA_PDB_Atom* at2 = fetchAtomBySerialNumber( atIndex2 );
	const MDTRA_PDB_Atom* at3 = fetchAtomBySerialNumber( atIndex3 );
	const MDTRA_PDB_Atom* at4 = fetchAtomBySerialNumber( atIndex4 );

	if (!at1 || !at2 || !at3 || !at4) {
		return 0.0f;
	}

	float n1[3];
	float n2[3];
	float dir[3];
	float cr[3];
	float flDot;

	UTIL_PlaneFromPoints( at2->xyz, at3->xyz, at4->xyz, n1, NULL );
	UTIL_PlaneFromPoints( at1->xyz, at2->xyz, at3->xyz, n2, NULL );

	Vec3_Sub( dir, at2->xyz, at3->xyz );
	Vec3_Dot( flDot, n1, n2 );

	if ( flDot < -1.0f )
		flDot = -1.0f;
	else if ( flDot > 1.0f )
		flDot = 1.0f;

	float flAng = acosf( flDot );

	if (!absValue) 
	{
		Vec3_Cross( cr, n1, n2 );
		Vec3_Dot( flDot, cr, dir );

		if (flDot < 0)
			flAng = -flAng;
	}

	return flAng;
}

float MDTRA_PDB_File :: get_angle_between_planes( int atIndex1, int atIndex2, int atIndex3, int atIndex4, int atIndex5, int atIndex6 ) const
{
	const MDTRA_PDB_Atom* at1 = fetchAtomBySerialNumber( atIndex1 );
	const MDTRA_PDB_Atom* at2 = fetchAtomBySerialNumber( atIndex2 );
	const MDTRA_PDB_Atom* at3 = fetchAtomBySerialNumber( atIndex3 );
	const MDTRA_PDB_Atom* at4 = fetchAtomBySerialNumber( atIndex4 );
	const MDTRA_PDB_Atom* at5 = fetchAtomBySerialNumber( atIndex5 );
	const MDTRA_PDB_Atom* at6 = fetchAtomBySerialNumber( atIndex6 );

	if (!at1 || !at2 || !at3 || !at4 || !at5 || !at6)
		return 0.0f;

	float vecDir1[3];
	float vecDir2[3];
	float vecN1[3];
	float vecN2[3];
	float flDot;

	Vec3_Sub( vecDir1, at3->xyz, at1->xyz );
	Vec3_Sub( vecDir2, at2->xyz, at1->xyz );
	Vec3_Cross( vecN1, vecDir1, vecDir2 );
	Vec3_Nrm( vecN1, vecN1 );
	Vec3_Sub( vecDir1, at6->xyz, at4->xyz );
	Vec3_Sub( vecDir2, at5->xyz, at4->xyz );
	Vec3_Cross( vecN2, vecDir1, vecDir2 );
	Vec3_Nrm( vecN2, vecN2 );
	Vec3_Dot( flDot, vecN1, vecN2 );

	if ( flDot < -1.0f )
		flDot = -1.0f;
	else if ( flDot > 1.0f )
		flDot = 1.0f;

	return acosf( flDot );
}

float MDTRA_PDB_File :: get_force( int atIndex1 ) const
{
	const MDTRA_PDB_Atom* at1 = fetchAtomBySerialNumber( atIndex1 );

	float flMagnutide = 0.0f;
	Vec3_Len( flMagnutide, at1->force );
	return flMagnutide;
}

float MDTRA_PDB_File :: get_resultant_force( int atIndex1, int atIndex2 ) const
{
	const MDTRA_PDB_Atom* at1 = fetchAtomBySerialNumber( atIndex1 );
	const MDTRA_PDB_Atom* at2 = fetchAtomBySerialNumber( atIndex2 );

	float vecDir[3];
	float fDot1, fDot2;

	Vec3_Sub( vecDir, at1->xyz, at2->xyz );
	Vec3_Nrm( vecDir, vecDir );
	Vec3_Dot( fDot1, at1->force, vecDir );
	Vec3_Dot( fDot2, at2->force, vecDir );

	return fDot1 + fDot1;
}

void MDTRA_PDB_File :: get_force_vector( int atIndex1, float *vecOut ) const
{
	const MDTRA_PDB_Atom* at1 = fetchAtomByIndex( atIndex1 );
	memcpy( vecOut, at1->force, sizeof(float)*3 );
}

float MDTRA_PDB_File :: get_hbenergy( int atIndex1, int atIndex2 ) const
{
	const MDTRA_PDB_Atom* at1 = fetchAtomBySerialNumber( atIndex1 );
	const MDTRA_PDB_Atom* at2 = fetchAtomBySerialNumber( atIndex2 );

	if (!at1 || !at2)
		return 0.0f;

	return HBCalcPair( this, at1, at2 );
}

// --- SELECTION ---

void MDTRA_PDB_File :: selectElement( const char *s, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const
{
	const MDTRA_PDB_Atom *pAt = m_pAtoms;
	for (int i = 0; i < m_iNumAtoms; i++, pAt++) {
		if (UTIL_SymbolMatch(pAt->symbol, s))
			pSet->setValue( i );
	}
}

void MDTRA_PDB_File :: selectAtomno( int atomno, int cmp, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const
{
	const MDTRA_PDB_Atom *pAt = m_pAtoms;
	for (int i = 0; i < m_iNumAtoms; i++, pAt++) {
		bool bCmp = false;
		switch (cmp) {
		case SELECT_OP_EQUAL:	bCmp = (pAt->serialnumber == atomno); break;
		case SELECT_OP_LESS:	bCmp = (pAt->serialnumber < atomno); break;
		case SELECT_OP_GREATER:	bCmp = (pAt->serialnumber > atomno); break;
		case SELECT_OP_LEQUAL:	bCmp = (pAt->serialnumber <= atomno); break;
		case SELECT_OP_GEQUAL:	bCmp = (pAt->serialnumber >= atomno); break;
		default:				break;
		}
		if (bCmp)
			pSet->setValue( i );
	}
}

void MDTRA_PDB_File :: selectChain( int chain, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const
{
	const MDTRA_PDB_Atom *pAt = m_pAtoms;
	for (int i = 0; i < m_iNumAtoms; i++, pAt++) {
		if (pAt->chainIndex == chain)
			pSet->setValue( i );
	}
}

void MDTRA_PDB_File :: selectAtom( const char *s, const char *res, int chain, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const
{
	const MDTRA_PDB_Atom *pAt = m_pAtoms;
	for (int i = 0; i < m_iNumAtoms; i++, pAt++) {
		if ((chain >= 0) && (pAt->chainIndex != chain))
			continue;
		if (res && !UTIL_strfilter(res, pAt->trimmed_residue, 0) && !UTIL_ResidueGroupMatch(res, pAt->trimmed_residue))
			continue;		
		if (UTIL_strfilter(s, pAt->trimmed_title, 0)) {
			pSet->setValue( i );
		}
	}
}

void MDTRA_PDB_File :: selectResidue( const char *s, int num, int chain, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const
{
	const MDTRA_PDB_Atom *pAt = m_pAtoms;
	for (int i = 0; i < m_iNumAtoms; i++, pAt++) {
		if ((chain >= 0) && (pAt->chainIndex != chain))
			continue;
		if ((num > 0) && (pAt->residuenumber != num))
			continue;
		if (UTIL_strfilter(s, pAt->trimmed_residue, 0) || UTIL_ResidueGroupMatch(s, pAt->trimmed_residue))
			pSet->setValue( i );
	}
}

void MDTRA_PDB_File :: selectResidueno( int min_i, int max_i, int chain, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const
{
	const MDTRA_PDB_Atom *pAt = m_pAtoms;
	for (int i = 0; i < m_iNumAtoms; i++, pAt++) {
		if ((chain >= 0) && (pAt->chainIndex != chain))
			continue;
		if (pAt->residuenumber < min_i || pAt->residuenumber > max_i)
			continue;
		pSet->setValue( i );
	}
}

void MDTRA_PDB_File :: selectResiduenoAtom( const char *s, int min_i, int max_i, int chain, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const
{
	const MDTRA_PDB_Atom *pAt = m_pAtoms;
	for (int i = 0; i < m_iNumAtoms; i++, pAt++) {
		if ((chain >= 0) && (pAt->chainIndex != chain))
			continue;
		if (pAt->residuenumber < min_i || pAt->residuenumber > max_i)
			continue;
		if (UTIL_strfilter(s, pAt->trimmed_title, 0))
			pSet->setValue( i );
	}
}

void MDTRA_PDB_File :: selectByFlags( int flags, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const
{
	const MDTRA_PDB_Atom *pAt = m_pAtoms;
	for (int i = 0; i < m_iNumAtoms; i++, pAt++) {
		if (pAt->atomFlags & flags)
			pSet->setValue( i );
	}
}

float MDTRA_PDB_File :: get_sas( int threadnum ) const
{
#if defined(MDTRA_ALLOW_CUDA)
	if ( MDTRA_CUDA_CanCalculateSAS( m_iNumAtoms ) )
		return MDTRA_CUDA_CalculateSAS( threadnum, m_pAtoms, m_iNumAtoms );
#endif
	return MDTRA_CalculateSAS( threadnum, m_pAtoms, m_iNumAtoms );
}

float MDTRA_PDB_File :: get_occa( int threadnum ) const
{
#if defined(MDTRA_ALLOW_CUDA)
	if ( MDTRA_CUDA_CanCalculateSAS( m_iNumAtoms ) ) {
	/*	float f = MDTRA_CUDA_CalculateOcclusion( threadnum, m_pAtoms, m_iNumAtoms );
		float f2 = MDTRA_CalculateOcclusion( threadnum, m_pAtoms, m_iNumAtoms );
		if ( fabsf(f - f2) > 1.0f )
			__asm int 3
		return f;*/
		return MDTRA_CUDA_CalculateOcclusion( threadnum, m_pAtoms, m_iNumAtoms );
	}
#endif
	return MDTRA_CalculateOcclusion( threadnum, m_pAtoms, m_iNumAtoms );
}

void MDTRA_PDB_File :: get_sas_of_residues( int threadnum, float *pOutData ) const
{
	memset( pOutData, 0, m_iNumResidues*sizeof(float) );
#if defined(MDTRA_ALLOW_CUDA)
	if ( MDTRA_CUDA_CanCalculateSAS( m_iNumAtoms ) ) {
		MDTRA_CUDA_CalculateSASPerResidue( threadnum, m_pAtoms, m_iNumAtoms, pOutData );
		return;
	}
#endif
	MDTRA_CalculateSASPerResidue( threadnum, m_pAtoms, m_iNumAtoms, pOutData );
}

void MDTRA_PDB_File :: get_occa_of_residues( int threadnum, float *pOutData ) const
{
	memset( pOutData, 0, m_iNumResidues*sizeof(float) );
#if defined(MDTRA_ALLOW_CUDA)
	if ( MDTRA_CUDA_CanCalculateSAS( m_iNumAtoms ) ) {
		MDTRA_CUDA_CalculateOcclusionPerResidue( threadnum, m_pAtoms, m_iNumAtoms, pOutData );
		return;
	}
#endif
	MDTRA_CalculateOcclusionPerResidue( threadnum, m_pAtoms, m_iNumAtoms, pOutData );
}

const MDTRA_SRFDef* MDTRA_PDB_File :: get_residue_SRFDef( const char *residueTitle ) const
{
	int numdef = sizeof(s_SRFDef)/sizeof(s_SRFDef[0]);
	for (int i = 0; i < numdef; i++) {
		if (!_strnicmp(s_SRFDef[i].residueTitle, residueTitle, strlen(s_SRFDef[i].residueTitle)))
			return &s_SRFDef[i];
	}
	return NULL;
}

bool MDTRA_PDB_File :: get_residue_transform( int atIndex, float* pOutRotation, float* pOutTranslation ) const
{
	float s_matrix[MAX_SRF_ATOMS*3];
	float e_matrix[MAX_SRF_ATOMS*3];
	float s_average[3];
	float e_average[3];
	float cov_matrix[9];
	float m_matrix[16];
	float eigen_values[4];
	float eigen_vectors[16];

	const MDTRA_PDB_Atom* at = fetchAtomBySerialNumber( atIndex );
	if (!at) return false;

	const MDTRA_SRFDef* pDef = get_residue_SRFDef( at->trimmed_residue );
	if ( !pDef ) return false;

	// create s_matrix and s_average
	Vec3_Set( s_average, 0, 0, 0 );
	for (int i = 0; i < pDef->numAtoms; i++) {
		s_matrix[i*3+0] = pDef->atomList[i].origin[0];
		s_matrix[i*3+1] = pDef->atomList[i].origin[1];
		s_matrix[i*3+2] = pDef->atomList[i].origin[2];
		s_average[0] += s_matrix[i*3+0];
		s_average[1] += s_matrix[i*3+1];
		s_average[2] += s_matrix[i*3+2];
	}
	s_average[0] /= pDef->numAtoms;
	s_average[1] /= pDef->numAtoms;
	s_average[2] /= pDef->numAtoms;
	
	// create e_matrix and e_average
	Vec3_Set( e_average, 0, 0, 0 );
	for (int i = 0; i < pDef->numAtoms; i++) {
		const MDTRA_PDB_Atom* pTestAtom = fetchAtomByDesc( at->chainIndex, at->residuenumber, pDef->atomList[i].atomTitle );
		if (!pTestAtom) return false;
		e_matrix[i*3+0] = pTestAtom->xyz[0];
		e_matrix[i*3+1] = pTestAtom->xyz[1];
		e_matrix[i*3+2] = pTestAtom->xyz[2];
		e_average[0] += e_matrix[i*3+0];
		e_average[1] += e_matrix[i*3+1];
		e_average[2] += e_matrix[i*3+2];
	}
	e_average[0] /= pDef->numAtoms;
	e_average[1] /= pDef->numAtoms;
	e_average[2] /= pDef->numAtoms;
	
	// create covariation matrix
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			float f1 = 0.0f;
			float f2 = 0.0f;
			float f3 = 0.0f;
			for (int k = 0; k < pDef->numAtoms; k++) {
				f1 += s_matrix[k*3+i]*e_matrix[k*3+j];
				f2 += s_matrix[k*3+i];
				f3 += e_matrix[k*3+j];
			}
			f1 -= (f2*f3) / pDef->numAtoms;
			cov_matrix[i*3+j] = f1 / (pDef->numAtoms-1); 
		}
	}

	/*
	create m_matrix:
				| c11+c22+c33	c23-c32			c31-c13			c12-c21			|
	m_matrix =	| c23-c32		c11-c22-c33		c12+c21			c31+c13			|
				| c31-c13		c12+c21			-c11+c22-c33	c23+c32			|
				| c12-c21		c31+c13			c23+c32			-c11-c22+c33	|
	*/
	m_matrix[0] = cov_matrix[0]+cov_matrix[4]+cov_matrix[8];
	m_matrix[1] = cov_matrix[5]-cov_matrix[7];
	m_matrix[2] = cov_matrix[6]-cov_matrix[2];
	m_matrix[3] = cov_matrix[1]-cov_matrix[3];
	m_matrix[4] = cov_matrix[5]-cov_matrix[7];
	m_matrix[5] = cov_matrix[0]-cov_matrix[4]-cov_matrix[8];
	m_matrix[6] = cov_matrix[1]+cov_matrix[3];
	m_matrix[7] = cov_matrix[6]+cov_matrix[2];
	m_matrix[8] = cov_matrix[6]-cov_matrix[2];
	m_matrix[9] = cov_matrix[1]+cov_matrix[3];
	m_matrix[10] = -cov_matrix[0]+cov_matrix[4]-cov_matrix[8];
	m_matrix[11] = cov_matrix[5]+cov_matrix[7];
	m_matrix[12] = cov_matrix[1]-cov_matrix[3];
	m_matrix[13] = cov_matrix[6]+cov_matrix[2];
	m_matrix[14] = cov_matrix[5]+cov_matrix[7];
	m_matrix[15] = -cov_matrix[0]-cov_matrix[4]+cov_matrix[8];

	//get eigenvalues and eigenvectors
	//eigenvector corresponding to the largest eigenvalue is a best-rotation quaternion
	if (!jacobi4( m_matrix, eigen_values, eigen_vectors )) 
		return false;
	eigsrt4( eigen_values, eigen_vectors );

	/*
	output rotation matrix:
		| q0q0+q1q1-q2q2-q3q3	2(q1q2-q0q3)			2(q1q3+q0q2)			|
	R = | 2(q2q1+q0q3)			q0q0-q1q1+q2q2-q3q3		2(q2q3-q0q1)			|
		| 2(q3q1-q0q2)			2(q3q2+q0q1)			q0q0-q1q1-q2q2+q3q3		|
	*/
	if ( pOutRotation ) {
		pOutRotation[0] = eigen_vectors[0]*eigen_vectors[0] + eigen_vectors[1]*eigen_vectors[1] - eigen_vectors[2]*eigen_vectors[2] - eigen_vectors[3]*eigen_vectors[3];
		pOutRotation[1] = 2.0f * (eigen_vectors[1]*eigen_vectors[2] - eigen_vectors[0]*eigen_vectors[3]);
		pOutRotation[2] = 2.0f * (eigen_vectors[1]*eigen_vectors[3] + eigen_vectors[0]*eigen_vectors[2]);
		pOutRotation[3] = 2.0f * (eigen_vectors[2]*eigen_vectors[1] + eigen_vectors[0]*eigen_vectors[3]);
		pOutRotation[4] = eigen_vectors[0]*eigen_vectors[0] - eigen_vectors[1]*eigen_vectors[1] + eigen_vectors[2]*eigen_vectors[2] - eigen_vectors[3]*eigen_vectors[3];
		pOutRotation[5] = 2.0f * (eigen_vectors[2]*eigen_vectors[3] - eigen_vectors[0]*eigen_vectors[1]);
		pOutRotation[6] = 2.0f * (eigen_vectors[3]*eigen_vectors[1] - eigen_vectors[0]*eigen_vectors[2]);
		pOutRotation[7] = 2.0f * (eigen_vectors[3]*eigen_vectors[2] + eigen_vectors[0]*eigen_vectors[1]);
		pOutRotation[8] = eigen_vectors[0]*eigen_vectors[0] - eigen_vectors[1]*eigen_vectors[1] - eigen_vectors[2]*eigen_vectors[2] + eigen_vectors[3]*eigen_vectors[3];

		if ( pOutTranslation ) {
			pOutTranslation[0] = e_average[0] - (s_average[0]*pOutRotation[0] + s_average[1]*pOutRotation[1] + s_average[2]*pOutRotation[2]);
			pOutTranslation[1] = e_average[1] - (s_average[0]*pOutRotation[3] + s_average[1]*pOutRotation[4] + s_average[2]*pOutRotation[5]);
			pOutTranslation[2] = e_average[2] - (s_average[0]*pOutRotation[6] + s_average[1]*pOutRotation[7] + s_average[2]*pOutRotation[8]);
		}
		return true;
	}

	return false;
}