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
//	Force search algorithm

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb.h"
#include "mdtra_progressDialog.h"
#include "mdtra_forceSearch.h"
#include "mdtra_math.h"

#include <QtGui/QMessageBox>

static inline void CalculateTableCell_SD( const MDTRA_PDB_File *pdb1, const MDTRA_PDB_File *pdb2, int index, int at1, int at2, MDTRA_StatParm parm, bool firstStep, float *pflCells )
{
	float f1[3], n1[3];
	float f2[3], n2[3];
	float l1, l2;
	float flDiff, flDot, flAngle;
	
	pdb1->get_force_vector( at1, f1 );
	pdb2->get_force_vector( at2, f2 );

	Vec3_Len( l1, f1 );
	Vec3_Len( l2, f2 );
	Vec3_Scale( n1, f1, 1.0f / l1 );
	Vec3_Scale( n2, f2, 1.0f / l2 );

	flDiff = fabsf(l1 - l2);
	Vec3_Dot( flDot, n1, n2 );
	if ( flDot < -1 ) flDot = -1;
	if ( flDot > 1 ) flDot = 1;
	flAngle = acosf( flDot );

	float *pflCell = pflCells + (index<<1);

	switch (parm) {
	case MDTRA_SP_ARITHMETIC_MEAN:
		*pflCell += flDiff;
		pflCell++;
		*pflCell += flAngle;
		break;
	case MDTRA_SP_HARMONIC_MEAN:
		*pflCell += (1.0f / flDiff);
		pflCell++;
		*pflCell += (1.0f / flAngle);
		break;
	case MDTRA_SP_QUADRATIC_MEAN:
		*pflCell += (flDiff*flDiff);
		pflCell++;
		*pflCell += (flAngle*flAngle);
		break;
	case MDTRA_SP_GEOMETRIC_MEAN:
		if (firstStep) {
			*pflCell = flDiff;
			pflCell++;
			*pflCell = flAngle;
		} else {
			*pflCell *= flDiff;
			pflCell++;
			*pflCell *= flAngle;
		}
		break;
	case MDTRA_SP_MIN_VALUE:
		if (firstStep) {
			*pflCell = flDiff;
			pflCell++;
			*pflCell = flAngle;
		} else {
			if (flDiff < *pflCell) *pflCell = flDiff;
			pflCell++;
			if (flAngle < *pflCell) *pflCell = flAngle;
		}
		break;
	case MDTRA_SP_MAX_VALUE:
		if (firstStep) {
			*pflCell = flDiff;
			pflCell++;
			*pflCell = flAngle;
		} else {
			if (flDiff > *pflCell) *pflCell = flDiff;
			pflCell++;
			if (flAngle > *pflCell) *pflCell = flAngle;
		}
		break;
	default:
		break;
	}
}

static inline void CalculateTableCell_DD( const MDTRA_PDB_File *pdb1, const MDTRA_PDB_File *pdb2, int index, int at1, int at2, MDTRA_StatParm parm, bool firstStep, float *pflCells )
{
	float f1[3], n1[3];
	float f2[3], n2[3];
	float l1, l2;
	float flDiff, flDot, flAngle;

	pdb1->get_force_vector( at1, f1 );
	pdb2->get_force_vector( at2, f2 );

	Vec3_Len( l1, f1 );
	Vec3_Len( l2, f2 );
	Vec3_Scale( n1, f1, 1.0f / l1 );
	Vec3_Scale( n2, f2, 1.0f / l2 );

	flDiff = fabsf(l1 - l2);
	Vec3_Dot( flDot, n1, n2 );
	if ( flDot < -1 ) flDot = -1;
	if ( flDot > 1 ) flDot = 1;
	flAngle = acosf( flDot );

	float *pflCell = pflCells + (index<<2);

	switch (parm) {
	case MDTRA_SP_RANGE:
	case MDTRA_SP_MIDRANGE:
		if (firstStep) {
			*pflCell = flDiff;
			pflCell++;
			*pflCell = flDiff;
			pflCell++;
			*pflCell = flAngle;
			pflCell++;
			*pflCell = flAngle;
		} else {
			if (flDiff < *pflCell) *pflCell = flDiff;
			pflCell++;
			if (flDiff > *pflCell) *pflCell = flDiff;
			pflCell++;
			if (flAngle < *pflCell) *pflCell = flAngle;
			pflCell++;
			if (flAngle > *pflCell) *pflCell = flAngle;
		}
		break;
	case MDTRA_SP_VARIANCE:
		*pflCell += flDiff;
		pflCell++;
		*pflCell += (flDiff*flDiff);
		pflCell++;
		*pflCell += flAngle;
		pflCell++;
		*pflCell += (flAngle*flAngle);
		break;
	default:
		break;
	}
}

