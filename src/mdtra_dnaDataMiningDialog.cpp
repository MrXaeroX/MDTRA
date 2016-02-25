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
//	Implementation of MDTRA_DNADataMiningDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb.h"
#include "mdtra_utils.h"
#include "mdtra_math.h"
#include "mdtra_select.h"
#include "mdtra_inputTextDialog.h"
#include "mdtra_dnaDataMiningDialog.h"
#include "mdtra_prog_compiler.h"
#include "mdtra_hbSearch.h"
#include "mdtra_dnaDickersonProgs.h"

#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QFileDialog>
#include <QtGui/QKeyEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>

typedef struct stMDTRA_WatsonCrickDesc
{
	const char *at;
	const char *h;
	const char *ffname;
	int			ffcode;
} MDTRA_WatsonCrickDesc;

typedef struct stMDTRA_NucleoBaseDesc
{
	const char *title;
	const char *complement;
	int numPossibleConnections;
	MDTRA_WatsonCrickDesc possibleConnections[6];
} MDTRA_NucleoBaseDesc;

typedef struct stMDTRA_ComplementaryDesc
{
	int residueNumber[2];
	const MDTRA_NucleoBaseDesc *residueDesc[2];
} MDTRA_ComplementaryDesc;

typedef struct stMDTRA_NeightbourPairsDesc
{
	int residueNumber[4];
	const MDTRA_NucleoBaseDesc *residueDesc[4];
} MDTRA_NeightbourPairsDesc;

#define TPDF_ATOM1_FROM_PREV_RESIDUE		(1<<0)
#define TPDF_ATOM3_FROM_NEXT_RESIDUE		(1<<1)
#define TPDF_ATOM4_FROM_NEXT_RESIDUE		(1<<2)
#define TPDF_PHASE_ANGLE_PROGRAM			(1<<3)

typedef struct stMDTRA_DNATorsionParmDesc
{
	const char *title;
	const char *arg1;
	const char *arg2;
	const char *arg3;
	const char *arg4;
	const char *arg3alt;
	const char *arg4alt;
	const char *arg5;
	int flags;
	MDTRA_DataType type;
	MDTRA_YScaleUnits yscale;
	MDTRA_Layout layout;
	QCheckBox *pCheckBox;
} MDTRA_DNATorsionParmDesc;

typedef struct stMDTRA_DNADickersonParmDesc
{
	const char *title;
	MDTRA_DataType type;
	MDTRA_YScaleUnits yscale;
	MDTRA_Layout layout;
	int numBases;
	const char *program;
	int bytecodesize;
	byte *bytecode;
	QCheckBox *pCheckBox;
} MDTRA_DNADickersonParmDesc;

static MDTRA_NucleoBaseDesc s_NucleoBaseDesc[] = 
{
{ "DA", "DT", 5,
{{ "N6", "H61", "H", -1 },
{ "N6", "H62", "H", -1 },
{ "N1", NULL, "NC", -1 },
{ "N3", NULL, "NC", -1 },
{ "N7", NULL, "NB", -1 }},
},
{ "DG", "DC", 6,
{{ "N1", "H1", "H", -1 },
{ "N2", "H21", "H", -1 },
{ "N2", "H22", "H", -1 },
{ "N3", NULL, "NC", -1 },
{ "N7", NULL, "NB", -1 },
{ "O6", NULL, "O", -1 }},
},
{ "DT", "DA", 3,
{{ "N3", "H3", "H", -1 },
{ "O2", NULL, "O", -1 },
{ "O4", NULL, "O", -1 }},
},
{ "DC", "DG", 4,
{{ "N4", "H41", "H", -1 },
{ "N4", "H42", "H", -1 },
{ "O2", NULL, "O", -1 },
{ "N3", NULL, "NC", -1 }},
}
};

