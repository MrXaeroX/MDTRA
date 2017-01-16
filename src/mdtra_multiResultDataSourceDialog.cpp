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
//	Implementation of MDTRA_MultiResultDataSourceDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_utils.h"
#include "mdtra_multiResultDataSourceDialog.h"

MDTRA_MultiResultDataSourceDialog :: MDTRA_MultiResultDataSourceDialog( enum eMDTRA_DataType type, QWidget *mainwindow, QWidget *parent )
								   : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(mainwindow);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/source.png") );
	setWindowTitle( tr("Add Multiple Result Data Sources") );
	
	//fill data source combo
	for (int i = 0, j = 0; i < m_pMainWindow->getProject()->getDataSourceCount(); i++) {
		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSource( i );
		if (pDS && UTIL_IsSharedPlotType(pDS->type, type)) {
			QListWidgetItem *pItem = new QListWidgetItem( tr("DATA SOURCE %1: %2").arg(pDS->index).arg(pDS->name),
														  dsList );
			pItem->setIcon( QIcon(":/png/16x16/source.png") );
			pItem->setData( Qt::UserRole, pDS->index );
			j++;
		}
	}

	if (dsList->count() > 0)
		dsList->setCurrentRow(0);
}

int MDTRA_MultiResultDataSourceDialog :: GetAvailableDataSourceCount( void )
{
	return dsList->count();
}

bool MDTRA_MultiResultDataSourceDialog :: GetResultDataSource( int index, struct stMDTRA_DSRef *pDSRefOut )
{
	if (index < 0 || index >= dsList->count())
		return false;

	if (!dsList->item( index )->isSelected())
		return false;

	pDSRefOut->dataSourceIndex = dsList->item( index )->data( Qt::UserRole ).toUInt();
	pDSRefOut->flags = cbVis->isChecked() ? DSREF_FLAG_VISIBLE : 0;
	pDSRefOut->yscale = dsScale->value();
	pDSRefOut->bias = dsBias->value();
	pDSRefOut->iDataSize = 0;
	pDSRefOut->iActualDataSize = 0;
	pDSRefOut->pData = NULL;
	pDSRefOut->pCorrelation = NULL;
	return true;
}