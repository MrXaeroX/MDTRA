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
//	Implementation of MDTRA_ProteinDataMiningDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb.h"
#include "mdtra_utils.h"
#include "mdtra_math.h"
#include "mdtra_select.h"
#include "mdtra_inputTextDialog.h"
#include "mdtra_proteinDataMiningDialog.h"

#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QFileDialog>
#include <QtGui/QKeyEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>

#define TPDF_ATOM1_FROM_PREV_RESIDUE		(1<<0)
#define TPDF_ATOM4_FROM_NEXT_RESIDUE		(1<<1)

typedef struct stMDTRA_ProteinTorsionParmDesc
{
	const char *title;
	const char *arg1;
	const char *arg2;
	const char *arg3;
	const char *arg4;
	int flags;
	MDTRA_DataType type;
	MDTRA_YScaleUnits yscale;
	MDTRA_Layout layout;
	QCheckBox *pCheckBox;
} MDTRA_ProteinTorsionParmDesc;

static MDTRA_ProteinTorsionParmDesc s_ProteinTorsionParmDesc[] = 
{
{ "Phi", "C", "N", "CA", "C", TPDF_ATOM1_FROM_PREV_RESIDUE,	MDTRA_DT_TORSION_UNSIGNED, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Psi", "N", "CA",	"C", "N", TPDF_ATOM4_FROM_NEXT_RESIDUE,	MDTRA_DT_TORSION_UNSIGNED, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
};

MDTRA_ProteinDataMiningDialog :: MDTRA_ProteinDataMiningDialog( QWidget *parent )
							   : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/pdm.png") );

#ifndef QT_NO_WHATSTHIS
	sel_string->setWhatsThis(QString("<b>Selection Quick Hint</b><br><br>atomno=100 <i>select single atom</i><br>atomno&gt;=100 and atomno&lt;=200 <i>select range of atoms</i><br>carbon <i>select all carbons</i><br>:a <i>select chain A</i><br>*.ca <i>select all alpha-carbons</i><br>lys.cg <i>select all gamma-carbons in lysine residues</i><br>:a.cg <i>select all gamma-carbons in chain A</i><br>1-3.ca <i>select alpha-carbons in residues 1, 2 and 3</i><br>*.h? <i>select hydrogens with 2-character name</i><br>*.h[12] <i>select hydrogens named H1 and H2</i><br>*.h[1-3] <i>select hydrogens named H1, H2 and H3</i><br>*.h* <i>select all hydrogens</i><br>lys <i>select all lysines (LYS)</i><br>{lys} <i>select all lysines, including neutral (LYS, LYN)</i><br>gl? <i>select all glutamates and glutamines</i><br>(lys, arg) and :b <i>select lysines and arginines in chain B</i><br>protein <i>select all protein atoms</i><br>dna <i>select all nucleic atoms (may also specify \"nucleic\" keyword)</i><br>water and *.o <i>select all oxygen atoms in water molecules</i><br>within (3.5, lys) <i>select all atoms within 3.5 angstroms of any lysine residue</i><br>not dna within (3.0, dna) <i>select non-nucleic atoms within 3.0 angstroms of nucleic acid</i>"));
#endif // QT_NO_WHATSTHIS

	m_iSelectionFlags = 0;
	m_iSelectionSize = 0;
	m_pSelectionData = NULL;
	m_bSelectionError = false;
	sel_string->clear();
	m_cachedSelectionText.clear();
	m_pSelectionParser = new MDTRA_SelectionParser<MDTRA_PDB_File>;
	sel_parse->setIcon( QIcon(":/png/16x16/csel.png") );
	m_changingCBState = true;

	//fill stream combo
	for (int i = 0; i < m_pMainWindow->getProject()->getStreamCount(); i++) {
		MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStream( i );
		if (pStream && pStream->pdb) {
			QListWidgetItem *pItem = new QListWidgetItem( tr("STREAM %1: %2 (%3 files)").arg(pStream->index).arg(pStream->name).arg(pStream->files.count()), 
														  sList );
			pItem->setIcon( QIcon(":/png/16x16/stream.png") );
			pItem->setData( Qt::UserRole, pStream->index );
		}
	}

	if (sList->count() > 0)
		sList->setCurrentRow(0);

	m_cachedStreamIndex = 0;

	sel_string->setText("protein");
	exec_on_selection_editingFinished();


	//fill DNA parameters
	int numTorsionParms = sizeof(s_ProteinTorsionParmDesc) / sizeof(s_ProteinTorsionParmDesc[0]);
	QGroupBox *torsionGroup = new QGroupBox( tr("Protein Torsion Angles"), scrollArea );
	torsionGroup->setCheckable( true );
	torsionGroup->setChecked( true );
	QGridLayout *torsionGroupGrid = new QGridLayout( torsionGroup );
	QCheckBox *pAllCheckBox = new QCheckBox( tr("(All)"), torsionGroup );
	pAllCheckBox->setChecked( true );
	torsionGroupGrid->addWidget( pAllCheckBox, 0, 0, Qt::AlignTop );
	connect(pAllCheckBox, SIGNAL(stateChanged(int)), this, SLOT(exec_on_select_all_or_none()));

	for (int i = 0; i < numTorsionParms; i++) {
		s_ProteinTorsionParmDesc[i].pCheckBox = new QCheckBox( s_ProteinTorsionParmDesc[i].title, torsionGroup );
		if ( s_ProteinTorsionParmDesc[i].pCheckBox ) {
			s_ProteinTorsionParmDesc[i].pCheckBox->setChecked( true );
			connect(s_ProteinTorsionParmDesc[i].pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(exec_on_select_something()));
			torsionGroupGrid->addWidget( s_ProteinTorsionParmDesc[i].pCheckBox, (i+1) / 4, (i+1) % 4, Qt::AlignTop );
		}
	}

	saLayout->addWidget(torsionGroup);

	connect(sList, SIGNAL(itemSelectionChanged()), this, SLOT(exec_on_update_stream()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_on_accept()));
	connect(sel_string, SIGNAL(editingFinished()), this, SLOT(exec_on_selection_editingFinished()));
	connect(sel_parse, SIGNAL(clicked()), this, SLOT(exec_on_selection_editingFinished()));
	connect(sel_labelResults, SIGNAL(linkActivated(const QString&)), this, SLOT(exec_on_selection_showWholeSelection()));
	m_changingCBState = false;
}