static MDTRA_DNATorsionParmDesc s_DNATorsionParmDesc[] = 
{
{ "Alpha",	"O3'",	"P",	"O5'",	"C5'",	NULL,	NULL,	NULL,	TPDF_ATOM1_FROM_PREV_RESIDUE,	MDTRA_DT_TORSION_UNSIGNED, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Beta",	"P",	"O5'",	"C5'",	"C4'",	NULL,	NULL,	NULL,	0,								MDTRA_DT_TORSION_UNSIGNED, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Gamma",	"O5'",	"C5'",	"C4'",	"C3'",	NULL,	NULL,	NULL,	0,								MDTRA_DT_TORSION_UNSIGNED, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Delta",	"O4'",	"C4'",	"C3'",	"O3'",	NULL,	NULL,	NULL,	0,								MDTRA_DT_TORSION_UNSIGNED, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Epsilon","C4'",	"C3'",	"O3'",	"P",	NULL,	NULL,	NULL,	TPDF_ATOM4_FROM_NEXT_RESIDUE,	MDTRA_DT_TORSION_UNSIGNED, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Zeta",	"C3'",	"O3'",	"P",	"O5'",	NULL,	NULL,	NULL,	TPDF_ATOM3_FROM_NEXT_RESIDUE|TPDF_ATOM4_FROM_NEXT_RESIDUE,	MDTRA_DT_TORSION_UNSIGNED, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Nu0",	"C4'",	"O4'",	"C1'",	"C2'",	NULL,	NULL,	NULL,	0,								MDTRA_DT_TORSION, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Nu1",	"O4'",	"C1'",	"C2'",	"C3'",	NULL,	NULL,	NULL,	0,								MDTRA_DT_TORSION, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Nu2",	"C1'",	"C2'",	"C3'",	"C4'",	NULL,	NULL,	NULL,	0,								MDTRA_DT_TORSION, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Nu3",	"C2'",	"C3'",	"C4'",	"O4'",	NULL,	NULL,	NULL,	0,								MDTRA_DT_TORSION, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Nu4",	"C3'",	"C4'",	"O4'",	"C1'",	NULL,	NULL,	NULL,	0,								MDTRA_DT_TORSION, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Chi",	"O4'",	"C1'",	"N9",	"C4",	"N1",	"C2",	NULL,	0,								MDTRA_DT_TORSION_UNSIGNED, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
{ "Phase Angle","C1'",	"C2'",	"C3'",	"C4'",	NULL,	NULL,	"O4'",	TPDF_PHASE_ANGLE_PROGRAM,	MDTRA_DT_USER, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME },
};

static stMDTRA_DNADickersonParmDesc s_DNADickersonParmDesc[] = 
{
{ "Shear",		MDTRA_DT_USER, MDTRA_YSU_ANGSTROMS, MDTRA_LAYOUT_TIME, 2, DNA_SHEAR_PROGRAM },
{ "Stretch",	MDTRA_DT_USER, MDTRA_YSU_ANGSTROMS, MDTRA_LAYOUT_TIME, 2, DNA_STRETCH_PROGRAM },
{ "Stagger",	MDTRA_DT_USER, MDTRA_YSU_ANGSTROMS, MDTRA_LAYOUT_TIME, 2, DNA_STAGGER_PROGRAM },
{ "Buckle",		MDTRA_DT_USER, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME, 2, DNA_BUCKLE_PROGRAM },
{ "Propeller",	MDTRA_DT_USER, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME, 2, DNA_PROPELLER_PROGRAM },
{ "Opening",	MDTRA_DT_USER, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME, 2, DNA_OPENING_PROGRAM },
{ "Shift",		MDTRA_DT_USER, MDTRA_YSU_ANGSTROMS, MDTRA_LAYOUT_TIME, 4, DNA_SHIFT_PROGRAM },
{ "Slide",		MDTRA_DT_USER, MDTRA_YSU_ANGSTROMS, MDTRA_LAYOUT_TIME, 4, DNA_SLIDE_PROGRAM },
{ "Rise",		MDTRA_DT_USER, MDTRA_YSU_ANGSTROMS, MDTRA_LAYOUT_TIME, 4, DNA_RISE_PROGRAM },
{ "Tilt",		MDTRA_DT_USER, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME, 4, DNA_TILT_PROGRAM },
{ "Roll",		MDTRA_DT_USER, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME, 4, DNA_ROLL_PROGRAM },
{ "Twist",		MDTRA_DT_USER, MDTRA_YSU_DEGREES, MDTRA_LAYOUT_TIME, 4, DNA_TWIST_PROGRAM },
};

MDTRA_DNADataMiningDialog :: MDTRA_DNADataMiningDialog( QWidget *parent )
							: QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/ddm.png") );

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

	sel_string->setText("dna");
	exec_on_selection_editingFinished();

	m_Compiler = new MDTRA_Program_Compiler;

	//fill DNA parameters
	int numTorsionParms = sizeof(s_DNATorsionParmDesc) / sizeof(s_DNATorsionParmDesc[0]);
	QGroupBox *torsionGroup = new QGroupBox( tr("Nucleotide Torsion Angles"), scrollArea );
	torsionGroup->setCheckable( true );
	torsionGroup->setChecked( true );
	QGridLayout *torsionGroupGrid = new QGridLayout( torsionGroup );
	QCheckBox *pAllCheckBox = new QCheckBox( tr("(All)"), torsionGroup );
	pAllCheckBox->setChecked( true );
	torsionGroupGrid->addWidget( pAllCheckBox, 0, 0, Qt::AlignTop );
	connect(pAllCheckBox, SIGNAL(stateChanged(int)), this, SLOT(exec_on_select_all_or_none()));

	for (int i = 0; i < numTorsionParms; i++) {
		s_DNATorsionParmDesc[i].pCheckBox = new QCheckBox( s_DNATorsionParmDesc[i].title, torsionGroup );
		if ( s_DNATorsionParmDesc[i].pCheckBox ) {
			s_DNATorsionParmDesc[i].pCheckBox->setChecked( true );
			connect(s_DNATorsionParmDesc[i].pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(exec_on_select_something()));
			torsionGroupGrid->addWidget( s_DNATorsionParmDesc[i].pCheckBox, (i+1) / 4, (i+1) % 4, Qt::AlignTop );
		}
	}

	int numDickersonParms = sizeof(s_DNADickersonParmDesc) / sizeof(s_DNADickersonParmDesc[0]);
	QGroupBox *dickersonGroup = new QGroupBox( tr("Dickerson Parameters"), scrollArea );
	dickersonGroup->setCheckable( true );
	dickersonGroup->setChecked( true );
	QGridLayout *dickersonGroupGrid = new QGridLayout( dickersonGroup );
	pAllCheckBox = new QCheckBox( tr("(All)"), dickersonGroup );
	pAllCheckBox->setChecked( true );
	dickersonGroupGrid->addWidget( pAllCheckBox, 0, 0, Qt::AlignTop );
	connect(pAllCheckBox, SIGNAL(stateChanged(int)), this, SLOT(exec_on_select_all_or_none()));

	for (int i = 0; i < numDickersonParms; i++) {
		s_DNADickersonParmDesc[i].pCheckBox = new QCheckBox( s_DNADickersonParmDesc[i].title, dickersonGroup );
		if ( s_DNADickersonParmDesc[i].pCheckBox ) {
			s_DNADickersonParmDesc[i].pCheckBox->setChecked( true );
			connect(s_DNADickersonParmDesc[i].pCheckBox, SIGNAL(stateChanged(int)), this, SLOT(exec_on_select_something()));
			dickersonGroupGrid->addWidget( s_DNADickersonParmDesc[i].pCheckBox, (i+1) / 4, (i+1) % 4, Qt::AlignTop );
		}
	}
	QSpacerItem *verticalSpacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
	dickersonGroupGrid->addItem( verticalSpacer, (numDickersonParms+1) / 4, 0 );
	
	saLayout->addWidget(torsionGroup);
	saLayout->addWidget(dickersonGroup);

	connect(sList, SIGNAL(itemSelectionChanged()), this, SLOT(exec_on_update_stream()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_on_accept()));
	connect(sel_string, SIGNAL(editingFinished()), this, SLOT(exec_on_selection_editingFinished()));
	connect(sel_parse, SIGNAL(clicked()), this, SLOT(exec_on_selection_editingFinished()));
	connect(sel_labelResults, SIGNAL(linkActivated(const QString&)), this, SLOT(exec_on_selection_showWholeSelection()));
	m_changingCBState = false;
}

