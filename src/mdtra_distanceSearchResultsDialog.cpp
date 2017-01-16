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
//	Implementation of MDTRA_DistanceSearchResultsDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb.h"
#include "mdtra_distanceSearch.h"
#include "mdtra_distanceSearchResultsDialog.h"
#include "mdtra_customTableWidgetItems.h"

#include <QtCore/QTextStream>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>

extern QVector<MDTRA_DistanceSearchPair> s_SignificantPairs;
extern MDTRA_DistanceSearchData s_ldsd[2];

MDTRA_DistanceSearchResultsDialog :: MDTRA_DistanceSearchResultsDialog( QWidget *parent )
								   : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/ds.png") );

	QStringList hrzHeader;
	hrzHeader << tr("First Atom");
	hrzHeader << tr("Second Atom");
	hrzHeader << tr("Stream1 Value");
	hrzHeader << tr("Stream2 Value");
	hrzHeader << tr("Difference");
	hrzHeader << tr("Options");
	resultTable->setHorizontalHeaderLabels(hrzHeader);
	resultTable->setRowCount( MDTRA_MAX( 15, s_SignificantPairs.count() ) );

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QApplication::processEvents();

	for (int i = 0; i < s_SignificantPairs.count(); i++) {
		const MDTRA_DistanceSearchPair *pPair = &s_SignificantPairs.at(i);
		const MDTRA_PDB_Atom *pCurrentAtom1 = s_ldsd[0].pStream->pdb->fetchAtomByIndex( pPair->atom1[0] );
		const MDTRA_PDB_Atom *pCurrentAtom2 = s_ldsd[0].pStream->pdb->fetchAtomByIndex( pPair->atom2[0] );
		QTableWidgetItem_SortByUserRole_i *pItemi;
		QTableWidgetItem_SortByUserRole_f *pItemf;
		QPushButtonWithTag *pButton;

		pItemi = new QTableWidgetItem_SortByUserRole_i(QString("%1-%2 %3").arg(pCurrentAtom1->trimmed_residue).arg(pCurrentAtom1->residuenumber).arg(pCurrentAtom1->trimmed_title));
		pItemi->setData(Qt::UserRole, pCurrentAtom1->serialnumber);
		pItemi->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(i, 0, pItemi);

		pItemi = new QTableWidgetItem_SortByUserRole_i(QString("%1-%2 %3").arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber).arg(pCurrentAtom2->trimmed_title));
		pItemi->setData(Qt::UserRole, pCurrentAtom2->serialnumber);
		pItemi->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(i, 1, pItemi);

		pItemf = new QTableWidgetItem_SortByUserRole_f();
		pItemf->setData(Qt::UserRole, pPair->value1);
		pItemf->setData(Qt::DisplayRole, pPair->value1);
		pItemf->setTextAlignment( Qt::AlignCenter );
		pItemf->setToolTip( s_ldsd[0].pStream->name );
		resultTable->setItem(i, 2, pItemf);

		pItemf = new QTableWidgetItem_SortByUserRole_f();
		pItemf->setData(Qt::UserRole, pPair->value2);
		pItemf->setData(Qt::DisplayRole, pPair->value2);
		pItemf->setTextAlignment( Qt::AlignCenter );
		pItemf->setToolTip( s_ldsd[1].pStream->name );
		resultTable->setItem(i, 3, pItemf);

		pItemf = new QTableWidgetItem_SortByUserRole_f();
		pItemf->setData(Qt::UserRole, pPair->diff);
		pItemf->setData(Qt::DisplayRole, pPair->diff);
		pItemf->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(i, 4, pItemf);

		pButton = new QPushButtonWithTag( i, tr("Add..."), resultTable );
		resultTable->setCellWidget(i, 5, pButton);
		connect(pButton, SIGNAL(clicked()), this, SLOT(exec_on_result_add()));
	}

	QApplication::restoreOverrideCursor();

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_save()));
	connect(resultTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(exec_on_header_clicked(int)));
	createPopupMenu();
}

void MDTRA_DistanceSearchResultsDialog :: exec_on_result_add( void )
{
	for (int row = 0; row < resultTable->rowCount(); row++) {
		if (sender() == resultTable->cellWidget(row, 5)) {
			QPushButtonWithTag *pButton = qobject_cast<QPushButtonWithTag*>(sender());
			m_iCurrentItem = pButton->getTag();
			QPoint p( 0, pButton->height() );
			m_pPopupMenu->popup( pButton->mapToGlobal( p ) );
		}
	}
}

