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
//	Implementation of MDTRA_PreferencesDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_colors.h"
#include "mdtra_preferencesDialog.h"
#include "mdtra_cpuid.h"
#include "mdtra_project.h"
#include "mdtra_pdb_format.h"
#include "mdtra_formatDialog.h"
#include "mdtra_cuda.h"
#include "mdtra_SAS.h"

#include <QtGui/QColorDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QListWidget>
#include <QtGui/QFileDialog>

static const char *szScaleUnitNames[MDTRA_XSU_MAX] = 
{
	"Snapshots",
	"Picoseconds",
	"Nanoseconds"
};

static const char *szLanguageNames[MDTRA_LANG_MAX] = 
{
	"English",
	"Русский"
};

MDTRA_PreferencesDialog :: MDTRA_PreferencesDialog( QWidget *parent )
						 : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );

	//!FIXME
	lblText->setVisible( false );
	fontButton->setVisible( false );

	cbMultisampleAA->setChecked( m_pMainWindow->multisampleAA() );
	cbLowPriority->setChecked( m_pMainWindow->lowPriority() );
#if defined(MDTRA_ALLOW_SSE)
	cbSSE->setChecked( m_pMainWindow->allowSSE() );
	cbSSE->setEnabled( g_bSupportsSSE );
#else
	cbSSE->setChecked( false );
	cbSSE->setEnabled( false );
#endif

#if defined(MDTRA_ALLOW_CUDA)
	cbUseCUDA->setChecked( m_pMainWindow->useCUDA() );
	cbUseCUDA->setEnabled( g_bSupportsCUDA );
#else
	cbUseCUDA->setChecked( false );
	cbUseCUDA->setEnabled( false );
#endif

	mtCombo->clear();
	mtCombo->addItem( "Autodetect" );
	mtCombo->addItem( "Single-threaded" );
	for (int i = 2; i <= MDTRA_MAX_THREADS; i++)
		mtCombo->addItem( tr("Multi-threaded (%1 threads)").arg(i) );

	int currentThreads = m_pMainWindow->numThreads();
	if (currentThreads < 0) 
		currentThreads = 0;
	if (currentThreads > MDTRA_MAX_THREADS)
		currentThreads = MDTRA_MAX_THREADS;
	mtCombo->setCurrentIndex( currentThreads );

	cbDataFilter->setChecked( m_pMainWindow->plotDataFilter() );
	sbDataFilter->setValue( m_pMainWindow->plotDataFilterSize() );
	cbPlotPolarAngles->setChecked( m_pMainWindow->plotPolarAngles() );

	xsuCombo->clear();
	for (int i = 0; i < MDTRA_XSU_MAX; i++)
		xsuCombo->addItem( szScaleUnitNames[i] );
	xsuCombo->setCurrentIndex( m_pMainWindow->XScaleUnits() );

	langCombo->clear();
	for (int i = 0; i < MDTRA_LANG_MAX; i++)
		langCombo->addItem( QString::fromLocal8Bit( szLanguageNames[i] ) );
	langCombo->setCurrentIndex( m_pMainWindow->PlotLanguage() );

	float probeRadius;
	int subdivisions;
	bool excludeWater;
	MDTRA_GetSASParms( probeRadius, subdivisions, excludeWater );
	sasProbeRadius->setValue( probeRadius );
	sasAccuracy->setCurrentIndex( subdivisions );
	cbSasNoWater->setChecked( excludeWater );

	m_ColorList.clear();
	for (int i = 0; i < m_pMainWindow->getColorManager()->numColors(); i++)
		m_ColorList << m_pMainWindow->getColorManager()->color(i);

	m_pMainWindow->getColorManager()->fillPreferencesTable( colorList );
	if (m_pMainWindow->getColorManager()->numColors() > 0) {
		colorList->item(0)->setSelected( true );
		exec_on_colorList_itemClicked( colorList->item(0) );
	}

	switch (m_pMainWindow->ViewerType()) {
	default:
	case 0: viewer0->setChecked( true ); break;
	case 1: viewer1->setChecked( true ); break;
	case 2: viewer2->setChecked( true ); break;
	case 3: viewer3->setChecked( true ); break;
	}
	strRasMol->setText( m_pMainWindow->getRasMolPath() );
	strVMD->setText( m_pMainWindow->getVMDPath() );

