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
//	H-Bonds search algorithm

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_math.h"
#include "mdtra_project.h"
#include "mdtra_pdb.h"
#include "mdtra_utils.h"
#include "mdtra_configFile.h"
#include "mdtra_progressDialog.h"
#include "mdtra_waitDialog.h"
#include "mdtra_hbSearch.h"

#include <QtGui/QMessageBox>

#define DF_NTERM	(1<<0)
#define DF_HGROUP	(1<<1)

static MDTRA_MainWindow *s_pMainWindow;
MDTRA_HBSearchData s_lhbsd;
int s_iHBBufferSize = 0;
int s_iHBRealSize = 0;
int s_iHBTotal;
float *s_flHBEnergy;
float *s_flHBLength;
int *s_iHBCount;
QVector<MDTRA_HBSearchTriplet> s_HBonds;
extern MDTRA_ProgressDialog *pProgressDialog;
static MDTRA_WaitDialog *pWaitDialog = NULL;

#if defined(HB_STAT_PARMS)
float *s_flHBStatParm1;
float *s_flHBStatParm2;
#endif

static QVector<MDTRA_HBTripletDonorInfo> s_TripletDonorInfo;
static QVector<MDTRA_HBTripletAcceptorInfo> s_TripletAcceptorInfo;
MDTRA_HBTripletParms **g_TripletParms = NULL;
static int s_TripletHMax = 0;
static int s_TripletYMax = 0;
static QMap<QString,int> s_TitleMapH;
static QMap<QString,int> s_TitleMapY;
static bool s_bConfigDataInit = false;

static const MDTRA_HBTripletDonorInfo *HBFetchDonor( const MDTRA_PDB_File *ppdb, const MDTRA_PDB_Atom *pAtom, int& firstDonor, const MDTRA_PDB_Atom **pHAtom )
{
	const int numDonors = s_TripletDonorInfo.count();
	for (int i = firstDonor; i < numDonors; i++) {
		if ( pAtom->residuenumber > 1 && (s_TripletDonorInfo[i].flags & DF_NTERM))
			continue;
		if (_stricmp( pAtom->trimmed_title, s_TripletDonorInfo[i].XTitle ))
			continue;
		if ( s_TripletDonorInfo[i].XResidue && _stricmp( pAtom->trimmed_residue, s_TripletDonorInfo[i].XResidue ))
			continue;

		// get a corresponding H-atom
		const MDTRA_PDB_Atom *pAtH = ppdb->fetchAtomByDesc( pAtom->chainIndex, pAtom->residuenumber, s_TripletDonorInfo[i].HTitle );
		if ( !pAtH )
			continue;
		if (pHAtom)
			*pHAtom = pAtH;

		firstDonor = i+1;
		return &s_TripletDonorInfo[i];
	}

	return NULL;
}

static const MDTRA_HBTripletAcceptorInfo *HBFetchAcceptor( const MDTRA_PDB_File *ppdb, const MDTRA_PDB_Atom *pAtom )
{
	const int numAcceptors = s_TripletAcceptorInfo.count();
	for (int i = 0; i < numAcceptors; i++) {
		if (_stricmp( pAtom->trimmed_title, s_TripletAcceptorInfo[i].YTitle ))
			continue;
		if ( s_TripletAcceptorInfo[i].YResidue && _stricmp( pAtom->trimmed_residue, s_TripletAcceptorInfo[i].YResidue ))
			continue;
		return &s_TripletAcceptorInfo[i];
	}

	return NULL;
}

static void HBInitConfigData_GetDimensions( MDTRA_ConfigFile *cfg )
{
	int hCounter = s_TitleMapH.count() + 1;
	int yCounter = s_TitleMapY.count() + 1;

	while ( cfg->parseToken( true ) ) {
		QString hCode = cfg->getToken();
		if ( hCode.length() > 0 && !s_TitleMapH.contains( hCode ) )
			s_TitleMapH.insert( hCode, hCounter++ );
		cfg->parseToken( false );
		QString yCode = cfg->getToken();
		if ( yCode.length() > 0 && !s_TitleMapY.contains( yCode ) )
			s_TitleMapY.insert( yCode, yCounter++ );
		cfg->skipRestOfLine();
	}
	cfg->rewind();
}

