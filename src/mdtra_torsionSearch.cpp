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
//	Torsion search algorithm

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_progressDialog.h"
#include "mdtra_waitDialog.h"
#include "mdtra_torsionSearch.h"
#include "mdtra_math.h"

#include <QtGui/QMessageBox>

//=================================================
static const MDTRA_TorsionSearchData *pLocalTorsionSearchData = NULL;
static int iLocalTorsionSearchDataIndex = 0;
extern MDTRA_ProgressDialog *pProgressDialog;
static MDTRA_WaitDialog *pWaitDialog = NULL;
static bool s_threadStarted[MDTRA_MAX_THREADS];

MDTRA_TorsionSearchData s_ltsd[2];
static MDTRA_MainWindow *s_pMainWindow;
static float s_flReference;
static int s_bufferDim;
QVector<MDTRA_TorsionSearchDef> s_TorsionsList;
int s_TorsionListValidCount;

static MDTRA_TorsionAngleDesc s_TorsionAnglesList[] = 
{
	// Protein Backbone
	{ "Phi", NULL, "C", "N", "CA", "C", NULL, TADF_ATOM1_FROM_PREV_RESIDUE },
	{ "Psi", NULL, "N", "CA", "C", "N", NULL, TADF_ATOM4_FROM_NEXT_RESIDUE },
	// Nucleic Acid Backbone
	{ "Alpha", NULL, "O3'", "P", "O5'", "C5'", NULL, TADF_ATOM1_FROM_PREV_RESIDUE },
	{ "Beta", NULL, "P", "O5'", "C5'", "C4'", NULL, 0 },
	{ "Gamma", NULL, "O5'", "C5'", "C4'", "C3'", NULL, 0 },
	{ "Delta", NULL, "O4'", "C4'", "C3'", "O3'", NULL, 0 },
	{ "Epsilon", NULL, "C4'", "C3'", "O3'", "P", NULL, TADF_ATOM4_FROM_NEXT_RESIDUE },
	{ "Zeta", NULL, "C3'", "O3'", "P", "O5'", NULL, TADF_ATOM3_FROM_NEXT_RESIDUE | TADF_ATOM4_FROM_NEXT_RESIDUE },
	{ "Nu0", NULL, "C4'", "O4'", "C1'", "C2'", NULL, 0 },
	{ "Nu1", NULL, "O4'", "C1'", "C2'", "C3'", NULL, 0 },
	{ "Nu2", NULL, "C1'", "C2'", "C3'", "C4'", NULL, 0 },
	{ "Nu3", NULL, "C2'", "C3'", "C4'", "O4'", NULL, 0 },
	{ "Nu4", NULL, "C3'", "C4'", "O4'", "C1'", NULL, 0 },
	{ "Chi", "DC", "O4'", "C1'", "N1", "C2", NULL, 0 },
	{ "Chi", "DC3", "O4'", "C1'", "N1", "C2", NULL, 0 },
	{ "Chi", "DC5", "O4'", "C1'", "N1", "C2", NULL, 0 },
	{ "Chi", "DT", "O4'", "C1'", "N1", "C2", NULL, 0 },
	{ "Chi", "DT3", "O4'", "C1'", "N1", "C2", NULL, 0 },
	{ "Chi", "DT5", "O4'", "C1'", "N1", "C2", NULL, 0 },
	{ "Chi", "DA", "O4'", "C1'", "N9", "C4", NULL, 0 },
	{ "Chi", "DA3", "O4'", "C1'", "N9", "C4", NULL, 0 },
	{ "Chi", "DA5", "O4'", "C1'", "N9", "C4", NULL, 0 },
	{ "Chi", "DG", "O4'", "C1'", "N9", "C4", NULL, 0 },
	{ "Chi", "DG3", "O4'", "C1'", "N9", "C4", NULL, 0 },
	{ "Chi", "DG5", "O4'", "C1'", "N9", "C4", NULL, 0 },
	{ "Chi", "ODG", "O4'", "C1'", "N9", "C4", NULL, 0 },
	// Protein Sidechain
	{ NULL, "VAL", "N", "CA", "CB", "CG1", NULL, 0 },
	{ NULL, "ILE", "N", "CA", "CB", "CG1", NULL, 0 },
	{ NULL, "ILE", "CA", "CB", "CG1", "CD1", NULL, 0 },
	{ NULL, "LEU", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "LEU", "CA", "CB", "CG", "CD1", NULL, 0 },
	{ NULL, "PHE", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "PHE", "CA", "CB", "CG", "CD1", "CD2", 0 },
	{ NULL, "MET", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "MET", "CA", "CB", "CG", "SD", NULL, 0 },
	{ NULL, "MET", "CB", "CG", "SD", "CE", NULL, 0 },
	{ NULL, "TRP", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "TRP", "CA", "CB", "CG", "CD1", NULL, 0 },
	{ NULL, "CYS", "N", "CA", "CB", "SG", NULL, 0 },
	{ NULL, "CYX", "N", "CA", "CB", "SG", NULL, 0 },
	{ NULL, "CYM", "N", "CA", "CB", "SG", NULL, 0 },
	{ NULL, "SER", "N", "CA", "CB", "OG", NULL, 0 },
	{ NULL, "THR", "N", "CA", "CB", "OG1", NULL, 0 },
	{ NULL, "ASN", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "ASN", "CA", "CB", "CG", "OD1", NULL, 0 },
	{ NULL, "GLN", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "GLN", "CA", "CB", "CG", "CD", NULL, 0 },
	{ NULL, "GLN", "CB", "CG", "CD", "OE1", NULL, 0 },
	{ NULL, "TYR", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "TYR", "CA", "CB", "CG", "CD1", NULL, 0 },
	{ NULL, "HIS", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "HIS", "CA", "CB", "CG", "ND1", NULL, 0 },
	{ NULL, "HID", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "HID", "CA", "CB", "CG", "ND1", NULL, 0 },
	{ NULL, "HIE", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "HIE", "CA", "CB", "CG", "ND1", NULL, 0 },
	{ NULL, "HIP", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "HIP", "CA", "CB", "CG", "ND1", NULL, 0 },
	{ NULL, "HSD", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "HSD", "CA", "CB", "CG", "ND1", NULL, 0 },
	{ NULL, "HSE", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "HSE", "CA", "CB", "CG", "ND1", NULL, 0 },
	{ NULL, "HSP", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "HSP", "CA", "CB", "CG", "ND1", NULL, 0 },
	{ NULL, "ASP", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "ASP", "CA", "CB", "CG", "OD1", "OD2", 0 },
	{ NULL, "ASH", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "ASH", "CA", "CB", "CG", "OD1", "OD2", 0 },
	{ NULL, "GLU", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "GLU", "CA", "CB", "CG", "CD", NULL, 0 },
	{ NULL, "GLU", "CB", "CG", "CD", "OE1", "OE2", 0 },
	{ NULL, "GLH", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "GLH", "CA", "CB", "CG", "CD", NULL, 0 },
	{ NULL, "GLH", "CB", "CG", "CD", "OE1", "OE2", 0 },
	{ NULL, "LYS", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "LYS", "CA", "CB", "CG", "CD", NULL, 0 },
	{ NULL, "LYS", "CB", "CG", "CD", "CE", NULL, 0 },
	{ NULL, "LYS", "CG", "CD", "CE", "NZ", NULL, 0 },
	{ NULL, "LYN", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "LYN", "CA", "CB", "CG", "CD", NULL, 0 },
	{ NULL, "LYN", "CB", "CG", "CD", "CE", NULL, 0 },
	{ NULL, "LYN", "CG", "CD", "CE", "NZ", NULL, 0 },
	{ NULL, "ARG", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "ARG", "CA", "CB", "CG", "CD", NULL, 0 },
	{ NULL, "ARG", "CB", "CG", "CD", "NE", NULL, 0 },
	{ NULL, "ARG", "CG", "CD", "NE", "CZ", NULL, 0 },
	{ NULL, "ARG", "CD", "NE", "CZ", "NH1", "NH2", 0 },
	{ NULL, "PRO", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "PRO", "CA", "CB", "CG", "CD", NULL, 0 },
	{ NULL, "PRO", "CB", "CG", "CD", "N", NULL, 0 },
	{ NULL, "PRO", "CG", "CD", "N", "CA", NULL, 0 },
	{ NULL, "PRH", "N", "CA", "CB", "CG", NULL, 0 },
	{ NULL, "PRH", "CA", "CB", "CG", "CD", NULL, 0 },
	{ NULL, "PRH", "CB", "CG", "CD", "N", NULL, 0 },
	{ NULL, "PRH", "CG", "CD", "N", "CA", NULL, 0 },
	{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0 },
};

