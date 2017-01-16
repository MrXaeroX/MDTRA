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
//	Implementation of MDTRA_DataSourceDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb.h"
#include "mdtra_utils.h"
#include "mdtra_select.h"
#include "mdtra_inputTextDialog.h"
#include "mdtra_dataSourceDialog.h"
#include "mdtra_occluderDialog.h"
#include "mdtra_userTypeDialog.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QKeyEvent>

MDTRA_DataSourceDialog :: MDTRA_DataSourceDialog( int index, QWidget *parent )
						: QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/source.png") );

	if (index < 0) {
		setWindowTitle( tr("Add Data Source") );
	} else {
		setWindowTitle( tr("Edit Data Source") );
	}

#ifndef QT_NO_WHATSTHIS
	sel_string->setWhatsThis(QString("<b>Selection Quick Hint</b><br><br>atomno=100 <i>select single atom</i><br>atomno&gt;=100 and atomno&lt;=200 <i>select range of atoms</i><br>carbon <i>select all carbons</i><br>:a <i>select chain A</i><br>*.ca <i>select all alpha-carbons</i><br>lys.cg <i>select all gamma-carbons in lysine residues</i><br>:a.cg <i>select all gamma-carbons in chain A</i><br>1-3.ca <i>select alpha-carbons in residues 1, 2 and 3</i><br>*.h? <i>select hydrogens with 2-character name</i><br>*.h[12] <i>select hydrogens named H1 and H2</i><br>*.h[1-3] <i>select hydrogens named H1, H2 and H3</i><br>*.h* <i>select all hydrogens</i><br>lys <i>select all lysines (LYS)</i><br>{lys} <i>select all lysines, including neutral (LYS, LYN)</i><br>gl? <i>select all glutamates and glutamines</i><br>(lys, arg) and :b <i>select lysines and arginines in chain B</i><br>protein <i>select all protein atoms</i><br>dna <i>select all nucleic atoms (may also specify \"nucleic\" keyword)</i><br>water and *.o <i>select all oxygen atoms in water molecules</i><br>within (3.5, lys) <i>select all atoms within 3.5 angstroms of any lysine residue</i><br>not dna within (3.0, dna) <i>select non-nucleic atoms within 3.0 angstroms of nucleic acid</i>"));
#endif // QT_NO_WHATSTHIS

	m_iDataSourceIndex = index;
	m_iCachedStreamIndex = -1;
	int sIndex = 0;
	MDTRA_DataType dType = MDTRA_DT_RMSD;
	MDTRA_DataArg dArg[MAX_DATA_SOURCE_ARGS];
	memset( dArg, 0, sizeof(dArg) );

	m_iSelectionFlags = 0;
	m_iSelectionSize = 0;
	m_pSelectionData = NULL;
	m_bSelectionError = false;
	sel_string->clear();
	m_pSelectionParser = NULL;
	m_cachedSelectionText.clear();
	sel_parse->setIcon( QIcon(":/png/16x16/csel.png") );
	m_Selection2 = "";
	m_iSelection2Flags = 0;
	m_iSelection2Size = 0;
	m_pSelection2Data = NULL;
	m_userData = "";
	m_userFlags = 0;
	m_ProgSource = "";
	m_ProgBytes = NULL;
	m_ProgSize = 0;
	adv_button->setEnabled( false );

	if (index < 0) {
		QString dataSourceTitle = tr("Data Source %1").arg(m_pMainWindow->getDataSourceCounter());
		lineEdit->setText(dataSourceTitle);
		for (int i = 0; i < MAX_DATA_SOURCE_ARGS; i++)
			dArg[i].atomIndex = 0;
	} else {
		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSourceByIndex( index );
		if (pDS) {
			lineEdit->setText( pDS->name );
			sel_string->setText( pDS->selection.string );
			m_cachedSelectionText = pDS->selection.string;
			sIndex = pDS->streamIndex;
			dType = pDS->type;
			for (int i = 0; i < MAX_DATA_SOURCE_ARGS; i++)
				dArg[i] = pDS->arg[i];
			m_iSelectionFlags = pDS->selection.flags;
			m_iSelectionSize = pDS->selection.size;
			if (m_iSelectionSize > 0) {
				m_pSelectionData = new int[m_iSelectionSize];
				memcpy(m_pSelectionData, pDS->selection.data, sizeof(int)*m_iSelectionSize);
			}
			m_Selection2 = pDS->selection2.string;
			m_iSelection2Flags = pDS->selection2.flags;
			m_iSelection2Size = pDS->selection2.size;
			if (m_iSelection2Size > 0) {
				m_pSelection2Data = new int[m_iSelection2Size];
				memcpy(m_pSelection2Data, pDS->selection2.data, sizeof(int)*m_iSelection2Size);
			}
			m_userData = pDS->userdata;
			m_userFlags = pDS->userFlags;
			m_ProgSource = pDS->prog.sourceCode;
			m_ProgSize = pDS->prog.byteCodeSize;
			if (m_ProgSize > 0) {
				m_ProgBytes = new byte[m_ProgSize];
				memcpy(m_ProgBytes, pDS->prog.byteCode, m_ProgSize);
			}
		}
	}

	//fill stream combo
	for (int i = 0, c = 0; i < m_pMainWindow->getProject()->getStreamCount(); i++) {
		MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStream( i );
		if (pStream && pStream->pdb) {
			sCombo->addItem( QIcon(":/png/16x16/stream.png"), 
							 tr("STREAM %1: %2 (%3 files)").arg(pStream->index).arg(pStream->name).arg(pStream->files.count()),
							 pStream->index );
			if (sIndex == pStream->index) sCombo->setCurrentIndex(c);
			c++;
		}
	}

	//fill data type combo
	for (int i = 0; i < MDTRA_DT_MAX; i++) {
		dataTypeCombo->addItem( UTIL_GetDataSourceTypeName(i) );
		if (UTIL_GetDataSourceTypeId(i) == dType) dataTypeCombo->setCurrentIndex(i);
	}

	a_atIndex->setText( QString("%1").arg( dArg[0].atomIndex ) );
	b_atIndex->setText( QString("%1").arg( dArg[1].atomIndex ) );
	c_atIndex->setText( QString("%1").arg( dArg[2].atomIndex ) );
	d_atIndex->setText( QString("%1").arg( dArg[3].atomIndex ) );
	e_atIndex->setText( QString("%1").arg( dArg[4].atomIndex ) );
	f_atIndex->setText( QString("%1").arg( dArg[5].atomIndex ) );

	if (use_selection() || use_selection2()) {
		m_pSelectionParser = new MDTRA_SelectionParser<MDTRA_PDB_File>;
		if (use_selection()) display_selection();
	}

	connect(sCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_stream(int)));
	connect(dataTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_check_args_enabled()));
	connect(dataTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_check_dataSourceInput()));
	connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(exec_on_check_dataSourceInput_noSel()));
	connect(sel_string, SIGNAL(textChanged(const QString&)), this, SLOT(exec_on_check_dataSourceInput_noSel()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_on_accept()));
	connect(sel_string, SIGNAL(editingFinished()), this, SLOT(exec_on_selection_editingFinished()));
	connect(sel_parse, SIGNAL(clicked()), this, SLOT(exec_on_selection_editingFinished()));
	connect(sel_labelResults, SIGNAL(linkActivated(const QString&)), this, SLOT(exec_on_selection_showWholeSelection()));
	connect(adv_button, SIGNAL(clicked()), this, SLOT(exec_on_advanced()));

	exec_on_check_args_enabled();
	exec_on_update_stream(sCombo->currentIndex());

	connect(a_atResidue, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_residue_a()));
	connect(b_atResidue, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_residue_b()));
	connect(c_atResidue, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_residue_c()));
	connect(d_atResidue, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_residue_d()));
	connect(e_atResidue, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_residue_e()));
	connect(f_atResidue, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_residue_f()));
	connect(a_atName, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_atom_a()));
	connect(b_atName, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_atom_b()));
	connect(c_atName, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_atom_c()));
	connect(d_atName, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_atom_d()));
	connect(e_atName, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_atom_e()));
	connect(f_atName, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_atom_f()));
}

