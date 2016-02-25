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
//	Implementation of MDTRA_HBSearchResultsDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_math.h"
#include "mdtra_project.h"
#include "mdtra_pdb.h"
#include "mdtra_hbSearch.h"
#include "mdtra_hbSearchResultsDialog.h"
#include "mdtra_customTableWidgetItems.h"

#include <QtCore/QTextStream>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>

extern QVector<MDTRA_HBSearchTriplet> s_HBonds;
extern MDTRA_HBSearchData s_lhbsd;
extern float *s_flHBEnergy;
extern float *s_flHBLength;
extern int *s_iHBCount;
extern int s_iHBTotal;
extern int s_iHBRealSize;

#if defined(HB_STAT_PARMS)
extern float *s_flHBStatParm1;
extern float *s_flHBStatParm2;
#endif

MDTRA_HBSearchResultsDialog :: MDTRA_HBSearchResultsDialog( QWidget *parent )
							 : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/hbond.png") );

	QStringList hrzHeader;
	hrzHeader << tr("X");
	hrzHeader << tr("H");
	hrzHeader << tr("Y");
	hrzHeader << tr("Energy, kcal/mol");
	hrzHeader << tr("Length, A");
	hrzHeader << tr("Occurence");
	hrzHeader << tr("Options");
	resultTable->setHorizontalHeaderLabels(hrzHeader);
	resultTable->setRowCount( MDTRA_MAX( 15, s_iHBTotal ) );

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QApplication::processEvents();	

	int c = 0;
	int numThreads = CountThreads();
	float* pEnergy = s_flHBEnergy;
	float* pLength = s_flHBLength;
	int* pCount = s_iHBCount;

	for (int i = 0; i < s_iHBRealSize; i++, pEnergy += numThreads, pLength += numThreads, pCount += numThreads) {
		const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at(i);
		if (!(pTriplet->flags & TF_VALID))
			continue;

		const MDTRA_PDB_Atom *pAtX = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atX );
		const MDTRA_PDB_Atom *pAtH = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atH[0] );
		const MDTRA_PDB_Atom *pAtY = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atY[0] );

		QTableWidgetItem_SortByUserRole_i *pItemi;
		QTableWidgetItem_SortByUserRole_f *pItemf;
		QPushButtonWithTag *pButton;
		QString hTitle = tr("%1-%2 %3").arg(pAtH->trimmed_residue).arg(pAtH->residuenumber).arg(pAtH->trimmed_title);
		QString yTitle = tr("%1-%2 %3").arg(pAtY->trimmed_residue).arg(pAtY->residuenumber).arg(pAtY->trimmed_title);

		for ( int j = 1; j < MAX_GROUPED_HYDROGENS; j++ ) {
			if ( pTriplet->atH[j] >= 0 ) {
				pAtH = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atH[j] );
				hTitle.append(QString("/%1").arg(pAtH->trimmed_title));
			}
		}
		for ( int j = 1; j < MAX_GROUPED_ACCEPTORS; j++ ) {
			if ( pTriplet->atY[j] >= 0 ) {
				pAtY = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atY[j] );
				yTitle.append(QString("/%1").arg(pAtY->trimmed_title));
			}
		}

		pItemi = new QTableWidgetItem_SortByUserRole_i(tr("%1-%2 %3").arg(pAtX->trimmed_residue).arg(pAtX->residuenumber).arg(pAtX->trimmed_title));
		pItemi->setData(Qt::UserRole, pTriplet->atX);
		pItemi->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(c, 0, pItemi);

		pItemi = new QTableWidgetItem_SortByUserRole_i(hTitle);
		pItemi->setData(Qt::UserRole, pTriplet->atH[0]);
		pItemi->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(c, 1, pItemi);

		pItemi = new QTableWidgetItem_SortByUserRole_i(yTitle);
		pItemi->setData(Qt::UserRole, pTriplet->atY[0]);
		pItemi->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(c, 2, pItemi);

		pItemf = new QTableWidgetItem_SortByUserRole_f();
		pItemf->setData(Qt::UserRole, *pEnergy);
		pItemf->setData(Qt::DisplayRole, QString::number( *pEnergy, 'f', 3 ));
		pItemf->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(c, 3, pItemf);

		pItemf = new QTableWidgetItem_SortByUserRole_f();
		pItemf->setData(Qt::UserRole, *pLength);
		pItemf->setData(Qt::DisplayRole, QString::number( *pLength, 'f', 3 ));
		pItemf->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(c, 4, pItemf);

		pItemi = new QTableWidgetItem_SortByUserRole_i();
		pItemi->setData(Qt::UserRole, *pCount);
		pItemi->setData(Qt::DisplayRole, QString("%1%").arg(QString::number( floor(100.0f * (float)(*pCount) / (float)s_lhbsd.workCount), 'f', 0 )));
		pItemi->setToolTip( QString("Found in %1/%2 snapshots").arg(*pCount).arg(s_lhbsd.workCount) );
		pItemi->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(c, 5, pItemi);

		pButton = new QPushButtonWithTag( i, tr("Add..."), resultTable );
		resultTable->setCellWidget(c, 6, pButton);
		connect(pButton, SIGNAL(clicked()), this, SLOT(exec_on_result_add()));
		c++;
	}

	resultTable->sortByColumn( 0, Qt::AscendingOrder );

	QApplication::restoreOverrideCursor();

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_save()));
	connect(resultTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(exec_on_header_clicked(int)));
	createPopupMenu();
}

