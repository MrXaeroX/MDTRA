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
//	Implementation of MDTRA_TorsionSearchDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb.h"
#include "mdtra_utils.h"
#include "mdtra_select.h"
#include "mdtra_inputTextDialog.h"
#include "mdtra_torsionSearch.h"
#include "mdtra_torsionSearchDialog.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QKeyEvent>

static struct {
	MDTRA_StatParm index;
	int bufferDim;
	const char *title;
} stat_criterion[] = {
	{ MDTRA_SP_ARITHMETIC_MEAN, 1, "Arithmetic Mean" },
	{ MDTRA_SP_GEOMETRIC_MEAN, 1, "Geometric Mean" },
	{ MDTRA_SP_HARMONIC_MEAN, 1, "Harmonic Mean" },
	{ MDTRA_SP_QUADRATIC_MEAN, 1, "Quadratic Mean" },
	{ MDTRA_SP_MIN_VALUE, 1, "Minimum Value" },
	{ MDTRA_SP_MAX_VALUE, 1, "Maximum Value" },
	{ MDTRA_SP_RANGE, 2, "Range" },
	{ MDTRA_SP_MIDRANGE, 2, "Midrange" },
	{ MDTRA_SP_VARIANCE, 2, "Sample Variance" },
};

MDTRA_TorsionSearchDialog :: MDTRA_TorsionSearchDialog( QWidget *parent )
							: QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/ts.png") );

	pTsInfo = new MDTRA_TorsionSearchInfo;

#ifndef QT_NO_WHATSTHIS
	sel_string->setWhatsThis(QString("<b>Selection Quick Hint</b><br><br>atomno=100 <i>select single atom</i><br>atomno&gt;=100 and atomno&lt;=200 <i>select range of atoms</i><br>carbon <i>select all carbons</i><br>:a <i>select chain A</i><br>*.ca <i>select all alpha-carbons</i><br>lys.cg <i>select all gamma-carbons in lysine residues</i><br>:a.cg <i>select all gamma-carbons in chain A</i><br>1-3.ca <i>select alpha-carbons in residues 1, 2 and 3</i><br>*.h? <i>select hydrogens with 2-character name</i><br>*.h[12] <i>select hydrogens named H1 and H2</i><br>*.h[1-3] <i>select hydrogens named H1, H2 and H3</i><br>*.h* <i>select all hydrogens</i><br>lys <i>select all lysines (LYS)</i><br>{lys} <i>select all lysines, including neutral (LYS, LYN)</i><br>gl? <i>select all glutamates and glutamines</i><br>(lys, arg) and :b <i>select lysines and arginines in chain B</i><br>protein <i>select all protein atoms</i><br>dna <i>select all nucleic atoms (may also specify \"nucleic\" keyword)</i><br>water and *.o <i>select all oxygen atoms in water molecules</i><br>within (3.5, lys) <i>select all atoms within 3.5 angstroms of any lysine residue</i><br>not dna within (3.0, dna) <i>select non-nucleic atoms within 3.0 angstroms of nucleic acid</i>"));