MDTRA_ProteinDataMiningDialog :: ~MDTRA_ProteinDataMiningDialog()
{
	if (m_pSelectionParser) {
		delete m_pSelectionParser;
		m_pSelectionParser = NULL;
	}
	if (m_pSelectionData) {
		delete [] m_pSelectionData;
		m_pSelectionData = NULL;
	}
}

bool MDTRA_ProteinDataMiningDialog :: event( QEvent *ev )
{
	if (ev->type() == QEvent::KeyPress) {
		QKeyEvent *kev = static_cast<QKeyEvent*>(ev);
		if (kev->key() == Qt::Key_Return) {
			if (sel_string->hasFocus()) {
				return true;
			}
		}
	}
	return QDialog::event( ev );
}

void MDTRA_ProteinDataMiningDialog :: exec_on_select_all_or_none( void )
{
	if (m_changingCBState)
		return;

	m_changingCBState = true;

	QObject *obj = QObject::sender();
	if (QCheckBox *cb = qobject_cast<QCheckBox*>(obj)) {
		QWidget *parent = cb->parentWidget();
		cb->setTristate(false);

		QList<QCheckBox*> cblist = parent->findChildren<QCheckBox*>();
		foreach(QCheckBox *w, cblist) {
			if (w != cb) w->setChecked(cb->isChecked());
		}
	}

	m_changingCBState = false;
}

