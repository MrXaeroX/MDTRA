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
//	Implementation of MDTRA_ResultDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_utils.h"
#include "mdtra_resultDialog.h"
#include "mdtra_resultDataSourceDialog.h"
#include "mdtra_multiResultDataSourceDialog.h"

#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>

static const char *szScaleUnitNames[MDTRA_YSU_MAX] = 
{
	"Angstroms",
	"Nanometers",
	"Degrees",
	"Radians",
	"Kcal/A",
	"Micronewtons",
	"Square Angstroms",
	"Square Nanometers",
};

static unsigned int uiScaleUnitMap[MDTRA_YSU_MAX] = 
{
	(1 << MDTRA_DT_RMSD) | (1 << MDTRA_DT_RMSD_SEL) | (1 << MDTRA_DT_RMSF) | (1 << MDTRA_DT_RMSF_SEL) | (1 << MDTRA_DT_RADIUS_OF_GYRATION) | (1 << MDTRA_DT_DISTANCE),
	(1 << MDTRA_DT_RMSD) | (1 << MDTRA_DT_RMSD_SEL) | (1 << MDTRA_DT_RMSF) | (1 << MDTRA_DT_RMSF_SEL) | (1 << MDTRA_DT_RADIUS_OF_GYRATION) | (1 << MDTRA_DT_DISTANCE),
	(1 << MDTRA_DT_ANGLE) | (1 << MDTRA_DT_ANGLE2) | (1 << MDTRA_DT_TORSION) | (1 << MDTRA_DT_TORSION_UNSIGNED) | (1 << MDTRA_DT_DIHEDRAL) | (1 << MDTRA_DT_DIHEDRAL_ABS) | (1 << MDTRA_DT_PLANEANGLE),
	(1 << MDTRA_DT_ANGLE) | (1 << MDTRA_DT_ANGLE2) | (1 << MDTRA_DT_TORSION) | (1 << MDTRA_DT_TORSION_UNSIGNED) | (1 << MDTRA_DT_DIHEDRAL) | (1 << MDTRA_DT_DIHEDRAL_ABS) | (1 << MDTRA_DT_PLANEANGLE),
	(1 << MDTRA_DT_FORCE) | (1 << MDTRA_DT_RESULTANT_FORCE),
	(1 << MDTRA_DT_FORCE) | (1 << MDTRA_DT_RESULTANT_FORCE),
	(1 << MDTRA_DT_SAS) | (1 << MDTRA_DT_SAS_SEL) | (1 << MDTRA_DT_OCCA) | (1 << MDTRA_DT_OCCA_SEL),
	(1 << MDTRA_DT_SAS) | (1 << MDTRA_DT_SAS_SEL) | (1 << MDTRA_DT_OCCA) | (1 << MDTRA_DT_OCCA_SEL),
};