void MDTRA_DistanceSearchResultsDialog :: exec_on_header_clicked( int headerIndex )
{
	if (headerIndex < 5)
		resultTable->sortByColumn( headerIndex );
}

void MDTRA_DistanceSearchResultsDialog :: createPopupMenu( void )
{
	QAction *pAction;

	m_pPopupMenu = new QMenu( this );

	pAction = new QAction( tr("New Result Collector..."), this );
	connect( pAction, SIGNAL(triggered()), this, SLOT(addToNewResultCollector()));
	m_pPopupMenu->addAction( pAction );

	if (m_pMainWindow->getProject()->getResultCountByType( MDTRA_DT_DISTANCE ) > 0) {
		m_pPopupMenu->addSeparator();

		for (int i = 0; i < m_pMainWindow->getProject()->getResultCount(); i++) {
			MDTRA_Result *pResult = m_pMainWindow->getProject()->fetchResult( i );
			if (pResult && (pResult->type == MDTRA_DT_DISTANCE)) {
				pAction = new QAction( tr("[%1] %2").arg(pResult->index).arg(pResult->name), this );
				pAction->setData( pResult->index );
				connect( pAction, SIGNAL(triggered()), this, SLOT(addToExistingResultCollector()));
				m_pPopupMenu->addAction( pAction );
			}
		}
	}
}

const MDTRA_DataSource* MDTRA_DistanceSearchResultsDialog :: buildDataSource( const MDTRA_Stream *pStream, int streamIndex, int pairIndex )
{
	const MDTRA_DistanceSearchPair *pPair = &s_SignificantPairs.at(pairIndex);
	const MDTRA_PDB_Atom *pCurrentAtom1 = pStream->pdb->fetchAtomByIndex( pPair->atom1[streamIndex] );
	const MDTRA_PDB_Atom *pCurrentAtom2 = pStream->pdb->fetchAtomByIndex( pPair->atom2[streamIndex] );

	//check if data source already exists
	for (int i = 0; i < m_pMainWindow->getProject()->getDataSourceCount(); i++) {
		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSource( i );
		if (pDS && (pDS->type == MDTRA_DT_DISTANCE) && 
			(pDS->streamIndex == pStream->index) &&
			(pDS->arg[0].atomIndex == pCurrentAtom1->serialnumber) &&
			(pDS->arg[1].atomIndex == pCurrentAtom2->serialnumber)) {
				//data source exists
				return pDS;
		}
	}
	
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

const MDTRA_Result* MDTRA_DistanceSearchResultsDialog :: buildResultCollector( MDTRA_Result *pHost, const MDTRA_DataSource *pDS1, const MDTRA_DataSource *pDS2, int pairIndex )
{
	const MDTRA_DistanceSearchPair *pPair = &s_SignificantPairs.at(pairIndex);
	const MDTRA_PDB_Atom *pCurrentAtom1 = s_ldsd[0].pStream->pdb->fetchAtomByIndex( pPair->atom1[0] );
	const MDTRA_PDB_Atom *pCurrentAtom2 = s_ldsd[0].pStream->pdb->fetchAtomByIndex( pPair->atom2[0] );
	QList<MDTRA_DSRef> m_DSRef;

	if (pHost) m_DSRef = pHost->sourceList;

	MDTRA_DSRef ref;
	memset( &ref, 0, sizeof(ref) );
	ref.dataSourceIndex = pDS1->index;
	ref.flags = DSREF_FLAG_VISIBLE;
	ref.yscale = 1.0f;
	m_DSRef << ref;
	memset( &ref, 0, sizeof(ref) );
	ref.dataSourceIndex = pDS2->index;
	ref.flags = DSREF_FLAG_VISIBLE;
	ref.yscale = 1.0f;
	m_DSRef << ref;

	if (!pHost) {
		//create new collector
		int iRC = m_pMainWindow->getProject()->registerResult( QString("Distance from %1 [%2-%3] to %4 [%5-%6]").arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom1->trimmed_residue).arg(pCurrentAtom1->residuenumber).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber),
															   MDTRA_DT_DISTANCE, MDTRA_YSU_ANGSTROMS, MDTRA_LAYOUT_TIME, m_DSRef, true );
		pHost = m_pMainWindow->getProject()->fetchResultByIndex( iRC );
	} else {
		//modify references
		m_pMainWindow->getProject()->modifyResult( pHost->index, pHost->name, pHost->type, pHost->units, pHost->layout, m_DSRef );
	}

	return pHost;
}