void MDTRA_ProteinDataMiningDialog :: exec_on_select_something( void )
{
	if (m_changingCBState)
		return;

	m_changingCBState = true;

	QCheckBox *allcb = NULL;
	QObject *obj = QObject::sender();
	int checked = 0;
	int total = 0;

	if (QCheckBox *cb = qobject_cast<QCheckBox*>(obj)) {
		QWidget *parent = cb->parentWidget();
		QList<QCheckBox*> cblist = parent->findChildren<QCheckBox*>();
		foreach(QCheckBox *w, cblist) {
			if (w->text() == tr("(All)")) {
				allcb = w;
			} else if ( w->isChecked() ) {
				checked++;
				total++;
			} else {
				total++;
			}
		}

		if (allcb) {
			if (checked == 0) {
				allcb->setTristate(false);
				allcb->setChecked(false);
			} else if (checked == total) {
				allcb->setTristate(false);
				allcb->setChecked(true);
			} else {
				allcb->setTristate(true);
				allcb->setCheckState(Qt::PartiallyChecked);
			}

			allcb->update();
		}
	}

	m_changingCBState = false;
}

void MDTRA_ProteinDataMiningDialog :: exec_on_update_stream( void )
{
	if (sList->selectedItems().count() > 0) {
		if ( m_cachedStreamIndex == sList->selectedItems().at(0)->data( Qt::UserRole ).toInt() )
			return;
		m_cachedStreamIndex = sList->selectedItems().at(0)->data( Qt::UserRole ).toInt();
	} else {
		m_cachedStreamIndex = -1;
	}

	const MDTRA_Stream *pStream = NULL;
	if (sList->selectedItems().count() > 0)
		pStream = m_pMainWindow->getProject()->fetchStreamByIndex( sList->selectedItems().at(0)->data( Qt::UserRole ).toInt() );

	if (!pStream || !pStream->pdb) {
		sel_atCount->setText("0");
		sel_labelResults->clear();
		return;
	}

	m_cachedSelectionText.clear();
	update_selection(0);
	display_selection();
}

void MDTRA_ProteinDataMiningDialog :: exec_on_selection_editingFinished( void )
{
	update_selection(0);
	display_selection();
}

static MDTRA_ProteinDataMiningDialog *pFuncSrcObject = NULL;

void pdm_print_f( const QString &msg )
{
	if (!pFuncSrcObject) return;
	pFuncSrcObject->sel_labelResults->setText( pFuncSrcObject->sel_labelResults->text().append( msg ) );
}

void pdm_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet )
{
	if (!pFuncSrcObject) return;
	pFuncSrcObject->m_iSelectionSize = 0;
	for (int i = 0; i < pSet->realsize(); i++)
		if (pSet->value(i)) pFuncSrcObject->m_iSelectionSize++;

	if (!pFuncSrcObject->m_iSelectionSize)
		return;

	pFuncSrcObject->m_pSelectionData = new int[pFuncSrcObject->m_iSelectionSize];
	for (int i = 0, j = 0; i < pSet->realsize(); i++)
		if (pSet->value(i)) pFuncSrcObject->m_pSelectionData[j++] = i;
}

void MDTRA_ProteinDataMiningDialog :: exec_on_selection_showWholeSelection( void )
{
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( sList->selectedItems().at(0)->data( Qt::UserRole ).toInt() );
	if (!pStream)
		return;

	QString selWholeText;

	for (int i = 0; i < m_iSelectionSize; i++) {

		if (selWholeText.length() > 0)
			selWholeText = selWholeText.append(", ");

		const MDTRA_PDB_Atom *pCurrentAtom = pStream->pdb->fetchAtomByIndex( m_pSelectionData[i] );
		if (!pCurrentAtom)
			selWholeText = selWholeText.append("<bad index>");
		else
			selWholeText = selWholeText.append(QString("%1-%2 %3").arg(pCurrentAtom->trimmed_residue).arg(pCurrentAtom->residuenumber).arg(pCurrentAtom->trimmed_title));
	}

	MDTRA_InputTextDialog dlg( tr("Selection: %1 atom(s)").arg(m_iSelectionSize), tr("<b>Expression:</b> %1").arg(sel_string->text()), selWholeText, true, this );
	dlg.exec();
}