MDTRA_DataSourceDialog :: ~MDTRA_DataSourceDialog()
{
	if (m_pSelectionParser) {
		delete m_pSelectionParser;
		m_pSelectionParser = NULL;
	}
	if (m_pSelectionData) {
		delete [] m_pSelectionData;
		m_pSelectionData = NULL;
	}
	if (m_pSelection2Data) {
		delete [] m_pSelection2Data;
		m_pSelection2Data = NULL;
	}
	if (m_ProgBytes) {
		delete[] m_ProgBytes;
		m_ProgBytes = NULL;
	}
}

void MDTRA_DataSourceDialog :: update_stream_data_decl( const MDTRA_Stream *pStream, QComboBox *pResidueCombo, QComboBox *pAtomCombo, QLabel *pAtomLabel )
{
	pResidueCombo->clear();
	pAtomCombo->clear();

	int iAtomIndex = pAtomLabel->text().toInt();
	const MDTRA_PDB_Atom *pAtom = pStream->pdb->fetchAtomBySerialNumber( iAtomIndex );

	//fill residue combo
	int oldResidueIndex = -1;
	bool bCurrentResidue = false;
	for (int i = 0; i < pStream->pdb->getAtomCount(); i++) {
		const MDTRA_PDB_Atom *pCurrentAtom = pStream->pdb->fetchAtomByIndex( i );
		if (pCurrentAtom->residueserial != oldResidueIndex) {
			oldResidueIndex = pCurrentAtom->residueserial;
			pResidueCombo->addItem( QString("%1-%2").arg(pCurrentAtom->trimmed_residue).arg(pCurrentAtom->residuenumber), pCurrentAtom->residueserial );
			bCurrentResidue = (pAtom && pAtom->residueserial == pCurrentAtom->residueserial);
			if (bCurrentResidue) pResidueCombo->setCurrentIndex( pResidueCombo->count() - 1 );
		}
		if (bCurrentResidue) {
			pAtomCombo->addItem( QString("%1").arg(pCurrentAtom->trimmed_title), pCurrentAtom->serialnumber );
			if (pCurrentAtom->serialnumber == pAtom->serialnumber) pAtomCombo->setCurrentIndex( pAtomCombo->count() - 1 );
		}
	}

	pResidueCombo->addItem( "???", 0 );
	pAtomCombo->addItem( "???", 0 );

	if (!pAtom) {
		pResidueCombo->setCurrentIndex( pResidueCombo->count() - 1 );
		pAtomCombo->setCurrentIndex( pAtomCombo->count() - 1 );
	}	
	exec_on_check_dataSourceInput_noSel();
}