const MDTRA_TorsionAngleDesc* TSGetDesc( const MDTRA_TorsionAngleDesc *pStart, const char *res_title, const char *at_title )
{
	if (!pStart)
		pStart = s_TorsionAnglesList;
	else
		pStart++;

	while ( pStart->AtomTitle1 ) {
		if ( pStart->ResidueTitle && _stricmp( pStart->ResidueTitle, res_title ) ) {
			pStart++;
			continue;
		}
		if ( _stricmp( pStart->AtomTitle1, at_title ) ) {
			pStart++;
			continue;
		}
		return pStart;
	}

	return NULL;
}

const MDTRA_PDB_Atom *TSFetchIndex2( const MDTRA_TorsionAngleDesc *pDesc, const MDTRA_PDB_File *pPdb, const MDTRA_PDB_Atom *pFirstAtom )
{
	int fetchResidue = pFirstAtom->residuenumber;
	if ( pDesc->Flags & TADF_ATOM1_FROM_PREV_RESIDUE )
		fetchResidue++;
	return pPdb->fetchAtomByDesc( pFirstAtom->chainIndex, fetchResidue, pDesc->AtomTitle2 );
}
const MDTRA_PDB_Atom *TSFetchIndex3( const MDTRA_TorsionAngleDesc *pDesc, const MDTRA_PDB_File *pPdb, const MDTRA_PDB_Atom *pFirstAtom )
{
	int fetchResidue = pFirstAtom->residuenumber;
	if ( pDesc->Flags & TADF_ATOM1_FROM_PREV_RESIDUE )
		fetchResidue++;
	if ( pDesc->Flags & TADF_ATOM3_FROM_NEXT_RESIDUE )
		fetchResidue++;
	return pPdb->fetchAtomByDesc( pFirstAtom->chainIndex, fetchResidue, pDesc->AtomTitle3 );
}
const MDTRA_PDB_Atom *TSFetchIndex4( const MDTRA_TorsionAngleDesc *pDesc, const MDTRA_PDB_File *pPdb, const MDTRA_PDB_Atom *pFirstAtom )
{
	int fetchResidue = pFirstAtom->residuenumber;
	if ( pDesc->Flags & TADF_ATOM1_FROM_PREV_RESIDUE )
		fetchResidue++;
	if ( pDesc->Flags & TADF_ATOM4_FROM_NEXT_RESIDUE )
		fetchResidue++;
	return pPdb->fetchAtomByDesc( pFirstAtom->chainIndex, fetchResidue, pDesc->AtomTitle4 );
}
const MDTRA_PDB_Atom *TSFetchIndex4Alt( const MDTRA_TorsionAngleDesc *pDesc, const MDTRA_PDB_File *pPdb, const MDTRA_PDB_Atom *pFirstAtom )
{
	int fetchResidue = pFirstAtom->residuenumber;
	if ( pDesc->Flags & TADF_ATOM1_FROM_PREV_RESIDUE )
		fetchResidue++;
	if ( pDesc->Flags & TADF_ATOM4_FROM_NEXT_RESIDUE )
		fetchResidue++;
	return pPdb->fetchAtomByDesc( pFirstAtom->chainIndex, fetchResidue, pDesc->AtomTitle4Alt );
}