void MDTRA_HBSearchResultsDialog :: exec_on_header_clicked( int headerIndex )
{
	if (headerIndex < 6)
		resultTable->sortByColumn( headerIndex );
}

#ifdef ENABLE_RES_EXPORT
void MDTRA_HBSearchResultsDialog :: exportResultsToRES( QTextStream *stream )
{
	*stream << QString(" hB128 energy List:");
	*stream << endl;
	*stream << QString(" ihB:   Y    Hx   X   eHB128   dyh     dxy    aXhY     dxh     occ");
	*stream << endl;

	float flTotalEnergy = 0.0f;
	float v1[3], v2[3];
	float dXY, dXH, dYH, a, occ;
	char lineBuffer[256];
	int c = 1;

	int numThreads = CountThreads();
	float* pEnergy = s_flHBEnergy;
	float* pLength = s_flHBLength;
	int* pCount = s_iHBCount;

	for (int i = 0; i < s_iHBRealSize; i++, pEnergy += numThreads, pLength += numThreads, pCount += numThreads) {
		const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at(i);
		if (!(pTriplet->flags & TF_VALID))
			continue;

		const MDTRA_PDB_Atom *pAtX = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atX );
		const MDTRA_PDB_Atom *pAtH = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atH[0] );
		const MDTRA_PDB_Atom *pAtY = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atY[0] );

		Vec3_Sub( v1, pAtX->xyz, pAtY->xyz );
		Vec3_Len( dXY, v1 );
		Vec3_Sub( v1, pAtX->xyz, pAtH->xyz );
		Vec3_Len( dXH, v1 );
		Vec3_Sub( v2, pAtY->xyz, pAtH->xyz );
		Vec3_Len( dYH, v2 );
		Vec3_Scale( v1, v1, 1.0f / dXH );
		Vec3_Scale( v2, v2, 1.0f / dYH );
		Vec3_Dot( a, v1, v2 );
		a = UTIL_rad2deg( acosf( a ) );

		flTotalEnergy += *pEnergy;
		occ = (float)(*pCount) / (float)s_lhbsd.workCount;
		 
		sprintf_s( lineBuffer, sizeof(lineBuffer), "%4i%5i%5i%5i%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f", 
			c, pTriplet->atY[0], pTriplet->atH[0], pTriplet->atX, *pEnergy, dYH, *pLength, a, dXH, occ );
		c++;

		*stream << QString(lineBuffer) << endl;
	}

	*stream << QString(" hB128EngTot:      %1").arg(flTotalEnergy);
	*stream << endl;
}
#endif

void MDTRA_HBSearchResultsDialog :: exportResultsToRasMolSelection( QTextStream *stream )
{
	QSet<int> residueSet;
	QList<int> residueList;

	for (int i = 0; i < s_iHBRealSize; i++) {
		const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at(i);
		if (!(pTriplet->flags & TF_VALID))
			continue;

		const MDTRA_PDB_Atom *pAtX = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atX );

		residueSet << pAtX->residuenumber;

		for (int j = 0; j < MAX_GROUPED_ACCEPTORS; j++) {
			if ( pTriplet->atY[j] >= 0 ) {
				const MDTRA_PDB_Atom *pAtY = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atY[j] );
				residueSet << pAtY->residuenumber;
			}
		}
	}

	residueList = residueSet.toList();
	qSort(residueList);

	bool range = false;
	int prev = -1;
	for (int i = 0; i < residueList.count(); i++) {
		int curr = residueList.at(i);
		if ( prev == -1 ) {
			*stream << QString("select %1").arg(curr);
		} else if ( prev == curr - 1 ) {
			//continue range
			range = true;
		} else {
			//close range
			if ( range ) {
				*stream << QString("-%1").arg(prev);
				range = false;
			}
			*stream << endl << QString("select selected, %1").arg(curr);
		}
		prev = curr;
	}
	*stream << endl;
}