static void HBInitConfigData_FillParms( MDTRA_ConfigFile *cfg )
{
	while ( cfg->parseToken( true ) ) {
		QString hCode = cfg->getToken();
		cfg->parseToken( false );
		QString yCode = cfg->getToken();
		cfg->parseToken( false );
		float fRmin = atof( cfg->getToken() );
		cfg->parseToken( false );
		float fEm = atof( cfg->getToken() );
		cfg->skipRestOfLine();
		int hIndex = s_TitleMapH.value( hCode ) - 1;
		int yIndex = s_TitleMapY.value( yCode ) - 1;
		if ( hIndex < 0 || hIndex >= s_TripletHMax )
			continue;
		if ( yIndex < 0 || yIndex >= s_TripletYMax )
			continue;
		g_TripletParms[hIndex][yIndex].Rmin = fRmin;
		g_TripletParms[hIndex][yIndex].Em = fEm;
	}
	cfg->rewind();
}

static void HBInitConfigData_LoadDonorsAndAcceptors( MDTRA_ConfigFile *cfg )
{
	char *currentRes;
	char *currentXY;
	char *currentH;
	int currentFlags;
	int currentGroup;
	int defType;

	while ( cfg->parseToken( true ) ) {
		// get residue
		const char *resname = cfg->getToken();
		if ( !_stricmp( resname, "ANY" ) )
			currentRes = NULL;
		else
			currentRes = UTIL_Strdup( resname );
		// get D/A code
		cfg->parseToken( false );
		const char *dacode = cfg->getToken();
		switch ( *dacode ) {
		case 'D':	defType = 1; break;
		case 'A':	defType = 2; break;
		default:	defType = 0; break;
		}
		if ( !defType ) {
			cfg->skipRestOfLine();
			if ( currentRes ) free( currentRes );
			continue;
		}
		// get XY atom name
		cfg->parseToken( false );
		currentXY = UTIL_Strdup( cfg->getToken() );
		// get H atom name
		cfg->parseToken( false );
		const char *hname = cfg->getToken();
		if ( defType == 2 && _stricmp( hname, "-" ) )
			cfg->unparseToken();
		if ( defType == 1 )
			currentH = UTIL_Strdup( hname );
		else
			currentH = NULL;
		// get FF code
		cfg->parseToken( false );
		const char *ffname = cfg->getToken();
		int ffCode;
		if ( defType == 1 )
			ffCode = s_TitleMapH.value( ffname ) - 1;
		else
			ffCode = s_TitleMapY.value( ffname ) - 1;
		if ( ffCode < 0 ) {
			if ( currentRes ) free( currentRes );
			if ( currentXY ) free( currentXY );
			if ( currentH ) free( currentH );
			continue;
		}
		// get flags
		currentFlags = 0;
		currentGroup = 0;
		while ( cfg->tokenAvailable() ) {
			cfg->parseToken( false );
			const char *flagvalue = cfg->getToken();
			if ( !_stricmp( flagvalue, "nt" ) ) {
				if ( defType == 1 )
					currentFlags |= DF_NTERM;
			} else if ( !_stricmp( flagvalue, "group" ) ) {
				if ( defType == 1 )
					currentFlags |= DF_HGROUP;
				else {
					cfg->parseToken( false );
					currentGroup = atoi( cfg->getToken() );
				}
			} else {
				// skip unknown flag
			}
		}
		if ( defType == 1 ) {
			MDTRA_HBTripletDonorInfo di;
			di.XResidue = currentRes;
			di.XTitle = currentXY;
			di.HTitle = currentH;
			di.ffCode = ffCode;
			di.flags = currentFlags;
			s_TripletDonorInfo.push_back( di );
			//QString s = QString( "DONOR: %1 %2-%3 %4\n" ).arg( currentRes ).arg( currentXY ).arg( currentH ).arg( ffCode );
			//OutputDebugString( s.toAscii().data() );

		} else {
			MDTRA_HBTripletAcceptorInfo ai;
			ai.YResidue = currentRes;
			ai.YTitle = currentXY;
			ai.ffCode = ffCode;
			ai.groupIndex = currentGroup;
			s_TripletAcceptorInfo.push_back( ai );
			//QString s = QString( "ACCEPTOR: %1 %2 %3\n" ).arg( currentRes ).arg( currentXY ).arg( ffCode );
			//OutputDebugString( s.toAscii().data() );
		}
	}
	cfg->rewind();
}