#if defined(WIN32)
#define SELECTION_LABEL_ITEMS	12
#else
#define SELECTION_LABEL_ITEMS	9
#endif

void MDTRA_ProteinDataMiningDialog :: display_selection( void )
{
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( sList->selectedItems().at(0)->data( Qt::UserRole ).toInt() );

	sel_atCount->setText( QString("%1").arg( m_iSelectionSize ) );

	if (!m_bSelectionError)
	{
		sel_labelResults->clear();

		//display max. of SELECTION_LABEL_ITEMS items of selection
		for (int i = 0; i < SELECTION_LABEL_ITEMS; i++) {
			if (i >= m_iSelectionSize)
				break;

			if (sel_labelResults->text().length() > 0)
				sel_labelResults->setText( sel_labelResults->text().append(", ") );

			const MDTRA_PDB_Atom *pCurrentAtom = pStream->pdb->fetchAtomByIndex( m_pSelectionData[i] );
			if (!pCurrentAtom)
				sel_labelResults->setText( sel_labelResults->text().append("<bad index>") );
			else
				sel_labelResults->setText( sel_labelResults->text().append(QString("%1-%2 %3").arg(pCurrentAtom->trimmed_residue).arg(pCurrentAtom->residuenumber).arg(pCurrentAtom->trimmed_title)) );
		}

		if (m_iSelectionSize > SELECTION_LABEL_ITEMS)
			sel_labelResults->setText( sel_labelResults->text().append(QString(" (<a href=\"showmore\">%1 more...</a>)").arg(m_iSelectionSize - SELECTION_LABEL_ITEMS)) );
	}
}

void MDTRA_ProteinDataMiningDialog :: update_selection( int listIndex )
{
	if ((m_cachedSelectionText == sel_string->text()) && (m_cachedStreamIndex == sList->selectedItems().at(listIndex)->data( Qt::UserRole ).toInt()))
		return;

	m_cachedSelectionText = sel_string->text();
	m_cachedStreamIndex = sList->selectedItems().at(listIndex)->data( Qt::UserRole ).toInt();

	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( m_cachedStreamIndex );
	assert(pStream->pdb != NULL);

	m_iSelectionFlags = 0;
	m_iSelectionSize = 0;
	m_bSelectionError = false;
	pFuncSrcObject = this;

	if (m_pSelectionData) {
		delete [] m_pSelectionData;
		m_pSelectionData = NULL;
	}

	if ( listIndex == 0 )
		sel_labelResults->setText(tr("Updating selection, please wait..."));
	QApplication::processEvents();
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	if (!m_pSelectionParser->parse(pStream->pdb, sel_string->text().toAscii(), pdm_select_f)) {
		sel_labelResults->clear();
		m_pSelectionParser->printErrors( pdm_print_f );
		m_bSelectionError = true;
	}

	pFuncSrcObject = NULL;
	QApplication::restoreOverrideCursor();
}

QString MDTRA_ProteinDataMiningDialog :: makeUniqueDataSourceName( const QString& suggestion )
{
	QString returnedName = suggestion;
	while (!m_pMainWindow->getProject()->checkUniqueDataSourceName( returnedName )) {
		returnedName.append(" Copy");
	}
	return returnedName;
}
QString MDTRA_ProteinDataMiningDialog :: makeUniqueResultName( const QString& suggestion )
{
	QString returnedName = suggestion;
	while (!m_pMainWindow->getProject()->checkUniqueResultName( returnedName )) {
		returnedName.append(" Copy");
	}
	return returnedName;
}