static const MDTRA_ForceSearchData *pLocalForceSearchData = NULL;
extern MDTRA_ProgressDialog *pProgressDialog;
static bool s_threadStarted[MDTRA_MAX_THREADS];

static void fn_ForceSearch_SD( int threadnum, int num )
{
	//Load PDB files
	MDTRA_PDB_File *pPdbFile1;
	MDTRA_PDB_File *pPdbFile2;

	if ( (pLocalForceSearchData->workStart + num) == 0) {
		pPdbFile1 = pLocalForceSearchData->pStream1->pdb;
		pPdbFile2 = pLocalForceSearchData->pStream2->pdb;
	} else {
		pPdbFile1 = pLocalForceSearchData->tempPDB[0][threadnum];
		pPdbFile1->load( threadnum, pLocalForceSearchData->pStream1->format_identifier, pLocalForceSearchData->pStream1->files.at(pLocalForceSearchData->workStart + num).toAscii(), pLocalForceSearchData->pStream1->flags );
		pPdbFile2 = pLocalForceSearchData->tempPDB[1][threadnum];
		pPdbFile2->load( threadnum, pLocalForceSearchData->pStream2->format_identifier, pLocalForceSearchData->pStream2->files.at(pLocalForceSearchData->workStart + num).toAscii(), pLocalForceSearchData->pStream2->flags );
	}

	bool firstStep = false;
	if (!s_threadStarted[threadnum]) {
		s_threadStarted[threadnum] = true;
		firstStep = true;
	}

	//Calculate all cells
	for (int i = 0; i < pLocalForceSearchData->selectionSize; i++) {
		CalculateTableCell_SD( pPdbFile1, pPdbFile2, i, 
								pLocalForceSearchData->selectionData[0][i], 
								pLocalForceSearchData->selectionData[1][i], 
								pLocalForceSearchData->statParm,
								firstStep,
								pLocalForceSearchData->pResults[threadnum] );
	}

	if (pProgressDialog) {
		pProgressDialog->advanceCurrentFile( num+1 );
		if (pProgressDialog->checkInterrupt()) {
			InterruptThreads();
		}
	}
}

static void fn_ForceSearch_DD( int threadnum, int num )
{
	//Load PDB files
	MDTRA_PDB_File *pPdbFile1;
	MDTRA_PDB_File *pPdbFile2;

	if ( (pLocalForceSearchData->workStart + num) == 0) {
		pPdbFile1 = pLocalForceSearchData->pStream1->pdb;
		pPdbFile2 = pLocalForceSearchData->pStream2->pdb;
	} else {
		pPdbFile1 = pLocalForceSearchData->tempPDB[0][threadnum];
		pPdbFile1->load( threadnum, pLocalForceSearchData->pStream1->format_identifier, pLocalForceSearchData->pStream1->files.at(pLocalForceSearchData->workStart + num).toAscii(), pLocalForceSearchData->pStream1->flags );
		pPdbFile2 = pLocalForceSearchData->tempPDB[1][threadnum];
		pPdbFile2->load( threadnum, pLocalForceSearchData->pStream2->format_identifier, pLocalForceSearchData->pStream2->files.at(pLocalForceSearchData->workStart + num).toAscii(), pLocalForceSearchData->pStream2->flags );
	}

	bool firstStep = false;
	if (!s_threadStarted[threadnum]) {
		s_threadStarted[threadnum] = true;
		firstStep = true;
	}

	//Calculate all cells
	for (int i = 0; i < pLocalForceSearchData->selectionSize; i++) {
		CalculateTableCell_DD( pPdbFile1, pPdbFile2, i,
								pLocalForceSearchData->selectionData[0][i], 
								pLocalForceSearchData->selectionData[1][i], 
								pLocalForceSearchData->statParm, 
								firstStep,
								pLocalForceSearchData->pResults[threadnum] );
	}

	if (pProgressDialog) {
		pProgressDialog->advanceCurrentFile( num+1 );
		if (pProgressDialog->checkInterrupt()) {
			InterruptThreads();
		}
	}
}