#if defined(NO_INTERNAL_PDB_RENDERER)
	if ( viewer1->isChecked() )
		viewer0->setChecked( true );
	viewer1->setEnabled( false );
#endif

	g_PDBFormatManager.pushFormats();
	g_PDBFormatManager.fillFormatList( formatList, 0, g_PDBFormatManager.getDefaultFormat() );
	exec_on_formatList_selChange();

	connect(formatList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(exec_on_formatList_selChange()));
	connect(btnFormatAdd, SIGNAL(clicked()), this, SLOT(exec_on_format_add()));
	connect(btnFormatEdit, SIGNAL(clicked()), this, SLOT(exec_on_format_edit()));
	connect(btnFormatDelete, SIGNAL(clicked()), this, SLOT(exec_on_format_delete()));
	connect(btnFormatDefault, SIGNAL(clicked()), this, SLOT(exec_on_format_setDefault()));
	connect(colorList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(exec_on_colorList_itemClicked(QListWidgetItem*)));
	connect(colorList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(exec_on_colorList_itemDblClicked(QListWidgetItem*)));
	connect(colorChangeButton, SIGNAL(clicked()), this, SLOT(exec_on_colorChangeButton_click()));
	connect(browseRasMol, SIGNAL(clicked()), this, SLOT(browse_RasMol()));
	connect(browseVMD, SIGNAL(clicked()), this, SLOT(browse_VMD()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_on_accept()));
}

void MDTRA_PreferencesDialog :: savePreferences( void )
{
	bool bNeedsRestart = (( !m_pMainWindow->multisampleContext() && cbMultisampleAA->isChecked() && (cbMultisampleAA->isChecked() != m_pMainWindow->multisampleAA()) ));

	m_pMainWindow->setMultisampleAA( cbMultisampleAA->isChecked() );
	m_pMainWindow->setLowPriority( cbLowPriority->isChecked() );

	bool bAllowSSE = cbSSE->isChecked();
	if (!g_bSupportsSSE)
		bAllowSSE = false;
	m_pMainWindow->setAllowSSE( bAllowSSE );
#if defined(MDTRA_ALLOW_SSE)
	g_bAllowSSE = bAllowSSE;
#endif

	bool bAllowCUDA = cbUseCUDA->isChecked();
	if (!g_bSupportsCUDA)
		bAllowCUDA = false;
	m_pMainWindow->setUseCUDA( bAllowCUDA );
#if defined(MDTRA_ALLOW_CUDA)
	g_bAllowCUDA = bAllowCUDA;
#endif

	m_pMainWindow->setNumThreads( mtCombo->currentIndex() ? mtCombo->currentIndex() : -1 );
	m_pMainWindow->setPlotDataFilter( cbDataFilter->isChecked() );
	m_pMainWindow->setPlotDataFilterSize( sbDataFilter->value() );
	m_pMainWindow->setPlotPolarAngles( cbPlotPolarAngles->isChecked() );
	m_pMainWindow->setXScaleUnits( xsuCombo->currentIndex() );
	m_pMainWindow->setPlotLanguage( (MDTRA_Languages)langCombo->currentIndex() );

	float probeRadius = sasProbeRadius->value();
	int subdivisions = sasAccuracy->currentIndex();
	bool excludeWater = cbSasNoWater->isChecked();
	MDTRA_SetSASParms( probeRadius, subdivisions, excludeWater );

	if (viewer0->isChecked())
		m_pMainWindow->setViewerType(0);
	else if (viewer1->isChecked())
		m_pMainWindow->setViewerType(1);
	else if (viewer2->isChecked()) {
		m_pMainWindow->setRasMolPath( strRasMol->text() );
		m_pMainWindow->setViewerType(2);
	} else if (viewer3->isChecked()) {
		m_pMainWindow->setVMDPath( strVMD->text() );
		m_pMainWindow->setViewerType(3);
	}

	m_pMainWindow->getColorManager()->applyColorChanges(m_ColorList);

	if (bNeedsRestart) {
		QMessageBox::warning(m_pMainWindow, tr(APPLICATION_TITLE_SMALL), tr("Some settings will take effect only upon restarting\n%1!").arg(APPLICATION_TITLE_FULL));
	}

	g_PDBFormatManager.popFormats( false );
}