void HBInitConfigData( void )
{
	if ( s_bConfigDataInit )
		return;

	s_TitleMapH.clear();
	s_TitleMapY.clear();
	s_TripletDonorInfo.clear();
	s_TripletAcceptorInfo.clear();

	MDTRA_ForEachConfig( CONFIG_TYPE_HBPARMS, HBInitConfigData_GetDimensions );

	s_TripletHMax = s_TitleMapH.count();
	s_TripletYMax = s_TitleMapY.count();

	s_bConfigDataInit = true;

	if ( !s_TripletHMax || !s_TripletYMax ) {
		// create dummy info
		s_TitleMapH["H"] = 1;
		s_TitleMapY["O"] = 1;
		s_TripletHMax = 1;
		s_TripletYMax = 1;
	}

	g_TripletParms = new MDTRA_HBTripletParms*[s_TripletHMax];
	for ( int i = 0; i < s_TripletHMax; ++i )
		g_TripletParms[i] = new MDTRA_HBTripletParms[s_TripletYMax];

	MDTRA_ForEachConfig( CONFIG_TYPE_HBPARMS, HBInitConfigData_FillParms );

	// the fastest way to compute is to precompute
	for (int i = 0; i < s_TripletHMax; i++) {
		for (int j = 0; j < s_TripletYMax; j++) {
			g_TripletParms[i][j].A12 = g_TripletParms[i][j].Em * HB_TWELFTH(g_TripletParms[i][j].Rmin);
			g_TripletParms[i][j].B6 = 2.0f * g_TripletParms[i][j].Em * HB_SIXTH(g_TripletParms[i][j].Rmin);
		}
	}

	s_TripletDonorInfo.reserve( 128 );
	s_TripletAcceptorInfo.reserve( 128 );

	MDTRA_ForEachConfig( CONFIG_TYPE_HBRES, HBInitConfigData_LoadDonorsAndAcceptors );
}

void HBFreeConfigData( void )
{
	const int numDonors = s_TripletDonorInfo.count();
	if ( numDonors ) {
		MDTRA_HBTripletDonorInfo *d = &s_TripletDonorInfo[0];
		for ( int i = 0; i < numDonors; ++i, ++d ) {
			if ( d->XResidue ) free( d->XResidue );
			if ( d->XTitle ) free( d->XTitle );
			if ( d->HTitle ) free( d->HTitle );
		}
	}
	const int numAcceptors = s_TripletAcceptorInfo.count();
	if ( numAcceptors ) {
		MDTRA_HBTripletAcceptorInfo *a = &s_TripletAcceptorInfo[0];
		for ( int i = 0; i < numAcceptors; ++i, ++a ) {
			if ( a->YResidue ) free( a->YResidue );
			if ( a->YTitle ) free( a->YTitle );
		}
	}
	if ( g_TripletParms ) {
		for ( int i = 0; i < s_TripletHMax; ++i )
			delete [] g_TripletParms[i];
		delete [] g_TripletParms;
		g_TripletParms = NULL;
	}

	s_bConfigDataInit = false;
}

int HBGetFFCode( const char *ffname )
{
	QString strName( ffname );
	int ffCode = s_TitleMapH.value( strName );
	if ( ffCode > 0 )
		return ffCode - 1;
	ffCode = s_TitleMapY.value( strName );
	if ( ffCode > 0 )
		return ffCode - 1;
	return -1;
}