int MDTRA_ProteinDataMiningDialog :: lookupExistingDataSource( const MDTRA_DataSource& check )
{
	int numDS = m_pMainWindow->getProject()->getDataSourceCount();
	for (int i = 0; i < numDS; i++) {
		const MDTRA_DataSource* pDS = m_pMainWindow->getProject()->fetchDataSource(i);
		if (!pDS || pDS->streamIndex != check.streamIndex || pDS->type != check.type)
			continue;
		if (memcmp(pDS->arg, check.arg, sizeof(check.arg)))
			continue;
		if (pDS->prog.byteCodeSize != check.prog.byteCodeSize)
			continue;
		if (pDS->prog.sourceCode != check.prog.sourceCode)
			continue;
		if (pDS->prog.byteCode && check.prog.byteCode && memcmp(pDS->prog.byteCode, check.prog.byteCode, check.prog.byteCodeSize))
			continue;
		return pDS->index;
	}
	return -1;
}

int MDTRA_ProteinDataMiningDialog :: findLocalDataSource( int iStart, const QVector<MDTRA_DataSource>& localList, int residueNumber, int dataNumber )
{
	for (int i = iStart; i < localList.count(); i++) {
		if ((localList.at(i).selection.size == residueNumber) &&
			(localList.at(i).selection.flags == dataNumber))
			return i;
	}

	return -1;
}