#endif // QT_NO_WHATSTHIS

	m_iSelectionFlags = 0;
	m_iSelectionSize = 0;
	m_pSelectionData = NULL;
	m_iSelectionSize2 = 0;
	m_pSelectionData2 = NULL;
	m_bSelectionError = false;
	m_bSelectionError2 = false;
	sel_string->clear();
	m_cachedSelectionText.clear();
	m_pSelectionParser = new MDTRA_SelectionParser<MDTRA_PDB_File>;
	sel_parse->setIcon( QIcon(":/png/16x16/csel.png") );

	//fill stream combo
	for (int i = 0; i < m_pMainWindow->getProject()->getStreamCount(); i++) {
		MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStream( i );
		if (pStream && pStream->pdb) {
			sCombo->addItem( QIcon(":/png/16x16/stream.png"), 
							 tr("STREAM %1: %2 (%3 files)").arg(pStream->index).arg(pStream->name).arg(pStream->files.count()),
							 pStream->index );
			sCombo2->addItem( QIcon(":/png/16x16/stream.png"), 
							 tr("STREAM %1: %2 (%3 files)").arg(pStream->index).arg(pStream->name).arg(pStream->files.count()),
							 pStream->index );
		}
	}
	sCombo->setCurrentIndex(0);
	sCombo2->setCurrentIndex(1);
	m_cachedStreamIndex = 0;
	exec_on_stream_change();

	//fill stat criteria
	int statSize = sizeof(stat_criterion) / sizeof(stat_criterion[0]);
	for (int i = 0; i < statSize; i++) {
		sCriterion->addItem( stat_criterion[i].title );
	}
	sCriterion->setCurrentIndex(0);
	
	connect(sCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_stream_change()));
	connect(sCombo2, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_stream_change()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_on_accept()));
	connect(sel_string, SIGNAL(editingFinished()), this, SLOT(exec_on_selection_editingFinished()));
	connect(sel_parse, SIGNAL(clicked()), this, SLOT(exec_on_selection_editingFinished()));
	connect(sel_labelResults, SIGNAL(linkActivated(const QString&)), this, SLOT(exec_on_selection_showWholeSelection()));
}

MDTRA_TorsionSearchDialog :: ~MDTRA_TorsionSearchDialog()
{
	if (m_pSelectionParser) {
		delete m_pSelectionParser;
		m_pSelectionParser = NULL;
	}
	if (m_pSelectionData) {
		delete [] m_pSelectionData;
		m_pSelectionData = NULL;
	}
	if (m_pSelectionData2) {
		delete [] m_pSelectionData2;
		m_pSelectionData2 = NULL;
	}
	if (pTsInfo) {
		delete [] pTsInfo;
		pTsInfo = NULL;
	}
}

bool MDTRA_TorsionSearchDialog :: event( QEvent *ev )
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

void MDTRA_TorsionSearchDialog :: exec_on_stream_change( void )
{
	int streamIndex1 = sCombo->itemData( sCombo->currentIndex() ).toInt();
	int streamIndex2 = sCombo2->itemData( sCombo2->currentIndex() ).toInt();

	const MDTRA_Stream *pStream1 = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex1 );
	const MDTRA_Stream *pStream2 = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex2 );

	eIndex->setMaximum( MDTRA_MIN( pStream1->files.count(), pStream2->files.count() ) );
}

void MDTRA_TorsionSearchDialog :: exec_on_selection_editingFinished( void )
{
	update_selection();
	display_selection();
}

static MDTRA_TorsionSearchDialog *pFuncSrcObject = NULL;

void torsionsearch_print_f( const QString &msg )
{
	if (!pFuncSrcObject) return;
	pFuncSrcObject->sel_labelResults->setText( pFuncSrcObject->sel_labelResults->text().append( msg ) );
}

void torsionsearch_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet )
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

void torsionsearch_select2_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet )
{
	if (!pFuncSrcObject) return;
	pFuncSrcObject->m_iSelectionSize2 = 0;
	for (int i = 0; i < pSet->realsize(); i++)
		if (pSet->value(i)) pFuncSrcObject->m_iSelectionSize2++;

	if (!pFuncSrcObject->m_iSelectionSize2)
		return;

	pFuncSrcObject->m_pSelectionData2 = new int[pFuncSrcObject->m_iSelectionSize2];
	for (int i = 0, j = 0; i < pSet->realsize(); i++)
		if (pSet->value(i)) pFuncSrcObject->m_pSelectionData2[j++] = i;
}

