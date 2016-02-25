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
#ifndef MDTRA_FORCESEARCH_H
#define MDTRA_FORCESEARCH_H

typedef struct stMDTRA_ForceSearchStreamInfo {
	int		index;
	int		size;
	int*	data;
} MDTRA_ForceSearchStreamInfo;

typedef struct stMDTRA_ForceSearchInfo {
	MDTRA_ForceSearchStreamInfo streamInfo[2];
	int	trajectoryMin;
	int	trajectoryMax;
	int bufferDim;
	enum eMDTRA_StatParm statCriterion;
	float statDiff;
	float statDot;
} MDTRA_ForceSearchInfo;

typedef struct stMDTRA_ForceSearchData
{
	int						workStart;
	int						workCount;
	const MDTRA_Stream*		pStream1;
	const MDTRA_Stream*		pStream2;
	int						selectionSize;
	const int*				selectionData[2];
	MDTRA_PDB_File*			tempPDB[2][MDTRA_MAX_THREADS];
	MDTRA_StatParm			statParm;
	float*					pResults[MDTRA_MAX_THREADS];
} MDTRA_ForceSearchData;

typedef struct stMDTRA_ForceSearchAtom
{
	int						atom[2];
	float					flDiff;
	float					flDot;
} MDTRA_ForceSearchAtom;

class MDTRA_MainWindow;

extern bool SetupForceSearch( MDTRA_MainWindow *pMainWindow, const MDTRA_ForceSearchInfo *pSearchInfo );
extern bool PerformForceSearch( void );
extern void FreeForceSearch( void );

#endif //MDTRA_FORCESEARCH_H