void MDTRA_HBSearchResultsDialog :: exportResults( const QString &fileSuffix, QTextStream *stream )
{
	bool bCSV = false;
	if (fileSuffix.toLower() == "csv") {
		bCSV = true;
	}
#ifdef ENABLE_RES_EXPORT
	else if (fileSuffix.toLower() == "res") {
		exportResultsToRES( stream );
		return;
	} 
#endif
	else if (fileSuffix.toLower() == "spt") {
		exportResultsToRasMolSelection( stream );
		return;
	} 

	if (bCSV)
		*stream << QString("\"\";\"X\";\"\";\"H\";\"\";\"Y\";\"Energy (kcal/mol)\";\"Length (A)\";\"Occurence\"");
	else
		*stream << QString("\t\"X\"\t\t\"H\"\t\t\"Y\"\t\"Energy (kcal/mol)\"\t\"Length (A)\"\t\"Occurence\"");

#if defined(HB_STAT_PARMS)
	if (bCSV)
		*stream << QString(";\"SumXi\";\"SumXi2\";\"N\"");
	else
		*stream << QString("\t\"SumXi\"\t\"SumXi2\"\t\"N\"");
#endif

	*stream << endl;

	int numThreads = CountThreads();
	float* pEnergy = s_flHBEnergy;
	float* pLength = s_flHBLength;
	int* pCount = s_iHBCount;
	float* pStatParm1 = NULL;
	float* pStatParm2 = NULL;
#if defined(HB_STAT_PARMS)
	pStatParm1 = s_flHBStatParm1;
	pStatParm2 = s_flHBStatParm2;
#endif

	for (int i = 0; i < s_iHBRealSize; i++, pEnergy += numThreads, pStatParm1 += numThreads, pStatParm2 += numThreads, pLength += numThreads, pCount += numThreads) {
		const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at(i);
		if (!(pTriplet->flags & TF_VALID))
			continue;

		const MDTRA_PDB_Atom *pAtX = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atX );
		const MDTRA_PDB_Atom *pAtH = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atH[0] );
		const MDTRA_PDB_Atom *pAtY = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atY[0] );

		QString sAtX = tr("%1-%2 %3").arg(pAtX->trimmed_residue).arg(pAtX->residuenumber).arg(pAtX->trimmed_title);
		QString sAtH = tr("%1-%2 %3").arg(pAtH->trimmed_residue).arg(pAtH->residuenumber).arg(pAtH->trimmed_title);
		QString sAtY = tr("%1-%2 %3").arg(pAtY->trimmed_residue).arg(pAtY->residuenumber).arg(pAtY->trimmed_title);

		for ( int j = 1; j < MAX_GROUPED_HYDROGENS; j++ ) {
			if ( pTriplet->atH[j] >= 0 ) {
				pAtH = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atH[j] );
				sAtH.append(QString("/%1").arg(pAtH->trimmed_title));
			}
		}
		for ( int j = 1; j < MAX_GROUPED_ACCEPTORS; j++ ) {
			if ( pTriplet->atY[j] >= 0 ) {
				pAtY = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pTriplet->atY[j] );
				sAtY.append(QString("/%1").arg(pAtY->trimmed_title));
			}
		}	

		float flP = floor( 100.0f * (float)(*pCount) / (float)s_lhbsd.workCount );

		if (bCSV) {
			*stream << QString("%1;\"%2\";%3;\"%4\";%5;\"%6\";%7;%8;%9")
				.arg(pTriplet->atX).arg(sAtX)
				.arg(pTriplet->atH[0]).arg(sAtH)
				.arg(pTriplet->atY[0]).arg(sAtY)
				.arg(*pEnergy, 0, 'f', 6).arg(*pLength, 0, 'f', 6).arg(flP);
		} else {
			*stream << QString("%1\t\"%2\"\t%3\t\"%4\"\t%5\t\"%6\"\t%7\t%8\t%9")
				.arg(pTriplet->atX).arg(sAtX)
				.arg(pTriplet->atH[0]).arg(sAtH)
				.arg(pTriplet->atY[0]).arg(sAtY)
				.arg(*pEnergy, 0, 'f', 6).arg(*pLength, 0, 'f', 6).arg(flP);
		}
#if defined(HB_STAT_PARMS)
		if (bCSV) {
			*stream << QString(";%1;%2;%3").arg(*pStatParm1, 0, 'f', 6).arg(*pStatParm2, 0, 'f', 6).arg(*pCount);
		} else {
			*stream << QString("\t%1\t%2\t%3").arg(*pStatParm1, 0, 'f', 6).arg(*pStatParm2, 0, 'f', 6).arg(*pCount);
		}