void MDTRA_DataSourceDialog :: update_residue( const MDTRA_Stream *pStream, QComboBox *pResidueCombo, QComboBox *pAtomCombo, QLabel *pAtomLabel )
{
	if (m_bFillingCombos)
		return;

	QString szCurrentAtomName = pAtomCombo->currentText();
	pAtomCombo->clear();

	int iCurrentResidueIndex = pResidueCombo->itemData( pResidueCombo->currentIndex() ).toInt();
	int iCurrentAtomIndex = 0;
	int iFirstAtomIndex = 0;

	int oldResidueIndex = -1;
	bool bCurrentResidue = false;

	for (int i = 0; i < pStream->pdb->getAtomCount(); i++) {
		const MDTRA_PDB_Atom *pCurrentAtom = pStream->pdb->fetchAtomByIndex( i );
		if (pCurrentAtom->residueserial != oldResidueIndex) {
			oldResidueIndex = pCurrentAtom->residueserial;
			bCurrentResidue = (pCurrentAtom->residueserial == iCurrentResidueIndex);
		}
		if (bCurrentResidue) {
			QString atName = QString("%1").arg(pCurrentAtom->trimmed_title);
			pAtomCombo->addItem( atName, pCurrentAtom->serialnumber );
			if (szCurrentAtomName == atName) {
				pAtomCombo->setCurrentIndex( pAtomCombo->count() - 1 );
				iCurrentAtomIndex = pCurrentAtom->serialnumber;
			}
			if (iFirstAtomIndex == 0) iFirstAtomIndex = pCurrentAtom->serialnumber;
		}
	}

	if ( !iCurrentAtomIndex )
		iCurrentAtomIndex = iFirstAtomIndex;

	pAtomCombo->addItem( "???", 0 );
	pAtomLabel->setText( QString("%1").arg(iCurrentAtomIndex) );
	exec_on_check_dataSourceInput_noSel();
}

void MDTRA_DataSourceDialog :: update_atom( QComboBox *pAtomCombo, QLabel *pAtomLabel )
{
	if (m_bFillingCombos)
		return;

	int iCurrentAtomIndex = pAtomCombo->itemData( pAtomCombo->currentIndex() ).toInt();
	pAtomLabel->setText( QString("%1").arg(iCurrentAtomIndex) );
	exec_on_check_dataSourceInput_noSel();
}

void MDTRA_DataSourceDialog :: exec_on_update_stream( int newIndex )
{
	int streamIndex = sCombo->itemData( newIndex ).toInt();
	if (m_iCachedStreamIndex == streamIndex)
		return;
	m_iCachedStreamIndex = streamIndex;

	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	if (!pStream || !pStream->pdb) {
		a_atResidue->clear();
		a_atResidue->addItem( "???", 0 );
		a_atName->clear();
		a_atName->addItem( "???", 0 );
		b_atResidue->clear();
		b_atResidue->addItem( "???", 0 );
		b_atName->clear();
		b_atName->addItem( "???", 0 );
		c_atResidue->clear();
		c_atResidue->addItem( "???", 0 );
		c_atName->clear();
		c_atName->addItem( "???", 0 );
		d_atResidue->clear();
		d_atResidue->addItem( "???", 0 );
		d_atName->clear();
		d_atName->addItem( "???", 0 );
		e_atResidue->clear();
		e_atResidue->addItem( "???", 0 );
		e_atName->clear();
		e_atName->addItem( "???", 0 );
		f_atResidue->clear();
		f_atResidue->addItem( "???", 0 );
		f_atName->clear();
		f_atName->addItem( "???", 0 );
		if (use_selection()) {
			sel_atCount->setText("0");
			sel_labelResults->clear();
		}
		exec_on_check_dataSourceInput_noSel();
		return;
	}

	m_bFillingCombos = true;

	update_stream_data_decl( pStream, a_atResidue, a_atName, a_atIndex );
	update_stream_data_decl( pStream, b_atResidue, b_atName, b_atIndex );
	update_stream_data_decl( pStream, c_atResidue, c_atName, c_atIndex );
	update_stream_data_decl( pStream, d_atResidue, d_atName, d_atIndex );
	update_stream_data_decl( pStream, e_atResidue, e_atName, e_atIndex );
	update_stream_data_decl( pStream, f_atResidue, f_atName, f_atIndex );

	m_bFillingCombos = false;

	m_cachedSelectionText.clear();

	if (use_selection2()) {
		update_selection2();
	}

	exec_on_check_dataSourceInput();
}

void MDTRA_DataSourceDialog :: exec_on_update_residue_a( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	if (!pStream || !pStream->pdb) return;
	update_residue( pStream, a_atResidue, a_atName, a_atIndex );
}
void MDTRA_DataSourceDialog :: exec_on_update_residue_b( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	if (!pStream || !pStream->pdb) return;
	update_residue( pStream, b_atResidue, b_atName, b_atIndex );
}
void MDTRA_DataSourceDialog :: exec_on_update_residue_c( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	if (!pStream || !pStream->pdb) return;
	update_residue( pStream, c_atResidue, c_atName, c_atIndex );
}
void MDTRA_DataSourceDialog :: exec_on_update_residue_d( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	if (!pStream || !pStream->pdb) return;
	update_residue( pStream, d_atResidue, d_atName, d_atIndex );
}
void MDTRA_DataSourceDialog :: exec_on_update_residue_e( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	if (!pStream || !pStream->pdb) return;
	update_residue( pStream, e_atResidue, e_atName, e_atIndex );
}
void MDTRA_DataSourceDialog :: exec_on_update_residue_f( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	if (!pStream || !pStream->pdb) return;
	update_residue( pStream, f_atResidue, f_atName, f_atIndex );
}
void MDTRA_DataSourceDialog :: exec_on_update_atom_a( void )
{
	update_atom( a_atName, a_atIndex );
}
void MDTRA_DataSourceDialog :: exec_on_update_atom_b( void )
{
	update_atom( b_atName, b_atIndex );
}
void MDTRA_DataSourceDialog :: exec_on_update_atom_c( void )
{
	update_atom( c_atName, c_atIndex );
}
void MDTRA_DataSourceDialog :: exec_on_update_atom_d( void )
{
	update_atom( d_atName, d_atIndex );
}
void MDTRA_DataSourceDialog :: exec_on_update_atom_e( void )
{
	update_atom( e_atName, e_atIndex );
}
void MDTRA_DataSourceDialog :: exec_on_update_atom_f( void )
{
	update_atom( f_atName, f_atIndex );
}

