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
//	Implementation of MDTRA_TorsionSearchResultsDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb.h"
#include "mdtra_torsionSearch.h"
#include "mdtra_torsionSearchResultsDialog.h"
#include "mdtra_customTableWidgetItems.h"

#include <QtCore/QTextStream>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>

extern QVector<MDTRA_TorsionSearchDef> s_TorsionsList;
extern int s_TorsionListValidCount;
extern MDTRA_TorsionSearchData s_ltsd[2];

MDTRA_TorsionSearchResultsDialog :: MDTRA_TorsionSearchResultsDialog( QWidget *parent )
								   : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/ts.png") );

	QStringList hrzHeader;
	hrzHeader << tr("Torsion Angle");
	hrzHeader << tr("Residue");
	hrzHeader << tr("Title");
	hrzHeader << tr("Stream1 Value");
	hrzHeader << tr("Stream2 Value");
	hrzHeader << tr("Difference");
	hrzHeader << tr("Options");
	resultTable->setHorizontalHeaderLabels(hrzHeader);
	resultTable->setRowCount( MDTRA_MAX( 15, s_TorsionListValidCount ) );

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QApplication::processEvents();

	int c = 0;

	for (int i = 0; i < s_TorsionsList.count(); i++) {
		const MDTRA_TorsionSearchDef *pDef = &s_TorsionsList.at(i);
		if ( !(pDef->flags & TSDF_VALID ) )
			continue;

		const MDTRA_PDB_Atom *pCurrentAtom1 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom1[0] );
		const MDTRA_PDB_Atom *pCurrentAtom2 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom2[0] );
		const MDTRA_PDB_Atom *pCurrentAtom3 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom3[0] );
		const MDTRA_PDB_Atom *pCurrentAtom4 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom4[0] );
		const MDTRA_PDB_Atom *pCurrentAtom2a = s_ltsd[1].pStream->pdb->fetchAtomBySerialNumber( pDef->atom2[1] );

		QTableWidgetItem *pItem;
		QTableWidgetItem_SortByUserRole_i *pItemi;
		QTableWidgetItem_SortByUserRole_f *pItemf;
		QPushButtonWithTag *pButton;

		if ( pDef->atom4alt[0] < 0 ) {
			pItemi = new QTableWidgetItem_SortByUserRole_i(QString("%1-%2-%3-%4").arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom3->trimmed_title).arg(pCurrentAtom4->trimmed_title));
		} else {
			const MDTRA_PDB_Atom *pCurrentAtom4Alt = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom4alt[0] );
			pItemi = new QTableWidgetItem_SortByUserRole_i(QString("%1-%2-%3-%4/%5").arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom3->trimmed_title).arg(pCurrentAtom4->trimmed_title).arg(pCurrentAtom4Alt->trimmed_title));
		}
		pItemi->setData(Qt::UserRole, pCurrentAtom1->serialnumber);
		pItemi->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(c, 0, pItemi);

		if (!strcmp( pCurrentAtom2->trimmed_residue, pCurrentAtom2a->trimmed_residue ))
			pItem = new QTableWidgetItem(QString("%1-%2").arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber));
		else
			pItem = new QTableWidgetItem(QString("%1/%2-%3").arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2a->trimmed_residue).arg(pCurrentAtom2->residuenumber));
		pItem->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(c, 1, pItem);

		if (pDef->title)
			pItem = new QTableWidgetItem(QString("%1").arg(pDef->title));
		else
			pItem = new QTableWidgetItem();
		pItem->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(c, 2, pItem);

		pItemf = new QTableWidgetItem_SortByUserRole_f();
		pItemf->setData(Qt::UserRole, pDef->value1);
		pItemf->setData(Qt::DisplayRole, pDef->value1);
		pItemf->setTextAlignment( Qt::AlignCenter );
		pItemf->setToolTip( s_ltsd[0].pStream->name );
		resultTable->setItem(c, 3, pItemf);

		pItemf = new QTableWidgetItem_SortByUserRole_f();
		pItemf->setData(Qt::UserRole, pDef->value2);
		pItemf->setData(Qt::DisplayRole, pDef->value2);
		pItemf->setTextAlignment( Qt::AlignCenter );
		pItemf->setToolTip( s_ltsd[1].pStream->name );
		resultTable->setItem(c, 4, pItemf);

		pItemf = new QTableWidgetItem_SortByUserRole_f();
		pItemf->setData(Qt::UserRole, pDef->diff);
		pItemf->setData(Qt::DisplayRole, pDef->diff);
		pItemf->setTextAlignment( Qt::AlignCenter );
		resultTable->setItem(c, 5, pItemf);

		pButton = new QPushButtonWithTag( i, tr("Add..."), resultTable );
		resultTable->setCellWidget(c, 6, pButton);
		connect(pButton, SIGNAL(clicked()), this, SLOT(exec_on_result_add()));
		c++;
	}

	QApplication::restoreOverrideCursor();

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_save()));
	connect(resultTable->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(exec_on_header_clicked(int)));
	createPopupMenu();
}