#endif
		*stream << endl;
	}
}

void MDTRA_HBSearchResultsDialog :: exec_save( void )
{
#ifdef ENABLE_RES_EXPORT
	QString exportFilter = "Text Files (*.txt);;CSV Files (*.csv);;BioPASED RES Files (*.res);;RasMol Selection (*.spt)";
#else
	QString exportFilter = "Text Files (*.txt);;CSV Files (*.csv);;RasMol Selection (*.spt)";
#endif
	QString selectedFilter;
	QMap<QString,QString> extMap;
	extMap["Text Files (*.txt)"] = ".txt";
	extMap["CSV Files (*.csv)"] = ".csv";
	extMap["RasMol Selection (*.spt)"] = ".spt";
	extMap["BioPASED RES Files (*.res)"] = ".res";

	QString fileName = QFileDialog::getSaveFileName( this, tr("Save H-Bond Search Results"), m_pMainWindow->getCurrentFileDir(), exportFilter, &selectedFilter );
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		if ( fi.suffix().isEmpty() ) {
			fileName.append( extMap[selectedFilter] );
			fi.setFile( fileName );
		}
		QFile f(fileName);

		//Open file for writing
		if (!f.open(QFile::WriteOnly | QFile::Truncate)) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open file for writing!\nFile: %1\nReason: %2").arg(fileName,f.errorString()));
			return;
		}

		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QTextStream stream(&f);
		exportResults( fi.suffix(), &stream );
		f.close();
		QApplication::restoreOverrideCursor();
	}
}

void MDTRA_HBSearchResultsDialog :: exec_on_result_add( void )
{
	for (int row = 0; row < resultTable->rowCount(); row++) {
		if (sender() == resultTable->cellWidget(row, 6)) {
			QPushButtonWithTag *pButton = qobject_cast<QPushButtonWithTag*>(sender());
			m_iCurrentItem = pButton->getTag();
			QPoint p( 0, pButton->height() );
			m_pPopupMenu->popup( pButton->mapToGlobal( p ) );
		}
	}
}

void MDTRA_HBSearchResultsDialog :: createPopupMenu( void )
{
	QAction *pAction;

	m_pSubMenu[0] = new QMenu( tr("X-Y Distance"), this );
	m_pSubMenu[1] = new QMenu( tr("H-Y Distance"), this );
	m_pSubMenu[2] = new QMenu( tr("X-H-Y Angle"), this );

	pAction = new QAction( tr("New Result Collector..."), this );
	connect( pAction, SIGNAL(triggered()), this, SLOT(addToNewResultCollectorXY()));
	m_pSubMenu[0]->addAction( pAction );
	pAction = new QAction( tr("New Result Collector..."), this );
	connect( pAction, SIGNAL(triggered()), this, SLOT(addToNewResultCollectorHY()));
	m_pSubMenu[1]->addAction( pAction );
	pAction = new QAction( tr("New Result Collector..."), this );
	connect( pAction, SIGNAL(triggered()), this, SLOT(addToNewResultCollectorXHY()));
	m_pSubMenu[2]->addAction( pAction );

	if (m_pMainWindow->getProject()->getResultCountByType( MDTRA_DT_DISTANCE ) > 0) {
		m_pSubMenu[0]->addSeparator();
		m_pSubMenu[1]->addSeparator();
		for (int i = 0; i < m_pMainWindow->getProject()->getResultCount(); i++) {
			MDTRA_Result *pResult = m_pMainWindow->getProject()->fetchResult( i );
			if (pResult && (pResult->type == MDTRA_DT_DISTANCE)) {
				pAction = new QAction( tr("[%1] %2").arg(pResult->index).arg(pResult->name), this );
				pAction->setData( pResult->index );
				connect( pAction, SIGNAL(triggered()), this, SLOT(addToExistingResultCollectorXY()));
				m_pSubMenu[0]->addAction( pAction );
				pAction = new QAction( tr("[%1] %2").arg(pResult->index).arg(pResult->name), this );
				pAction->setData( pResult->index );
				connect( pAction, SIGNAL(triggered()), this, SLOT(addToExistingResultCollectorHY()));
				m_pSubMenu[1]->addAction( pAction );
			}
		}
	}

	if (m_pMainWindow->getProject()->getResultCountByType( MDTRA_DT_ANGLE ) > 0) {
		m_pSubMenu[2]->addSeparator();
		for (int i = 0; i < m_pMainWindow->getProject()->getResultCount(); i++) {
			MDTRA_Result *pResult = m_pMainWindow->getProject()->fetchResult( i );
			if (pResult && (pResult->type == MDTRA_DT_ANGLE)) {
				pAction = new QAction( tr("[%1] %2").arg(pResult->index).arg(pResult->name), this );
				pAction->setData( pResult->index );
				connect( pAction, SIGNAL(triggered()), this, SLOT(addToExistingResultCollectorXHY()));
				m_pSubMenu[2]->addAction( pAction );
			}
		}
	}

	m_pPopupMenu = new QMenu( this );
	for (int i = 0; i < 3; i++) m_pPopupMenu->addMenu(m_pSubMenu[i]);
}