float HBCalcPair( const MDTRA_PDB_File *ppdb, const MDTRA_PDB_Atom *pDonor, const MDTRA_PDB_Atom *pAcceptor )
{
	float v1[3], v2[3];
	float d1, d2, fcos;
	float bestEnergy = 9999;
	bool  bestFound = false;

	HBInitConfigData();
	
	//set atom pointers
	const MDTRA_PDB_Atom *pAtX = pDonor;
	const MDTRA_PDB_Atom *pAtH = NULL;
	const MDTRA_PDB_Atom *pAtY = pAcceptor;

	//get acceptor info
	const MDTRA_HBTripletAcceptorInfo *pAcceptorInfo = HBFetchAcceptor( ppdb, pAtY );
	if (!pAcceptorInfo)
		return 0.0f;

	//get donor info
	const MDTRA_HBTripletDonorInfo *pDonorInfo;
	int firstDonor = 0;
	
	while ((pDonorInfo = HBFetchDonor( ppdb, pAtX, firstDonor, &pAtH ))) {
		//X-H pair is valid
		//ignore the neighbour residue, if both residue titles are NULL
		if ( !pDonorInfo->XResidue && !pAcceptorInfo->YResidue &&
			((pAtY->residueserial == pAtX->residueserial - 1) || (pAtY->residueserial == pAtX->residueserial + 1)))
			continue;

		//X-H-Y triple is valid
		Vec3_Sub( v1, pAtY->xyz, pAtH->xyz );
		Vec3_LenSq( d1, v1 );

		if ( d1 > HB_CUTOFF_DIST_SQ )
			continue;

		const MDTRA_HBTripletParms *pTParms = &g_TripletParms[pDonorInfo->ffCode][pAcceptorInfo->ffCode];

		d1 = sqrtf( d1 );
		Vec3_Sub( v2, pAtX->xyz, pAtH->xyz );
		Vec3_Len( d2, v2 );
		Vec3_Scale( v1, v1, 1.0f / d1 );
		Vec3_Scale( v2, v2, 1.0f / d2 );
		Vec3_Dot( fcos, v1, v2 );

		float flEnergy = exp( -HB_SQR(fcos + 1.0f) / HB_SIGMA2 );

		if (d1 <= pTParms->Rmin) 
			flEnergy *= -pTParms->Em;
		else 
			flEnergy *= (pTParms->A12 / HB_TWELFTH( d1 )) - (pTParms->B6 / HB_SIXTH( d1 ));

		if (fabsf(flEnergy) < 0.001f)
			continue;
		if ( flEnergy > bestEnergy )
			continue;

		bestFound = true;
		bestEnergy = flEnergy;
		Vec3_Sub( v1, pAtX->xyz, pAtY->xyz );
		Vec3_Len( d1, v1 );
	}

	if ( !bestFound )
		return 0.0f;

	return bestEnergy;
}

