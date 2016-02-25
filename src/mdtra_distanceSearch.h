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
#ifndef MDTRA_DISTANCESEARCH_H
#define MDTRA_DISTANCESEARCH_H

typedef struct stMDTRA_DistanceSearchStreamInfo {
	int		index;
	int		size;
	int*	data;
} MDTRA_DistanceSearchStreamInfo;

typedef struct stMDTRA_DistanceSearchInfo {
	MDTRA_DistanceSearchStreamInfo streamInfo[2];
	int	trajectoryMin;
	int	trajectoryMax;
	int bufferDim;
	enum eMDTRA_StatParm statCriterion;
	bool ignoreSameResidue;
	float statMin;
	float statMax;
	float statReference;
} MDTRA_DistanceSearchInfo;

typedef struct stMDTRA_DistanceSearchData
{
	int						workStart;
	int						workCount;
	const MDTRA_Stream*		pStream;
	bool					ignoreSameResidue;
	int						selectionSize;
	const int*				selectionData;
	MDTRA_PDB_File*			tempPDB[MDTRA_MAX_THREADS];
	MDTRA_StatParm			statParm;
	float*					pResults[MDTRA_MAX_THREADS];
} MDTRA_DistanceSearchData;

typedef struct stMDTRA_DistanceSearchPair
{
	int						atom1[2];
	int						atom2[2];
	float					value1;
	float					value2;
	float					diff;
} MDTRA_DistanceSearchPair;

class MDTRA_MainWindow;

extern bool SetupDistanceSearch( MDTRA_MainWindow *pMainWindow, const MDTRA_DistanceSearchInfo *pSearchInfo );
extern bool PerformDistanceSearch( void );
extern void FreeDistanceSearch( void );

#endif //MDTRA_DISTANCESEARCH_H