const MDTRA_DataSource* MDTRA_HBSearchResultsDialog :: buildDataSourceDistanceXY( const MDTRA_Stream *pStream, int bondIndex, int aIndex )
{
	const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at(bondIndex);

	//check if data source already exists
	for (int i = 0; i < m_pMainWindow->getProject()->getDataSourceCount(); i++) {
		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSource( i );
		if (pDS && (pDS->type == MDTRA_DT_DISTANCE) && 
			(pDS->streamIndex == pStream->index) &&
			(pDS->arg[0].atomIndex == pTriplet->atX) &&
			(pDS->arg[1].atomIndex == pTriplet->atY[aIndex])) {
				//data source exists
				return pDS;
		}
	}

	const MDTRA_PDB_Atom *pCurrentAtom1 = pStream->pdb->fetchAtomBySerialNumber( pTriplet->atX );
	const MDTRA_PDB_Atom *pCurrentAtom2 = pStream->pdb->fetchAtomBySerialNumber( pTriplet->atY[aIndex] );
	
	//create new data source
	MDTRA_DataArg arg[6];
	memset( arg, 0, sizeof(arg) );
	arg[0].atomIndex = pCurrentAtom1->serialnumber;
	arg[1].atomIndex = pCurrentAtom2->serialnumber;
	MDTRA_Selection emptySel;
	emptySel.string = "";
	emptySel.size = 0;
	emptySel.flags = 0;
	emptySel.data = NULL;
	MDTRA_Prog emptyProg;
	emptyProg.sourceCode = "";
	emptyProg.byteCode = NULL;
	emptyProg.byteCodeSize = 0;

	int iDS = m_pMainWindow->getProject()->registerDataSource(QString("Distance from %1 [%2-%3] to %4 [%5-%6] (%7)").arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom1->trimmed_residue).arg(pCurrentAtom1->residuenumber).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber).arg(pStream->name),
															   pStream->index, MDTRA_DT_DISTANCE, arg, emptySel, emptySel, emptyProg, QString(), 0, true );

	return m_pMainWindow->getProject()->fetchDataSourceByIndex( iDS );
}

const MDTRA_DataSource* MDTRA_HBSearchResultsDialog :: buildDataSourceDistanceHY( const MDTRA_Stream *pStream, int bondIndex, int hIndex, int aIndex )
{
	const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at(bondIndex);

	//check if data source already exists
	for (int i = 0; i < m_pMainWindow->getProject()->getDataSourceCount(); i++) {
		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSource( i );
		if (pDS && (pDS->type == MDTRA_DT_DISTANCE) && 
			(pDS->streamIndex == pStream->index) &&
			(pDS->arg[0].atomIndex == pTriplet->atH[hIndex]) &&
			(pDS->arg[1].atomIndex == pTriplet->atY[aIndex])) {
				//data source exists
				return pDS;
		}
	}

	const MDTRA_PDB_Atom *pCurrentAtom1 = pStream->pdb->fetchAtomBySerialNumber( pTriplet->atH[hIndex] );
	const MDTRA_PDB_Atom *pCurrentAtom2 = pStream->pdb->fetchAtomBySerialNumber( pTriplet->atY[aIndex] );
	
	//create new data source
	MDTRA_DataArg arg[6];
	memset( arg, 0, sizeof(arg) );
	arg[0].atomIndex = pCurrentAtom1->serialnumber;
	arg[1].atomIndex = pCurrentAtom2->serialnumber;
	MDTRA_Selection emptySel;
	emptySel.string = "";
	emptySel.size = 0;
	emptySel.flags = 0;
	emptySel.data = NULL;
	MDTRA_Prog emptyProg;
	emptyProg.sourceCode = "";
	emptyProg.byteCode = NULL;
	emptyProg.byteCodeSize = 0;

	int iDS = m_pMainWindow->getProject()->registerDataSource(QString("Distance from %1 [%2-%3] to %4 [%5-%6] (%7)").arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom1->trimmed_residue).arg(pCurrentAtom1->residuenumber).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber).arg(pStream->name),
															   pStream->index, MDTRA_DT_DISTANCE, arg, emptySel, emptySel, emptyProg, QString(), 0, true );

	return m_pMainWindow->getProject()->fetchDataSourceByIndex( iDS );
}