void MDTRA_TorsionSearchResultsDialog :: exec_on_result_add( void )
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

void MDTRA_TorsionSearchResultsDialog :: exec_on_header_clicked( int headerIndex )
{
	if (headerIndex < 6)
		resultTable->sortByColumn( headerIndex );
}

void MDTRA_TorsionSearchResultsDialog :: createPopupMenu( void )
{
	QAction *pAction;

	m_pPopupMenu = new QMenu( this );

	pAction = new QAction( tr("New Result Collector..."), this );
	connect( pAction, SIGNAL(triggered()), this, SLOT(addToNewResultCollector()));
	m_pPopupMenu->addAction( pAction );

	if ((m_pMainWindow->getProject()->getResultCountByType( MDTRA_DT_TORSION ) + m_pMainWindow->getProject()->getResultCountByType( MDTRA_DT_TORSION_UNSIGNED )) > 0) {
		m_pPopupMenu->addSeparator();

		for (int i = 0; i < m_pMainWindow->getProject()->getResultCount(); i++) {
			MDTRA_Result *pResult = m_pMainWindow->getProject()->fetchResult( i );
			if (pResult && (pResult->type == MDTRA_DT_TORSION || pResult->type == MDTRA_DT_TORSION_UNSIGNED)) {
				pAction = new QAction( tr("[%1] %2").arg(pResult->index).arg(pResult->name), this );
				pAction->setData( pResult->index );
				connect( pAction, SIGNAL(triggered()), this, SLOT(addToExistingResultCollector()));
				m_pPopupMenu->addAction( pAction );
			}
		}
	}
}

const MDTRA_DataSource* MDTRA_TorsionSearchResultsDialog :: buildDataSource( const MDTRA_Stream *pStream, int streamIndex, int taIndex )
{
	const MDTRA_TorsionSearchDef *pDef = &s_TorsionsList.at(taIndex);
	const MDTRA_PDB_Atom *pCurrentAtom1 = pStream->pdb->fetchAtomBySerialNumber( pDef->atom1[streamIndex] );
	const MDTRA_PDB_Atom *pCurrentAtom2 = pStream->pdb->fetchAtomBySerialNumber( pDef->atom2[streamIndex] );
	const MDTRA_PDB_Atom *pCurrentAtom3 = pStream->pdb->fetchAtomBySerialNumber( pDef->atom3[streamIndex] );
	const MDTRA_PDB_Atom *pCurrentAtom4 = pStream->pdb->fetchAtomBySerialNumber( pDef->atom4[streamIndex] );

	//check if data source already exists
	for (int i = 0; i < m_pMainWindow->getProject()->getDataSourceCount(); i++) {
		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSource( i );
		if (pDS && (pDS->type == MDTRA_DT_TORSION_UNSIGNED) && 
			(pDS->streamIndex == pStream->index) &&
			(pDS->arg[0].atomIndex == pCurrentAtom1->serialnumber) &&
			(pDS->arg[1].atomIndex == pCurrentAtom2->serialnumber) &&
			(pDS->arg[2].atomIndex == pCurrentAtom3->serialnumber) &&
			(pDS->arg[3].atomIndex == pCurrentAtom4->serialnumber)) {
				//data source exists
				return pDS;
		}
	}
	
	//create new data source
	MDTRA_DataArg arg[6];
	memset( arg, 0, sizeof(arg) );
	arg[0].atomIndex = pCurrentAtom1->serialnumber;
	arg[1].atomIndex = pCurrentAtom2->serialnumber;
	arg[2].atomIndex = pCurrentAtom3->serialnumber;
	arg[3].atomIndex = pCurrentAtom4->serialnumber;
	MDTRA_Selection emptySel;
	emptySel.string = "";
	emptySel.size = 0;
	emptySel.flags = 0;
	emptySel.data = NULL;
	MDTRA_Prog emptyProg;
	emptyProg.sourceCode = "";
	emptyProg.byteCode = NULL;
	emptyProg.byteCodeSize = 0;

	QString dsTitle;
	if ( pDef->title )
		dsTitle = QString("Torsion angle %1 in %2-%3 [%4-%5-%6-%7] (%8)").arg(pDef->title).arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber).arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom3->trimmed_title).arg(pCurrentAtom4->trimmed_title).arg(pStream->name);
	else
		dsTitle = QString("Torsion angle in %1-%2 [%3-%4-%5-%6] (%7)").arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber).arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom3->trimmed_title).arg(pCurrentAtom4->trimmed_title).arg(pStream->name);

	int iDS = m_pMainWindow->getProject()->registerDataSource( dsTitle, pStream->index, MDTRA_DT_TORSION_UNSIGNED, arg, emptySel, emptySel, emptyProg, QString(), 0, true );

	return m_pMainWindow->getProject()->fetchDataSourceByIndex( iDS );
}