void MDTRA_DataSourceDialog :: exec_on_selection_editingFinished( void )
{
	if (use_selection()) {
		update_selection();
		display_selection();
	}
}

void MDTRA_DataSourceDialog :: exec_on_selection_showWholeSelection( void )
{
	if (!use_selection())
		return;

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

void MDTRA_DataSourceDialog :: exec_on_check_dataSourceInput( void )
{
	int aIndex = a_atIndex->text().toInt();
	int bIndex = b_atIndex->text().toInt();
	int cIndex = c_atIndex->text().toInt();
	int dIndex = d_atIndex->text().toInt();
	int eIndex = e_atIndex->text().toInt();
	int fIndex = f_atIndex->text().toInt();

	bool bOkEnabled = (lineEdit->text().length() > 0) && 
					  (!aIndex || ((aIndex != bIndex) && (aIndex != cIndex))) &&
					  (!bIndex || ((bIndex != aIndex) && (bIndex != cIndex))) &&
					  (!cIndex || ((cIndex != aIndex) && (cIndex != bIndex))) &&
					  (!dIndex || ((dIndex != eIndex) && (aIndex != fIndex))) &&
					  (!eIndex || ((eIndex != dIndex) && (eIndex != fIndex))) &&
					  (!fIndex || ((fIndex != dIndex) && (fIndex != eIndex)));

	MDTRA_DataType dataType = UTIL_GetDataSourceTypeId( dataTypeCombo->currentIndex() );
	if (dataType == MDTRA_DT_DISTANCE) {
		if (!aIndex || !bIndex) bOkEnabled = false;
	} else if (dataType == MDTRA_DT_ANGLE) {
		if (!aIndex || !bIndex || !cIndex) bOkEnabled = false;
	} else if (dataType == MDTRA_DT_ANGLE2) {
		if (!aIndex || !bIndex || !cIndex || !dIndex) bOkEnabled = false;
	} else if (dataType == MDTRA_DT_TORSION || dataType == MDTRA_DT_TORSION_UNSIGNED || dataType == MDTRA_DT_DIHEDRAL || dataType == MDTRA_DT_DIHEDRAL_ABS) {
		if (!aIndex || !bIndex || !cIndex || !dIndex) bOkEnabled = false;
	} else if (dataType == MDTRA_DT_PLANEANGLE) {
		if (!aIndex || !bIndex || !cIndex || !dIndex || !eIndex || !fIndex) bOkEnabled = false;
	} else if (dataType == MDTRA_DT_FORCE) {
		if (!aIndex) bOkEnabled = false;
	} else if (dataType == MDTRA_DT_RESULTANT_FORCE) {
		if (!aIndex || !bIndex) bOkEnabled = false;
	}

	if (use_selection()) {
		update_selection();
		display_selection();
		if (sel_string->text().length() <= 0) bOkEnabled = false;
	}
	if (use_selection2()) {
		if (m_iSelection2Size <= 0) bOkEnabled = false;
	}
	if (dataType == MDTRA_DT_USER) {
		if (m_ProgSize <= 0) bOkEnabled = false;
	}

	buttonBox->button( QDialogButtonBox::Ok )->setEnabled( bOkEnabled );
}

void MDTRA_DataSourceDialog :: exec_on_check_dataSourceInput_noSel( void )
{
	int aIndex = a_atIndex->text().toInt();
	int bIndex = b_atIndex->text().toInt();
	int cIndex = c_atIndex->text().toInt();
	int dIndex = d_atIndex->text().toInt();
	int eIndex = e_atIndex->text().toInt();
	int fIndex = f_atIndex->text().toInt();

	bool bOkEnabled = (lineEdit->text().length() > 0) && 
					  (!aIndex || ((aIndex != bIndex) && (aIndex != cIndex))) &&
					  (!bIndex || ((bIndex != aIndex) && (bIndex != cIndex))) &&
					  (!cIndex || ((cIndex != aIndex) && (cIndex != bIndex))) &&
					  (!dIndex || ((dIndex != eIndex) && (aIndex != fIndex))) &&
					  (!eIndex || ((eIndex != dIndex) && (eIndex != fIndex))) &&
					  (!fIndex || ((fIndex != dIndex) && (fIndex != eIndex)));

	MDTRA_DataType dataType = UTIL_GetDataSourceTypeId( dataTypeCombo->currentIndex() );
	if (dataType == MDTRA_DT_DISTANCE) {
		if (!aIndex || !bIndex) bOkEnabled = false;
	} else if (dataType == MDTRA_DT_ANGLE) {
		if (!aIndex || !bIndex || !cIndex) bOkEnabled = false;
	} else if (dataType == MDTRA_DT_ANGLE2) {
		if (!aIndex || !bIndex || !cIndex || !dIndex) bOkEnabled = false;
	} else if (dataType == MDTRA_DT_TORSION || dataType == MDTRA_DT_TORSION_UNSIGNED || dataType == MDTRA_DT_DIHEDRAL || dataType == MDTRA_DT_DIHEDRAL_ABS) {
		if (!aIndex || !bIndex || !cIndex || !dIndex) bOkEnabled = false;
	} else if (dataType == MDTRA_DT_PLANEANGLE) {
		if (!aIndex || !bIndex || !cIndex || !dIndex || !eIndex || !fIndex) bOkEnabled = false;
	} else if (dataType == MDTRA_DT_FORCE) {
		if (!aIndex) bOkEnabled = false;
	} else if (dataType == MDTRA_DT_RESULTANT_FORCE) {
		if (!aIndex || !bIndex) bOkEnabled = false;
	}

	if (use_selection()) {
		if (sel_string->text().length() <= 0) bOkEnabled = false;
	}
	if (use_selection2()) {
		if (m_iSelection2Size <= 0) bOkEnabled = false;
	}
	if (dataType == MDTRA_DT_USER) {
		if (m_ProgSize <= 0) bOkEnabled = false;
	}

	buttonBox->button( QDialogButtonBox::Ok )->setEnabled( bOkEnabled );
}

static MDTRA_DataSourceDialog *pFuncSrcObject = NULL;

void datasource_print_f( const QString &msg )
{
	if (!pFuncSrcObject) return;
	pFuncSrcObject->sel_labelResults->setText( pFuncSrcObject->sel_labelResults->text().append( msg ) );
}

void datasource_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet )
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