static void fn_ForceSearchJoin_SD( int threadnum )
{
	//Join cells
	for (int i = 0; i < pLocalForceSearchData->selectionSize; i++) {
		float *pflSrcCell = pLocalForceSearchData->pResults[threadnum] + (i<<1);
		float *pflDstCell = pLocalForceSearchData->pResults[0] + (i<<1);
		if ( *pflSrcCell >= 0 || *pflDstCell >= 0 ) {
			switch (pLocalForceSearchData->statParm) {
				case MDTRA_SP_MIN_VALUE: if (*pflSrcCell < *pflDstCell) *pflDstCell = *pflSrcCell; break;
				case MDTRA_SP_MAX_VALUE: if (*pflSrcCell > *pflDstCell) *pflDstCell = *pflSrcCell; break;
				default: *pflDstCell += *pflSrcCell; break;
			}
		}
		pflSrcCell++; pflDstCell++;
		if ( *pflSrcCell >= 0 || *pflDstCell >= 0 ) {
			switch (pLocalForceSearchData->statParm) {
				case MDTRA_SP_MIN_VALUE: if (*pflSrcCell < *pflDstCell) *pflDstCell = *pflSrcCell; break;
				case MDTRA_SP_MAX_VALUE: if (*pflSrcCell > *pflDstCell) *pflDstCell = *pflSrcCell; break;
				default: *pflDstCell += *pflSrcCell; break;
			}
		}
	}
}

static void fn_ForceSearchJoin_DD( int threadnum )
{
	//Join cells
	for (int i = 0; i < pLocalForceSearchData->selectionSize; i++) {
		float *pflSrcCell = pLocalForceSearchData->pResults[threadnum] + (i<<2);
		float *pflDstCell = pLocalForceSearchData->pResults[0] + (i<<2);
		
		if (*pflSrcCell >= 0 || *pflDstCell >= 0 ) {
			switch (pLocalForceSearchData->statParm) {
			case MDTRA_SP_RANGE:
			case MDTRA_SP_MIDRANGE:
				if (*pflSrcCell < *pflDstCell) *pflDstCell = *pflSrcCell; 
				pflSrcCell++; pflDstCell++;
				if (*pflSrcCell > *pflDstCell) *pflDstCell = *pflSrcCell; 
				break;
			default: 
				*pflDstCell += *pflSrcCell;
				pflSrcCell++; pflDstCell++;
				*pflDstCell += *pflSrcCell; 
				break;
			}
		}
		pflSrcCell++; pflDstCell++;
		if (*pflSrcCell >= 0 || *pflDstCell >= 0 ) {
			switch (pLocalForceSearchData->statParm) {
			case MDTRA_SP_RANGE:
			case MDTRA_SP_MIDRANGE:
				if (*pflSrcCell < *pflDstCell) *pflDstCell = *pflSrcCell; 
				pflSrcCell++; pflDstCell++;
				if (*pflSrcCell > *pflDstCell) *pflDstCell = *pflSrcCell; 
				break;
			default: 
				*pflDstCell += *pflSrcCell;
				pflSrcCell++; pflDstCell++;
				*pflDstCell += *pflSrcCell; 
				break;
			}
		}
	}
}

static void fn_ForceSearchFinalize_SD( void )
{
	float flDataSize = pLocalForceSearchData->workCount;
	float flInvDataSize = 1.0f / flDataSize;

	//finalize cells
	switch (pLocalForceSearchData->statParm) {
	case MDTRA_SP_ARITHMETIC_MEAN:
		{
			for (int i = 0; i < pLocalForceSearchData->selectionSize; i++) {
				float *pflCell = pLocalForceSearchData->pResults[0] + (i<<1);
				if (*pflCell > 0) *pflCell *= flInvDataSize;
				pflCell++;
				if (*pflCell > 0) *pflCell *= flInvDataSize;
			}
		}
		break;
	case MDTRA_SP_GEOMETRIC_MEAN:
		{
			for (int i = 0; i < pLocalForceSearchData->selectionSize; i++) {
				float *pflCell = pLocalForceSearchData->pResults[0] + (i<<1);
				if (*pflCell > 0) *pflCell = pow( *pflCell, flInvDataSize );
				pflCell++;
				if (*pflCell > 0) *pflCell = pow( *pflCell, flInvDataSize );
			}
		}
		break;
	case MDTRA_SP_QUADRATIC_MEAN:
		{
			for (int i = 0; i < pLocalForceSearchData->selectionSize; i++) {
				float *pflCell = pLocalForceSearchData->pResults[0] + (i<<1);
				if (*pflCell > 0) *pflCell = sqrtf( *pflCell * flInvDataSize );
				pflCell++;
				if (*pflCell > 0) *pflCell = sqrtf( *pflCell * flInvDataSize );
			}
		}
		break;
	case MDTRA_SP_HARMONIC_MEAN:
		{
			for (int i = 0; i < pLocalForceSearchData->selectionSize; i++) {
				float *pflCell = pLocalForceSearchData->pResults[0] + (i<<1);
				if (*pflCell > 0) *pflCell = flDataSize * (1.0f / (*pflCell));
				pflCell++;
				if (*pflCell > 0) *pflCell = flDataSize * (1.0f / (*pflCell));
			}
		}
		break;
	default:
		break;
	}
}