void MDTRA_PreferencesDialog :: discardPreferences( void )
{
	g_PDBFormatManager.popFormats( true );
}

void MDTRA_PreferencesDialog :: exec_on_colorList_itemClicked( QListWidgetItem *item )
{
	int colorIndex = 0;
	if (item) 
		colorIndex = item->data(Qt::UserRole).toInt();
	lblColorName->setText(QString("<b>%1</b>").arg(m_pMainWindow->getColorManager()->colorTitle(colorIndex)));
	lblColorValue->setPalette(QPalette(m_ColorList[colorIndex]));
}

void MDTRA_PreferencesDialog :: exec_on_colorList_itemDblClicked( QListWidgetItem *item )
{
	int colorIndex = 0;
	if (item) 
		colorIndex = item->data(Qt::UserRole).toInt();
	QColor newColor = QColorDialog::getColor( m_ColorList[colorIndex], this, tr("Choose Color") );
	if ( newColor.isValid() ) {
		m_ColorList[colorIndex] = newColor;
		lblColorValue->setPalette(QPalette(m_ColorList[colorIndex]));
	}
}

void MDTRA_PreferencesDialog :: exec_on_colorChangeButton_click( void )
{
	int colorIndex = 0;
	if (colorList->currentItem())
		colorIndex = colorList->currentItem()->data(Qt::UserRole).toInt();
	QColor newColor = QColorDialog::getColor( m_ColorList[colorIndex], this, tr("Choose Color") );
	if ( newColor.isValid() ) {
		m_ColorList[colorIndex] = newColor;
		lblColorValue->setPalette(QPalette(m_ColorList[colorIndex]));
	}
}

void MDTRA_PreferencesDialog :: exec_on_formatList_selChange( void )
{
	btnFormatEdit->setEnabled( false );
	btnFormatDelete->setEnabled( false );
	btnFormatDefault->setEnabled( false );

	if (!formatList->currentItem())
		return;

	unsigned int formatIdent = formatList->currentItem()->data(Qt::UserRole).toInt();
	const PDBFormat_t *pFormat = g_PDBFormatManager.fetchFormat( formatIdent );
	if (!pFormat)
		return;

	btnFormatEdit->setEnabled( pFormat->editable );
	btnFormatDelete->setEnabled( pFormat->editable );
	btnFormatDefault->setEnabled( true );
}

void MDTRA_PreferencesDialog :: exec_on_format_add( void )
{
	MDTRA_FormatDialog dialog( 0xFFFFFFFF, m_pMainWindow, this );
	if (dialog.exec()) {
		PDBFormat_t tempFormat;
		dialog.getFormatInfo( &tempFormat );
		unsigned int ident = g_PDBFormatManager.registerFormat( &tempFormat );
		g_PDBFormatManager.fillFormatList( formatList, ident, g_PDBFormatManager.getDefaultFormat() );
	}
}

void MDTRA_PreferencesDialog :: exec_on_format_edit( void )
{
	if (!formatList->currentItem())
		return;

	unsigned int formatIdent = formatList->currentItem()->data(Qt::UserRole).toInt();
	const PDBFormat_t *pFormat = g_PDBFormatManager.fetchFormat( formatIdent );
	if (!pFormat)
		return;
	if (!pFormat->editable)
		return;

	MDTRA_FormatDialog dialog( formatIdent, m_pMainWindow, this );
	if (dialog.exec()) {
		PDBFormat_t tempFormat;
		dialog.getFormatInfo( &tempFormat );
		g_PDBFormatManager.modifyFormat( pFormat, tempFormat.title, tempFormat.fields );
	}
}

