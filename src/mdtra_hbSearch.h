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
#ifndef MDTRA_HBSEARCH_H
#define MDTRA_HBSEARCH_H

#define HB_STAT_PARMS

//#define XY_CUTOFF
#define XY_CUTOFF_DIST		32.0f
#define HB_CUTOFF_DIST		3.5f
#define HB_SIGMA2			0.018f

#define HB_SQR(x)			((x)*(x))
#define HB_QUAD(x)			(HB_SQR(x)*HB_SQR(x))
#define HB_SIXTH(x)			(HB_QUAD(x)*HB_SQR(x))
#define HB_TWELFTH(x)		(HB_SIXTH(x)*HB_SIXTH(x))
#define XY_CUTOFF_DIST_SQ	(HB_SQR(XY_CUTOFF_DIST))
#define HB_CUTOFF_DIST_SQ	(HB_SQR(HB_CUTOFF_DIST))

//XHY-triplets are defined as follows:
//X = proton donor
//H = hydrogen atom bound to donor
//Y = proton acceptor

#define MAX_GROUPED_HYDROGENS	3
#define MAX_GROUPED_ACCEPTORS	2

#define DF_NTERM	(1<<0)
#define DF_HGROUP	(1<<1)

typedef struct stMDTRA_HBTripletDonorInfo
{
	char	*XResidue;
	char	*XTitle;
	char	*HTitle;
	int		ffCode;
	int		flags;
} MDTRA_HBTripletDonorInfo;

typedef struct stMDTRA_HBTripletAcceptorInfo
{
	char	*YResidue;
	char	*YTitle;
	int		ffCode;
	int		groupIndex;
} MDTRA_HBTripletAcceptorInfo;

typedef struct stMDTRA_HBTripletParms
{
	float Rmin;
	float Em;
	float A12;
	float B6;
} MDTRA_HBTripletParms;

typedef struct stMDTRA_HBSearchInfo {
	int		streamIndex;
	int		trajectoryMin;
	int		trajectoryMax;
	int		minPercent;
	float	minEnergy;
	bool	grouping;
} MDTRA_HBSearchInfo;

typedef struct stMDTRA_HBSearchData
{
	int						workStart;
	int						workCount;
	int						minCount;
	float					minEnergy;
	bool					grouping;
	const MDTRA_Stream*		pStream;	
	MDTRA_PDB_File*			tempPDB[MDTRA_MAX_THREADS];
} MDTRA_HBSearchData;

#define TF_VALID			(1<<0)

typedef struct stMDTRA_HBSearchTriplet
{
public:
	stMDTRA_HBSearchTriplet() {};
	~stMDTRA_HBSearchTriplet() {};

	int						atX;						//X atom serial
	int						atH[MAX_GROUPED_HYDROGENS];	//H atom serial(s)
	int						atY[MAX_GROUPED_ACCEPTORS];	//Y atom serial(s)
	short					flags;						//Triplet flags
	short					groupIndex;					//Y atom groupindex
	short					xff;						//X ff code
	short					yff;						//Y ff code
} MDTRA_HBSearchTriplet;

class MDTRA_MainWindow;

extern float HBCalcPair( const MDTRA_PDB_File *ppdb, const MDTRA_PDB_Atom *pDonor, const MDTRA_PDB_Atom *pAcceptor );
extern bool SetupHBSearch( MDTRA_MainWindow *pMainWindow, const MDTRA_HBSearchInfo *pSearchInfo );
extern bool PerformHBSearch( void );
extern void FreeHBSearch( void );
extern void FreeHBSearchOnExit( void );
extern void HBInitConfigData( void );
extern void HBFreeConfigData( void );
extern int HBGetFFCode( const char *ffname );

extern MDTRA_HBTripletParms **g_TripletParms;

#endif //MDTRA_HBSEARCH_H