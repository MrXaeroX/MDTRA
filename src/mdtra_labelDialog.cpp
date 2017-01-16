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
//	Implementation of MDTRA_LabelDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_labelDialog.h"

MDTRA_LabelDialog :: MDTRA_LabelDialog( bool bEditMode, QWidget *parent ) : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/labels.png") );
	dsCombo->clear();

	if (bEditMode) {
		setWindowTitle( tr("Edit Label") );
	} else {
		setWindowTitle( tr("Add Label") );
	}

	m_pLabelData = NULL;
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_on_accept()));
}

void MDTRA_LabelDialog :: addDataSourceRef( int index, const QString &title )
{
	dsCombo->addItem( QIcon(":/png/16x16/source.png"), 
							 tr("DATA SOURCE %1: %2").arg(index).arg(title),
							 index );
}

void MDTRA_LabelDialog :: setLabelData( MDTRA_Label* pLabel )
{
	m_pLabelData = pLabel;
	teLabel->setPlainText( pLabel->text );
	dsCombo->setCurrentIndex( pLabel->sourceNum );
	spinBox->setValue( pLabel->snapshotNum );
	cbBorder->setChecked( (pLabel->flags & PLOTLABEL_FLAG_BORDER) > 0 );
	cbSolid->setChecked( (pLabel->flags & PLOTLABEL_FLAG_TRANSPARENT) == 0 );
	cbConnect->setChecked( (pLabel->flags & PLOTLABEL_FLAG_CONNECTION) > 0 );
	cbVertline->setChecked( (pLabel->flags & PLOTLABEL_FLAG_VERTLINE) > 0 );
}

void MDTRA_LabelDialog :: exec_on_accept( void )
{
	if (m_pLabelData) {
		m_pLabelData->text = teLabel->toPlainText();
		m_pLabelData->sourceNum = dsCombo->currentIndex();
		m_pLabelData->snapshotNum = spinBox->value();
		if (cbBorder->isChecked()) m_pLabelData->flags |= PLOTLABEL_FLAG_BORDER; else m_pLabelData->flags &= ~PLOTLABEL_FLAG_BORDER;
		if (!cbSolid->isChecked()) m_pLabelData->flags |= PLOTLABEL_FLAG_TRANSPARENT; else m_pLabelData->flags &= ~PLOTLABEL_FLAG_TRANSPARENT;
		if (cbConnect->isChecked()) m_pLabelData->flags |= PLOTLABEL_FLAG_CONNECTION; else m_pLabelData->flags &= ~PLOTLABEL_FLAG_CONNECTION;
		if (cbVertline->isChecked()) m_pLabelData->flags |= PLOTLABEL_FLAG_VERTLINE; else m_pLabelData->flags &= ~PLOTLABEL_FLAG_VERTLINE;
	}
	accept();
}