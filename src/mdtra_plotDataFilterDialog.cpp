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
//	Implementation of MDTRA_PlotDataFilterDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_plotDataFilterDialog.h"
#include "mdtra_utils.h"

#include <QtGui/QGridLayout>
#include <QtGui/QScrollBar>

static int s_PE_indices[] = {
	MDTRA_SP_ARITHMETIC_MEAN,
	MDTRA_SP_GEOMETRIC_MEAN,
	MDTRA_SP_HARMONIC_MEAN,
	MDTRA_SP_QUADRATIC_MEAN,
	MDTRA_SP_MEDIAN
};

static int s_IE_indices[][2] = {
	{ MDTRA_SP_STDDEV, 1 },
	{ MDTRA_SP_STDDEV, 2 },
	{ MDTRA_SP_STDDEV, 3 },
	{ MDTRA_SP_STDERR, 1 },
	{ MDTRA_SP_VARIANCE, 1 },
};

MDTRA_PlotDataFilterDialog :: MDTRA_PlotDataFilterDialog( int index, QWidget *parent )
							: QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setWindowIcon( QIcon(":/png/16x16/pdv.png") );
	
	MDTRA_Result *pResult = m_pMainWindow->getProject()->fetchResultByIndex( index );
	if (pResult) {

		groupBox->setTitle( pResult->name );
		m_iNumCheckBoxes = pResult->sourceList.count();
		m_pCheckBoxes = new QCheckBox*[m_iNumCheckBoxes];
		m_pPECombos = new QComboBox*[m_iNumCheckBoxes];
		m_pIECombos = new QComboBox*[m_iNumCheckBoxes];
		m_pPELabels = new QLabel*[m_iNumCheckBoxes];
		m_pIELabels = new QLabel*[m_iNumCheckBoxes];

		QWidget *groupWidget = new QWidget( this );
		groupWidget->resize( scrollArea->width() - scrollArea->verticalScrollBar()->width() - 2, m_iNumCheckBoxes * 24 );
		QGridLayout *gridData = new QGridLayout( groupWidget );
		gridData->setColumnMinimumWidth( 0, 220 );
	
		for (int i = 0; i < m_iNumCheckBoxes; i++) {
			MDTRA_DSRef *pRef = const_cast<MDTRA_DSRef*>(&pResult->sourceList.at(i));
			MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSourceByIndex( pRef->dataSourceIndex );
			bool bVis = (pRef->flags & DSREF_FLAG_VISIBLE)>0;
			int currentPE, currentIE, currentIS, cIndex;

			currentPE = (( pRef->flags >> DSREF_PE_SHIFT ) & DSREF_PE_MASK) - 1;
			currentIE = (( pRef->flags >> DSREF_IE_SHIFT ) & DSREF_IE_MASK) - 1;
			currentIS = (( pRef->flags >> DSREF_IS_SHIFT ) & DSREF_IS_MASK);

			m_pCheckBoxes[i] = new QCheckBox( pDS->name, groupBox );
			m_pCheckBoxes[i]->setChecked( bVis );
			m_pCheckBoxes[i]->setMaximumHeight( 50 );
			connect(m_pCheckBoxes[i], SIGNAL(clicked()), this, SLOT(checkbox_clicked()));

			m_pPECombos[i] = new QComboBox( groupBox );
			m_pPECombos[i]->addItem( tr("None") );
			cIndex = -1;
			for ( size_t j = 0; j < (sizeof(s_PE_indices)/sizeof(s_PE_indices[0])); j++) {
				m_pPECombos[i]->addItem( UTIL_GetStatParmShortName( s_PE_indices[j] ) );
				if ( s_PE_indices[j] == currentPE ) cIndex = j;
			}
			m_pPECombos[i]->setCurrentIndex( cIndex+1 );
			m_pPECombos[i]->setEnabled( bVis );
			
			connect(m_pPECombos[i], SIGNAL(currentIndexChanged(int)), this, SLOT(pe_combo_changed()));

			m_pIECombos[i] = new QComboBox( groupBox );
			m_pIECombos[i]->addItem( tr("None") );
			cIndex = -1;
			for ( size_t j = 0; j < (sizeof(s_IE_indices)/sizeof(s_IE_indices[0])); j++) {
				const char *parmShortName = UTIL_GetStatParmShortName( s_IE_indices[j][0] );
				if ( s_IE_indices[j][1] <= 1 )
					m_pIECombos[i]->addItem( parmShortName );
				else
					m_pIECombos[i]->addItem( QString("%1 (%2x)").arg(parmShortName).arg(s_IE_indices[j][1]) );
				if ( (s_IE_indices[j][0] == currentIE) && (s_IE_indices[j][1] == currentIS) ) cIndex = j;
			}
			m_pIECombos[i]->setCurrentIndex( cIndex+1 );
			m_pIECombos[i]->setEnabled( bVis && (currentPE >= 0) );
			connect(m_pIECombos[i], SIGNAL(currentIndexChanged(int)), this, SLOT(ie_combo_changed()));

			m_pPELabels[i] = new QLabel( tr("Point: "), groupBox );
			m_pPELabels[i]->setAlignment( Qt::AlignRight | Qt::AlignCenter );
			m_pPELabels[i]->setMinimumHeight( 22 );
			m_pPELabels[i]->setEnabled( bVis );
			m_pIELabels[i] = new QLabel( tr("Interval: "), groupBox );
			m_pIELabels[i]->setAlignment( Qt::AlignRight | Qt::AlignCenter );
			m_pIELabels[i]->setMinimumHeight( 22 );
			m_pIELabels[i]->setEnabled( bVis && (currentPE >= 0) );

			QSpacerItem *hrzSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
		
			gridData->addWidget( m_pCheckBoxes[i], i, 0, Qt::AlignTop );
			gridData->addItem( hrzSpacer, i, 1 );
			gridData->addWidget( m_pPELabels[i], i, 2, Qt::AlignTop );
			gridData->addWidget( m_pPECombos[i], i, 3, Qt::AlignTop );
			gridData->addWidget( m_pIELabels[i], i, 4, Qt::AlignTop );
			gridData->addWidget( m_pIECombos[i], i, 5, Qt::AlignTop );

			m_InitialFlags << pRef->flags;
			m_DataRefList << pRef;
		}

		QSpacerItem *verticalSpacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
		gridData->addItem( verticalSpacer, m_iNumCheckBoxes, 0 );

		scrollArea->setWidget( groupWidget );
	}	
}