MDTRA_DNADataMiningDialog :: ~MDTRA_DNADataMiningDialog()
{
	if (m_pSelectionParser) {
		delete m_pSelectionParser;
		m_pSelectionParser = NULL;
	}
	if (m_pSelectionData) {
		delete [] m_pSelectionData;
		m_pSelectionData = NULL;
	}
	if ( m_Compiler ) {
		delete m_Compiler;
		m_Compiler = NULL;
	}
}

bool MDTRA_DNADataMiningDialog :: event( QEvent *ev )
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

void MDTRA_DNADataMiningDialog :: exec_on_select_all_or_none( void )
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

void MDTRA_DNADataMiningDialog :: exec_on_select_something( void )
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

void MDTRA_DNADataMiningDialog :: exec_on_update_stream( void )
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

void MDTRA_DNADataMiningDialog :: exec_on_selection_editingFinished( void )
{
	update_selection(0);
	display_selection();
}

static MDTRA_DNADataMiningDialog *pFuncSrcObject = NULL;

void ddm_print_f( const QString &msg )
{
	if (!pFuncSrcObject) return;
	pFuncSrcObject->sel_labelResults->setText( pFuncSrcObject->sel_labelResults->text().append( msg ) );
}

void ddm_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet )
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

void MDTRA_DNADataMiningDialog :: exec_on_selection_showWholeSelection( void )
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

void MDTRA_DNADataMiningDialog :: display_selection( void )
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

void MDTRA_DNADataMiningDialog :: update_selection( int listIndex )
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
	if (!m_pSelectionParser->parse(pStream->pdb, sel_string->text().toAscii(), ddm_select_f)) {
		sel_labelResults->clear();
		m_pSelectionParser->printErrors( ddm_print_f );
		m_bSelectionError = true;
	}

	pFuncSrcObject = NULL;
	QApplication::restoreOverrideCursor();
}

QString MDTRA_DNADataMiningDialog :: makeUniqueDataSourceName( const QString& suggestion )
{
	QString returnedName = suggestion;
	while (!m_pMainWindow->getProject()->checkUniqueDataSourceName( returnedName )) {
		returnedName.append(" Copy");
	}
	return returnedName;
}
QString MDTRA_DNADataMiningDialog :: makeUniqueResultName( const QString& suggestion )
{
	QString returnedName = suggestion;
	while (!m_pMainWindow->getProject()->checkUniqueResultName( returnedName )) {
		returnedName.append(" Copy");
	}
	return returnedName;
}

int MDTRA_DNADataMiningDialog :: lookupExistingDataSource( const MDTRA_DataSource& check )
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

int MDTRA_DNADataMiningDialog :: findLocalDataSource( int iStart, const QVector<MDTRA_DataSource>& localList, int residueNumber, int dataNumber )
{
	for (int i = iStart; i < localList.count(); i++) {
		if ((localList.at(i).selection.size == residueNumber) &&
			(localList.at(i).selection.flags == dataNumber))
			return i;
	}

	return -1;
}

const MDTRA_NucleoBaseDesc* MDTRA_DNADataMiningDialog :: lookupNucleoBaseDesc( const char *residueTitle )
{
	int numDesc = sizeof(s_NucleoBaseDesc) / sizeof(s_NucleoBaseDesc[0]);
	for (int i = 0; i < numDesc; i++) {
		if (!_strnicmp( s_NucleoBaseDesc[i].title, residueTitle, strlen(s_NucleoBaseDesc[i].title) )) {
			MDTRA_NucleoBaseDesc *desc = &s_NucleoBaseDesc[i];
			for ( int j = 0; j < desc->numPossibleConnections; ++j ) {
				if ( desc->possibleConnections[j].ffcode == -1 )
					desc->possibleConnections[j].ffcode = HBGetFFCode( desc->possibleConnections[j].ffname );
			}
			return desc;
		}
	}
	return NULL;
}