void MDTRA_TorsionSearchDialog :: exec_on_selection_showWholeSelection( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
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

void MDTRA_TorsionSearchDialog :: display_selection( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );

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

void MDTRA_TorsionSearchDialog :: update_selection( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();

	if ((m_cachedSelectionText == sel_string->text()) && (m_cachedStreamIndex == streamIndex))
		return;

	m_cachedSelectionText = sel_string->text();
	m_cachedStreamIndex = streamIndex;

	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	assert(pStream->pdb != NULL);

	m_iSelectionFlags = 0;
	m_iSelectionSize = 0;
	m_bSelectionError = false;
	pFuncSrcObject = this;

	if (m_pSelectionData) {
		delete [] m_pSelectionData;
		m_pSelectionData = NULL;
	}

	sel_labelResults->setText(tr("Updating selection, please wait..."));
	QApplication::processEvents();
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	if (!m_pSelectionParser->parse(pStream->pdb, sel_string->text().toAscii(), torsionsearch_select_f)) {
		sel_labelResults->clear();
		m_pSelectionParser->printErrors( torsionsearch_print_f );
		m_bSelectionError = true;
	}

	pFuncSrcObject = NULL;
	QApplication::restoreOverrideCursor();
}

void MDTRA_TorsionSearchDialog :: update_selection2( void )
{
	int streamIndex = sCombo2->itemData( sCombo2->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	assert(pStream->pdb != NULL);

	m_iSelectionSize2 = 0;
	m_bSelectionError2 = false;
	pFuncSrcObject = this;

	if (m_pSelectionData2) {
		delete [] m_pSelectionData2;
		m_pSelectionData2 = NULL;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	if (!m_pSelectionParser->parse(pStream->pdb, sel_string->text().toAscii(), torsionsearch_select2_f)) {
		m_bSelectionError2 = true;
	}

	pFuncSrcObject = NULL;
	QApplication::restoreOverrideCursor();
}

typedef struct {
	int sindex1;
	int sindex2;
	int resnum;
	char atname[6];
} atdef_t;

static bool atdef_in_list( const QVector<atdef_t> &pList, int resnum, const char *atname )
{
	for (int i = 0; i < pList.size(); i++) {
		const atdef_t *pDef = &pList.at(i);
		if (pDef->resnum == resnum && !_stricmp(pDef->atname, atname))
			return true;
	}
	return false;
}

void MDTRA_TorsionSearchDialog :: build_common_atom_set( void )
{
	int streamIndex1 = sCombo->itemData( sCombo->currentIndex() ).toInt();
	int streamIndex2 = sCombo2->itemData( sCombo2->currentIndex() ).toInt();

	const MDTRA_Stream *pStream1 = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex1 );
	const MDTRA_Stream *pStream2 = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex2 );

	QVector<atdef_t> m_AtDefList;
	
	//Get all atom definitions from selections
	for (int i = 0; i < m_iSelectionSize; i++) {
		const MDTRA_PDB_Atom *pAtom = pStream1->pdb->fetchAtomByIndex( m_pSelectionData[i] );
		//this atom must also exist in stream2
		int sindex2 = pStream2->pdb->fetchAtomIndexByDesc( pAtom->chainIndex, pAtom->residuenumber, pAtom->trimmed_title );
		if (sindex2 < 0) continue;
		if (atdef_in_list(m_AtDefList, pAtom->residuenumber, pAtom->trimmed_title)) continue;
		atdef_t atdef;
		memset( &atdef, 0, sizeof(atdef_t) );
		atdef.sindex1 = m_pSelectionData[i];
		atdef.sindex2 = sindex2;
		atdef.resnum = pAtom->residuenumber;
		strcpy_s( atdef.atname, 6, pAtom->trimmed_title );
		m_AtDefList << atdef;
	}
	for (int i = 0; i < m_iSelectionSize2; i++) {
		const MDTRA_PDB_Atom *pAtom = pStream2->pdb->fetchAtomByIndex( m_pSelectionData2[i] );
		//this atom must also exist in stream1
		int sindex1 = pStream1->pdb->fetchAtomIndexByDesc( pAtom->chainIndex, pAtom->residuenumber, pAtom->trimmed_title );
		if (sindex1 < 0) continue;
		if (atdef_in_list(m_AtDefList, pAtom->residuenumber, pAtom->trimmed_title)) continue;
		atdef_t atdef;
		memset( &atdef, 0, sizeof(atdef_t) );
		atdef.sindex1 = sindex1;
		atdef.sindex2 = m_pSelectionData2[i];
		atdef.resnum = pAtom->residuenumber;
		strcpy_s( atdef.atname, 6, pAtom->trimmed_title );
		m_AtDefList << atdef;
	}

/*
	OutputDebugString( QString("Sel1 Size: %1\n").arg(m_iSelectionSize).toAscii() );
	OutputDebugString( QString("Sel2 Size: %1\n").arg(m_iSelectionSize2).toAscii() );
	OutputDebugString( QString("Set Size: %1\n").arg(m_AtDefList.count()).toAscii() );
*/
	//Build indices for common set
	delete [] m_pSelectionData;
	delete [] m_pSelectionData2;

	m_iSelectionSize = m_AtDefList.count();
	m_iSelectionSize2 = m_iSelectionSize;
	m_pSelectionData = new int[m_iSelectionSize];
	m_pSelectionData2 = new int[m_iSelectionSize2];

	for (int i = 0; i < m_AtDefList.size(); i++) {
		const atdef_t *pDef = &m_AtDefList.at(i);
		m_pSelectionData[i] = pDef->sindex1;
		m_pSelectionData2[i] = pDef->sindex2;
	//	OutputDebugString( QString("Wrote item %1: %2 - %3\n").arg(i).arg(m_pSelectionData[i]).arg(m_pSelectionData2[i]).toAscii() );
	}

	m_AtDefList.clear();
}

void MDTRA_TorsionSearchDialog :: exec_on_accept( void )
{
	if (sCombo->currentIndex() == sCombo2->currentIndex()) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Please select two different streams!"));
		return;
	}

	int streamIndex1 = sCombo->itemData( sCombo->currentIndex() ).toInt();
	int streamIndex2 = sCombo2->itemData( sCombo2->currentIndex() ).toInt();

	const MDTRA_Stream *pStream1 = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex1 );
	const MDTRA_Stream *pStream2 = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex2 );

	if (!pStream1 || !pStream2 || !pStream1->pdb || !pStream2->pdb) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Bad stream(s) selected!"));
		return;
	}

	if (sel_string->text().isEmpty()) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Selection expression is empty!"));
		return;
	}

	update_selection();
	if (m_bSelectionError) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Error in selection expression:\n\n%1").arg(sel_string->text()));
		return;
	} else if (m_iSelectionSize <= 0) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No atoms were selected using selection expression:\n\n%1").arg(sel_string->text()));
		return;
	}

	update_selection2();
	if (m_bSelectionError2) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Error in selection expression (for stream 2):\n\n%1").arg(sel_string->text()));
		return;
	} else if (m_iSelectionSize2 <= 0) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No atoms were selected using selection expression (for stream 2):\n\n%1").arg(sel_string->text()));
		return;
	}

	build_common_atom_set();

	//Define streams
	pTsInfo->streamInfo[0].index = pStream1->index;
	pTsInfo->streamInfo[0].size = m_iSelectionSize;
	pTsInfo->streamInfo[0].data = m_pSelectionData;
	pTsInfo->streamInfo[1].index = pStream2->index;
	pTsInfo->streamInfo[1].size = m_iSelectionSize2;
	pTsInfo->streamInfo[1].data = m_pSelectionData2;

	//Define trajectory fragment to analyze
	pTsInfo->trajectoryMin = 1;
	pTsInfo->trajectoryMax = MDTRA_MIN(pStream1->files.count(), pStream2->files.count());
	if (trajectoryRange->isChecked()) {
		pTsInfo->trajectoryMin = MDTRA_MAX( pTsInfo->trajectoryMin, sIndex->value() );
		pTsInfo->trajectoryMax = MDTRA_MIN( pTsInfo->trajectoryMax, eIndex->value() );
	}

	//Define criterion
	pTsInfo->statCriterion = stat_criterion[sCriterion->currentIndex()].index;
	pTsInfo->bufferDim = stat_criterion[sCriterion->currentIndex()].bufferDim;
	pTsInfo->statReference = (float)spinCriterion->value();

	accept();
}