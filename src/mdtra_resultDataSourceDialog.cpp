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
//	Implementation of MDTRA_ResultDataSourceDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_utils.h"
#include "mdtra_resultDataSourceDialog.h"

MDTRA_ResultDataSourceDialog :: MDTRA_ResultDataSourceDialog( enum eMDTRA_DataType type, const MDTRA_DSRef *pDSRef, QWidget *mainwindow, QWidget *parent )
							  : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(mainwindow);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/source.png") );

	m_Flags = 0;

	if (!pDSRef) {
		setWindowTitle( tr("Add Result Data Source") );
	} else {
		setWindowTitle( tr("Edit Result Data Source") );
	}

	//fill data source combo
	for (int i = 0, j = 0; i < m_pMainWindow->getProject()->getDataSourceCount(); i++) {
		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSource( i );
		if (pDS && UTIL_IsSharedPlotType(pDS->type, type)) {
			dsCombo->addItem( QIcon(":/png/16x16/source.png"), 
							 tr("DATA SOURCE %1: %2").arg(pDS->index).arg(pDS->name),
							 pDS->index );
			if (pDSRef && (pDSRef->dataSourceIndex == pDS->index)) dsCombo->setCurrentIndex(j);
			j++;
		}
	}

	if (pDSRef) {
		dsScale->setValue( pDSRef->yscale );
		dsBias->setValue( pDSRef->bias );
		cbVis->setChecked( (pDSRef->flags & DSREF_FLAG_VISIBLE) > 0 );
		m_Flags = pDSRef->flags;
	}
}

int MDTRA_ResultDataSourceDialog :: GetAvailableDataSourceCount( void )
{
	return dsCombo->count();
}

void MDTRA_ResultDataSourceDialog :: GetResultDataSource( struct stMDTRA_DSRef *pDSRefOut )
{
	pDSRefOut->dataSourceIndex = dsCombo->itemData( dsCombo->currentIndex() ).toUInt();

	pDSRefOut->flags = m_Flags;
	if ( cbVis->isChecked() ) pDSRefOut->flags |= DSREF_FLAG_VISIBLE;
	else pDSRefOut->flags &= ~DSREF_FLAG_VISIBLE;

	pDSRefOut->yscale = dsScale->value();
	pDSRefOut->bias = dsBias->value();
	pDSRefOut->iDataSize = 0;
	pDSRefOut->iActualDataSize = 0;
	pDSRefOut->pData = NULL;
	pDSRefOut->pCorrelation = NULL;
}