static bool TSGetAllTorsionAngles( void )
{
	MDTRA_TorsionSearchDef localTA;
	memset( &localTA, 0, sizeof(localTA) );

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	//walk through each atom in stream 1...
	//if atom is not within both selections (by title AND resnum, not restitle!) - continue
	//if atom is not a part of a torsion angle - continue
	//if other atoms of that torsion angle are not within both selections - continue
	//otherwise, torsion angle is valid, add it to the list

	s_ltsd[0].pStream->pdb->set_flag( s_ltsd[0].selectionSize, s_ltsd[0].selectionData, PDB_FLAG_SELECTED );
	s_ltsd[1].pStream->pdb->set_flag( s_ltsd[1].selectionSize, s_ltsd[1].selectionData, PDB_FLAG_SELECTED );

	for ( int i = 0; i < s_ltsd[0].selectionSize; i++) {
		// get atom
		const MDTRA_PDB_Atom *pAtom1 = s_ltsd[0].pStream->pdb->fetchAtomByIndex( s_ltsd[0].selectionData[i] );
		if (!pAtom1)
			continue;

		// check if atom is also within stream 2 selection
		const MDTRA_PDB_Atom *pAtom2 = s_ltsd[1].pStream->pdb->fetchAtomByDesc( pAtom1->chainIndex, pAtom1->residuenumber, pAtom1->trimmed_title );
		if (!pAtom2 || !(pAtom2->atomFlags & PDB_FLAG_SELECTED))
			continue;
		
		// get corresponding torsion angles
		const MDTRA_TorsionAngleDesc *pTD = TSGetDesc( NULL, pAtom1->trimmed_residue, pAtom1->trimmed_title );
		while ( pTD ) {
			const MDTRA_PDB_Atom *pAtom1_2 = NULL;
			const MDTRA_PDB_Atom *pAtom1_3 = NULL;
			const MDTRA_PDB_Atom *pAtom1_4 = NULL;
			const MDTRA_PDB_Atom *pAtom2_2 = NULL;
			const MDTRA_PDB_Atom *pAtom2_3 = NULL;
			const MDTRA_PDB_Atom *pAtom2_4 = NULL;
			const MDTRA_PDB_Atom *pAtom1_4alt = NULL;
			const MDTRA_PDB_Atom *pAtom2_4alt = NULL;

			// get all other atoms
			pAtom1_2 = TSFetchIndex2( pTD, s_ltsd[0].pStream->pdb, pAtom1 );
			if ( !pAtom1_2 || !(pAtom1_2->atomFlags & PDB_FLAG_SELECTED) ) goto bail;

			pAtom1_3 = TSFetchIndex3( pTD, s_ltsd[0].pStream->pdb, pAtom1 );
			if ( !pAtom1_3 || !(pAtom1_3->atomFlags & PDB_FLAG_SELECTED) ) goto bail;
			
			pAtom1_4 = TSFetchIndex4( pTD, s_ltsd[0].pStream->pdb, pAtom1 );
			if ( !pAtom1_4 || !(pAtom1_4->atomFlags & PDB_FLAG_SELECTED) ) goto bail;
	
			pAtom2_2 = TSFetchIndex2( pTD, s_ltsd[1].pStream->pdb, pAtom2 );
			if ( !pAtom2_2 || !(pAtom2_2->atomFlags & PDB_FLAG_SELECTED) ) goto bail;

			pAtom2_3 = TSFetchIndex3( pTD, s_ltsd[1].pStream->pdb, pAtom2 );
			if ( !pAtom2_3 || !(pAtom2_3->atomFlags & PDB_FLAG_SELECTED) ) goto bail;
			
			pAtom2_4 = TSFetchIndex4( pTD, s_ltsd[1].pStream->pdb, pAtom2 );
			if ( !pAtom2_4 || !(pAtom2_4->atomFlags & PDB_FLAG_SELECTED) ) goto bail;

			// check for alternate 4th atom
			if ( pTD->AtomTitle4Alt ) {
				pAtom1_4alt = TSFetchIndex4Alt( pTD, s_ltsd[0].pStream->pdb, pAtom1 );
				pAtom2_4alt = TSFetchIndex4Alt( pTD, s_ltsd[1].pStream->pdb, pAtom2 );
				if ( !pAtom1_4alt || !(pAtom1_4alt->atomFlags & PDB_FLAG_SELECTED) ||
					 !pAtom2_4alt || !(pAtom2_4alt->atomFlags & PDB_FLAG_SELECTED) ) {
					pAtom1_4alt = NULL;
					pAtom2_4alt = NULL;
				}
			}

			// all other atoms are valid and selected
			// add this torsion angle
			localTA.atom1[0] = pAtom1->serialnumber;
			localTA.atom1[1] = pAtom2->serialnumber;
			localTA.atom2[0] = pAtom1_2->serialnumber;
			localTA.atom2[1] = pAtom2_2->serialnumber;
			localTA.atom3[0] = pAtom1_3->serialnumber;
			localTA.atom3[1] = pAtom2_3->serialnumber;
			localTA.atom4[0] = pAtom1_4->serialnumber;
			localTA.atom4[1] = pAtom2_4->serialnumber;
			localTA.atom4alt[0] = pAtom1_4alt ? pAtom1_4alt->serialnumber : -1;
			localTA.atom4alt[1] = pAtom2_4alt ? pAtom2_4alt->serialnumber : -1;
			localTA.title = pTD->AngleTitle;
			s_TorsionsList << localTA;

		bail:
			pTD = TSGetDesc( pTD, pAtom1->trimmed_residue, pAtom1->trimmed_title );
		}
	}

	QApplication::restoreOverrideCursor();
	return ( s_TorsionsList.count() > 0 );
}

