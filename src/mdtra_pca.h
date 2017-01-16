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
#ifndef MDTRA_PCA_H
#define MDTRA_PCA_H

typedef struct stMDTRA_PCAInfo {
	int		streamIndex;
	int		trajectoryMin;
	int		trajectoryMax;
	int		selectionSize;
	int*	selectionData;
	int		numDisplayPC;
} MDTRA_PCAInfo;

typedef struct stMDTRA_PCAData
{
	int						workStart;
	int						workCount;
	int						selectionSize;
	int*					selectionData;
	const MDTRA_Stream*		pStream;	
	MDTRA_PDB_File*			tempPDB[MDTRA_MAX_THREADS];
} MDTRA_PCAData;

extern bool SetupPCA( MDTRA_MainWindow *pMainWindow, const MDTRA_PCAInfo *pInfo, size_t *pOutOfMemSize );
extern bool PerformPCA( void );
extern void FreePCA( void );

#endif //MDTRA_PCA_H