static void fn_ForceSearchFinalize_DD( void )
{
	//finalize cells
	switch (pLocalForceSearchData->statParm) {
	case MDTRA_SP_RANGE:
		{
			for (int i = 0; i < pLocalForceSearchData->selectionSize; i++) {
				float *pflCell = pLocalForceSearchData->pResults[0] + (i<<2);
				if (*pflCell > 0) *pflCell = *(pflCell+1) - *pflCell;
				pflCell+=2;
				if (*pflCell > 0) *pflCell = *(pflCell+1) - *pflCell;
			}
		}
		break;
	case MDTRA_SP_MIDRANGE:
		{
			for (int i = 0; i < pLocalForceSearchData->selectionSize; i++) {
				float *pflCell = pLocalForceSearchData->pResults[0] + (i<<2);
				if (*pflCell > 0) *pflCell = (*(pflCell+1) - *pflCell) * 0.5f;
				pflCell+=2;
				if (*pflCell > 0) *pflCell = (*(pflCell+1) - *pflCell) * 0.5f;
			}
		}
		break;
	case MDTRA_SP_VARIANCE:
		{
			float flDataSize = pLocalForceSearchData->workCount;
			float flInvDataSize = 1.0f / flDataSize;
			float flVarianceDataSize = flDataSize / (flDataSize - 1.0f);

			for (int i = 0; i < pLocalForceSearchData->selectionSize; i++) {
				float *pflCell = pLocalForceSearchData->pResults[0] + (i<<2);
				if (*pflCell > 0) {
					float q = *(pflCell+1) * flInvDataSize;
					float m = *pflCell * flInvDataSize;
					*pflCell = (flVarianceDataSize * q) - (m * m);
				}
				pflCell+=2;
				if (*pflCell > 0) {
					float q = *(pflCell+1) * flInvDataSize;
					float m = *pflCell * flInvDataSize;
					*pflCell = (flVarianceDataSize * q) - (m * m);
				}
			}
		}
		break;
	default:
		break;
	}
}

//=================================================
MDTRA_ForceSearchData s_lfsd;
static MDTRA_MainWindow *s_pMainWindow;
static float s_flDiff, s_flDot;
static int s_bufferDim;
QVector<MDTRA_ForceSearchAtom> s_SignificantAtoms;

static void ForceSearchCheckAgainstCriterion( int bufferDim, float flDiffCriterion, float flDotCriterion )
{
	for (int i = 0; i < pLocalForceSearchData->selectionSize; i++) {
		float *pflCell1;
		float *pflCell2;

		if (bufferDim == 1) {
			pflCell1 = s_lfsd.pResults[0] + (i<<1);
			pflCell2 = s_lfsd.pResults[0] + (i<<1) + 1;
		} else {
			pflCell1 = s_lfsd.pResults[0] + (i<<2);
			pflCell2 = s_lfsd.pResults[0] + (i<<2) + 1;
		}

		float flDiff = (*pflCell1);
		float flAngle = (*pflCell2)*(180.0f / M_PI_F);

		if (flDiff < flDiffCriterion && flAngle < flDotCriterion )
			continue;
	
		//found significant atom
		MDTRA_ForceSearchAtom p;
		p.atom[0] = s_lfsd.selectionData[0][i];
		p.atom[1] = s_lfsd.selectionData[1][i];
		p.flDiff = flDiff;
		p.flDot = flAngle;
		s_SignificantAtoms << p;
	}

	//Sort pairs by diff
	struct qsigatpairSortFunctor {
		static bool sortFunc(const MDTRA_ForceSearchAtom &t1, const MDTRA_ForceSearchAtom &t2) {
			return (t1.flDiff > t2.flDiff);
		}
	};
	if (s_SignificantAtoms.count() > 1)
		qSort(s_SignificantAtoms.begin(), s_SignificantAtoms.end(), &(qsigatpairSortFunctor::sortFunc));
}