static void fn_TorsionSearch( int threadnum, int num )
{
	//Load PDB file
	MDTRA_PDB_File *pPdbFile;
	if ( (pLocalTorsionSearchData->workStart + num) == 0) {
		pPdbFile = pLocalTorsionSearchData->pStream->pdb;
	} else {
		pPdbFile = pLocalTorsionSearchData->tempPDB[threadnum];
		pPdbFile->load( threadnum, pLocalTorsionSearchData->pStream->format_identifier, pLocalTorsionSearchData->pStream->files.at(pLocalTorsionSearchData->workStart + num).toAscii(), pLocalTorsionSearchData->pStream->flags );
	}

	bool firstStep = false;
	if (!s_threadStarted[threadnum]) {
		s_threadStarted[threadnum] = true;
		firstStep = true;
	}

	//Calculate all torsion angles
	for ( int i = 0; i < s_TorsionsList.count(); i++ ) {
		const MDTRA_TorsionSearchDef *pTSD = &s_TorsionsList.at( i );
		float *pflCell = pLocalTorsionSearchData->pResults[threadnum] + i * s_bufferDim;

		float flAngle = pPdbFile->get_torsion( pTSD->atom1[iLocalTorsionSearchDataIndex],
											 pTSD->atom2[iLocalTorsionSearchDataIndex],
											 pTSD->atom3[iLocalTorsionSearchDataIndex],
											 pTSD->atom4[iLocalTorsionSearchDataIndex], 
											 false );

		if ( pTSD->atom4alt[iLocalTorsionSearchDataIndex] > 0 ) {
			float flAngle2 = pPdbFile->get_torsion( pTSD->atom1[iLocalTorsionSearchDataIndex],
												  pTSD->atom2[iLocalTorsionSearchDataIndex],
												  pTSD->atom3[iLocalTorsionSearchDataIndex],
												  pTSD->atom4alt[iLocalTorsionSearchDataIndex], 
												  false );
			if ( flAngle2 < flAngle )
				flAngle = flAngle2;
		}

		flAngle = UTIL_rad2deg( flAngle );
		if ( flAngle > 180 ) flAngle = 360 - flAngle;

		switch (pLocalTorsionSearchData->statParm) {
		case MDTRA_SP_ARITHMETIC_MEAN:
			*pflCell += flAngle;
			break;
		case MDTRA_SP_HARMONIC_MEAN:
			*pflCell += (1.0f / flAngle);
			break;
		case MDTRA_SP_QUADRATIC_MEAN:
			*pflCell += (flAngle*flAngle);
			break;
		case MDTRA_SP_GEOMETRIC_MEAN:
			if (firstStep) *pflCell = flAngle;
			else *pflCell *= flAngle;
			break;
		case MDTRA_SP_MIN_VALUE:
			if (firstStep) *pflCell = flAngle;
			else if (flAngle < *pflCell) *pflCell = flAngle;
			break;
		case MDTRA_SP_MAX_VALUE:
			if (firstStep) *pflCell = flAngle;
			else if (flAngle > *pflCell) *pflCell = flAngle;
			break;
		case MDTRA_SP_RANGE:
		case MDTRA_SP_MIDRANGE:
			if (firstStep) *pflCell = flAngle;
			else if (flAngle < *pflCell) *pflCell = flAngle;
			pflCell++;
			if (firstStep) *pflCell = flAngle;
			else if (flAngle > *pflCell) *pflCell = flAngle;
			break;
		case MDTRA_SP_VARIANCE:
			*pflCell += flAngle;
			pflCell++;
			*pflCell += (flAngle*flAngle);
			break;
		default:
			break;
		}
	}

	if (pProgressDialog) {
		pProgressDialog->advanceCurrentFile( num+1 );
		if (pProgressDialog->checkInterrupt()) {
			InterruptThreads();
		}
	}
}