static bool HBGetTriplets( void )
{
	MDTRA_HBSearchTriplet localTriplet;
	localTriplet.flags = 0;

#ifdef XY_CUTOFF
	float vecDist[3];
	float lensq;
#endif

	for ( int i = 0; i < s_lhbsd.pStream->pdb->getAtomCount(); i++ ) {
		//get X atom pointer
		const MDTRA_PDB_Atom *pAtX = s_lhbsd.pStream->pdb->fetchAtomByIndex( i );
		const MDTRA_PDB_Atom *pAtH = NULL;

		//update wait dialog
		QApplication::processEvents();
		if (pWaitDialog->checkInterrupt())
			return false;

		//get donor info
		const MDTRA_HBTripletDonorInfo *pDonorInfo;
		int firstDonor = 0;
		int startSize = s_iHBRealSize;
		
		while ((pDonorInfo = HBFetchDonor( s_lhbsd.pStream->pdb, pAtX, firstDonor, &pAtH ))) {
			//X-H pair is valid
			localTriplet.atX = pAtX->serialnumber;
			localTriplet.atH[0] = pAtH->serialnumber;
			localTriplet.xff = pDonorInfo->ffCode;

			for ( int j = 1; j < MAX_GROUPED_HYDROGENS; j++ )
				localTriplet.atH[j] = -1;	//not grouped (one hydrogen per triplet)
			for ( int j = 1; j < MAX_GROUPED_ACCEPTORS; j++ )
				localTriplet.atY[j] = -1;	//not grouped (one acceptor per triplet)

			for ( int j = 0; j < s_lhbsd.pStream->pdb->getAtomCount(); j++) {
				//get Y atom pointer
				if ( i == j ) continue;
				const MDTRA_PDB_Atom *pAtY = s_lhbsd.pStream->pdb->fetchAtomByIndex( j );

				//ignore the same residue
				if ( pAtY->residueserial == pAtX->residueserial )
					continue;

#ifdef XY_CUTOFF
				//cut by X-Y distance
				//NB: distance must be large enough to ensure these atoms
				//	  will never get close along the trajectory dynamics!
				Vec3_Sub( vecDist, pAtX->xyz, pAtY->xyz );
				Vec3_LenSq( lensq, vecDist );
				if ( lensq > XY_CUTOFF_DIST_SQ )
					continue;
#endif

				//get acceptor info
				const MDTRA_HBTripletAcceptorInfo *pAcceptorInfo = HBFetchAcceptor( s_lhbsd.pStream->pdb, pAtY );
				if (!pAcceptorInfo)
					continue;

				//ignore the neighbour residue, if both residue titles are NULL
				if ( !pDonorInfo->XResidue && !pAcceptorInfo->YResidue &&
					((pAtY->residueserial == pAtX->residueserial - 1) || (pAtY->residueserial == pAtX->residueserial + 1)))
					continue;
			
				//X-H-Y triple is valid
				localTriplet.atY[0] = pAtY->serialnumber;
				localTriplet.yff = pAcceptorInfo->ffCode;
				localTriplet.groupIndex = pAcceptorInfo->groupIndex;
			
				/*OutputDebugString( QString("Triplet: %1%2%3 - %4%5%6 - %7%8%9\n")
					.arg(pAtX->trimmed_residue).arg(pAtX->residuenumber).arg(pAtX->trimmed_title)
					.arg(pAtH->trimmed_residue).arg(pAtH->residuenumber).arg(pAtH->trimmed_title)
					.arg(pAtY->trimmed_residue).arg(pAtY->residuenumber).arg(pAtY->trimmed_title)
					.toAscii());*/

				if ( s_lhbsd.grouping && (pDonorInfo->flags & DF_HGROUP) ) {
					//Find existing X-Y triplet and add new H-atom
					bool bFound = false;
					for ( int k = startSize; k < s_iHBRealSize; k++ ) {
						MDTRA_HBSearchTriplet *pExistingTriplet = const_cast<MDTRA_HBSearchTriplet*>(&s_HBonds.at(k));
						if ( pExistingTriplet->atX != localTriplet.atX )
							continue;
						int l;
						for ( l = 0; l < MAX_GROUPED_ACCEPTORS; l++ ) {
							if ( pExistingTriplet->atY[l] == localTriplet.atY[0] )
								break;
						}
						if ( l == MAX_GROUPED_ACCEPTORS )
							continue;

						for ( l = 0; l < MAX_GROUPED_HYDROGENS; l++ ) {
							if ( pExistingTriplet->atH[l] == localTriplet.atH[0] ) {
								//already exists
								bFound = true;
								break;
							} else if ( pExistingTriplet->atH[l] < 0 ) {
								pExistingTriplet->atH[l] = localTriplet.atH[0];
								bFound = true;
								break;
							}
						}
						break;
					}
					if ( bFound )
						continue;
				}

				if ( s_lhbsd.grouping && pAcceptorInfo->groupIndex ) {
					//Find existing X-Y triplet and add new Y-atom
					bool bFound = false;
					for ( int k = startSize; k < s_iHBRealSize; k++ ) {
						MDTRA_HBSearchTriplet *pExistingTriplet = const_cast<MDTRA_HBSearchTriplet*>(&s_HBonds.at(k));
						if ( pExistingTriplet->atX != localTriplet.atX )
							continue;
						if ( pExistingTriplet->groupIndex != localTriplet.groupIndex )
							continue;
						//Y-atom must be of the same residue
						const MDTRA_PDB_Atom *pAtY2 = s_lhbsd.pStream->pdb->fetchAtomByIndex( pExistingTriplet->atY[0] );
						if ( pAtY2->residueserial != pAtY->residueserial )
							continue;
						int l;
						for ( l = 0; l < MAX_GROUPED_ACCEPTORS; l++ ) {
							if ( pExistingTriplet->atY[l] == localTriplet.atY[0] ) {
								//already exists
								bFound = true;
								break;
							} else if ( pExistingTriplet->atY[l] < 0 ) {
								pExistingTriplet->atY[l] = localTriplet.atY[0];
								bFound = true;
								break;
							}
						}
						break;
					}
					if ( bFound )
						continue;
				}

				if (s_iHBRealSize >= s_iHBBufferSize-1) {
					s_HBonds << localTriplet;
					s_iHBBufferSize++;
				} else {
					s_HBonds.replace( s_iHBRealSize, localTriplet );
				}
				s_iHBRealSize++;
			}
		}
	}

	return true;
}