void datasource_select2_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet )
{
	if (!pFuncSrcObject) return;
	pFuncSrcObject->m_iSelection2Size = 0;
	for (int i = 0; i < pSet->realsize(); i++)
		if (pSet->value(i)) pFuncSrcObject->m_iSelection2Size++;

	if (!pFuncSrcObject->m_iSelection2Size)
		return;

	pFuncSrcObject->m_pSelection2Data = new int[pFuncSrcObject->m_iSelection2Size];
	for (int i = 0, j = 0; i < pSet->realsize(); i++)
		if (pSet->value(i)) pFuncSrcObject->m_pSelection2Data[j++] = i;
}

#if defined(WIN32)
#define SELECTION_LABEL_ITEMS	12
#else
#define SELECTION_LABEL_ITEMS	9
#endif

void MDTRA_DataSourceDialog :: display_selection( void )
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

void MDTRA_DataSourceDialog :: update_selection( void )
{
	if (m_cachedSelectionText == sel_string->text())
		return;
	m_cachedSelectionText = sel_string->text();

	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
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
	if (!m_pSelectionParser->parse(pStream->pdb, sel_string->text().toAscii(), datasource_select_f)) {
		sel_labelResults->clear();
		m_pSelectionParser->printErrors( datasource_print_f );
		m_bSelectionError = true;
	}

	pFuncSrcObject = NULL;
	QApplication::restoreOverrideCursor();
}

bool MDTRA_DataSourceDialog :: use_selection( void )
{
	MDTRA_DataType dataType = UTIL_GetDataSourceTypeId( dataTypeCombo->currentIndex() );
	return (dataType == MDTRA_DT_RMSD_SEL ||
			dataType == MDTRA_DT_RMSF_SEL ||
			dataType == MDTRA_DT_SAS_SEL ||
			dataType == MDTRA_DT_OCCA_SEL );
}

bool MDTRA_DataSourceDialog :: use_selection2( void )
{
	MDTRA_DataType dataType = UTIL_GetDataSourceTypeId( dataTypeCombo->currentIndex() );
	return (dataType == MDTRA_DT_OCCA || 
		    dataType == MDTRA_DT_OCCA_SEL);
}

bool MDTRA_DataSourceDialog :: use_advanced( void )
{
	MDTRA_DataType dataType = UTIL_GetDataSourceTypeId( dataTypeCombo->currentIndex() );
	return (dataType == MDTRA_DT_OCCA || 
		    dataType == MDTRA_DT_OCCA_SEL || 
		    dataType == MDTRA_DT_USER);
}