static void fn_TorsionSearchJoin( int threadnum )
{
	//Join cells
	for ( int i = 0; i < s_TorsionsList.count(); i++ ) {
		float *pflSrcCell = pLocalTorsionSearchData->pResults[threadnum] + i*s_bufferDim;
		float *pflDstCell = pLocalTorsionSearchData->pResults[0] + i*s_bufferDim;
		if (*pflSrcCell < 0 || *pflDstCell < 0)
			continue;

		switch (pLocalTorsionSearchData->statParm) {
		case MDTRA_SP_MIN_VALUE: if (*pflSrcCell < *pflDstCell) *pflDstCell = *pflSrcCell; break;
		case MDTRA_SP_MAX_VALUE: if (*pflSrcCell > *pflDstCell) *pflDstCell = *pflSrcCell; break;
		case MDTRA_SP_RANGE:
		case MDTRA_SP_MIDRANGE:
			if (*pflSrcCell < *pflDstCell) *pflDstCell = *pflSrcCell; 
			pflSrcCell++; pflDstCell++;
			if (*pflSrcCell > *pflDstCell) *pflDstCell = *pflSrcCell; 
			break;
		case MDTRA_SP_VARIANCE:
			*pflDstCell += *pflSrcCell;
			pflSrcCell++; pflDstCell++;
			*pflDstCell += *pflSrcCell; 
			break;
		default: *pflDstCell += *pflSrcCell; break;
		}
	}
}