void MDTRA_DistanceSearchResultsDialog :: addToNewResultCollector( void )
{
	const MDTRA_DataSource *pDS1 = buildDataSource( s_ldsd[0].pStream, 0, m_iCurrentItem );
	const MDTRA_DataSource *pDS2 = buildDataSource( s_ldsd[1].pStream, 1, m_iCurrentItem );
	const MDTRA_Result *pResult = buildResultCollector( NULL, pDS1, pDS2, m_iCurrentItem );

	//modify project
	m_pMainWindow->updateTitleBar( true );

	//modify menu
	QAction *pAction = new QAction( tr("[%1] %2").arg(pResult->index).arg(pResult->name), this );
	pAction->setData( pResult->index );
	connect( pAction, SIGNAL(triggered()), this, SLOT(addToExistingResultCollector()));
	if (m_pPopupMenu->actions().count() == 1) m_pPopupMenu->addSeparator();
	m_pPopupMenu->addAction( pAction );
}

void MDTRA_DistanceSearchResultsDialog :: addToExistingResultCollector( void )
{
	QObject *obj = QObject::sender();
	if (QAction *act = qobject_cast<QAction*>(obj)) {
		int rcIndex = act->data().toInt();
		MDTRA_Result *pRC = m_pMainWindow->getProject()->fetchResultByIndex( rcIndex );
		if (pRC) {
			const MDTRA_DataSource *pDS1 = buildDataSource( s_ldsd[0].pStream, 0, m_iCurrentItem );
			const MDTRA_DataSource *pDS2 = buildDataSource( s_ldsd[1].pStream, 1, m_iCurrentItem );
			buildResultCollector( pRC, pDS1, pDS2, m_iCurrentItem );

			//modify project
			m_pMainWindow->updateTitleBar( true );
		}
	}
}

void MDTRA_DistanceSearchResultsDialog :: exportResults( const QString &fileSuffix, QTextStream *stream )
{
	bool bCSV = false;
	if (fileSuffix.toLower() == "csv") {
		bCSV = true;
	}

	if (bCSV)
		*stream << QString("\"\";\"First Atom\";\"\";\"Second Atom\";\"Stream1 Value\";\"Stream2 Value\";\"Difference\"");
	else
		*stream << QString("\t\"First Atom\"\t\t\"Second Atom\"\t\"Stream1 Value\"\t\"Stream2 Value\"\t\"Difference\"");
	*stream << endl;

	for (int i = 0; i < s_SignificantPairs.count(); i++) {
		const MDTRA_DistanceSearchPair *pPair = &s_SignificantPairs.at(i);
		const MDTRA_PDB_Atom *pCurrentAtom1 = s_ldsd[0].pStream->pdb->fetchAtomByIndex( pPair->atom1[0] );
		const MDTRA_PDB_Atom *pCurrentAtom2 = s_ldsd[0].pStream->pdb->fetchAtomByIndex( pPair->atom2[0] );

		QString sAt1 = tr("%1-%2 %3").arg(pCurrentAtom1->trimmed_residue).arg(pCurrentAtom1->residuenumber).arg(pCurrentAtom1->trimmed_title);
		QString sAt2 = tr("%1-%2 %3").arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber).arg(pCurrentAtom2->trimmed_title);

		if (bCSV) {
			*stream << QString("%1;\"%2\";%3;\"%4\";%5;%6;%7")
				.arg(pPair->atom1[0]).arg(sAt1)
				.arg(pPair->atom2[0]).arg(sAt2)
				.arg(pPair->value1, 0, 'f', 6).arg(pPair->value2, 0, 'f', 6).arg(pPair->diff, 0, 'f', 6);
		} else {
			*stream << QString("%1\t\"%2\"\t%3\t\"%4\"\t%5\t%6\t%7")
				.arg(pPair->atom1[0]).arg(sAt1)
				.arg(pPair->atom2[0]).arg(sAt2)
				.arg(pPair->value1, 0, 'f', 6).arg(pPair->value2, 0, 'f', 6).arg(pPair->diff, 0, 'f', 6);
		}
		*stream << endl;
	}
}

void MDTRA_DistanceSearchResultsDialog :: exec_save( void )
{
	QString exportFilter = "Text Files (*.txt);;CSV Files (*.csv)";
	QString selectedFilter;
	QMap<QString,QString> extMap;
	extMap["Text Files (*.txt)"] = ".txt";
	extMap["CSV Files (*.csv)"] = ".csv";

	QString fileName = QFileDialog::getSaveFileName( this, tr("Save Distance Search Results"), m_pMainWindow->getCurrentFileDir(), exportFilter, &selectedFilter );
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