void MDTRA_DataSourceDialog :: exec_on_check_args_enabled( void )
{
	bool bUseSelection = use_selection();
	bool bUseAdvanced = use_advanced();

	sel_label->setEnabled( bUseSelection );
	sel_label2->setEnabled( bUseSelection );
	sel_labelResults->setEnabled( bUseSelection );
	sel_string->setEnabled( bUseSelection );
	sel_parse->setEnabled( bUseSelection );
	sel_atCount->setEnabled( bUseSelection );
	adv_button->setEnabled( bUseAdvanced );

	// set advanced label & message
	MDTRA_DataType dataType = UTIL_GetDataSourceTypeId( dataTypeCombo->currentIndex() );
	switch (dataType) {
	case MDTRA_DT_OCCA:
	case MDTRA_DT_OCCA_SEL:
		{
			adv_label->setText( tr("Occluder:") );
			if ( m_Selection2 == "" ) {
				m_Selection2 = "dna";
				update_selection2();
			}
			if ( m_iSelection2Size >= 0 )
				adv_message->setText( QString("<b>%1</b> [%2 atom(s)]").arg(m_Selection2).arg(m_iSelection2Size) );
			else
				adv_message->setText( QString("<b>%1</b> <font color=red>[error]</font>").arg(m_Selection2) );
		}
		break;
	case MDTRA_DT_USER:
		{
			QStringList lines = m_ProgSource.split('\n');
			QString firstLine = lines.at(0);
			if (lines.count() > 1) firstLine.append("...");
			adv_label->setText( tr("Program:") );
			adv_message->setText( firstLine );
		}
		break;
	default:
		{
			adv_label->clear();
			adv_message->clear();
		}
		break;
	}

	switch (dataType) {
	default:
	case MDTRA_DT_RMSD:
	case MDTRA_DT_RMSD_SEL:
	case MDTRA_DT_RMSF:
	case MDTRA_DT_RMSF_SEL:
	case MDTRA_DT_RADIUS_OF_GYRATION:
	case MDTRA_DT_SAS:
	case MDTRA_DT_SAS_SEL:
	case MDTRA_DT_OCCA:
	case MDTRA_DT_OCCA_SEL:
		{
			//disable all args
			a_label->setEnabled( false );
			a_atName->setEnabled( false );
			a_atResidue->setEnabled( false );
			a_atIndex->setEnabled( false );
			a_atNameLabel->setEnabled( false );
			a_atResidueLabel->setEnabled( false );

			b_label->setEnabled( false );
			b_atName->setEnabled( false );
			b_atResidue->setEnabled( false );
			b_atIndex->setEnabled( false );
			b_atNameLabel->setEnabled( false );
			b_atResidueLabel->setEnabled( false );

			c_label->setEnabled( false );
			c_atName->setEnabled( false );
			c_atResidue->setEnabled( false );
			c_atIndex->setEnabled( false );
			c_atNameLabel->setEnabled( false );
			c_atResidueLabel->setEnabled( false );

			d_label->setEnabled( false );
			d_atName->setEnabled( false );
			d_atResidue->setEnabled( false );
			d_atIndex->setEnabled( false );
			d_atNameLabel->setEnabled( false );
			d_atResidueLabel->setEnabled( false );

			e_label->setEnabled( false );
			e_atName->setEnabled( false );
			e_atResidue->setEnabled( false );
			e_atIndex->setEnabled( false );
			e_atNameLabel->setEnabled( false );
			e_atResidueLabel->setEnabled( false );

			f_label->setEnabled( false );
			f_atName->setEnabled( false );
			f_atResidue->setEnabled( false );
			f_atIndex->setEnabled( false );
			f_atNameLabel->setEnabled( false );
			f_atResidueLabel->setEnabled( false );
		}
		break;
	case MDTRA_DT_FORCE:
		{
			//disable all args except A
			a_label->setEnabled( true );
			a_atName->setEnabled( true );
			a_atResidue->setEnabled( true );
			a_atIndex->setEnabled( true );
			a_atNameLabel->setEnabled( true );
			a_atResidueLabel->setEnabled( true );

			b_label->setEnabled( false );
			b_atName->setEnabled( false );
			b_atResidue->setEnabled( false );
			b_atIndex->setEnabled( false );
			b_atNameLabel->setEnabled( false );
			b_atResidueLabel->setEnabled( false );

			c_label->setEnabled( false );
			c_atName->setEnabled( false );
			c_atResidue->setEnabled( false );
			c_atIndex->setEnabled( false );
			c_atNameLabel->setEnabled( false );
			c_atResidueLabel->setEnabled( false );

			d_label->setEnabled( false );
			d_atName->setEnabled( false );
			d_atResidue->setEnabled( false );
			d_atIndex->setEnabled( false );
			d_atNameLabel->setEnabled( false );
			d_atResidueLabel->setEnabled( false );

			e_label->setEnabled( false );
			e_atName->setEnabled( false );
			e_atResidue->setEnabled( false );
			e_atIndex->setEnabled( false );
			e_atNameLabel->setEnabled( false );
			e_atResidueLabel->setEnabled( false );

			f_label->setEnabled( false );
			f_atName->setEnabled( false );
			f_atResidue->setEnabled( false );
			f_atIndex->setEnabled( false );
			f_atNameLabel->setEnabled( false );
			f_atResidueLabel->setEnabled( false );
		}
		break;
	case MDTRA_DT_DISTANCE:
	case MDTRA_DT_RESULTANT_FORCE:
		{
			//disable all args except A and B
			a_label->setEnabled( true );
			a_atName->setEnabled( true );
			a_atResidue->setEnabled( true );
			a_atIndex->setEnabled( true );
			a_atNameLabel->setEnabled( true );
			a_atResidueLabel->setEnabled( true );

			b_label->setEnabled( true );
			b_atName->setEnabled( true );
			b_atResidue->setEnabled( true );
			b_atIndex->setEnabled( true );
			b_atNameLabel->setEnabled( true );
			b_atResidueLabel->setEnabled( true );

			c_label->setEnabled( false );
			c_atName->setEnabled( false );
			c_atResidue->setEnabled( false );
			c_atIndex->setEnabled( false );
			c_atNameLabel->setEnabled( false );
			c_atResidueLabel->setEnabled( false );

			d_label->setEnabled( false );
			d_atName->setEnabled( false );
			d_atResidue->setEnabled( false );
			d_atIndex->setEnabled( false );
			d_atNameLabel->setEnabled( false );
			d_atResidueLabel->setEnabled( false );

			e_label->setEnabled( false );
			e_atName->setEnabled( false );
			e_atResidue->setEnabled( false );
			e_atIndex->setEnabled( false );
			e_atNameLabel->setEnabled( false );
			e_atResidueLabel->setEnabled( false );

			f_label->setEnabled( false );
			f_atName->setEnabled( false );
			f_atResidue->setEnabled( false );
			f_atIndex->setEnabled( false );
			f_atNameLabel->setEnabled( false );
			f_atResidueLabel->setEnabled( false );
		}
		break;
	case MDTRA_DT_ANGLE:
		{
			//enable all args except D, E and F
			a_label->setEnabled( true );
			a_atName->setEnabled( true );
			a_atResidue->setEnabled( true );
			a_atIndex->setEnabled( true );
			a_atNameLabel->setEnabled( true );
			a_atResidueLabel->setEnabled( true );

			b_label->setEnabled( true );
			b_atName->setEnabled( true );
			b_atResidue->setEnabled( true );
			b_atIndex->setEnabled( true );
			b_atNameLabel->setEnabled( true );
			b_atResidueLabel->setEnabled( true );

			c_label->setEnabled( true );
			c_atName->setEnabled( true );
			c_atResidue->setEnabled( true );
			c_atIndex->setEnabled( true );
			c_atNameLabel->setEnabled( true );
			c_atResidueLabel->setEnabled( true );

			d_label->setEnabled( false );
			d_atName->setEnabled( false );
			d_atResidue->setEnabled( false );
			d_atIndex->setEnabled( false );
			d_atNameLabel->setEnabled( false );
			d_atResidueLabel->setEnabled( false );

			e_label->setEnabled( false );
			e_atName->setEnabled( false );
			e_atResidue->setEnabled( false );
			e_atIndex->setEnabled( false );
			e_atNameLabel->setEnabled( false );
			e_atResidueLabel->setEnabled( false );

			f_label->setEnabled( false );
			f_atName->setEnabled( false );
			f_atResidue->setEnabled( false );
			f_atIndex->setEnabled( false );
			f_atNameLabel->setEnabled( false );
			f_atResidueLabel->setEnabled( false );
		}
		break;
	case MDTRA_DT_ANGLE2:
	case MDTRA_DT_TORSION:
	case MDTRA_DT_TORSION_UNSIGNED:
	case MDTRA_DT_DIHEDRAL:
	case MDTRA_DT_DIHEDRAL_ABS:
		{
			//enable all args except E and F
			a_label->setEnabled( true );
			a_atName->setEnabled( true );
			a_atResidue->setEnabled( true );
			a_atIndex->setEnabled( true );
			a_atNameLabel->setEnabled( true );
			a_atResidueLabel->setEnabled( true );

			b_label->setEnabled( true );
			b_atName->setEnabled( true );
			b_atResidue->setEnabled( true );
			b_atIndex->setEnabled( true );
			b_atNameLabel->setEnabled( true );
			b_atResidueLabel->setEnabled( true );

			c_label->setEnabled( true );
			c_atName->setEnabled( true );
			c_atResidue->setEnabled( true );
			c_atIndex->setEnabled( true );
			c_atNameLabel->setEnabled( true );
			c_atResidueLabel->setEnabled( true );

			d_label->setEnabled( true );
			d_atName->setEnabled( true );
			d_atResidue->setEnabled( true );
			d_atIndex->setEnabled( true );
			d_atNameLabel->setEnabled( true );
			d_atResidueLabel->setEnabled( true );

			e_label->setEnabled( false );
			e_atName->setEnabled( false );
			e_atResidue->setEnabled( false );
			e_atIndex->setEnabled( false );
			e_atNameLabel->setEnabled( false );
			e_atResidueLabel->setEnabled( false );

			f_label->setEnabled( false );
			f_atName->setEnabled( false );
			f_atResidue->setEnabled( false );
			f_atIndex->setEnabled( false );
			f_atNameLabel->setEnabled( false );
			f_atResidueLabel->setEnabled( false );
		}
		break;
	case MDTRA_DT_PLANEANGLE:
	case MDTRA_DT_USER:
		{
			//enable all args
			a_label->setEnabled( true );
			a_atName->setEnabled( true );
			a_atResidue->setEnabled( true );
			a_atIndex->setEnabled( true );
			a_atNameLabel->setEnabled( true );
			a_atResidueLabel->setEnabled( true );

			b_label->setEnabled( true );
			b_atName->setEnabled( true );
			b_atResidue->setEnabled( true );
			b_atIndex->setEnabled( true );
			b_atNameLabel->setEnabled( true );
			b_atResidueLabel->setEnabled( true );

			c_label->setEnabled( true );
			c_atName->setEnabled( true );
			c_atResidue->setEnabled( true );
			c_atIndex->setEnabled( true );
			c_atNameLabel->setEnabled( true );
			c_atResidueLabel->setEnabled( true );

			d_label->setEnabled( true );
			d_atName->setEnabled( true );
			d_atResidue->setEnabled( true );
			d_atIndex->setEnabled( true );
			d_atNameLabel->setEnabled( true );
			d_atResidueLabel->setEnabled( true );

			e_label->setEnabled( true );
			e_atName->setEnabled( true );
			e_atResidue->setEnabled( true );
			e_atIndex->setEnabled( true );
			e_atNameLabel->setEnabled( true );
			e_atResidueLabel->setEnabled( true );

			f_label->setEnabled( true );
			f_atName->setEnabled( true );
			f_atResidue->setEnabled( true );
			f_atIndex->setEnabled( true );
			f_atNameLabel->setEnabled( true );
			f_atResidueLabel->setEnabled( true );
		}
		break;
	}
}