static void fn_TorsionSearchFinalize( void )
{
	float flDataSize = pLocalTorsionSearchData->workCount;
	float flInvDataSize = 1.0f / flDataSize;
	

	//finalize cells
	switch (pLocalTorsionSearchData->statParm) {
	case MDTRA_SP_ARITHMETIC_MEAN:
		{
			for ( int i = 0; i < s_TorsionsList.count(); i++ ) {
				MDTRA_TorsionSearchDef *pTSD = const_cast<MDTRA_TorsionSearchDef*>(&s_TorsionsList.at( i ));
				float *pflOutput = (iLocalTorsionSearchDataIndex == 0) ? &pTSD->value1 : &pTSD->value2;
				float *pflCell = pLocalTorsionSearchData->pResults[0] + i*s_bufferDim;
				if (*pflCell > 0) *pflCell *= flInvDataSize;
				*pflOutput = *pflCell;
			}
		}
		break;
	case MDTRA_SP_GEOMETRIC_MEAN:
		{
			for ( int i = 0; i < s_TorsionsList.count(); i++ ) {
				MDTRA_TorsionSearchDef *pTSD = const_cast<MDTRA_TorsionSearchDef*>(&s_TorsionsList.at( i ));
				float *pflOutput = (iLocalTorsionSearchDataIndex == 0) ? &pTSD->value1 : &pTSD->value2;
				float *pflCell = pLocalTorsionSearchData->pResults[0] + i*s_bufferDim;
				if (*pflCell > 0) *pflCell = pow( *pflCell, flInvDataSize );
				*pflOutput = *pflCell;
			}
		}
		break;
	case MDTRA_SP_QUADRATIC_MEAN:
		{
			for ( int i = 0; i < s_TorsionsList.count(); i++ ) {
				MDTRA_TorsionSearchDef *pTSD = const_cast<MDTRA_TorsionSearchDef*>(&s_TorsionsList.at( i ));
				float *pflOutput = (iLocalTorsionSearchDataIndex == 0) ? &pTSD->value1 : &pTSD->value2;
				float *pflCell = pLocalTorsionSearchData->pResults[0] + i*s_bufferDim;
				if (*pflCell > 0) *pflCell = sqrtf( *pflCell * flInvDataSize );
				*pflOutput = *pflCell;
			}
		}
		break;
	case MDTRA_SP_HARMONIC_MEAN:
		{
			for ( int i = 0; i < s_TorsionsList.count(); i++ ) {
				MDTRA_TorsionSearchDef *pTSD = const_cast<MDTRA_TorsionSearchDef*>(&s_TorsionsList.at( i ));
				float *pflOutput = (iLocalTorsionSearchDataIndex == 0) ? &pTSD->value1 : &pTSD->value2;
				float *pflCell = pLocalTorsionSearchData->pResults[0] + i*s_bufferDim;
				if (*pflCell > 0) *pflCell = flDataSize * (1.0f / (*pflCell));
				*pflOutput = *pflCell;
			}
		}
		break;
	case MDTRA_SP_MIN_VALUE:
	case MDTRA_SP_MAX_VALUE:
		{
			for ( int i = 0; i < s_TorsionsList.count(); i++ ) {
				MDTRA_TorsionSearchDef *pTSD = const_cast<MDTRA_TorsionSearchDef*>(&s_TorsionsList.at( i ));
				float *pflOutput = (iLocalTorsionSearchDataIndex == 0) ? &pTSD->value1 : &pTSD->value2;
				float *pflCell = pLocalTorsionSearchData->pResults[0] + i*s_bufferDim;
				*pflOutput = *pflCell;
			}
		}
		break;
	case MDTRA_SP_RANGE:
		{
			for ( int i = 0; i < s_TorsionsList.count(); i++ ) {
				MDTRA_TorsionSearchDef *pTSD = const_cast<MDTRA_TorsionSearchDef*>(&s_TorsionsList.at( i ));
				float *pflOutput = (iLocalTorsionSearchDataIndex == 0) ? &pTSD->value1 : &pTSD->value2;
				float *pflCell = pLocalTorsionSearchData->pResults[0] + i*s_bufferDim;
				if (*pflCell > 0) *pflCell = *(pflCell+1) - *pflCell;
				*pflOutput = *pflCell;
			}
		}
		break;
	case MDTRA_SP_MIDRANGE:
		{
			for ( int i = 0; i < s_TorsionsList.count(); i++ ) {
				MDTRA_TorsionSearchDef *pTSD = const_cast<MDTRA_TorsionSearchDef*>(&s_TorsionsList.at( i ));
				float *pflOutput = (iLocalTorsionSearchDataIndex == 0) ? &pTSD->value1 : &pTSD->value2;
				float *pflCell = pLocalTorsionSearchData->pResults[0] + i*s_bufferDim;
				if (*pflCell > 0) *pflCell = (*(pflCell+1) - *pflCell) * 0.5f;
				*pflOutput = *pflCell;
			}
		}
		break;
	case MDTRA_SP_VARIANCE:
		{
			float flVarianceDataSize = flDataSize / (flDataSize - 1.0f);

			for ( int i = 0; i < s_TorsionsList.count(); i++ ) {
				MDTRA_TorsionSearchDef *pTSD = const_cast<MDTRA_TorsionSearchDef*>(&s_TorsionsList.at( i ));
				float *pflOutput = (iLocalTorsionSearchDataIndex == 0) ? &pTSD->value1 : &pTSD->value2;
				float *pflCell = pLocalTorsionSearchData->pResults[0] + i*s_bufferDim;
				if (*pflCell > 0) {
					float q = *(pflCell+1) * flInvDataSize;
					float m = *pflCell * flInvDataSize;
					*pflCell = (flVarianceDataSize * q) - (m * m);
				}
				*pflOutput = *pflCell;
			}
		}
		break;
	default:
		break;
	}
}