void MDTRA_PreferencesDialog :: exec_on_format_delete( void )
{
	if (!formatList->currentItem())
		return;

	unsigned int formatIdent = formatList->currentItem()->data(Qt::UserRole).toInt();
	const PDBFormat_t *pFormat = g_PDBFormatManager.fetchFormat( formatIdent );
	if (!pFormat)
		return;
	if (!pFormat->editable)
		return;

	if (int c = m_pMainWindow->getProject()->getStreamCountByFormatIdentifier( formatIdent )) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot delete format \"%1\": it is used by %2 stream(s)!").arg(pFormat->title).arg(c));
		return;
	}

	int r = QMessageBox::warning( this, tr("Confirm"), tr("Are you sure to delete format \"%1\"?").arg(pFormat->title), 
								  QMessageBox::Yes | QMessageBox::Default, QMessageBox::No );
	if (r == QMessageBox::No)
		return;

	if (g_PDBFormatManager.getDefaultFormat() == formatIdent)
		g_PDBFormatManager.setDefaultFormat( PDB_GENERIC_FORMAT );

	g_PDBFormatManager.unregisterFormat( pFormat );
	g_PDBFormatManager.fillFormatList( formatList, 0, g_PDBFormatManager.getDefaultFormat() );
}

void MDTRA_PreferencesDialog :: exec_on_format_setDefault( void )
{
	if (!formatList->currentItem())
		return;

	unsigned int formatIdent = formatList->currentItem()->data(Qt::UserRole).toInt();
	if (g_PDBFormatManager.getDefaultFormat() != formatIdent) {
		g_PDBFormatManager.setDefaultFormat( formatIdent );
		g_PDBFormatManager.fillFormatList( formatList, formatIdent, formatIdent );
	}
}

void MDTRA_PreferencesDialog :: browse_RasMol( void )
{
#if defined(WIN32)
	char *programFilesPath = NULL;
	_dupenv_s(&programFilesPath, NULL, "PROGRAMFILES");
	QString fileName = QFileDialog::getOpenFileName( this, tr("Browse RasMol"), programFilesPath, "RasMol Executable (*.exe)" );
#else
	const char *programFilesPath = "/usr/local/bin";
	QString fileName = QFileDialog::getOpenFileName( this, tr("Browse RasMol"), programFilesPath, "RasMol Executable (*)" );
#endif

	if (!fileName.isEmpty())
		strRasMol->setText( fileName );

#if defined(WIN32)
	if (programFilesPath)
		free(programFilesPath);
#endif
}

void MDTRA_PreferencesDialog :: browse_VMD( void )
{
#if defined(WIN32)
	char *programFilesPath = NULL;
	_dupenv_s(&programFilesPath, NULL, "PROGRAMFILES");
	QString fileName = QFileDialog::getOpenFileName( this, tr("Browse VMD"), programFilesPath, "VMD Executable (*.exe)" );
#else
	const char *programFilesPath = "/usr/local/bin";
	QString fileName = QFileDialog::getOpenFileName( this, tr("Browse VMD"), programFilesPath, "VMD Executable (*)" );
#endif

	if (!fileName.isEmpty())
		strVMD->setText( fileName );

#if defined(WIN32)
	if (programFilesPath)
		free(programFilesPath);
#endif
}

void MDTRA_PreferencesDialog :: exec_on_accept( void )
{
	if (viewer2->isChecked()) {
		if (strRasMol->text().isEmpty()) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Please specify the path to RasMol executable!"));
			return;
		}
		QFileInfo fi(strRasMol->text());
		if (!fi.exists()) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot find RasMol executable:\n\n%1").arg(strRasMol->text()));
			return;
		}
	} 
	if (viewer3->isChecked()) {
		if (strVMD->text().isEmpty()) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Please specify the path to VMD executable!"));
			return;
		}
		QFileInfo fi(strVMD->text());
		if (!fi.exists()) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot find VMD executable:\n\n%1").arg(strVMD->text()));
			return;
		}
	} 

	accept();
}