float MDTRA_DNADataMiningDialog :: calc_bond_energy( const MDTRA_PDB_Atom *pAtX, const MDTRA_PDB_Atom *pAtH, const MDTRA_PDB_Atom *pAtY, short xff, short yff )
{
	float v1[3], v2[3];
	float d1, d2, fcos;

	Vec3_Sub( v1, pAtY->xyz, pAtH->xyz );
	Vec3_LenSq( d1, v1 );

	if ( d1 > HB_CUTOFF_DIST_SQ )
		return 0;

	const MDTRA_HBTripletParms *pTParms = &g_TripletParms[xff][yff];

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

	return flEnergy;
}

int MDTRA_DNADataMiningDialog :: findComplementaryResidue( MDTRA_PDB_File *ppdb, QList<int> &selList, int residueNumber, const MDTRA_NucleoBaseDesc **pd1, const MDTRA_NucleoBaseDesc **pd2 )
{
	if (pd1) *pd1 = NULL;
	if (pd2) *pd2 = NULL;

	const MDTRA_PDB_Atom *pResidueAtom = ppdb->fetchAtomByResidueSerial( residueNumber );
	if (!pResidueAtom)
		return 0;
	const MDTRA_NucleoBaseDesc *pBaseDesc = lookupNucleoBaseDesc( pResidueAtom->trimmed_residue );
	if (!pBaseDesc)
		return 0;

	if (pd1) *pd1 = pBaseDesc;

	int bestResidue = 0;
	int bestBonds = 0;
	float bestEnergy = 0;

	//loop through all selected nucleic residues...
	for (int i = 1; i <= ppdb->getResidueCount(); i++) {
		if ( i == residueNumber )
			continue;
		if (selList.indexOf(i) < 0)
			continue;

		const MDTRA_PDB_Atom *pResidueAtom2 = ppdb->fetchAtomByResidueSerial( i );
		if (!pResidueAtom2)
			continue;
		const MDTRA_NucleoBaseDesc* pBaseDesc2 = lookupNucleoBaseDesc( pResidueAtom2->trimmed_residue );
		if (!pBaseDesc2)
			continue;
		
		//check if not complementary
		if (strcmp(pBaseDesc->title, pBaseDesc2->complement))
			continue;

		//estimate energy
		float energy = 0;
		int numbonds = 0;

		//add X-H..Y connections
		for ( int j = 0; j < pBaseDesc->numPossibleConnections; j++ ) {
			if ( !pBaseDesc->possibleConnections[j].h ) continue;
			const MDTRA_PDB_Atom *pAtX = ppdb->fetchAtomByDesc( pResidueAtom->chainIndex, residueNumber, pBaseDesc->possibleConnections[j].at );
			const MDTRA_PDB_Atom *pAtH = ppdb->fetchAtomByDesc( pResidueAtom->chainIndex, residueNumber, pBaseDesc->possibleConnections[j].h );
			if (!pAtX || !pAtH)continue;

			for ( int k = 0; k < pBaseDesc2->numPossibleConnections; k++ ) {
				if ( pBaseDesc2->possibleConnections[k].h ) continue;
				const MDTRA_PDB_Atom *pAtY = ppdb->fetchAtomByDesc( pResidueAtom2->chainIndex, i, pBaseDesc2->possibleConnections[k].at );
				if (!pAtY) continue;
				float testEnergy = calc_bond_energy( pAtX, pAtH, pAtY, pBaseDesc->possibleConnections[j].ffcode, pBaseDesc2->possibleConnections[k].ffcode );
				if ( testEnergy > -1.0f )
					continue;
				energy += testEnergy;
				numbonds++;
			}
		}
		//add Y..H-X connections
		for ( int j = 0; j < pBaseDesc->numPossibleConnections; j++ ) {
			if ( pBaseDesc->possibleConnections[j].h ) continue;
			const MDTRA_PDB_Atom *pAtY = ppdb->fetchAtomByDesc( pResidueAtom->chainIndex, residueNumber, pBaseDesc->possibleConnections[j].at );
			if (!pAtY) continue;

			for ( int k = 0; k < pBaseDesc2->numPossibleConnections; k++ ) {
				if ( !pBaseDesc2->possibleConnections[k].h ) continue;
				const MDTRA_PDB_Atom *pAtX = ppdb->fetchAtomByDesc( pResidueAtom2->chainIndex, i, pBaseDesc2->possibleConnections[k].at );
				const MDTRA_PDB_Atom *pAtH = ppdb->fetchAtomByDesc( pResidueAtom2->chainIndex, i, pBaseDesc2->possibleConnections[k].h );
				if (!pAtX || !pAtH)continue;
				
				float testEnergy = calc_bond_energy( pAtX, pAtH, pAtY, pBaseDesc2->possibleConnections[k].ffcode, pBaseDesc->possibleConnections[j].ffcode );
				if ( testEnergy > -1.0f )
					continue;
				energy += testEnergy;
				numbonds++;
			}
		}

		if ( energy < bestEnergy ) {
			bestEnergy = energy;
			bestResidue = i;
			bestBonds = numbonds;
			if (pd2) *pd2 = pBaseDesc2;
		}
	}

/*	const MDTRA_PDB_Atom *testat = ppdb->fetchAtomByResidueNumber( bestResidue );
	if (testat)
		OutputDebugString(QString("Complement for %1-%2 : %3-%4 (%5 bonds) [E = %6]\n").arg(pResidueAtom->trimmed_residue).arg(pResidueAtom->residuenumber).arg(testat->trimmed_residue).arg(testat->residuenumber).arg(bestBonds).arg(bestEnergy).toAscii());
	else
		OutputDebugString(QString("Complement for %1-%2 : N/A\n").arg(pResidueAtom->trimmed_residue).arg(pResidueAtom->residuenumber).toAscii());
*/

	return bestResidue;
}