void MDTRA_DataSourceDialog :: exec_on_accept( void )
{
	if (m_iDataSourceIndex < 0) {
		if (!m_pMainWindow->getProject()->checkUniqueDataSourceName( lineEdit->text() )) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Data source \"%1\" already registered.\nPlease enter another data source title.").arg(lineEdit->text()));
			return;
		}
	}

	if (use_selection()) {
		update_selection();
		if (m_bSelectionError) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Error in selection expression:\n\n%1").arg(sel_string->text()));
			return;
		} else if (m_iSelectionSize <= 0) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No atoms were selected using selection expression:\n\n%1").arg(sel_string->text()));
			return;
		}
	}

	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	MDTRA_DataType dataType = UTIL_GetDataSourceTypeId(dataTypeCombo->currentIndex());
	MDTRA_DataArg dArg[MAX_DATA_SOURCE_ARGS];
	dArg[0].atomIndex = a_atIndex->text().toInt();
	dArg[1].atomIndex = b_atIndex->text().toInt();
	dArg[2].atomIndex = c_atIndex->text().toInt();
	dArg[3].atomIndex = d_atIndex->text().toInt();
	dArg[4].atomIndex = e_atIndex->text().toInt();
	dArg[5].atomIndex = f_atIndex->text().toInt();
	MDTRA_Selection sel, sel2;
	sel.string = sel_string->text();
	sel.size = m_iSelectionSize;
	sel.flags = m_iSelectionFlags;
	sel.data = m_pSelectionData;
	sel2.string = m_Selection2;
	sel2.size = m_iSelection2Size;
	sel2.flags = m_iSelection2Flags;
	sel2.data = m_pSelection2Data;
	MDTRA_Prog prog;
	prog.sourceCode = m_ProgSource;
	prog.byteCodeSize = m_ProgSize;
	prog.byteCode = NULL;
	if ( m_ProgSize > 0 ) {
		prog.byteCode = new byte[m_ProgSize];
		memcpy( prog.byteCode, m_ProgBytes, m_ProgSize );
	}

	if (m_iDataSourceIndex < 0) {
		m_pMainWindow->getProject()->registerDataSource( lineEdit->text(), streamIndex, dataType, dArg, sel, sel2, prog, m_userData, m_userFlags, true );
	} else {
		m_pMainWindow->getProject()->modifyDataSource( m_iDataSourceIndex, lineEdit->text(), streamIndex, dataType, dArg, sel, sel2, prog, m_userData, m_userFlags );
	}

	accept();
}

