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
//	Distance search algorithm

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb.h"
#include "mdtra_progressDialog.h"
#include "mdtra_distanceSearch.h"

#include <QtGui/QMessageBox>

static inline int TableCellIndex_SD( int at1, int at2 )
{
	assert( at1 < at2 );
	return ((at2*(at2-1))>>1) + at1;
}

static inline int TableCellIndex_DD( int at1, int at2 )
{
	assert( at1 < at2 );
	return (at2*(at2-1)) + (at1<<1);
}

static inline int TableCellSize_SD( int atnum )
{
	return (atnum*(atnum-1))>>1;
}

static inline int TableCellSize_DD( int atnum )
{
	return (atnum*(atnum-1));
}

static inline void CalculateTableCell_SD( const MDTRA_PDB_File *pdb, int i1, int at1, int i2, int at2, MDTRA_StatParm parm, bool firstStep, float *pflCells )
{
	float flResultData = pdb->get_selection_pair_distance( at1, at2 );

	float *pflCell = pflCells + TableCellIndex_SD( i1, i2 );
/*
#ifdef _DEBUG
	OutputDebugString( QString("CalculateTableCell: cell = %1 (%2, %3)\n").arg(TableCellIndex( i1, i2 )).arg(i1).arg(i2).toAscii() );
#endif
*/
	switch (parm) {
	case MDTRA_SP_ARITHMETIC_MEAN:
		*pflCell += flResultData;
		break;
	case MDTRA_SP_HARMONIC_MEAN:
		*pflCell += (1.0f / flResultData);
		break;
	case MDTRA_SP_QUADRATIC_MEAN:
		*pflCell += (flResultData*flResultData);
		break;
	case MDTRA_SP_GEOMETRIC_MEAN:
		if (firstStep) *pflCell = flResultData;
		else *pflCell *= flResultData;
		break;
	case MDTRA_SP_MIN_VALUE:
		if (firstStep) *pflCell = flResultData;
		else if (flResultData < *pflCell) *pflCell = flResultData;
		break;
	case MDTRA_SP_MAX_VALUE:
		if (firstStep) *pflCell = flResultData;
		else if (flResultData > *pflCell) *pflCell = flResultData;
		break;
	default:
		break;
	}
}

static inline void CalculateTableCell_DD( const MDTRA_PDB_File *pdb, int i1, int at1, int i2, int at2, MDTRA_StatParm parm, bool firstStep, float *pflCells )
{
	float flResultData = pdb->get_selection_pair_distance( at1, at2 );

	float *pflCell = pflCells + TableCellIndex_DD( i1, i2 );
/*
#ifdef _DEBUG
	OutputDebugString( QString("CalculateTableCell: cell = %1 (%2, %3)\n").arg(TableCellIndex( i1, i2 )).arg(i1).arg(i2).toAscii() );
#endif
*/
	switch (parm) {
	case MDTRA_SP_RANGE:
	case MDTRA_SP_MIDRANGE:
		if (firstStep) *pflCell = flResultData;
		else if (flResultData < *pflCell) *pflCell = flResultData;
		pflCell++;
		if (firstStep) *pflCell = flResultData;
		else if (flResultData > *pflCell) *pflCell = flResultData;
		break;
	case MDTRA_SP_VARIANCE:
		*pflCell += flResultData;
		pflCell++;
		*pflCell += (flResultData*flResultData);
		break;
	default:
		break;
	}
}

static const MDTRA_DistanceSearchData *pLocalDistanceSearchData = NULL;
extern MDTRA_ProgressDialog *pProgressDialog;
static bool s_threadStarted[MDTRA_MAX_THREADS];