static bool HBGetAllTriplets( void )
{
	if (!s_lhbsd.pStream || !s_lhbsd.pStream->pdb)
		return false;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	HBInitConfigData();

	if (!HBGetTriplets()) {
		QApplication::restoreOverrideCursor();
		return false;
	}

	QApplication::restoreOverrideCursor();
	
	//OutputDebugString(QString("Total %1 tr\n").arg(s_iHBRealSize).toAscii());

	return (s_iHBRealSize > 0);
}

static void HBCalcTriplet( int threadnum, MDTRA_PDB_File *ppdb, int index )
{
	const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at(index);
	const MDTRA_PDB_Atom *pAtX = ppdb->fetchAtomBySerialNumber( pTriplet->atX );
	const MDTRA_HBTripletParms *pTParms = &g_TripletParms[pTriplet->xff][pTriplet->yff];
	const MDTRA_PDB_Atom *pAtH;
	const MDTRA_PDB_Atom *pAtY;
	float bestEnergy = 9999;
	bool  bestFound = false;
	float bestLength = 0;

	for ( int i = 0; i < MAX_GROUPED_HYDROGENS; i++ ) {
		if ( pTriplet->atH[i] < 0 )
			break;
	
		pAtH = ppdb->fetchAtomBySerialNumber( pTriplet->atH[i] );

		for ( int j = 0; j < MAX_GROUPED_ACCEPTORS; j++ ) {
			if ( pTriplet->atY[j] < 0 )
				break;

			pAtY = ppdb->fetchAtomBySerialNumber( pTriplet->atY[j] );

			float v1[3], v2[3];
			float d1, d2, fcos;

			Vec3_Sub( v1, pAtY->xyz, pAtH->xyz );
			Vec3_LenSq( d1, v1 );

			if ( d1 > HB_CUTOFF_DIST_SQ )
				continue;

			d1 = sqrtf( d1 );
			Vec3_Sub( v2, pAtX->xyz, pAtH->xyz );
			Vec3_Len( d2, v2 );
			Vec3_Scale( v1, v1, 1.0f / d1 );
			Vec3_Scale( v2, v2, 1.0f / d2 );
			Vec3_Dot( fcos, v1, v2 );

			float flEnergy = exp( -HB_SQR(fcos + 1.0f) / HB_SIGMA2 );

			if (d1 <= pTParms->Rmin) 
				flEnergy *= -pTParms->Em;
			else 
				flEnergy *= (pTParms->A12 / HB_TWELFTH( d1 )) - (pTParms->B6 / HB_SIXTH( d1 ));

			if (fabsf(flEnergy) < 0.001f || fabsf(flEnergy) < s_lhbsd.minEnergy)
				continue;
			if ( flEnergy > bestEnergy )
				continue;

			bestFound = true;
			bestEnergy = flEnergy;
			Vec3_Sub( v1, pAtX->xyz, pAtY->xyz );
			Vec3_Len( d1, v1 );
			bestLength = d1;
		}
	}

	if ( !bestFound )
		return;

	int numThreads = CountThreads();
	int arrayIndex = index * numThreads + threadnum;
	s_flHBEnergy[arrayIndex] += bestEnergy;
	s_flHBLength[arrayIndex] += bestLength;
	s_iHBCount[arrayIndex]++;

#if defined(HB_STAT_PARMS)
	s_flHBStatParm1[arrayIndex] += bestEnergy;
	s_flHBStatParm2[arrayIndex] += bestEnergy*bestEnergy;
#endif
}