static void TorsionSearchCheckAgainstCriterion( float flCriterion )
{
	s_TorsionListValidCount = 0;

	for ( int i = 0; i < s_TorsionsList.count(); i++ ) {
		MDTRA_TorsionSearchDef *pTSD = const_cast<MDTRA_TorsionSearchDef*>(&s_TorsionsList.at( i ));
		float flCheck;

		switch ( s_ltsd[0].statParm ) {
		case MDTRA_SP_RANGE:
		case MDTRA_SP_MIDRANGE:
		case MDTRA_SP_VARIANCE:
			flCheck = fabsf( pTSD->value1 - pTSD->value2 );
			break;
		default:
			flCheck = fabsf( UTIL_angleDiff( pTSD->value1, pTSD->value2 ) );
			break;
		}

		if ( flCheck >= flCriterion ) {
			s_TorsionListValidCount++;
			pTSD->diff = flCheck;
			pTSD->flags |= TSDF_VALID;
		}
	}
}

bool SetupTorsionSearch( MDTRA_MainWindow *pMainWindow, const MDTRA_TorsionSearchInfo *pSearchInfo )
{
	s_pMainWindow = pMainWindow;
	assert( s_pMainWindow != NULL );

	s_TorsionsList.clear();
	memset( s_ltsd, 0, sizeof(MDTRA_TorsionSearchData)*2 );
	s_flReference = pSearchInfo->statReference;
	s_bufferDim = pSearchInfo->bufferDim;

	for (int i = 0; i < 2; i++) {
		s_ltsd[i].workStart = pSearchInfo->trajectoryMin - 1;
		s_ltsd[i].workCount = pSearchInfo->trajectoryMax - pSearchInfo->trajectoryMin + 1;
		s_ltsd[i].selectionSize = pSearchInfo->streamInfo[i].size;
		s_ltsd[i].selectionData = pSearchInfo->streamInfo[i].data;
		s_ltsd[i].statParm = pSearchInfo->statCriterion;
		s_ltsd[i].pStream = s_pMainWindow->getProject()->fetchStreamByIndex( pSearchInfo->streamInfo[i].index );
		if (!s_ltsd[i].pStream)
			return false;
	}

	MDTRA_WaitDialog dlgWait( s_pMainWindow );
	dlgWait.setMessage( QObject::tr("Building list of torsion angles within the selection, please wait...") );
	dlgWait.show();
	pWaitDialog = &dlgWait;
	QApplication::processEvents();

	//get all triplets
	if (!TSGetAllTorsionAngles())  {
		dlgWait.hide();
		return false;
	}

	int numAngles = s_TorsionsList.count();

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < CountThreads(); j++) {
			s_ltsd[i].pResults[j] = new float[numAngles*s_bufferDim];
			if (!s_ltsd[i].pResults[j])
				return false;

			memset( s_ltsd[i].pResults[j], 0, sizeof(float)*numAngles*s_bufferDim );

			s_ltsd[i].tempPDB[j] = new MDTRA_PDB_File;
			if (!s_ltsd[i].tempPDB[j])
				return false;
		}
	}

	pWaitDialog = NULL;
	dlgWait.hide();
	QApplication::processEvents();

	return true;
}