void MDTRA_ProteinDataMiningDialog :: exec_on_accept( void )
{
	QVector<MDTRA_DataSource> localDataSourceList;
	QSet<unsigned int> resultSet;

	if (sList->selectedItems().count() <= 0)
		return;

	int numTorsionParms = sizeof(s_ProteinTorsionParmDesc) / sizeof(s_ProteinTorsionParmDesc[0]);
	int numTorsionParmsSel = 0;

	for (int i = 0; i < numTorsionParms; i++) {
		if (s_ProteinTorsionParmDesc[i].pCheckBox->isEnabled() && s_ProteinTorsionParmDesc[i].pCheckBox->isChecked())
			numTorsionParmsSel++;
	}

	if (!numTorsionParmsSel) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No protein parameters were selected!"));
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	MDTRA_Selection emptySel;
	emptySel.string = "";
	emptySel.size = 0;
	emptySel.flags = 0;
	emptySel.data = NULL;
	MDTRA_Prog emptyProg;
	emptyProg.sourceCode = "";
	emptyProg.byteCodeSize = 0;
	emptyProg.byteCode = NULL;

	int newDsCount = 0;

	//Build data sources and result collectors for each stream selected
	for (int i = 0; i < sList->selectedItems().count(); i++) {
		int streamIndex = sList->selectedItems().at( i )->data( Qt::UserRole ).toInt();
		MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
		if (!pStream) 
			continue;

		//update selection
		m_cachedSelectionText = "";
		update_selection( i );
		if ( i == 0 ) display_selection();
		if (m_bSelectionError) {
			QApplication::restoreOverrideCursor();
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Error in selection expression:\n\n%1").arg(sel_string->text()));
			return;
		} else if (m_iSelectionSize <= 0) {
			QApplication::restoreOverrideCursor();
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No atoms were selected in stream #%1 using selection expression:\n\n%2").arg(i+1).arg(sel_string->text()));
			return;
		}

		//build a list of protein residues within the selection
		QSet<int> residueSet;
		QList<int> residueList;
		for ( int j = 0; j < m_iSelectionSize; j++ ) {
			MDTRA_PDB_Atom *pAt = const_cast<MDTRA_PDB_Atom*>(pStream->pdb->fetchAtomByIndex( m_pSelectionData[j] ));
			if ( pAt && ( pAt->atomFlags & PDB_FLAG_PROTEIN ) )
				residueSet << pAt->residueserial;
		}
		residueList = residueSet.toList();
		qSort( residueList );

		MDTRA_DataSource currentDS;
		int dataNumber = 0;

		//for each torsion parameter selected...
		//for each residue in set...
		for (int j = 0; j < numTorsionParms; j++) {
			dataNumber++;
			if (!s_ProteinTorsionParmDesc[j].pCheckBox->isEnabled() || !s_ProteinTorsionParmDesc[j].pCheckBox->isChecked())
				continue;

			for (int k = 0; k < residueList.count(); k++) {
				unsigned int resultSetValue = (dataNumber << 16) | residueList.at(k);
				resultSet << resultSetValue;

				const MDTRA_PDB_Atom *pResidueAtom = pStream->pdb->fetchAtomByResidueSerial( residueList.at(k) );
				assert(pResidueAtom != NULL);

				memset( currentDS.arg, 0, sizeof(currentDS.arg) );

				currentDS.arg[0].atomIndex = pStream->pdb->fetchAtomIndexByDesc(
					pResidueAtom->chainIndex,
					(s_ProteinTorsionParmDesc[j].flags & TPDF_ATOM1_FROM_PREV_RESIDUE)?(pResidueAtom->residuenumber-1):pResidueAtom->residuenumber,
					 s_ProteinTorsionParmDesc[j].arg1);
				currentDS.arg[1].atomIndex = pStream->pdb->fetchAtomIndexByDesc( 
					pResidueAtom->chainIndex,
					pResidueAtom->residuenumber,
					 s_ProteinTorsionParmDesc[j].arg2);
				currentDS.arg[2].atomIndex = pStream->pdb->fetchAtomIndexByDesc( 
					pResidueAtom->chainIndex,
					pResidueAtom->residuenumber,
					 s_ProteinTorsionParmDesc[j].arg3);
				currentDS.arg[3].atomIndex = pStream->pdb->fetchAtomIndexByDesc( 
					pResidueAtom->chainIndex,
					(s_ProteinTorsionParmDesc[j].flags & TPDF_ATOM4_FROM_NEXT_RESIDUE)?(pResidueAtom->residuenumber+1):pResidueAtom->residuenumber,
					 s_ProteinTorsionParmDesc[j].arg4);

				if (currentDS.arg[0].atomIndex < 0 ||
					currentDS.arg[1].atomIndex < 0 ||
					currentDS.arg[2].atomIndex < 0 ||
					currentDS.arg[3].atomIndex < 0) {
					continue;
				}

				currentDS.arg[0].atomIndex = pStream->pdb->fetchAtomByIndex( currentDS.arg[0].atomIndex )->serialnumber;
				currentDS.arg[1].atomIndex = pStream->pdb->fetchAtomByIndex( currentDS.arg[1].atomIndex )->serialnumber;
				currentDS.arg[2].atomIndex = pStream->pdb->fetchAtomByIndex( currentDS.arg[2].atomIndex )->serialnumber;
				currentDS.arg[3].atomIndex = pStream->pdb->fetchAtomByIndex( currentDS.arg[3].atomIndex )->serialnumber;
	
				currentDS.name = makeUniqueDataSourceName( QString("%1 in %2-%3 (%4)").arg(s_ProteinTorsionParmDesc[j].title).arg(pResidueAtom->trimmed_residue).arg(pResidueAtom->residuenumber).arg(pStream->name) );
				currentDS.temp = makeUniqueResultName( QString("%1 in %2-%3").arg(s_ProteinTorsionParmDesc[j].title).arg(pResidueAtom->trimmed_residue).arg(pResidueAtom->residuenumber) );
				currentDS.streamIndex = streamIndex;
				currentDS.userdata = QString();
				currentDS.prog = emptyProg;

				currentDS.type = s_ProteinTorsionParmDesc[j].type;
				currentDS.selection.size = pResidueAtom->residuenumber;
				currentDS.selection.flags = dataNumber;
				currentDS.selection2.flags = s_ProteinTorsionParmDesc[j].yscale;
				currentDS.selection2.size = s_ProteinTorsionParmDesc[j].layout;
				currentDS.index = lookupExistingDataSource( currentDS );
				if (currentDS.index < 0) newDsCount++;
				
				localDataSourceList << currentDS;
			}
		}
	}

	if (!localDataSourceList.count()) {
		QApplication::restoreOverrideCursor();
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No protein data selected can be mined from residues matching selection expression:\n\n%1").arg(sel_string->text()));
		return;
	}

	QList<unsigned int> resultList = resultSet.toList();
	qSort( resultList );

	//count exact number of results
	int newRCount = 0;
	for (int i = 0; i < resultList.count(); i++) {
		unsigned int resultSetValue = resultList.at(i);
		int dataNumber = resultSetValue >> 16;
		int residueNumber = resultSetValue & 0xFFFF;

		int lds = -1;
		int dsr = 0;
	
		while ( (lds = findLocalDataSource( lds + 1, localDataSourceList, residueNumber, dataNumber )) >= 0 )
			dsr++;
	
		if (dsr > 0)
			newRCount++;
	}

	QApplication::restoreOverrideCursor();

	if (QMessageBox::No == QMessageBox::information(this, tr(APPLICATION_TITLE_SMALL), tr("Protein Mining Tool will generate %1 new data sources and %2 new result collectors. Would you like to continue?").arg(newDsCount).arg(newRCount), 
													QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape)) {
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	//OK to generate data sources and results
	for (int i = 0; i < resultList.count(); i++) {
		unsigned int resultSetValue = resultList.at(i);
		int dataNumber = resultSetValue >> 16;
		int residueNumber = resultSetValue & 0xFFFF;

		int lds = -1;
		MDTRA_DataSource* first_ds = NULL;
		QList<MDTRA_DSRef> dsRefList;
	
		while ( (lds = findLocalDataSource( lds + 1, localDataSourceList, residueNumber, dataNumber )) >= 0 ) {
			MDTRA_DataSource& ds = const_cast<MDTRA_DataSource&>(localDataSourceList.at(lds));
			MDTRA_DSRef dsRef;
			if (!first_ds) first_ds = &ds;
			if (ds.index < 0) {
				// create it
				int userFlags = 0;
				if ( ds.selection2.flags == MDTRA_YSU_DEGREES )
					userFlags = DATASOURCE_USERFLAG_ANGLE_DEG;
				else if ( ds.selection2.flags == MDTRA_YSU_RADIANS )
					userFlags = DATASOURCE_USERFLAG_ANGLE_RAD;
				ds.index = m_pMainWindow->getProject()->registerDataSource( ds.name, ds.streamIndex, ds.type, ds.arg, emptySel, emptySel, ds.prog, ds.userdata, userFlags, false);
			} else {
				// clean prog
				if ( ds.prog.byteCode ) {
					delete [] ds.prog.byteCode;
					ds.prog.byteCode = NULL;
				}
			}
			dsRef.dataSourceIndex = ds.index;
			dsRef.bias = 0;
			dsRef.yscale = 1;
			dsRef.flags = DSREF_FLAG_VISIBLE;
			dsRef.iDataSize = 0;
			dsRef.iActualDataSize = 0;
			dsRef.pData = NULL;
			dsRef.pCorrelation = NULL;
			dsRefList << dsRef;
		}

		if (dsRefList.count() > 0) {
			MDTRA_YScaleUnits ysu;
			if ( (first_ds->type == MDTRA_DT_USER) && (first_ds->selection2.flags == MDTRA_YSU_DEGREES) ) {
				ysu = MDTRA_YSU_RADIANS;	//Lua script performs conversion automatically
			} else {
				ysu = (MDTRA_YScaleUnits)first_ds->selection2.flags;
			}
			m_pMainWindow->getProject()->registerResult( first_ds->temp, first_ds->type, ysu, (MDTRA_Layout)first_ds->selection2.size, dsRefList, false );
		}
	}

	m_pMainWindow->getProject()->updateDataSourceList();
	m_pMainWindow->getProject()->updateResultList();

	m_pMainWindow->updateTitleBar( true );
	QApplication::restoreOverrideCursor();

	accept();
}

