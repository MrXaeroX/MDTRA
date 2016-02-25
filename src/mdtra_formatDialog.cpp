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
//	Implementation of MDTRA_FormatDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb_format.h"
#include "mdtra_formatDialog.h"

static bool s_bExplicitField[PDB_FS_MAX] =
{
	true, // PDB_FS_SERIALNUMBER
	true, // PDB_FS_RESIDUENUMBER
	true, // PDB_FS_ATOMTITLE
	true, // PDB_FS_RESIDUETITLE
	false, // PDB_FS_CHAIN
	true, // PDB_FS_COORD_X
	true, // PDB_FS_COORD_Y
	true, // PDB_FS_COORD_Z
	false, // PDB_FS_FORCE_X
	false, // PDB_FS_FORCE_Y
	false, // PDB_FS_FORCE_Z
};

static const char *s_pszFieldTypeName[PDB_FT_MAX] =
{
	"STRING", // PDB_FT_STRING
	"CHARACTER", // PDB_FT_CHARACTER
	"INTEGER", // PDB_FT_INTEGER
	"FLOAT", // PDB_FT_FLOAT
};

MDTRA_FormatDialog :: MDTRA_FormatDialog( unsigned int formatIdent, QWidget *mainwindow, QWidget *parent )
					: QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(mainwindow);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );

	if (formatIdent == 0xFFFFFFFF) {
		setWindowTitle( tr("Add Format") );
	} else {
		setWindowTitle( tr("Edit Format") );
	}

	if (formatIdent == 0xFFFFFFFF) {
		QString formatTitle = tr("Format %1").arg(g_PDBFormatManager.getFreeIdentifier() - PDB_USER_FORMAT + 1);
		lineEdit->setText(formatTitle);

		//setup default fields
		const PDBFormat_t *pDefaultFormat = g_PDBFormatManager.fetchFormat( PDB_GENERIC_FORMAT );
		assert( pDefaultFormat != NULL );

		setupField( PDB_FS_SERIALNUMBER, &pDefaultFormat->fields[PDB_FS_SERIALNUMBER], cb1, type1, start1, size1 );
		setupField( PDB_FS_ATOMTITLE, &pDefaultFormat->fields[PDB_FS_ATOMTITLE], cb2, type2, start2, size2 );
		setupField( PDB_FS_RESIDUENUMBER, &pDefaultFormat->fields[PDB_FS_RESIDUENUMBER], cb3, type3, start3, size3 );
		setupField( PDB_FS_RESIDUETITLE, &pDefaultFormat->fields[PDB_FS_RESIDUETITLE], cb4, type4, start4, size4 );
		setupField( PDB_FS_CHAIN, &pDefaultFormat->fields[PDB_FS_CHAIN], cb5, type5, start5, size5 );
		setupField( PDB_FS_COORD_X, &pDefaultFormat->fields[PDB_FS_COORD_X], cb6, type6, start6, size6 );
		setupField( PDB_FS_COORD_Y, &pDefaultFormat->fields[PDB_FS_COORD_Y], cb7, type7, start7, size7 );
		setupField( PDB_FS_COORD_Z, &pDefaultFormat->fields[PDB_FS_COORD_Z], cb8, type8, start8, size8 );
		setupField( PDB_FS_FORCE_X, &pDefaultFormat->fields[PDB_FS_FORCE_X], cb9, type9, start9, size9 );
		setupField( PDB_FS_FORCE_Y, &pDefaultFormat->fields[PDB_FS_FORCE_Y], cb10, type10, start10, size10 );
		setupField( PDB_FS_FORCE_Z, &pDefaultFormat->fields[PDB_FS_FORCE_Z], cb11, type11, start11, size11 );

	} else {
		const PDBFormat_t *pFormat = g_PDBFormatManager.fetchFormat( formatIdent );
		if (pFormat) {
			lineEdit->setText( pFormat->title );

			setupField( PDB_FS_SERIALNUMBER, &pFormat->fields[PDB_FS_SERIALNUMBER], cb1, type1, start1, size1 );
			setupField( PDB_FS_ATOMTITLE, &pFormat->fields[PDB_FS_ATOMTITLE], cb2, type2, start2, size2 );
			setupField( PDB_FS_RESIDUENUMBER, &pFormat->fields[PDB_FS_RESIDUENUMBER], cb3, type3, start3, size3 );
			setupField( PDB_FS_RESIDUETITLE, &pFormat->fields[PDB_FS_RESIDUETITLE], cb4, type4, start4, size4 );
			setupField( PDB_FS_CHAIN, &pFormat->fields[PDB_FS_CHAIN], cb5, type5, start5, size5 );
			setupField( PDB_FS_COORD_X, &pFormat->fields[PDB_FS_COORD_X], cb6, type6, start6, size6 );
			setupField( PDB_FS_COORD_Y, &pFormat->fields[PDB_FS_COORD_Y], cb7, type7, start7, size7 );
			setupField( PDB_FS_COORD_Z, &pFormat->fields[PDB_FS_COORD_Z], cb8, type8, start8, size8 );
			setupField( PDB_FS_FORCE_X, &pFormat->fields[PDB_FS_FORCE_X], cb9, type9, start9, size9 );
			setupField( PDB_FS_FORCE_Y, &pFormat->fields[PDB_FS_FORCE_Y], cb10, type10, start10, size10 );
			setupField( PDB_FS_FORCE_Z, &pFormat->fields[PDB_FS_FORCE_Z], cb11, type11, start11, size11 );
		}
	}

	m_uiIdent = formatIdent;
}