bool PerformTorsionSearch( void )
{
	MDTRA_ProgressDialog dlgProgress( s_pMainWindow );
	dlgProgress.setStreamCount( 2 );
	dlgProgress.show();

	pProgressDialog = &dlgProgress;

	for (int i = 0; i < 2; i++) {
		pLocalTorsionSearchData = &s_ltsd[i];
		iLocalTorsionSearchDataIndex = i;

		dlgProgress.setFileCount( pLocalTorsionSearchData->workCount );
		dlgProgress.setCurrentStream( i );
		dlgProgress.setCurrentFile( 0 );

		memset( s_threadStarted, 0, sizeof(bool) * CountThreads() );

		RunThreadsOnIndividual( pLocalTorsionSearchData->workCount, fn_TorsionSearch );
		for (int j = 1; j < CountThreads(); j++) fn_TorsionSearchJoin( j );
		fn_TorsionSearchFinalize();

		if (dlgProgress.checkInterrupt())
			return false;
	}

	dlgProgress.setProgressAtMax();

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QApplication::processEvents();

	//now check against criterion and add to result list
	TorsionSearchCheckAgainstCriterion( s_flReference );

	QApplication::restoreOverrideCursor();
	dlgProgress.hide();
	QApplication::processEvents();

	if (!s_TorsionListValidCount) {
		QMessageBox::warning(s_pMainWindow, "Torsion Search Results", "No significantly different torsion angles were found!\nPlease specify another atom set or different significance criterion.");
		return false;
	}

	return true;
}

void FreeTorsionSearch( void )
{
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < CountThreads(); j++) {
			if (s_ltsd[i].pResults[j]) {
				delete [] s_ltsd[i].pResults[j];
				s_ltsd[i].pResults[j] = NULL;
			}
			if (s_ltsd[i].tempPDB[j]) {
				delete s_ltsd[i].tempPDB[j];
				s_ltsd[i].tempPDB[j] = NULL;
			}
		}
	}

	s_pMainWindow = NULL;
	s_TorsionsList.clear();
}