void MDTRA_DNADataMiningDialog :: exec_on_accept( void )
{
	QVector<MDTRA_DataSource> localDataSourceList;
	QSet<unsigned int> resultSet;

	if (sList->selectedItems().count() <= 0)
		return;

	HBInitConfigData();

	int numTorsionParms = sizeof(s_DNATorsionParmDesc) / sizeof(s_DNATorsionParmDesc[0]);
	int numDickersonParms = sizeof(s_DNADickersonParmDesc) / sizeof(s_DNADickersonParmDesc[0]);

	int numTorsionParmsSel = 0;
	int numDickersonParmsSel = 0;

	for (int i = 0; i < numTorsionParms; i++) {
		if (s_DNATorsionParmDesc[i].pCheckBox->isEnabled() && s_DNATorsionParmDesc[i].pCheckBox->isChecked())
			numTorsionParmsSel++;
	}
	for (int i = 0; i < numDickersonParms; i++) {
		if (s_DNADickersonParmDesc[i].pCheckBox->isEnabled() && s_DNADickersonParmDesc[i].pCheckBox->isChecked())
			numDickersonParmsSel++;
	}

	if (!(numTorsionParmsSel+numDickersonParmsSel)) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No DNA parameters were selected!"));
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

	MDTRA_Prog phaseAngleProg;
	phaseAngleProg.sourceCode = QString(DNA_PHASE_ANGLE_PROGRAM);
	if ( m_Compiler->Compile( phaseAngleProg.sourceCode ) ) {
		phaseAngleProg.byteCodeSize = m_Compiler->GetByteCodeSize();
		phaseAngleProg.byteCode = new byte[phaseAngleProg.byteCodeSize];
		memcpy(phaseAngleProg.byteCode,m_Compiler->GetByteCode(),phaseAngleProg.byteCodeSize);
	} else {
		//should not happen
		phaseAngleProg.byteCodeSize = 0;
		phaseAngleProg.byteCode = NULL;
	}

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

		//build a list of DNA residues within the selection
		QSet<int> residueSet;
		QList<int> residueList;
		for ( int j = 0; j < m_iSelectionSize; j++ ) {
			MDTRA_PDB_Atom *pAt = const_cast<MDTRA_PDB_Atom*>(pStream->pdb->fetchAtomByIndex( m_pSelectionData[j] ));
			if ( pAt && ( pAt->atomFlags & PDB_FLAG_NUCLEIC ) )
				residueSet << pAt->residueserial;
		}
		residueList = residueSet.toList();
		qSort( residueList );

		//build complementary information
		QVector<MDTRA_ComplementaryDesc> complementaryList;
		for (int j = 0; j < residueList.count(); j++) {
			const MDTRA_NucleoBaseDesc *d1 = NULL;
			const MDTRA_NucleoBaseDesc *d2 = NULL;
			int current = residueList.at(j);
			int complementary = findComplementaryResidue( pStream->pdb, residueList, current, &d1, &d2 );
			if ( !complementary )
				continue;
			bool exists = false;
			for (int k = 0; k < complementaryList.count(); k++) {
				if ((complementaryList.at(k).residueNumber[0] == current) &&
					(complementaryList.at(k).residueNumber[1] == complementary)) {
					exists = true; break;
				}
				if ((complementaryList.at(k).residueNumber[1] == current) &&
					(complementaryList.at(k).residueNumber[0] == complementary)) {
					exists = true; break;
				}
			}
			if (!exists) {
				MDTRA_ComplementaryDesc cd;
				cd.residueNumber[0] = current;
				cd.residueDesc[0] = d1;
				cd.residueNumber[1] = complementary;
				cd.residueDesc[1] = d2;
				complementaryList << cd;
			}
		}

		//complementary list contains all pairs sorted by first residue's index
		//build neighbour pairs list then
		QVector<MDTRA_NeightbourPairsDesc> neightbourPairsList;
		for (int j = 1; j < complementaryList.count(); j++) {
			if ( complementaryList.at(j).residueNumber[0] - complementaryList.at(j-1).residueNumber[0] == 1 ) {
				MDTRA_NeightbourPairsDesc npd;
				npd.residueNumber[0] = complementaryList.at(j-1).residueNumber[0];
				npd.residueDesc[0] = complementaryList.at(j-1).residueDesc[0];
				npd.residueNumber[1] = complementaryList.at(j-1).residueNumber[1];
				npd.residueDesc[1] = complementaryList.at(j-1).residueDesc[1];
				npd.residueNumber[2] = complementaryList.at(j).residueNumber[0];
				npd.residueDesc[2] = complementaryList.at(j).residueDesc[0];
				npd.residueNumber[3] = complementaryList.at(j).residueNumber[1];
				npd.residueDesc[3] = complementaryList.at(j).residueDesc[1];
				neightbourPairsList << npd;
			}
		}

#ifdef _DEBUG
		OutputDebugString(QString("STREAM %1: %2 pairs\n").arg(streamIndex).arg(complementaryList.count()).toAscii());
		for (int ccc = 0; ccc < complementaryList.count(); ccc++) {
			const MDTRA_PDB_Atom *pAt0 = pStream->pdb->fetchAtomByResidueSerial( complementaryList.at(ccc).residueNumber[0] );
			const MDTRA_PDB_Atom *pAt1 = pStream->pdb->fetchAtomByResidueSerial( complementaryList.at(ccc).residueNumber[1] );

			OutputDebugString(QString("%1-%2 ... %3-%4\n").arg(pAt0->trimmed_residue).arg(pAt0->residuenumber).arg(pAt1->trimmed_residue).arg(pAt1->residuenumber).toAscii());
		}
#endif
		MDTRA_DataSource currentDS;
		int dataNumber = 0;

		//for each torsion parameter selected...
		//for each residue in set...
		for (int j = 0; j < numTorsionParms; j++) {
			dataNumber++;
			if (!s_DNATorsionParmDesc[j].pCheckBox->isEnabled() || !s_DNATorsionParmDesc[j].pCheckBox->isChecked())
				continue;

			for (int k = 0; k < residueList.count(); k++) {
				unsigned int resultSetValue = (dataNumber << 16) | residueList.at(k);
				resultSet << resultSetValue;

				const MDTRA_PDB_Atom *pResidueAtom = pStream->pdb->fetchAtomByResidueSerial( residueList.at(k) );
				assert(pResidueAtom != NULL);

				memset( currentDS.arg, 0, sizeof(currentDS.arg) );

				currentDS.arg[0].atomIndex = pStream->pdb->fetchAtomIndexByDesc(
					pResidueAtom->chainIndex,
					(s_DNATorsionParmDesc[j].flags & TPDF_ATOM1_FROM_PREV_RESIDUE)?(pResidueAtom->residuenumber-1):pResidueAtom->residuenumber,
					 s_DNATorsionParmDesc[j].arg1);
				currentDS.arg[1].atomIndex = pStream->pdb->fetchAtomIndexByDesc( 
					pResidueAtom->chainIndex,
					pResidueAtom->residuenumber,
					 s_DNATorsionParmDesc[j].arg2);
				currentDS.arg[2].atomIndex = pStream->pdb->fetchAtomIndexByDesc( 
					pResidueAtom->chainIndex,
					(s_DNATorsionParmDesc[j].flags & TPDF_ATOM3_FROM_NEXT_RESIDUE)?(pResidueAtom->residuenumber+1):pResidueAtom->residuenumber,
					 s_DNATorsionParmDesc[j].arg3);
				currentDS.arg[3].atomIndex = pStream->pdb->fetchAtomIndexByDesc( 
					pResidueAtom->chainIndex,
					(s_DNATorsionParmDesc[j].flags & TPDF_ATOM4_FROM_NEXT_RESIDUE)?(pResidueAtom->residuenumber+1):pResidueAtom->residuenumber,
					 s_DNATorsionParmDesc[j].arg4);
				if ( (currentDS.arg[2].atomIndex < 0 || currentDS.arg[3].atomIndex < 0) &&
					s_DNATorsionParmDesc[j].arg3alt && s_DNATorsionParmDesc[j].arg4alt ) {
					currentDS.arg[2].atomIndex = pStream->pdb->fetchAtomIndexByDesc( 
						pResidueAtom->chainIndex,
						(s_DNATorsionParmDesc[j].flags & TPDF_ATOM3_FROM_NEXT_RESIDUE)?(pResidueAtom->residuenumber+1):pResidueAtom->residuenumber,
						 s_DNATorsionParmDesc[j].arg3alt);
					currentDS.arg[3].atomIndex = pStream->pdb->fetchAtomIndexByDesc( 
						pResidueAtom->chainIndex,
						(s_DNATorsionParmDesc[j].flags & TPDF_ATOM4_FROM_NEXT_RESIDUE)?(pResidueAtom->residuenumber+1):pResidueAtom->residuenumber,
						 s_DNATorsionParmDesc[j].arg4alt);
				}

				if (currentDS.arg[0].atomIndex < 0 ||
					currentDS.arg[1].atomIndex < 0 ||
					currentDS.arg[2].atomIndex < 0 ||
					currentDS.arg[3].atomIndex < 0) {
					continue;
				}

				if ( s_DNATorsionParmDesc[j].arg5 ) {
					currentDS.arg[4].atomIndex = pStream->pdb->fetchAtomIndexByDesc( pResidueAtom->chainIndex, pResidueAtom->residuenumber, s_DNATorsionParmDesc[j].arg5);
					if (currentDS.arg[4].atomIndex < 0)
						continue;
				}

				currentDS.arg[0].atomIndex = pStream->pdb->fetchAtomByIndex( currentDS.arg[0].atomIndex )->serialnumber;
				currentDS.arg[1].atomIndex = pStream->pdb->fetchAtomByIndex( currentDS.arg[1].atomIndex )->serialnumber;
				currentDS.arg[2].atomIndex = pStream->pdb->fetchAtomByIndex( currentDS.arg[2].atomIndex )->serialnumber;
				currentDS.arg[3].atomIndex = pStream->pdb->fetchAtomByIndex( currentDS.arg[3].atomIndex )->serialnumber;

				if ( s_DNATorsionParmDesc[j].arg5 )
					currentDS.arg[4].atomIndex = pStream->pdb->fetchAtomByIndex( currentDS.arg[4].atomIndex )->serialnumber;
					
				currentDS.name = makeUniqueDataSourceName( QString("%1 in %2-%3 (%4)").arg(s_DNATorsionParmDesc[j].title).arg(pResidueAtom->trimmed_residue).arg(pResidueAtom->residuenumber).arg(pStream->name) );
				currentDS.temp = makeUniqueResultName( QString("%1 in %2-%3").arg(s_DNATorsionParmDesc[j].title).arg(pResidueAtom->trimmed_residue).arg(pResidueAtom->residuenumber) );
				currentDS.streamIndex = streamIndex;
				if (s_DNATorsionParmDesc[j].flags & TPDF_PHASE_ANGLE_PROGRAM) {
					currentDS.userdata = QString("Phase Angle, deg");
					currentDS.prog.sourceCode = phaseAngleProg.sourceCode;
					currentDS.prog.byteCodeSize = phaseAngleProg.byteCodeSize;
					currentDS.prog.byteCode = new byte[phaseAngleProg.byteCodeSize];
					memcpy(currentDS.prog.byteCode,phaseAngleProg.byteCode,phaseAngleProg.byteCodeSize);
				} else {
					currentDS.userdata = QString();
					currentDS.prog = emptyProg;
				}
				currentDS.type = s_DNATorsionParmDesc[j].type;
				currentDS.selection.size = pResidueAtom->residuenumber;
				currentDS.selection.flags = dataNumber;
				currentDS.selection2.flags = s_DNATorsionParmDesc[j].yscale;
				currentDS.selection2.size = s_DNATorsionParmDesc[j].layout;
				currentDS.index = lookupExistingDataSource( currentDS );
				if (currentDS.index < 0) newDsCount++;
				
				localDataSourceList << currentDS;
			}
		}

		//for each dickerson parameter selected...
		//for each complementary pair in set...
		for (int j = 0; j < numDickersonParms; j++) {
			dataNumber++;
			if (!s_DNADickersonParmDesc[j].pCheckBox->isEnabled() || !s_DNADickersonParmDesc[j].pCheckBox->isChecked())
				continue;

			MDTRA_Prog currentDickersonProg;
			currentDickersonProg.sourceCode = QString(s_DNADickersonParmDesc[j].program);
			if ( (currentDickersonProg.sourceCode.length() > 0) && 
				  m_Compiler->Compile( currentDickersonProg.sourceCode ) ) {
				currentDickersonProg.byteCodeSize = m_Compiler->GetByteCodeSize();
				currentDickersonProg.byteCode = new byte[currentDickersonProg.byteCodeSize];
				memcpy(currentDickersonProg.byteCode,m_Compiler->GetByteCode(),currentDickersonProg.byteCodeSize);
			} else {
				//should not happen
				currentDickersonProg.byteCodeSize = 0;
				currentDickersonProg.byteCode = NULL;
			}

			if ( s_DNADickersonParmDesc[j].numBases == 2 ) {
				// Walk through complementary list
				for (int k = 0; k < complementaryList.count(); k++) {
					unsigned int resultSetValue = (dataNumber << 16) | complementaryList.at(k).residueNumber[0];
					resultSet << resultSetValue;

					const MDTRA_PDB_Atom *pResidueAtom1 = pStream->pdb->fetchAtomByResidueSerial( complementaryList.at(k).residueNumber[0] );
					const MDTRA_PDB_Atom *pResidueAtom2 = pStream->pdb->fetchAtomByResidueSerial( complementaryList.at(k).residueNumber[1] );
					assert(pResidueAtom1 != NULL);
					assert(pResidueAtom2 != NULL);

					memset( currentDS.arg, 0, sizeof(currentDS.arg) );

					currentDS.arg[0].atomIndex = pResidueAtom1->serialnumber;
					currentDS.arg[1].atomIndex = pResidueAtom2->serialnumber;

					currentDS.name = makeUniqueDataSourceName( QString("%1 for %2-%3 : %4-%5 Pair (%6)").arg(s_DNADickersonParmDesc[j].title).arg(pResidueAtom1->trimmed_residue).arg(pResidueAtom1->residuenumber).arg(pResidueAtom2->trimmed_residue).arg(pResidueAtom2->residuenumber).arg(pStream->name) );
					currentDS.temp = makeUniqueResultName( QString("%1 for %2-%3 : %4-%5 Pair").arg(s_DNADickersonParmDesc[j].title).arg(pResidueAtom1->trimmed_residue).arg(pResidueAtom1->residuenumber).arg(pResidueAtom2->trimmed_residue).arg(pResidueAtom2->residuenumber) );
					currentDS.streamIndex = streamIndex;
					if ( s_DNADickersonParmDesc[j].yscale == MDTRA_YSU_DEGREES )
						currentDS.userdata = QString("%1 [deg]").arg(s_DNADickersonParmDesc[j].title);
					else
						currentDS.userdata = QString("%1 [A]").arg(s_DNADickersonParmDesc[j].title);
					currentDS.prog.sourceCode = currentDickersonProg.sourceCode;
					currentDS.prog.byteCodeSize = currentDickersonProg.byteCodeSize;
					currentDS.prog.byteCode = new byte[currentDickersonProg.byteCodeSize];
					memcpy(currentDS.prog.byteCode,currentDickersonProg.byteCode,currentDickersonProg.byteCodeSize);
					currentDS.type = s_DNADickersonParmDesc[j].type;
					currentDS.selection.size = pResidueAtom1->residuenumber;
					currentDS.selection.flags = dataNumber;
					currentDS.selection2.flags = s_DNADickersonParmDesc[j].yscale;
					currentDS.selection2.size = s_DNADickersonParmDesc[j].layout;
					currentDS.index = lookupExistingDataSource( currentDS );
					if (currentDS.index < 0) newDsCount++;
				
					localDataSourceList << currentDS;
				}
			} else if ( s_DNADickersonParmDesc[j].numBases == 4 ) {
				// Walk through neighbouring pairs list
				for (int k = 0; k < neightbourPairsList.count(); k++) {
					unsigned int resultSetValue = (dataNumber << 16) | neightbourPairsList.at(k).residueNumber[0];
					resultSet << resultSetValue;

					const MDTRA_PDB_Atom *pResidueAtom1 = pStream->pdb->fetchAtomByResidueSerial( neightbourPairsList.at(k).residueNumber[0] );
					const MDTRA_PDB_Atom *pResidueAtom2 = pStream->pdb->fetchAtomByResidueSerial( neightbourPairsList.at(k).residueNumber[1] );
					const MDTRA_PDB_Atom *pResidueAtom3 = pStream->pdb->fetchAtomByResidueSerial( neightbourPairsList.at(k).residueNumber[2] );
					const MDTRA_PDB_Atom *pResidueAtom4 = pStream->pdb->fetchAtomByResidueSerial( neightbourPairsList.at(k).residueNumber[3] );
					assert(pResidueAtom1 != NULL);
					assert(pResidueAtom2 != NULL);
					assert(pResidueAtom3 != NULL);
					assert(pResidueAtom4 != NULL);

					memset( currentDS.arg, 0, sizeof(currentDS.arg) );

					currentDS.arg[0].atomIndex = pResidueAtom1->serialnumber;
					currentDS.arg[1].atomIndex = pResidueAtom2->serialnumber;
					currentDS.arg[2].atomIndex = pResidueAtom3->serialnumber;
					currentDS.arg[3].atomIndex = pResidueAtom4->serialnumber;

					currentDS.name = makeUniqueDataSourceName( QString("%1 for %2-%3 : %4-%5 - %6-%7 : %8-%9 Pairs (").arg(s_DNADickersonParmDesc[j].title).arg(pResidueAtom1->trimmed_residue).arg(pResidueAtom1->residuenumber).arg(pResidueAtom2->trimmed_residue).arg(pResidueAtom2->residuenumber).arg(pResidueAtom3->trimmed_residue).arg(pResidueAtom3->residuenumber).arg(pResidueAtom4->trimmed_residue).arg(pResidueAtom4->residuenumber).append(pStream->name).append(")") );
					currentDS.temp = makeUniqueResultName( QString("%1 for %2-%3 : %4-%5 - %6-%7 : %8-%9 Pairs").arg(s_DNADickersonParmDesc[j].title).arg(pResidueAtom1->trimmed_residue).arg(pResidueAtom1->residuenumber).arg(pResidueAtom2->trimmed_residue).arg(pResidueAtom2->residuenumber).arg(pResidueAtom3->trimmed_residue).arg(pResidueAtom3->residuenumber).arg(pResidueAtom4->trimmed_residue).arg(pResidueAtom4->residuenumber) );
					currentDS.streamIndex = streamIndex;
					if ( s_DNADickersonParmDesc[j].yscale == MDTRA_YSU_DEGREES )
						currentDS.userdata = QString("%1 [deg]").arg(s_DNADickersonParmDesc[j].title);
					else
						currentDS.userdata = QString("%1 [A]").arg(s_DNADickersonParmDesc[j].title);
					currentDS.prog.sourceCode = currentDickersonProg.sourceCode;
					currentDS.prog.byteCodeSize = currentDickersonProg.byteCodeSize;
					currentDS.prog.byteCode = new byte[currentDickersonProg.byteCodeSize];
					memcpy(currentDS.prog.byteCode,currentDickersonProg.byteCode,currentDickersonProg.byteCodeSize);
					currentDS.type = s_DNADickersonParmDesc[j].type;
					currentDS.selection.size = pResidueAtom1->residuenumber;
					currentDS.selection.flags = dataNumber;
					currentDS.selection2.flags = s_DNADickersonParmDesc[j].yscale;
					currentDS.selection2.size = s_DNADickersonParmDesc[j].layout;
					currentDS.index = lookupExistingDataSource( currentDS );
					if (currentDS.index < 0) newDsCount++;
				
					localDataSourceList << currentDS;
				}
			}

			if ( currentDickersonProg.byteCode ) {
				delete [] currentDickersonProg.byteCode;
				currentDickersonProg.byteCode = NULL;
			}
		}
	}

	if (!localDataSourceList.count()) {
		QApplication::restoreOverrideCursor();
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No DNA data selected can be mined from residues matching selection expression:\n\n%1").arg(sel_string->text()));
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

	if (QMessageBox::No == QMessageBox::information(this, tr(APPLICATION_TITLE_SMALL), tr("DNA Mining Tool will generate %1 new data sources and %2 new result collectors. Would you like to continue?").arg(newDsCount).arg(newRCount), 
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

	if ( phaseAngleProg.byteCode ) {
		delete [] phaseAngleProg.byteCode;
		phaseAngleProg.byteCode = NULL;
	}

	m_pMainWindow->updateTitleBar( true );
	QApplication::restoreOverrideCursor();

	accept();
}