MDTRA_ResultDialog :: MDTRA_ResultDialog( int index, QWidget *parent )
				    : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/result.png") );

	if (index < 0) {
		setWindowTitle( tr("Add Result Collector") );
	} else {
		setWindowTitle( tr("Edit Result Collector") );
	}

	m_iResultIndex = index;

	MDTRA_DataType currentDT = MDTRA_DT_RMSD;
	MDTRA_YScaleUnits currentSU = MDTRA_YSU_ANGSTROMS;
	MDTRA_Layout currentL = UTIL_GetDataSourceDefaultLayout(currentDT);

	if (index < 0) {
		QString resultTitle = tr("Result %1").arg(m_pMainWindow->getResultCounter());
		lineEdit->setText(resultTitle);
	} else {
		MDTRA_Result *pResult = m_pMainWindow->getProject()->fetchResultByIndex( index );
		if (pResult) {
			lineEdit->setText( pResult->name );
			currentDT = pResult->type;
			currentSU = pResult->units;
			currentL = pResult->layout;
			dsScaleUnitsCombo->setEnabled( currentDT != MDTRA_DT_USER );

			for (int i = 0; i < pResult->sourceList.count(); i++) {
				m_dsRefList << pResult->sourceList.at(i);
				MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSourceByIndex( pResult->sourceList.at(i).dataSourceIndex );
				QListWidgetItem *pItem = new QListWidgetItem( QObject::tr("DATA SOURCE %1: %2\nScale = %3   Bias = %4")
														.arg(pDS->index)
														.arg(pDS->name)
														.arg(pResult->sourceList.at(i).yscale)
														.arg(pResult->sourceList.at(i).bias),
														dsList );
				pItem->setIcon( QIcon(":/png/16x16/source.png") );
			}
		}
	}

	for (int i = 0; i < MDTRA_DT_MAX; i++) {
		dsTypeCombo->addItem( UTIL_GetDataSourceShortTypeName(i) );
		if (UTIL_GetDataSourceTypeId(i) == currentDT) dsTypeCombo->setCurrentIndex(i);
	}
	for (int i = 0, j = 0; i < MDTRA_YSU_MAX; i++) {
		if (uiScaleUnitMap[i] & (1 << (int)currentDT)) {
			dsScaleUnitsCombo->addItem( szScaleUnitNames[i], i );
			if (i == (int)currentSU) dsScaleUnitsCombo->setCurrentIndex(j);
			j++;
		}
	}

	switch (currentL) {
	default:
	case MDTRA_LAYOUT_TIME: rbTime->setChecked( true ); break;
	case MDTRA_LAYOUT_RESIDUE: rbRes->setChecked( true ); break;
	}

	rbLabel->setEnabled( UTIL_IsDataSourceLayoutChangeable(currentDT) );
	rbTime->setEnabled( UTIL_IsDataSourceLayoutChangeable(currentDT) );
	rbRes->setEnabled( UTIL_IsDataSourceLayoutChangeable(currentDT) );

	connect(dsTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_update_layout_and_scale_units()));
	connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(exec_on_check_resultInput()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_on_accept()));

	connect(dsAdd, SIGNAL(clicked()), this, SLOT(exec_on_add_result_data_source()));
	connect(dsAddMulti, SIGNAL(clicked()), this, SLOT(exec_on_add_multiple_result_data_sources()));
	connect(dsEdit, SIGNAL(clicked()), this, SLOT(exec_on_edit_result_data_source()));
	connect(dsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(exec_on_edit_result_data_source()));
	connect(dsRemove, SIGNAL(clicked()), this, SLOT(exec_on_remove_result_data_source()));

	connect(dsUp, SIGNAL(clicked()), this, SLOT(exec_on_up_result_data_source()));
	connect(dsDown, SIGNAL(clicked()), this, SLOT(exec_on_down_result_data_source()));

	exec_on_check_resultInput();
}

void MDTRA_ResultDialog :: exec_on_update_layout_and_scale_units( void )
{
	MDTRA_DataType currentDT = UTIL_GetDataSourceTypeId(dsTypeCombo->currentIndex());
	int currentDTi = (int)currentDT;
	int currentSU = dsScaleUnitsCombo->itemData(dsScaleUnitsCombo->currentIndex()).toUInt();
	MDTRA_Layout currentL = UTIL_GetDataSourceDefaultLayout(currentDT);

	rbLabel->setEnabled( UTIL_IsDataSourceLayoutChangeable(currentDT) );
	rbTime->setEnabled( UTIL_IsDataSourceLayoutChangeable(currentDT) );
	rbRes->setEnabled( UTIL_IsDataSourceLayoutChangeable(currentDT) );

	switch (currentL) {
	default:
	case MDTRA_LAYOUT_TIME: rbTime->setChecked( true ); break;
	case MDTRA_LAYOUT_RESIDUE: rbRes->setChecked( true ); break;
	}

	dsScaleUnitsCombo->clear();

	for (int i = 0, j = 0; i < MDTRA_YSU_MAX; i++) {
		if (uiScaleUnitMap[i] & (1 << currentDTi)) {
			dsScaleUnitsCombo->addItem( szScaleUnitNames[i], i );
			if (i == currentSU) dsScaleUnitsCombo->setCurrentIndex(j);
			j++;
		}
	}

	dsScaleUnitsCombo->setEnabled( currentDT != MDTRA_DT_USER );
}

