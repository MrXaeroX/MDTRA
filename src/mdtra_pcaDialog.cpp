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
//	Implementation of MDTRA_PCADialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb.h"
#include "mdtra_select.h"
#include "mdtra_inputTextDialog.h"
#include "mdtra_pca.h"
#include "mdtra_pcaDialog.h"

#include <QtGui/QMessageBox>

MDTRA_PCADialog :: MDTRA_PCADialog( QWidget *parent )
				 : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/pca.png") );

	pInfo = new MDTRA_PCAInfo;

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

	//fill stream combo
	for (int i = 0; i < m_pMainWindow->getProject()->getStreamCount(); i++) {
		MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStream( i );
		if (pStream && pStream->pdb) {
			sCombo->addItem( QIcon(":/png/16x16/stream.png"), 
							 tr("STREAM %1: %2 (%3 files)").arg(pStream->index).arg(pStream->name).arg(pStream->files.count()),
							 pStream->index );
		}
	}
	sCombo->setCurrentIndex(0);
	m_cachedStreamIndex = 0;
	exec_on_stream_change();

	connect(sCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_stream_change()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_on_accept()));
	connect(sel_string, SIGNAL(editingFinished()), this, SLOT(exec_on_selection_editingFinished()));
	connect(sel_parse, SIGNAL(clicked()), this, SLOT(exec_on_selection_editingFinished()));
	connect(sel_labelResults, SIGNAL(linkActivated(const QString&)), this, SLOT(exec_on_selection_showWholeSelection()));
}

MDTRA_PCADialog :: ~MDTRA_PCADialog()
{
	if (m_pSelectionParser) {
		delete m_pSelectionParser;
		m_pSelectionParser = NULL;
	}
	if (m_pSelectionData) {
		delete [] m_pSelectionData;
		m_pSelectionData = NULL;
	}
	if (pInfo) {
		delete [] pInfo;
		pInfo = NULL;
	}
}

bool MDTRA_PCADialog :: event( QEvent *ev )
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

void MDTRA_PCADialog :: exec_on_stream_change( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	eIndex->setMaximum( pStream->files.count() );
}

void MDTRA_PCADialog :: exec_on_selection_editingFinished( void )
{
	update_selection();
	display_selection();
}

static MDTRA_PCADialog *pFuncSrcObject = NULL;

void pca_print_f( const QString &msg )
{
	if (!pFuncSrcObject) return;
	pFuncSrcObject->sel_labelResults->setText( pFuncSrcObject->sel_labelResults->text().append( msg ) );
}

void pca_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet )
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

void MDTRA_PCADialog :: exec_on_selection_showWholeSelection( void )
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
#define SELECTION_LABEL_ITEMS	15
#else
#define SELECTION_LABEL_ITEMS	10
#endif

void MDTRA_PCADialog :: display_selection( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );

	sel_atCount->setText( QString("%1").arg( m_iSelectionSize ) );
	numPC->setMaximum( m_iSelectionSize*3 );

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

void MDTRA_PCADialog :: update_selection( void )
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
	if (!m_pSelectionParser->parse(pStream->pdb, sel_string->text().toAscii(), pca_select_f)) {
		sel_labelResults->clear();
		m_pSelectionParser->printErrors( pca_print_f );
		m_bSelectionError = true;
	}

	pFuncSrcObject = NULL;
	QApplication::restoreOverrideCursor();
	return;
}

void MDTRA_PCADialog :: exec_on_accept( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );

	if (!pStream || !pStream->pdb) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Bad stream(s) selected!"));
		return;
	}

	if (sel_string->text().isEmpty()) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Selection expression is empty!"));
		return;
	}

	update_selection();
	display_selection();
	if (m_bSelectionError) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Error in selection expression:\n\n%1").arg(sel_string->text()));
		return;
	} else if (m_iSelectionSize <= 0) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No atoms were selected using selection expression:\n\n%1").arg(sel_string->text()));
		return;
	}

	//Define stream
	pInfo->streamIndex = pStream->index;

	//Define trajectory fragment to analyze
	pInfo->trajectoryMin = 1;
	pInfo->trajectoryMax = pStream->files.count();
	if (trajectoryRange->isChecked()) {
		pInfo->trajectoryMin = MDTRA_MAX( pInfo->trajectoryMin, sIndex->value() );
		pInfo->trajectoryMax = MDTRA_MIN( pInfo->trajectoryMax, eIndex->value() );
	}

	//Define selection
	pInfo->selectionSize = m_iSelectionSize;
	pInfo->selectionData = m_pSelectionData;

	//Define options
	pInfo->numDisplayPC = MDTRA_MAX(1, MDTRA_MIN( m_iSelectionSize*3, numPC->value() ));
	
	accept();
}