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
//	Implementation of MDTRA_HBSearchDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb.h"
#include "mdtra_utils.h"
#include "mdtra_select.h"
#include "mdtra_inputTextDialog.h"
#include "mdtra_hbSearch.h"
#include "mdtra_hbSearchDialog.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QKeyEvent>

MDTRA_HBSearchDialog :: MDTRA_HBSearchDialog( QWidget *parent )
					  : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/hbond.png") );

	pHBsInfo = new MDTRA_HBSearchInfo;

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
}

MDTRA_HBSearchDialog :: ~MDTRA_HBSearchDialog()
{
	if (pHBsInfo) {
		delete [] pHBsInfo;
		pHBsInfo = NULL;
	}
}

void MDTRA_HBSearchDialog :: exec_on_stream_change( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	eIndex->setMaximum( pStream->files.count() );
}

void MDTRA_HBSearchDialog :: exec_on_accept( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );

	if (!pStream || !pStream->pdb) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Bad stream(s) selected!"));
		return;
	}

	//Define stream
	pHBsInfo->streamIndex = pStream->index;

	//Define trajectory fragment to analyze
	pHBsInfo->trajectoryMin = 1;
	pHBsInfo->trajectoryMax = pStream->files.count();
	if (trajectoryRange->isChecked()) {
		pHBsInfo->trajectoryMin = MDTRA_MAX( pHBsInfo->trajectoryMin, sIndex->value() );
		pHBsInfo->trajectoryMax = MDTRA_MIN( pHBsInfo->trajectoryMax, eIndex->value() );
	}

	//Define significance criterion
	pHBsInfo->minEnergy = (cbEnergyTreshold->isChecked()) ? (spinEnergy->value()) : 0.0f;
	pHBsInfo->minPercent = (cbPercentTreshold->isChecked()) ? (spinPercent->value()) : 0;

	//Options
	pHBsInfo->grouping = cbGrouping->isChecked();

	accept();
}