static void fn_DistanceSearch_SD( int threadnum, int num )
{
	//Load PDB file
	MDTRA_PDB_File *pPdbFile;
	if ( (pLocalDistanceSearchData->workStart + num) == 0) {
		pPdbFile = pLocalDistanceSearchData->pStream->pdb;
	} else {
		pPdbFile = pLocalDistanceSearchData->tempPDB[threadnum];
		pPdbFile->load( threadnum, pLocalDistanceSearchData->pStream->format_identifier, pLocalDistanceSearchData->pStream->files.at(pLocalDistanceSearchData->workStart + num).toAscii(), pLocalDistanceSearchData->pStream->flags );
	}

	bool firstStep = false;
	if (!s_threadStarted[threadnum]) {
		s_threadStarted[threadnum] = true;
		firstStep = true;
	}

	//Calculate all cells
	for (int i = 1; i < pLocalDistanceSearchData->selectionSize; i++) {
		for (int j = 0; j < i; j++) {
			if (pLocalDistanceSearchData->ignoreSameResidue) {
				if (pPdbFile->is_selection_pair_of_same_residue( pLocalDistanceSearchData->selectionData[j], pLocalDistanceSearchData->selectionData[i] )) {
					pLocalDistanceSearchData->pResults[threadnum][TableCellIndex_SD( j, i )] = -1.0f;
					continue;
				}
			}
			CalculateTableCell_SD( pPdbFile, 
								j,
								pLocalDistanceSearchData->selectionData[j], 
								i,
								pLocalDistanceSearchData->selectionData[i], 
								pLocalDistanceSearchData->statParm, 
								firstStep,
								pLocalDistanceSearchData->pResults[threadnum] );
		}
	}

	if (pProgressDialog) {
		pProgressDialog->advanceCurrentFile( num+1 );
		if (pProgressDialog->checkInterrupt()) {
			InterruptThreads();
		}
	}
}

static void fn_DistanceSearch_DD( int threadnum, int num )
{
	//Load PDB file
	MDTRA_PDB_File *pPdbFile;
	if ( (pLocalDistanceSearchData->workStart + num) == 0) {
		pPdbFile = pLocalDistanceSearchData->pStream->pdb;
	} else {
		pPdbFile = pLocalDistanceSearchData->tempPDB[threadnum];
		pPdbFile->load( threadnum, pLocalDistanceSearchData->pStream->format_identifier, pLocalDistanceSearchData->pStream->files.at(pLocalDistanceSearchData->workStart + num).toAscii(), pLocalDistanceSearchData->pStream->flags );
	}

	bool firstStep = false;
	if (!s_threadStarted[threadnum]) {
		s_threadStarted[threadnum] = true;
		firstStep = true;
	}

	//Calculate all cells
	for (int i = 1; i < pLocalDistanceSearchData->selectionSize; i++) {
		for (int j = 0; j < i; j++) {
			if (pLocalDistanceSearchData->ignoreSameResidue) {
				if (pPdbFile->is_selection_pair_of_same_residue( pLocalDistanceSearchData->selectionData[j], pLocalDistanceSearchData->selectionData[i] )) {
					pLocalDistanceSearchData->pResults[threadnum][TableCellIndex_DD( j, i )] = -1.0f;
					continue;
				}
			}
			CalculateTableCell_DD( pPdbFile, 
								j,
								pLocalDistanceSearchData->selectionData[j], 
								i,
								pLocalDistanceSearchData->selectionData[i], 
								pLocalDistanceSearchData->statParm, 
								firstStep,
								pLocalDistanceSearchData->pResults[threadnum] );
		}
	}

	if (pProgressDialog) {
		pProgressDialog->advanceCurrentFile( num+1 );
		if (pProgressDialog->checkInterrupt()) {
			InterruptThreads();
		}
	}
}

static void fn_DistanceSearchJoin_SD( int threadnum )
{
	//Join cells
	for (int i = 1; i < pLocalDistanceSearchData->selectionSize; i++) {
		for (int j = 0; j < i; j++) {
			float *pflSrcCell = pLocalDistanceSearchData->pResults[threadnum] + TableCellIndex_SD( j, i );
			float *pflDstCell = pLocalDistanceSearchData->pResults[0] + TableCellIndex_SD( j, i );
			if (*pflSrcCell < 0 || *pflDstCell < 0 )
				continue;
			switch (pLocalDistanceSearchData->statParm) {
			case MDTRA_SP_MIN_VALUE: if (*pflSrcCell < *pflDstCell) *pflDstCell = *pflSrcCell; break;
			case MDTRA_SP_MAX_VALUE: if (*pflSrcCell > *pflDstCell) *pflDstCell = *pflSrcCell; break;
			default: *pflDstCell += *pflSrcCell; break;
			}
		}
	}
}