static void f_HBSearch( int threadnum, int num )
{
	//Load PDB file
	MDTRA_PDB_File *pPdbFile;
	if ( (s_lhbsd.workStart + num) == 0) {
		pPdbFile = s_lhbsd.pStream->pdb;
	} else {
		pPdbFile = s_lhbsd.tempPDB[threadnum];
		pPdbFile->load( threadnum, s_lhbsd.pStream->format_identifier, s_lhbsd.pStream->files.at(s_lhbsd.workStart + num).toAscii(), s_lhbsd.pStream->flags );
	}

	//Calculate H-Bonds
	for (int i = 0; i < s_iHBRealSize; i++)
		HBCalcTriplet( threadnum, pPdbFile, i );

	if (pProgressDialog) {
		pProgressDialog->advanceCurrentFile( num+1 );
		if (pProgressDialog->checkInterrupt()) {
			InterruptThreads();
		}
	}
}

static void f_HBJoin( int threadnum )
{
	int numThreads = CountThreads();
	for (int i = 0; i < s_iHBRealSize; i++) {
		int arrayIndex = i * numThreads;
		s_flHBEnergy[arrayIndex] += s_flHBEnergy[arrayIndex + threadnum];
		s_flHBLength[arrayIndex] += s_flHBLength[arrayIndex + threadnum];
		s_iHBCount[arrayIndex] += s_iHBCount[arrayIndex + threadnum];
#if defined(HB_STAT_PARMS)
		s_flHBStatParm1[arrayIndex] += s_flHBStatParm1[arrayIndex + threadnum];
		s_flHBStatParm2[arrayIndex] += s_flHBStatParm2[arrayIndex + threadnum];
#endif
	}
}

static bool f_HBFinalize( void )
{
	s_iHBTotal = 0;
	int numThreads = CountThreads();

	for (int i = 0; i < s_iHBRealSize; i++) {
		int iC = s_iHBCount[i*numThreads];
		if (!iC || iC < s_lhbsd.minCount)
			continue;

		float fiC = 1.0f / (float)iC;
		s_flHBEnergy[i*numThreads] *= fiC;
		s_flHBLength[i*numThreads] *= fiC;
		MDTRA_HBSearchTriplet *pTriplet = const_cast<MDTRA_HBSearchTriplet*>(&s_HBonds.at(i));
		pTriplet->flags |= TF_VALID;
		s_iHBTotal++;
	}

	return (s_iHBTotal > 0);
}