bool SetupForceSearch( MDTRA_MainWindow *pMainWindow, const MDTRA_ForceSearchInfo *pSearchInfo )
{
	s_pMainWindow = pMainWindow;
	assert( s_pMainWindow != NULL );

	if ( pSearchInfo->streamInfo[0].size != pSearchInfo->streamInfo[1].size ) {
		//sanity check
		return false;
	}

	s_SignificantAtoms.clear();
	memset( &s_lfsd, 0, sizeof(MDTRA_ForceSearchData) );
	s_flDiff = pSearchInfo->statDiff;
	s_flDot = pSearchInfo->statDot;
	s_bufferDim = pSearchInfo->bufferDim;

	s_lfsd.workStart = pSearchInfo->trajectoryMin - 1;
	s_lfsd.workCount = pSearchInfo->trajectoryMax - pSearchInfo->trajectoryMin + 1;

	s_lfsd.selectionSize = pSearchInfo->streamInfo[0].size;
	s_lfsd.selectionData[0] = pSearchInfo->streamInfo[0].data;
	s_lfsd.selectionData[1] = pSearchInfo->streamInfo[1].data;

	s_lfsd.statParm = pSearchInfo->statCriterion;

	s_lfsd.pStream1 = s_pMainWindow->getProject()->fetchStreamByIndex( pSearchInfo->streamInfo[0].index );
	if (!s_lfsd.pStream1)
		return false;
	s_lfsd.pStream2 = s_pMainWindow->getProject()->fetchStreamByIndex( pSearchInfo->streamInfo[1].index );
	if (!s_lfsd.pStream2)
		return false;

	for (int j = 0; j < CountThreads(); j++) {
		s_lfsd.pResults[j] = new float[s_lfsd.selectionSize*s_bufferDim*2];
		if (!s_lfsd.pResults[j])
			return false;

		memset( s_lfsd.pResults[j], 0, sizeof(float)*(s_lfsd.selectionSize*s_bufferDim*2) );

		s_lfsd.tempPDB[0][j] = new MDTRA_PDB_File;
		if (!s_lfsd.tempPDB[0][j])
			return false;
		s_lfsd.tempPDB[1][j] = new MDTRA_PDB_File;
		if (!s_lfsd.tempPDB[1][j])
			return false;
	}

	return true;
}

bool PerformForceSearch( void )
{
	MDTRA_ProgressDialog dlgProgress( s_pMainWindow );
	dlgProgress.setStreamCount( 1 );
	dlgProgress.show();

	pProgressDialog = &dlgProgress;

	pLocalForceSearchData = &s_lfsd;

	dlgProgress.setFileCount( pLocalForceSearchData->workCount );
	dlgProgress.setCurrentStream( 0 );
	dlgProgress.setCurrentFile( 0 );

	memset( s_threadStarted, 0, sizeof(bool) * CountThreads() );
	
	if (s_bufferDim == 1) {
		RunThreadsOnIndividual( pLocalForceSearchData->workCount, fn_ForceSearch_SD );
		for (int j = 1; j < CountThreads(); j++) fn_ForceSearchJoin_SD( j );
		fn_ForceSearchFinalize_SD();
	} else {
		RunThreadsOnIndividual( pLocalForceSearchData->workCount, fn_ForceSearch_DD );
		for (int j = 1; j < CountThreads(); j++) fn_ForceSearchJoin_DD( j );
		fn_ForceSearchFinalize_DD();
	}

	dlgProgress.setProgressAtMax();

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QApplication::processEvents();

	//now check against criterion and add to result list
	ForceSearchCheckAgainstCriterion( s_bufferDim, s_flDiff, s_flDot );

	QApplication::restoreOverrideCursor();
	dlgProgress.hide();
	QApplication::processEvents();

	if (!s_SignificantAtoms.count()) {
		QMessageBox::warning(s_pMainWindow, "Force Search Results", "No atoms with significant force difference were found!\nPlease specify another atom set or different significance criterion.");
		return false;
	}

	return true;
}

void FreeForceSearch( void )
{
	for (int j = 0; j < CountThreads(); j++) {
		if (s_lfsd.pResults[j]) {
			delete [] s_lfsd.pResults[j];
			s_lfsd.pResults[j] = NULL;
		}
		if (s_lfsd.tempPDB[0][j]) {
			delete s_lfsd.tempPDB[0][j];
			s_lfsd.tempPDB[0][j] = NULL;
		}
		if (s_lfsd.tempPDB[1][j]) {
			delete s_lfsd.tempPDB[1][j];
			s_lfsd.tempPDB[1][j] = NULL;
		}
	}

	s_pMainWindow = NULL;
	s_SignificantAtoms.clear();
}