static void fn_DistanceSearchJoin_DD( int threadnum )
{
	//Join cells
	for (int i = 1; i < pLocalDistanceSearchData->selectionSize; i++) {
		for (int j = 0; j < i; j++) {
			float *pflSrcCell = pLocalDistanceSearchData->pResults[threadnum] + TableCellIndex_DD( j, i );
			float *pflDstCell = pLocalDistanceSearchData->pResults[0] + TableCellIndex_DD( j, i );
			if (*pflSrcCell < 0 || *pflDstCell < 0 )
				continue;

			switch (pLocalDistanceSearchData->statParm) {
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

static void fn_DistanceSearchFinalize_SD( void )
{
	float flDataSize = pLocalDistanceSearchData->workCount;
	float flInvDataSize = 1.0f / flDataSize;

	//finalize cells
	switch (pLocalDistanceSearchData->statParm) {
	case MDTRA_SP_ARITHMETIC_MEAN:
		{
			for (int i = 1; i < pLocalDistanceSearchData->selectionSize; i++) {
				for (int j = 0; j < i; j++) {
					float *pflCell = pLocalDistanceSearchData->pResults[0] + TableCellIndex_SD( j, i );
					if (*pflCell > 0) *pflCell *= flInvDataSize;
				}
			}
		}
		break;
	case MDTRA_SP_GEOMETRIC_MEAN:
		{
			for (int i = 1; i < pLocalDistanceSearchData->selectionSize; i++) {
				for (int j = 0; j < i; j++) {
					float *pflCell = pLocalDistanceSearchData->pResults[0] + TableCellIndex_SD( j, i );
					if (*pflCell > 0) *pflCell = pow( *pflCell, flInvDataSize );
				}
			}
		}
		break;
	case MDTRA_SP_QUADRATIC_MEAN:
		{
			for (int i = 1; i < pLocalDistanceSearchData->selectionSize; i++) {
				for (int j = 0; j < i; j++) {
					float *pflCell = pLocalDistanceSearchData->pResults[0] + TableCellIndex_SD( j, i );
					if (*pflCell > 0) *pflCell = sqrtf( *pflCell * flInvDataSize );
				}
			}
		}
		break;
	case MDTRA_SP_HARMONIC_MEAN:
		{
			for (int i = 1; i < pLocalDistanceSearchData->selectionSize; i++) {
				for (int j = 0; j < i; j++) {
					float *pflCell = pLocalDistanceSearchData->pResults[0] + TableCellIndex_SD( j, i );
					if (*pflCell > 0) *pflCell = flDataSize * (1.0f / (*pflCell));
				}
			}
		}
		break;
	default:
		break;
	}
}

static void fn_DistanceSearchFinalize_DD( void )
{
	//finalize cells
	switch (pLocalDistanceSearchData->statParm) {
	case MDTRA_SP_RANGE:
		{
			for (int i = 1; i < pLocalDistanceSearchData->selectionSize; i++) {
				for (int j = 0; j < i; j++) {
					float *pflCell = pLocalDistanceSearchData->pResults[0] + TableCellIndex_DD( j, i );
					if (*pflCell > 0) *pflCell = *(pflCell+1) - *pflCell;
				}
			}
		}
		break;
	case MDTRA_SP_MIDRANGE:
		{
			for (int i = 1; i < pLocalDistanceSearchData->selectionSize; i++) {
				for (int j = 0; j < i; j++) {
					float *pflCell = pLocalDistanceSearchData->pResults[0] + TableCellIndex_DD( j, i );
					if (*pflCell > 0) *pflCell = (*(pflCell+1) - *pflCell) * 0.5f;
				}
			}
		}
		break;
	case MDTRA_SP_VARIANCE:
		{
			float flDataSize = pLocalDistanceSearchData->workCount;
			float flInvDataSize = 1.0f / flDataSize;
			float flVarianceDataSize = flDataSize / (flDataSize - 1.0f);

			for (int i = 1; i < pLocalDistanceSearchData->selectionSize; i++) {
				for (int j = 0; j < i; j++) {
					float *pflCell = pLocalDistanceSearchData->pResults[0] + TableCellIndex_DD( j, i );
					if (*pflCell > 0) {
						float q = *(pflCell+1) * flInvDataSize;
						float m = *pflCell * flInvDataSize;
						*pflCell = (flVarianceDataSize * q) - (m * m);
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

//=================================================
MDTRA_DistanceSearchData s_ldsd[2];
static MDTRA_MainWindow *s_pMainWindow;
static float s_flMin, s_flMax, s_flReference;
static int s_bufferDim;
QVector<MDTRA_DistanceSearchPair> s_SignificantPairs;

static void DistanceSearchCheckAgainstCriterion( int bufferDim, float flMin, float flMax, float flCriterion )
{
	for (int i = 1; i < pLocalDistanceSearchData->selectionSize; i++) {
		for (int j = 0; j < i; j++) {
			float *pflCell1;
			float *pflCell2;

			if (bufferDim == 1) {
				pflCell1 = s_ldsd[0].pResults[0] + TableCellIndex_SD( j, i );
				pflCell2 = s_ldsd[1].pResults[0] + TableCellIndex_SD( j, i );
			} else {
				pflCell1 = s_ldsd[0].pResults[0] + TableCellIndex_DD( j, i );
				pflCell2 = s_ldsd[1].pResults[0] + TableCellIndex_DD( j, i );
			}

			if ((*pflCell1) < 0) continue;
			if ((*pflCell2) < 0) continue;

			if (((*pflCell1) < flMin) && ((*pflCell2) < flMin))
				continue;
			if (((*pflCell1) > flMax) && ((*pflCell2) > flMax))
				continue;

			float flStreamDiff = fabsf( (*pflCell1) - (*pflCell2) );
			if (flStreamDiff < flCriterion)
				continue;

			//found significant pair
			MDTRA_DistanceSearchPair p;
			p.atom1[0] = s_ldsd[0].selectionData[j];
			p.atom2[0] = s_ldsd[0].selectionData[i];
			p.atom1[1] = s_ldsd[1].selectionData[j];
			p.atom2[1] = s_ldsd[1].selectionData[i];
			p.value1 = (*pflCell1);
			p.value2 = (*pflCell2);
			p.diff = flStreamDiff;
			s_SignificantPairs << p;
		}
	}

	//Sort pairs by diff
	struct qsigatpairSortFunctor {
		static bool sortFunc(const MDTRA_DistanceSearchPair &t1, const MDTRA_DistanceSearchPair &t2) {
			return (t1.diff > t2.diff);
		}
	};
	if (s_SignificantPairs.count() > 1)
		qSort(s_SignificantPairs.begin(), s_SignificantPairs.end(), &(qsigatpairSortFunctor::sortFunc));
}

bool SetupDistanceSearch( MDTRA_MainWindow *pMainWindow, const MDTRA_DistanceSearchInfo *pSearchInfo )
{
	s_pMainWindow = pMainWindow;
	assert( s_pMainWindow != NULL );

	s_SignificantPairs.clear();
	memset( s_ldsd, 0, sizeof(MDTRA_DistanceSearchData)*2 );
	s_flMin = pSearchInfo->statMin;
	s_flMax = pSearchInfo->statMax;
	s_flReference = pSearchInfo->statReference;
	s_bufferDim = pSearchInfo->bufferDim;

	for (int i = 0; i < 2; i++) {
		s_ldsd[i].workStart = pSearchInfo->trajectoryMin - 1;
		s_ldsd[i].workCount = pSearchInfo->trajectoryMax - pSearchInfo->trajectoryMin + 1;
		s_ldsd[i].selectionSize = pSearchInfo->streamInfo[i].size;
		s_ldsd[i].selectionData = pSearchInfo->streamInfo[i].data;
		s_ldsd[i].statParm = pSearchInfo->statCriterion;
		s_ldsd[i].ignoreSameResidue = pSearchInfo->ignoreSameResidue;
		s_ldsd[i].pStream = s_pMainWindow->getProject()->fetchStreamByIndex( pSearchInfo->streamInfo[i].index );
		if (!s_ldsd[i].pStream)
			return false;
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < CountThreads(); j++) {
			if (s_bufferDim == 1) s_ldsd[i].pResults[j] = new float[TableCellSize_SD(s_ldsd[i].selectionSize)];
			else s_ldsd[i].pResults[j] = new float[TableCellSize_DD(s_ldsd[i].selectionSize)];
			if (!s_ldsd[i].pResults[j])
				return false;

			if (s_bufferDim == 1) memset( s_ldsd[i].pResults[j], 0, sizeof(float)*TableCellSize_SD(s_ldsd[i].selectionSize) );
			else memset( s_ldsd[i].pResults[j], 0, sizeof(float)*TableCellSize_DD(s_ldsd[i].selectionSize) );

			s_ldsd[i].tempPDB[j] = new MDTRA_PDB_File;
			if (!s_ldsd[i].tempPDB[j])
				return false;
		}
	}

	return true;
}

bool PerformDistanceSearch( void )
{
	MDTRA_ProgressDialog dlgProgress( s_pMainWindow );
	dlgProgress.setStreamCount( 2 );
	dlgProgress.show();

	pProgressDialog = &dlgProgress;

	for (int i = 0; i < 2; i++) {
		pLocalDistanceSearchData = &s_ldsd[i];

		dlgProgress.setFileCount( pLocalDistanceSearchData->workCount );
		dlgProgress.setCurrentStream( i );
		dlgProgress.setCurrentFile( 0 );

		memset( s_threadStarted, 0, sizeof(bool) * CountThreads() );

		if (s_bufferDim == 1) {
			RunThreadsOnIndividual( pLocalDistanceSearchData->workCount, fn_DistanceSearch_SD );
			for (int j = 1; j < CountThreads(); j++) fn_DistanceSearchJoin_SD( j );
			fn_DistanceSearchFinalize_SD();
		} else {
			RunThreadsOnIndividual( pLocalDistanceSearchData->workCount, fn_DistanceSearch_DD );
			for (int j = 1; j < CountThreads(); j++) fn_DistanceSearchJoin_DD( j );
			fn_DistanceSearchFinalize_DD();
		}

		if (dlgProgress.checkInterrupt())
			return false;
	}

	dlgProgress.setProgressAtMax();

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QApplication::processEvents();

	//now check against criterion and add to result list
	DistanceSearchCheckAgainstCriterion( s_bufferDim, s_flMin, s_flMax, s_flReference );

	QApplication::restoreOverrideCursor();
	dlgProgress.hide();
	QApplication::processEvents();

	if (!s_SignificantPairs.count()) {
		QMessageBox::warning(s_pMainWindow, "Distance Search Results", "No significant pairs were found!\nPlease specify another atom set or different significance criterion.");
		return false;
	}

	return true;
}

void FreeDistanceSearch( void )
{
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < CountThreads(); j++) {
			if (s_ldsd[i].pResults[j]) {
				delete [] s_ldsd[i].pResults[j];
				s_ldsd[i].pResults[j] = NULL;
			}
			if (s_ldsd[i].tempPDB[j]) {
				delete s_ldsd[i].tempPDB[j];
				s_ldsd[i].tempPDB[j] = NULL;
			}
		}
	}

	s_pMainWindow = NULL;
	s_SignificantPairs.clear();
}