void MDTRA_ResultDialog :: exec_on_check_resultInput( void )
{
	dsTypeCombo->setEnabled( dsList->count() == 0 );
	dsUp->setEnabled( dsList->count() > 1 );
	dsDown->setEnabled( dsList->count() > 1 );

	buttonBox->button( QDialogButtonBox::Ok )->setEnabled( 
		(lineEdit->text().length() > 0) &&
		(dsList->count() > 0));
}

void MDTRA_ResultDialog :: exec_on_accept( void )
{
	if (m_iResultIndex < 0) {
		if (!m_pMainWindow->getProject()->checkUniqueResultName( lineEdit->text() )) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Result \"%1\" already registered.\nPlease enter another result title.").arg(lineEdit->text()));
			return;
		}
	}

	MDTRA_DataType currentDT = UTIL_GetDataSourceTypeId(dsTypeCombo->currentIndex());
	MDTRA_YScaleUnits currentSU = (MDTRA_YScaleUnits)dsScaleUnitsCombo->itemData(dsScaleUnitsCombo->currentIndex()).toUInt();
	MDTRA_Layout currentL;

	if (rbRes->isChecked()) 
		currentL = MDTRA_LAYOUT_RESIDUE;
	else
		currentL = MDTRA_LAYOUT_TIME;

	if (m_iResultIndex < 0) {
		m_pMainWindow->getProject()->registerResult( lineEdit->text(), currentDT, currentSU, currentL, m_dsRefList, true );
	} else {
		m_pMainWindow->getProject()->modifyResult( m_iResultIndex, lineEdit->text(), currentDT, currentSU, currentL, m_dsRefList );
	}

	accept();
}

void MDTRA_ResultDialog :: exec_on_add_result_data_source( void )
{
	MDTRA_DataType currentDT = UTIL_GetDataSourceTypeId(dsTypeCombo->currentIndex());
	MDTRA_ResultDataSourceDialog dialog( currentDT, NULL, m_pMainWindow, this );

	if (!dialog.GetAvailableDataSourceCount()) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No data sources of type \"%1\" are registered!").arg(UTIL_GetDataSourceShortTypeName(dsTypeCombo->currentIndex())));
        return;
	}

	QString sCheckUserData("");
	bool bCheckUserData = false;
	if (m_dsRefList.count() > 0) {
		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSourceByIndex( m_dsRefList.at(0).dataSourceIndex );
		sCheckUserData = pDS->userdata;
		bCheckUserData = true;
	}

	if (dialog.exec()) {
		MDTRA_DSRef newdsref;
		dialog.GetResultDataSource( &newdsref );

		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSourceByIndex( newdsref.dataSourceIndex );
		if ( bCheckUserData && (sCheckUserData != pDS->userdata) ) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot add user-defined Data Source of type \"%1\"!\nThe Result Collector already has user-defined Data Source of type \"%2\".").arg(pDS->userdata).arg(sCheckUserData));
			return;
		}

		m_dsRefList << newdsref;
		QListWidgetItem *pItem = new QListWidgetItem( QObject::tr("DATA SOURCE %1: %2\nScale = %3   Bias = %4")
														.arg(pDS->index)
														.arg(pDS->name)
														.arg(newdsref.yscale)
														.arg(newdsref.bias), 
														dsList );
		pItem->setIcon( QIcon(":/png/16x16/source.png") );
		exec_on_check_resultInput();
	}
}

void MDTRA_ResultDialog :: exec_on_edit_result_data_source( void )
{
	MDTRA_DataType currentDT = UTIL_GetDataSourceTypeId(dsTypeCombo->currentIndex());
	QListWidgetItem *pItem = dsList->currentItem();
	MDTRA_DSRef *pCurrentRef = const_cast<MDTRA_DSRef*>(&m_dsRefList.at(dsList->currentRow()));
	MDTRA_ResultDataSourceDialog dialog( currentDT, pCurrentRef, m_pMainWindow, this );

	if (dialog.exec()) {
		dialog.GetResultDataSource( pCurrentRef );
	
		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSourceByIndex( pCurrentRef->dataSourceIndex );
		pItem->setText( QObject::tr("DATA SOURCE %1: %2\nScale = %3   Bias = %4")
														.arg(pDS->index)
														.arg(pDS->name)
														.arg(pCurrentRef->yscale)
														.arg(pCurrentRef->bias));
	}
}