bool MDTRA_DataSourceDialog :: event( QEvent *ev )
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

void MDTRA_DataSourceDialog :: update_selection2( void )
{
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( m_iCachedStreamIndex );
	assert(pStream->pdb != NULL);

	pFuncSrcObject = this;

	m_iSelection2Size = 0;

	if (m_pSelection2Data) {
		delete [] m_pSelection2Data;
		m_pSelection2Data = NULL;
	}

	adv_message->setText(tr("Updating selection, please wait..."));
	QApplication::processEvents();
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	m_pSelectionParser->parse(pStream->pdb, m_Selection2.toAscii(), datasource_select2_f);
	adv_message->setText( QString("<b>%1</b> [%2 atom(s)]").arg(m_Selection2).arg(m_iSelection2Size) );

	pFuncSrcObject = NULL;
	QApplication::restoreOverrideCursor();
	
}

void MDTRA_DataSourceDialog :: exec_on_advanced_occluderInfo( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	if ( !pStream || !pStream->pdb )
		return;

	MDTRA_Selection sel;
	sel.string = m_Selection2;
	sel.flags = m_iSelection2Flags;
	sel.size = m_iSelection2Size;
	sel.data = NULL;

	MDTRA_OccluderDialog dialog( pStream->index, &sel, m_pMainWindow, this );
	if ( dialog.exec() ) {
		dialog.getSelection( &m_Selection2, &m_iSelection2Flags, &m_iSelection2Size, &m_pSelection2Data );
		if ( m_iSelection2Size >= 0 )
			adv_message->setText( QString("<b>%1</b> [%2 atom(s)]").arg(m_Selection2).arg(m_iSelection2Size) );
		else
			adv_message->setText( QString("<b>%1</b> <font color=red>[error]</font>").arg(m_Selection2) );
	}
}

void MDTRA_DataSourceDialog :: exec_on_advanced_program( void )
{
	MDTRA_UserTypeDialog dialog( m_pMainWindow, this );
	dialog.setUserTypeInfo( &m_userData, &m_userFlags, &m_ProgSource );
	if ( dialog.exec() ) {
		QString newProgSource;
		int newByteCodeSize;
		byte* newByteCodeData;
		dialog.getUserTypeInfo( &m_userData, &m_userFlags, &newProgSource, &newByteCodeSize, &newByteCodeData );
		if ( newProgSource != m_ProgSource ) {
			m_ProgSource = newProgSource;
			//reallocate program bytecode
			if ( m_ProgBytes ) {
				delete[] m_ProgBytes;
				m_ProgBytes = NULL;
			}
			m_ProgSize = newByteCodeSize;
			if ( m_ProgSize > 0 ) {
				m_ProgBytes = new byte[m_ProgSize];
				memcpy( m_ProgBytes, newByteCodeData, m_ProgSize );
			}
			QStringList lines = newProgSource.split('\n');
			QString firstLine = lines.at(0);
			if (lines.count() > 1) firstLine.append("...");
			adv_message->setText( firstLine );
		}
	}
}


void MDTRA_DataSourceDialog :: exec_on_advanced( void )
{
	MDTRA_DataType dataType = UTIL_GetDataSourceTypeId( dataTypeCombo->currentIndex() );
	// do "Advanced" dialog
	switch (dataType) {
	case MDTRA_DT_OCCA:
	case MDTRA_DT_OCCA_SEL:
		{
			// get occluder information
			exec_on_advanced_occluderInfo();
		}
		break;
	case MDTRA_DT_USER:
		{
			// get program
			exec_on_advanced_program();
		}
		break;
	default:
		break;
	}

	exec_on_check_dataSourceInput_noSel();
}