MDTRA_PlotDataFilterDialog :: ~MDTRA_PlotDataFilterDialog()
{
	if (m_pCheckBoxes)
		delete [] m_pCheckBoxes;
	if (m_pPECombos)
		delete [] m_pPECombos;
	if (m_pIECombos)
		delete [] m_pIECombos;
	if (m_pPELabels)
		delete [] m_pPELabels;
	if (m_pIELabels)
		delete [] m_pIELabels;
}

void MDTRA_PlotDataFilterDialog :: reject( void )
{
	for (int i = 0; i < m_iNumCheckBoxes; i++) {
		MDTRA_DSRef *pRef = m_DataRefList.at(i);
		pRef->flags = m_InitialFlags.at(i);
	}
	m_pMainWindow->updatePlot();
	QDialog::reject();
}

void MDTRA_PlotDataFilterDialog :: checkbox_clicked( void )
{
	QObject *obj = QObject::sender();
	QCheckBox *cb = qobject_cast<QCheckBox*>(obj);
	if (!cb) return;

	int dsNum = -1;
	for (int i = 0; i < m_iNumCheckBoxes; i++) {
		if (m_pCheckBoxes[i] == cb) {
			dsNum = i;
			break;
		}
	}

	if (dsNum < 0)
		return;

	MDTRA_DSRef *pRef = m_DataRefList.at(dsNum);
	bool bVis = cb->isChecked();

	if ( bVis ) pRef->flags |= DSREF_FLAG_VISIBLE;
	else pRef->flags &= ~DSREF_FLAG_VISIBLE;

	m_pPECombos[dsNum]->setEnabled( bVis );
	m_pIECombos[dsNum]->setEnabled( bVis && m_pPECombos[dsNum]->currentIndex() > 0 );
	m_pPELabels[dsNum]->setEnabled( bVis );
	m_pIELabels[dsNum]->setEnabled( bVis && m_pPECombos[dsNum]->currentIndex() > 0 );

	m_pMainWindow->updatePlot();
}

void MDTRA_PlotDataFilterDialog :: pe_combo_changed( void )
{
	QObject *obj = QObject::sender();
	QComboBox *cb = qobject_cast<QComboBox*>(obj);
	if (!cb) return;

	int dsNum = -1;
	for (int i = 0; i < m_iNumCheckBoxes; i++) {
		if (m_pPECombos[i] == cb) {
			dsNum = i;
			break;
		}
	}

	if (dsNum < 0)
		return;

	MDTRA_DSRef *pRef = m_DataRefList.at(dsNum);
	pRef->flags &= ~(DSREF_PE_MASK << DSREF_PE_SHIFT);

	if ( cb->currentIndex() > 0 ) {
		int peValue = s_PE_indices[ cb->currentIndex() - 1 ] + 1;
		pRef->flags |= ((peValue & DSREF_PE_MASK) << DSREF_PE_SHIFT);
		m_pIECombos[dsNum]->setEnabled( true );
		m_pIELabels[dsNum]->setEnabled( true );
	} else {
		m_pIECombos[dsNum]->setEnabled( false );
		m_pIELabels[dsNum]->setEnabled( false );
	}

	m_pMainWindow->updatePlot();
}

void MDTRA_PlotDataFilterDialog :: ie_combo_changed( void )
{
	QObject *obj = QObject::sender();
	QComboBox *cb = qobject_cast<QComboBox*>(obj);
	if (!cb) return;

	int dsNum = -1;
	for (int i = 0; i < m_iNumCheckBoxes; i++) {
		if (m_pIECombos[i] == cb) {
			dsNum = i;
			break;
		}
	}

	if (dsNum < 0)
		return;

	MDTRA_DSRef *pRef = m_DataRefList.at(dsNum);
	pRef->flags &= ~(DSREF_IE_MASK << DSREF_IE_SHIFT);
	pRef->flags &= ~(DSREF_IS_MASK << DSREF_IS_SHIFT);

	if ( cb->currentIndex() > 0 ) {
		int ieValue = s_IE_indices[ cb->currentIndex() - 1 ][0] + 1;
		int isValue = s_IE_indices[ cb->currentIndex() - 1 ][1];
		pRef->flags |= ((ieValue & DSREF_IE_MASK) << DSREF_IE_SHIFT);
		pRef->flags |= ((isValue & DSREF_IS_MASK) << DSREF_IS_SHIFT);
	}

	m_pMainWindow->updatePlot();
}