const MDTRA_Result* MDTRA_TorsionSearchResultsDialog :: buildResultCollector( MDTRA_Result *pHost, const MDTRA_DataSource *pDS1, const MDTRA_DataSource *pDS2, int taIndex )
{
	const MDTRA_TorsionSearchDef *pDef = &s_TorsionsList.at(taIndex);
	const MDTRA_PDB_Atom *pCurrentAtom1 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom1[0] );
	const MDTRA_PDB_Atom *pCurrentAtom2 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom2[0] );
	const MDTRA_PDB_Atom *pCurrentAtom3 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom3[0] );
	const MDTRA_PDB_Atom *pCurrentAtom4 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom4[0] );
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
		QString rcTitle;
		if ( pDef->title )
			rcTitle = QString("Torsion angle %1 in %2-%3 [%4-%5-%6-%7]").arg(pDef->title).arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber).arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom3->trimmed_title).arg(pCurrentAtom4->trimmed_title);
		else
			rcTitle = QString("Torsion angle in %1-%2 [%3-%4-%5-%6]").arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber).arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom3->trimmed_title).arg(pCurrentAtom4->trimmed_title);

		int iRC = m_pMainWindow->getProject()->registerResult( rcTitle, MDTRA_DT_TORSION_UNSIGNED, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME, m_DSRef, true );
		pHost = m_pMainWindow->getProject()->fetchResultByIndex( iRC );
	} else {
		//modify references
		m_pMainWindow->getProject()->modifyResult( pHost->index, pHost->name, pHost->type, pHost->units, pHost->layout, m_DSRef );
	}

	return pHost;
}