const MDTRA_DataSource* MDTRA_HBSearchResultsDialog :: buildDataSourceAngleXHY( const MDTRA_Stream *pStream, int bondIndex, int hIndex, int aIndex )
{
	const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at(bondIndex);

	//check if data source already exists
	for (int i = 0; i < m_pMainWindow->getProject()->getDataSourceCount(); i++) {
		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSource( i );
		if (pDS && (pDS->type == MDTRA_DT_ANGLE) && 
			(pDS->streamIndex == pStream->index) &&
			(pDS->arg[0].atomIndex == pTriplet->atX) &&
			(pDS->arg[1].atomIndex == pTriplet->atH[hIndex]) &&
			(pDS->arg[2].atomIndex == pTriplet->atY[aIndex])) {
				//data source exists
				return pDS;
		}
	}

	const MDTRA_PDB_Atom *pCurrentAtom1 = pStream->pdb->fetchAtomBySerialNumber( pTriplet->atX );
	const MDTRA_PDB_Atom *pCurrentAtom2 = pStream->pdb->fetchAtomBySerialNumber( pTriplet->atH[hIndex] );
	const MDTRA_PDB_Atom *pCurrentAtom3 = pStream->pdb->fetchAtomBySerialNumber( pTriplet->atY[aIndex] );
	
	//create new data source
	MDTRA_DataArg arg[6];
	memset( arg, 0, sizeof(arg) );
	arg[0].atomIndex = pCurrentAtom1->serialnumber;
	arg[1].atomIndex = pCurrentAtom2->serialnumber;
	arg[2].atomIndex = pCurrentAtom3->serialnumber;
	MDTRA_Selection emptySel;
	emptySel.string = "";
	emptySel.size = 0;
	emptySel.flags = 0;
	emptySel.data = NULL;
	MDTRA_Prog emptyProg;
	emptyProg.sourceCode = "";
	emptyProg.byteCode = NULL;
	emptyProg.byteCodeSize = 0;

	int iDS = m_pMainWindow->getProject()->registerDataSource(QString("Angle %1 [%2-%3] - %4 [%5-%6] - %7 [%8-%9] (%10)").arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom1->trimmed_residue).arg(pCurrentAtom1->residuenumber).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber).arg(pCurrentAtom3->trimmed_title).arg(pCurrentAtom3->trimmed_residue).arg(pCurrentAtom3->residuenumber).arg(pStream->name),
															   pStream->index, MDTRA_DT_ANGLE, arg, emptySel, emptySel, emptyProg, QString(), 0, true );

	return m_pMainWindow->getProject()->fetchDataSourceByIndex( iDS );
}

const MDTRA_Result* MDTRA_HBSearchResultsDialog :: buildResultCollector( MDTRA_Result *pHost, int hostType, const MDTRA_DataSource *pDS )
{
	const MDTRA_PDB_Atom *pCurrentAtom1 = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pDS->arg[0].atomIndex );
	const MDTRA_PDB_Atom *pCurrentAtom2 = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pDS->arg[1].atomIndex );
	const MDTRA_PDB_Atom *pCurrentAtom3 = NULL;

	if (hostType == MDTRA_DT_ANGLE)
		pCurrentAtom3 = s_lhbsd.pStream->pdb->fetchAtomBySerialNumber( pDS->arg[2].atomIndex );

	QList<MDTRA_DSRef> m_DSRef;

	if (pHost) m_DSRef = pHost->sourceList;

	MDTRA_DSRef ref;
	memset( &ref, 0, sizeof(ref) );
	ref.dataSourceIndex = pDS->index;
	ref.flags = DSREF_FLAG_VISIBLE;
	ref.yscale = 1.0f;
	m_DSRef << ref;

	if (!pHost) {
		//create new collector
		int iRC;
		
		if (hostType == MDTRA_DT_ANGLE) {
			iRC = m_pMainWindow->getProject()->registerResult( QString("Angle %1 [%2-%3] - %4 [%5-%6] - %7 [%8-%9]").arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom1->trimmed_residue).arg(pCurrentAtom1->residuenumber).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber).arg(pCurrentAtom3->trimmed_title).arg(pCurrentAtom3->trimmed_residue).arg(pCurrentAtom3->residuenumber),
																   MDTRA_DT_ANGLE, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME, m_DSRef, true );
		} else {
			iRC = m_pMainWindow->getProject()->registerResult( QString("Distance from %1 [%2-%3] to %4 [%5-%6]").arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom1->trimmed_residue).arg(pCurrentAtom1->residuenumber).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber),
																   MDTRA_DT_DISTANCE, MDTRA_YSU_ANGSTROMS, MDTRA_LAYOUT_TIME, m_DSRef, true );
		}
		pHost = m_pMainWindow->getProject()->fetchResultByIndex( iRC );
	} else {
		//modify references
		m_pMainWindow->getProject()->modifyResult( pHost->index, pHost->name, pHost->type, pHost->units, pHost->layout, m_DSRef );
	}

	return pHost;
}