bool SetupHBSearch( MDTRA_MainWindow *pMainWindow, const MDTRA_HBSearchInfo *pSearchInfo )
{
	s_pMainWindow = pMainWindow;
	assert( s_pMainWindow != NULL );

	//clear existing bonds
	s_iHBTotal = 0;
	s_iHBRealSize = 0;
	if (s_iHBBufferSize < 256000) {
		s_HBonds.reserve( 256000 ); //~5 Mb
		s_iHBBufferSize = s_HBonds.count();
	}

	//setup local search data
	s_lhbsd.workStart = pSearchInfo->trajectoryMin - 1;
	s_lhbsd.workCount = pSearchInfo->trajectoryMax - pSearchInfo->trajectoryMin + 1;
	s_lhbsd.minCount = (int)ceil(((float)pSearchInfo->minPercent / 100.0f) * s_lhbsd.workCount);
	s_lhbsd.minEnergy = pSearchInfo->minEnergy;
	s_lhbsd.grouping = pSearchInfo->grouping;
	s_lhbsd.pStream = s_pMainWindow->getProject()->fetchStreamByIndex( pSearchInfo->streamIndex );
	if (!s_lhbsd.pStream || !s_lhbsd.pStream->pdb)
		return false;

	MDTRA_WaitDialog dlgWait( s_pMainWindow );
	dlgWait.setMessage( QObject::tr("Building list of all possible H-Bond triplets, please wait...") );
	dlgWait.show();
	pWaitDialog = &dlgWait;
	QApplication::processEvents();

	//get all triplets
	if (!HBGetAllTriplets())  {
		dlgWait.hide();
		return false;
	}

	pWaitDialog = NULL;
	dlgWait.hide();
	QApplication::processEvents();

	//allocate thread memory
	for (int i = 0; i < CountThreads(); i++) {
		s_lhbsd.tempPDB[i] = new MDTRA_PDB_File;
		if (!s_lhbsd.tempPDB[i])
			return false;
	}

	s_flHBEnergy = new float[s_iHBRealSize * CountThreads()];
	s_flHBLength = new float[s_iHBRealSize * CountThreads()];
	s_iHBCount = new int[s_iHBRealSize * CountThreads()];
	if (!s_flHBEnergy || !s_flHBLength || !s_iHBCount)
		return false;

#if defined(HB_STAT_PARMS)
	s_flHBStatParm1 = new float[s_iHBRealSize * CountThreads()];
	s_flHBStatParm2 = new float[s_iHBRealSize * CountThreads()];
	if (!s_flHBStatParm1 || !s_flHBStatParm2 )
		return false;

	memset( s_flHBStatParm1, 0, s_iHBRealSize * CountThreads() * sizeof(float) );
	memset( s_flHBStatParm2, 0, s_iHBRealSize * CountThreads() * sizeof(float) );
#endif

	memset( s_flHBEnergy, 0, s_iHBRealSize * CountThreads() * sizeof(float) );
	memset( s_flHBLength, 0, s_iHBRealSize * CountThreads() * sizeof(float) );
	memset( s_iHBCount, 0, s_iHBRealSize * CountThreads() * sizeof(int) );

	return true;
}

bool PerformHBSearch( void )
{
	MDTRA_ProgressDialog dlgProgress( s_pMainWindow );
	dlgProgress.setStreamCount( 1 );
	dlgProgress.show();

	pProgressDialog = &dlgProgress;

	dlgProgress.setFileCount( s_lhbsd.workCount );
	dlgProgress.setCurrentStream( 0 );
	dlgProgress.setCurrentFile( 0 );

	RunThreadsOnIndividual( s_lhbsd.workCount, f_HBSearch );

	if (dlgProgress.checkInterrupt())
		return false;

	dlgProgress.setProgressAtMax();

	for (int i = 1; i < CountThreads(); i++) f_HBJoin( i );
	bool b = f_HBFinalize();

	if (!b) {
		QMessageBox::warning(s_pMainWindow, "H-Bond Search Results", "No significant hydrogen bonds were found!\nPlease make your significance criterion less strict.");
		return false;
	}

	dlgProgress.hide();
	QApplication::processEvents();

	return true;
}

void FreeHBSearch( void )
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	for (int i = 0; i < CountThreads(); i++) {
		if (s_lhbsd.tempPDB[i]) {
			delete s_lhbsd.tempPDB[i];
			s_lhbsd.tempPDB[i] = NULL;
		}
	}

	if (s_flHBEnergy) {
		delete [] s_flHBEnergy;
		s_flHBEnergy = NULL;
	}
	if (s_flHBLength) {
		delete [] s_flHBLength;
		s_flHBLength = NULL;
	}
	if (s_iHBCount) {
		delete [] s_iHBCount;
		s_iHBCount = NULL;
	}
#if defined(HB_STAT_PARMS)
	if (s_flHBStatParm1) {
		delete [] s_flHBStatParm1;
		s_flHBStatParm1 = NULL;
	}
	if (s_flHBStatParm2) {
		delete [] s_flHBStatParm2;
		s_flHBStatParm2 = NULL;
	}
#endif

	s_pMainWindow = NULL;
	s_iHBRealSize = 0;

	QApplication::restoreOverrideCursor();
}

void FreeHBSearchOnExit( void )
{
	s_HBonds.clear();
	s_iHBBufferSize = 0;
}