MDTRA_FormatDialog :: ~MDTRA_FormatDialog()
{
}

void MDTRA_FormatDialog :: restore( void )
{
	QObject *obj = QObject::sender();
	QCheckBox *cb = qobject_cast<QCheckBox*>(obj);
	if (cb && !cb->isChecked()) cb->setChecked( true );
}

void MDTRA_FormatDialog :: setupField( int sense, const PDBField_t *pField, QCheckBox *pCheck, QComboBox *pCombo, QSpinBox *pStart, QSpinBox *pSize )
{
	if (s_bExplicitField[sense]) {
		pCheck->setChecked( true );
		connect(pCheck, SIGNAL(toggled(bool)), this, SLOT(restore()));
	} else {
		pCheck->setChecked( pField->size > 0 );
	}

	pCombo->clear();
	for (int i = 0; i < PDB_FT_MAX; i++) {
		pCombo->addItem( s_pszFieldTypeName[i] );
		if (pField->type == i)
			pCombo->setCurrentIndex( i );
	}

	pStart->setValue( pField->start );
	pSize->setValue( pField->size );
}

void MDTRA_FormatDialog :: grabField( int sense, PDBField_t *pField, QCheckBox *pCheck, QComboBox *pCombo, QSpinBox *pStart, QSpinBox *pSize )
{
	pField->type = (PDBFieldType_e)pCombo->currentIndex();

	if (pCheck->isChecked()) {
		pField->start = pStart->value();
		pField->size = pSize->value();
	} else {
		pField->start = 0;
		pField->size = 0;
	}
}

void MDTRA_FormatDialog :: getFormatInfo( struct PDBFormat_s *pFormat )
{
	assert( pFormat != NULL );
	memset( pFormat, 0, sizeof(*pFormat) );
	pFormat->identifier = m_uiIdent;
	pFormat->editable = true;
	strncpy_s( pFormat->title, lineEdit->text().toAscii(), sizeof(pFormat->title)-1 );

	grabField( PDB_FS_SERIALNUMBER, &pFormat->fields[PDB_FS_SERIALNUMBER], cb1, type1, start1, size1 );
	grabField( PDB_FS_ATOMTITLE, &pFormat->fields[PDB_FS_ATOMTITLE], cb2, type2, start2, size2 );
	grabField( PDB_FS_RESIDUENUMBER, &pFormat->fields[PDB_FS_RESIDUENUMBER], cb3, type3, start3, size3 );
	grabField( PDB_FS_RESIDUETITLE, &pFormat->fields[PDB_FS_RESIDUETITLE], cb4, type4, start4, size4 );
	grabField( PDB_FS_CHAIN, &pFormat->fields[PDB_FS_CHAIN], cb5, type5, start5, size5 );
	grabField( PDB_FS_COORD_X, &pFormat->fields[PDB_FS_COORD_X], cb6, type6, start6, size6 );
	grabField( PDB_FS_COORD_Y, &pFormat->fields[PDB_FS_COORD_Y], cb7, type7, start7, size7 );
	grabField( PDB_FS_COORD_Z, &pFormat->fields[PDB_FS_COORD_Z], cb8, type8, start8, size8 );
	grabField( PDB_FS_FORCE_X, &pFormat->fields[PDB_FS_FORCE_X], cb9, type9, start9, size9 );
	grabField( PDB_FS_FORCE_Y, &pFormat->fields[PDB_FS_FORCE_Y], cb10, type10, start10, size10 );
	grabField( PDB_FS_FORCE_Z, &pFormat->fields[PDB_FS_FORCE_Z], cb11, type11, start11, size11 );
}