void MDTRA_HBSearchResultsDialog :: addToNewResultCollectorXY( void )
{
	const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at( m_iCurrentItem );
	const MDTRA_DataSource *pDS = buildDataSourceDistanceXY( s_lhbsd.pStream, m_iCurrentItem, 0 );
	const MDTRA_Result *pResult = buildResultCollector( NULL, MDTRA_DT_DISTANCE, pDS );

	//add additional acceptors, if any
	for (int i = 1; i < MAX_GROUPED_ACCEPTORS; i++) {
		if ( pTriplet->atY[i] >= 0 ) {
			pDS = buildDataSourceDistanceXY( s_lhbsd.pStream, m_iCurrentItem, i );
			buildResultCollector( const_cast<MDTRA_Result*>(pResult), MDTRA_DT_DISTANCE, pDS );
		}
	}

	//modify project
	m_pMainWindow->updateTitleBar( true );

	//modify menu
	QAction *pAction = new QAction( tr("[%1] %2").arg(pResult->index).arg(pResult->name), this );
	pAction->setData( pResult->index );
	connect( pAction, SIGNAL(triggered()), this, SLOT(addToExistingResultCollectorXY()));
	if (m_pSubMenu[0]->actions().count() == 1) m_pSubMenu[0]->addSeparator();
	m_pSubMenu[0]->addAction( pAction );
	if (m_pSubMenu[1]->actions().count() == 1) m_pSubMenu[1]->addSeparator();
	m_pSubMenu[1]->addAction( pAction );
}

void MDTRA_HBSearchResultsDialog :: addToExistingResultCollectorXY( void )
{
	QObject *obj = QObject::sender();
	if (QAction *act = qobject_cast<QAction*>(obj)) {
		int rcIndex = act->data().toInt();
		MDTRA_Result *pRC = m_pMainWindow->getProject()->fetchResultByIndex( rcIndex );
		if (pRC) {
			const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at( m_iCurrentItem );
			const MDTRA_DataSource *pDS = buildDataSourceDistanceXY( s_lhbsd.pStream, m_iCurrentItem, 0 );
			buildResultCollector( pRC, MDTRA_DT_DISTANCE, pDS );

			//add additional acceptors, if any
			for (int i = 1; i < MAX_GROUPED_ACCEPTORS; i++) {
				if ( pTriplet->atY[i] >= 0 ) {
					pDS = buildDataSourceDistanceXY( s_lhbsd.pStream, m_iCurrentItem, i );
					buildResultCollector( pRC, MDTRA_DT_DISTANCE, pDS );
				}
			}

			//modify project
			m_pMainWindow->updateTitleBar( true );
		}
	}
}

void MDTRA_HBSearchResultsDialog :: addToNewResultCollectorHY( void )
{
	const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at( m_iCurrentItem );
	const MDTRA_DataSource *pDS = buildDataSourceDistanceHY( s_lhbsd.pStream, m_iCurrentItem, 0, 0 );
	const MDTRA_Result *pResult = buildResultCollector( NULL, MDTRA_DT_DISTANCE, pDS );

	//add additional hydrogens and acceptors, if any
	for (int i = 0; i < MAX_GROUPED_HYDROGENS; i++) {
		if ( pTriplet->atH[i] < 0 ) continue;
		for (int j = 0; j < MAX_GROUPED_ACCEPTORS; j++) {
			if ( !i && !j ) continue;
			if ( pTriplet->atY[j] < 0 ) continue;
			pDS = buildDataSourceDistanceHY( s_lhbsd.pStream, m_iCurrentItem, i, j );
			buildResultCollector( const_cast<MDTRA_Result*>(pResult), MDTRA_DT_DISTANCE, pDS );
		}
	}

	//modify project
	m_pMainWindow->updateTitleBar( true );

	//modify menu
	QAction *pAction = new QAction( tr("[%1] %2").arg(pResult->index).arg(pResult->name), this );
	pAction->setData( pResult->index );
	connect( pAction, SIGNAL(triggered()), this, SLOT(addToExistingResultCollectorHY()));
	if (m_pSubMenu[0]->actions().count() == 1) m_pSubMenu[0]->addSeparator();
	m_pSubMenu[0]->addAction( pAction );
	if (m_pSubMenu[1]->actions().count() == 1) m_pSubMenu[1]->addSeparator();
	m_pSubMenu[1]->addAction( pAction );
}