void MDTRA_ResultDialog :: exec_on_remove_result_data_source( void )
{
	if (dsList->selectedItems().count() <= 0)
		return;

	if (QMessageBox::No == QMessageBox::warning( this, tr("Confirm"), tr("Do you want to remove selected result data source from the list?"), 
												 QMessageBox::Yes | QMessageBox::Default,
												 QMessageBox::No | QMessageBox::Escape )) {
		return;
	}

	int itemIndex = dsList->currentRow();
	m_dsRefList.removeAt( itemIndex );
	QListWidgetItem *pItem = dsList->currentItem();
	dsList->removeItemWidget( pItem );
	delete pItem;
	exec_on_check_resultInput();
}

void MDTRA_ResultDialog :: exec_on_up_result_data_source( void )
{
	if (dsList->selectedItems().count() <= 0)
		return;

	int itemIndex = dsList->currentRow();
	if (itemIndex <= 0)
		return;

	m_dsRefList.swap( itemIndex, itemIndex-1 );
	
    QListWidgetItem *currentItem = dsList->takeItem( itemIndex );
    dsList->insertItem( itemIndex - 1, currentItem );
	dsList->setCurrentItem( currentItem );
}

void MDTRA_ResultDialog :: exec_on_down_result_data_source( void )
{
	if (dsList->selectedItems().count() <= 0)
		return;

	int itemIndex = dsList->currentRow();
	if (itemIndex >= dsList->count()-1)
		return;

	m_dsRefList.swap( itemIndex, itemIndex+1 );
	
    QListWidgetItem *currentItem = dsList->takeItem( itemIndex );
    dsList->insertItem( itemIndex + 1, currentItem );
	dsList->setCurrentItem( currentItem );
}

void MDTRA_ResultDialog :: exec_on_add_multiple_result_data_sources( void )
{
	MDTRA_DataType currentDT = UTIL_GetDataSourceTypeId(dsTypeCombo->currentIndex());
	MDTRA_MultiResultDataSourceDialog dialog( currentDT, m_pMainWindow, this );

	if (!dialog.GetAvailableDataSourceCount()) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No data sources of type \"%1\" are registered!").arg(UTIL_GetDataSourceShortTypeName(dsTypeCombo->currentIndex())));
        return;
	}

	QString sCheckUserData("");
	bool bCheckUserData = false;
	if (m_dsRefList.count() > 0) {
		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSourceByIndex( m_dsRefList.at(0).dataSourceIndex );
		sCheckUserData = pDS->userdata;
		bCheckUserData = true;
	}

	if (dialog.exec()) {
		for (int i = 0; i < dialog.GetAvailableDataSourceCount(); i++) {
			MDTRA_DSRef newdsref;
			if (!dialog.GetResultDataSource( i, &newdsref ))
				continue;

			MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSourceByIndex( newdsref.dataSourceIndex );
			if ( bCheckUserData && (sCheckUserData != pDS->userdata) ) {
				QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot add user-defined Data Source of type \"%1\"!\nThe Result Collector already has user-defined Data Source of type \"%2\".").arg(pDS->userdata).arg(sCheckUserData));
				continue;
			}

			m_dsRefList << newdsref;
			QListWidgetItem *pItem = new QListWidgetItem( QObject::tr("DATA SOURCE %1: %2\nScale = %3   Bias = %4")
															.arg(pDS->index)
															.arg(pDS->name)
															.arg(newdsref.yscale)
															.arg(newdsref.bias), 
															dsList );
			pItem->setIcon( QIcon(":/png/16x16/source.png") );

			if ( !bCheckUserData ) {
				sCheckUserData = pDS->userdata;
				bCheckUserData = true;
			}
		}
		exec_on_check_resultInput();
	}
}