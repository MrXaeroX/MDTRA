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
#ifndef MDTRA_TORSIONSEARCH_H
#define MDTRA_TORSIONSEARCH_H

#define TADF_ATOM1_FROM_PREV_RESIDUE		(1<<0)
#define TADF_ATOM3_FROM_NEXT_RESIDUE		(1<<1)
#define TADF_ATOM4_FROM_NEXT_RESIDUE		(1<<2)

typedef struct stMDTRA_TorsionAngleDesc
{
	const char* AngleTitle;
	const char*	ResidueTitle;
	const char*	AtomTitle1;
	const char*	AtomTitle2;
	const char*	AtomTitle3;
	const char*	AtomTitle4;
	const char*	AtomTitle4Alt;
	unsigned int Flags;
} MDTRA_TorsionAngleDesc;

typedef struct stMDTRA_TorsionSearchStreamInfo {
	int		index;
	int		size;
	int*	data;
} MDTRA_TorsionSearchStreamInfo;

typedef struct stMDTRA_TorsionSearchInfo {
	MDTRA_TorsionSearchStreamInfo streamInfo[2];
	int	trajectoryMin;
	int	trajectoryMax;
	int bufferDim;
	enum eMDTRA_StatParm statCriterion;
	float statReference;
} MDTRA_TorsionSearchInfo;

typedef struct stMDTRA_TorsionSearchData
{
	int						workStart;
	int						workCount;
	const MDTRA_Stream*		pStream;
	int						selectionSize;
	const int*				selectionData;
	MDTRA_PDB_File*			tempPDB[MDTRA_MAX_THREADS];
	MDTRA_StatParm			statParm;
	float*					pResults[MDTRA_MAX_THREADS];
} MDTRA_TorsionSearchData;

#define TSDF_VALID			(1<<0)

typedef struct stMDTRA_TorsionSearchDef
{
	int						atom1[2];
	int						atom2[2];
	int						atom3[2];
	int						atom4[2];
	int						atom4alt[2];
	int						flags;
	float					value1;
	float					value2;
	float					diff;
	const char*				title;
} MDTRA_TorsionSearchDef;

class MDTRA_MainWindow;

extern bool SetupTorsionSearch( MDTRA_MainWindow *pMainWindow, const MDTRA_TorsionSearchInfo *pSearchInfo );
extern bool PerformTorsionSearch( void );
extern void FreeTorsionSearch( void );

#endif //MDTRA_TORSIONSEARCH_H