void MDTRA_HBSearchResultsDialog :: addToExistingResultCollectorHY( void )
{
	QObject *obj = QObject::sender();
	if (QAction *act = qobject_cast<QAction*>(obj)) {
		int rcIndex = act->data().toInt();
		MDTRA_Result *pRC = m_pMainWindow->getProject()->fetchResultByIndex( rcIndex );
		if (pRC) {
			const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at( m_iCurrentItem );
			const MDTRA_DataSource *pDS = buildDataSourceDistanceHY( s_lhbsd.pStream, m_iCurrentItem, 0, 0 );
			buildResultCollector( pRC, MDTRA_DT_DISTANCE, pDS );

			//add additional hydrogens and acceptors, if any
			for (int i = 0; i < MAX_GROUPED_HYDROGENS; i++) {
				if ( pTriplet->atH[i] < 0 ) continue;
				for (int j = 0; j < MAX_GROUPED_ACCEPTORS; j++) {
					if ( !i && !j ) continue;
					if ( pTriplet->atY[j] < 0 ) continue;
					pDS = buildDataSourceDistanceHY( s_lhbsd.pStream, m_iCurrentItem, i, j );
					buildResultCollector( pRC, MDTRA_DT_DISTANCE, pDS );
				}
			}

			//modify project
			m_pMainWindow->updateTitleBar( true );
		}
	}
}

void MDTRA_HBSearchResultsDialog :: addToNewResultCollectorXHY( void )
{
	const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at( m_iCurrentItem );
	const MDTRA_DataSource *pDS = buildDataSourceAngleXHY( s_lhbsd.pStream, m_iCurrentItem, 0, 0 );
	const MDTRA_Result *pResult = buildResultCollector( NULL, MDTRA_DT_ANGLE, pDS );

	//add additional hydrogens and acceptors, if any
	for (int i = 0; i < MAX_GROUPED_HYDROGENS; i++) {
		if ( pTriplet->atH[i] < 0 ) continue;
		for (int j = 0; j < MAX_GROUPED_ACCEPTORS; j++) {
			if ( !i && !j ) continue;
			if ( pTriplet->atY[j] < 0 ) continue;
			pDS = buildDataSourceAngleXHY( s_lhbsd.pStream, m_iCurrentItem, i, j );
			buildResultCollector( const_cast<MDTRA_Result*>(pResult), MDTRA_DT_ANGLE, pDS );
		}
	}

	//modify project
	m_pMainWindow->updateTitleBar( true );

	//modify menu
	QAction *pAction = new QAction( tr("[%1] %2").arg(pResult->index).arg(pResult->name), this );
	pAction->setData( pResult->index );
	connect( pAction, SIGNAL(triggered()), this, SLOT(addToExistingResultCollectorXHY()));
	if (m_pSubMenu[2]->actions().count() == 1) m_pSubMenu[2]->addSeparator();
	m_pSubMenu[2]->addAction( pAction );
}

void MDTRA_HBSearchResultsDialog :: addToExistingResultCollectorXHY( void )
{
	QObject *obj = QObject::sender();
	if (QAction *act = qobject_cast<QAction*>(obj)) {
		int rcIndex = act->data().toInt();
		MDTRA_Result *pRC = m_pMainWindow->getProject()->fetchResultByIndex( rcIndex );
		if (pRC) {
			const MDTRA_HBSearchTriplet *pTriplet = &s_HBonds.at( m_iCurrentItem );
			const MDTRA_DataSource *pDS = buildDataSourceAngleXHY( s_lhbsd.pStream, m_iCurrentItem, 0, 0 );
			buildResultCollector( pRC, MDTRA_DT_ANGLE, pDS );

			//add additional hydrogens and acceptors, if any
			for (int i = 0; i < MAX_GROUPED_HYDROGENS; i++) {
				if ( pTriplet->atH[i] < 0 ) continue;
				for (int j = 0; j < MAX_GROUPED_ACCEPTORS; j++) {
					if ( !i && !j ) continue;
					if ( pTriplet->atY[j] < 0 ) continue;
					pDS = buildDataSourceAngleXHY( s_lhbsd.pStream, m_iCurrentItem, i, j );
					buildResultCollector( pRC, MDTRA_DT_ANGLE, pDS );
				}
			}

			//modify project
			m_pMainWindow->updateTitleBar( true );
		}
	}
}