void MDTRA_TorsionSearchResultsDialog :: addToNewResultCollector( void )
{
	const MDTRA_DataSource *pDS1 = buildDataSource( s_ltsd[0].pStream, 0, m_iCurrentItem );
	const MDTRA_DataSource *pDS2 = buildDataSource( s_ltsd[1].pStream, 1, m_iCurrentItem );
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

void MDTRA_TorsionSearchResultsDialog :: addToExistingResultCollector( void )
{
	QObject *obj = QObject::sender();
	if (QAction *act = qobject_cast<QAction*>(obj)) {
		int rcIndex = act->data().toInt();
		MDTRA_Result *pRC = m_pMainWindow->getProject()->fetchResultByIndex( rcIndex );
		if (pRC) {
			const MDTRA_DataSource *pDS1 = buildDataSource( s_ltsd[0].pStream, 0, m_iCurrentItem );
			const MDTRA_DataSource *pDS2 = buildDataSource( s_ltsd[1].pStream, 1, m_iCurrentItem );
			buildResultCollector( pRC, pDS1, pDS2, m_iCurrentItem );

			//modify project
			m_pMainWindow->updateTitleBar( true );
		}
	}
}

void MDTRA_TorsionSearchResultsDialog :: exportResultsToRasMolSelection( QTextStream *stream )
{
	QSet<int> residueSet;
	QList<int> residueList;

	for (int i = 0; i < s_TorsionsList.count(); i++) {
		const MDTRA_TorsionSearchDef *pDef = &s_TorsionsList.at(i);
		if ( !(pDef->flags & TSDF_VALID ) )
			continue;

		const MDTRA_PDB_Atom *pAtom2 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom2[0] );
		residueSet << pAtom2->residuenumber;
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

void MDTRA_TorsionSearchResultsDialog :: exportResults( const QString &fileSuffix, QTextStream *stream )
{
	bool bCSV = false;
	if (fileSuffix.toLower() == "csv") {
		bCSV = true;
	}
	else if (fileSuffix.toLower() == "spt") {
		exportResultsToRasMolSelection( stream );
		return;
	} 

	if (bCSV)
		*stream << QString("\"\";\"Torsion Angle\";\"Residue\";\"Title\";\"Stream1 Value\";\"Stream2 Value\";\"Difference\"");
	else
		*stream << QString("\t\"Torsion Angle\"\t\"Residue\"\t\"Title\"\t\"Stream1 Value\"\t\"Stream2 Value\"\t\"Difference\"");
	*stream << endl;

	int c = 1;

	for (int i = 0; i < s_TorsionsList.count(); i++) {
		const MDTRA_TorsionSearchDef *pDef = &s_TorsionsList.at(i);
		if ( !(pDef->flags & TSDF_VALID ) )
			continue;

		const MDTRA_PDB_Atom *pCurrentAtom1 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom1[0] );
		const MDTRA_PDB_Atom *pCurrentAtom2 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom2[0] );
		const MDTRA_PDB_Atom *pCurrentAtom3 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom3[0] );
		const MDTRA_PDB_Atom *pCurrentAtom4 = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom4[0] );
		const MDTRA_PDB_Atom *pCurrentAtom2a = s_ltsd[1].pStream->pdb->fetchAtomBySerialNumber( pDef->atom2[1] );

		QString taTitle;
		QString resTitle;
		QString aTitle("");

		if ( pDef->atom4alt[0] < 0 ) {
			taTitle = QString("%1-%2-%3-%4").arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom3->trimmed_title).arg(pCurrentAtom4->trimmed_title);
		} else {
			const MDTRA_PDB_Atom *pCurrentAtom4Alt = s_ltsd[0].pStream->pdb->fetchAtomBySerialNumber( pDef->atom4alt[0] );
			taTitle = QString("%1-%2-%3-%4/%5").arg(pCurrentAtom1->trimmed_title).arg(pCurrentAtom2->trimmed_title).arg(pCurrentAtom3->trimmed_title).arg(pCurrentAtom4->trimmed_title).arg(pCurrentAtom4Alt->trimmed_title);
		}
		if (!strcmp( pCurrentAtom2->trimmed_residue, pCurrentAtom2a->trimmed_residue )) {
			resTitle = QString("%1-%2").arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2->residuenumber);
		} else {
			resTitle = QString("%1/%2-%3").arg(pCurrentAtom2->trimmed_residue).arg(pCurrentAtom2a->trimmed_residue).arg(pCurrentAtom2->residuenumber);
		}

		if (pDef->title) aTitle = QString("%1").arg(pDef->title);
		
		if (bCSV) {
			*stream << QString("%1;\"%2\";\"%3\";\"%4\";%5;%6;%7")
				.arg(c).arg(taTitle).arg(resTitle).arg(aTitle)
				.arg(pDef->value1, 0, 'f', 6).arg(pDef->value2, 0, 'f', 6).arg(pDef->diff, 0, 'f', 6);
		} else {
			*stream << QString("%1\t\"%2\"\t\"%3\"\t\"%4\"\t%5\t%6\t%7")
				.arg(c).arg(taTitle).arg(resTitle).arg(aTitle)
				.arg(pDef->value1, 0, 'f', 6).arg(pDef->value2, 0, 'f', 6).arg(pDef->diff, 0, 'f', 6);
		}
		*stream << endl;
		c++;
	}
}

void MDTRA_TorsionSearchResultsDialog :: exec_save( void )
{
	QString exportFilter = "Text Files (*.txt);;CSV Files (*.csv);;RasMol Selection (*.spt)";
	QString selectedFilter;
	QMap<QString,QString> extMap;
	extMap["Text Files (*.txt)"] = ".txt";
	extMap["CSV Files (*.csv)"] = ".csv";
	extMap["RasMol Selection (*.spt)"] = ".spt";

	QString fileName = QFileDialog::getSaveFileName( this, tr("Save Torsion Search Results"), m_pMainWindow->getCurrentFileDir(), exportFilter, &selectedFilter );
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