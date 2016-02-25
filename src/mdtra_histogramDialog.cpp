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
//	Implementation of MDTRA_HistogramDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_colors.h"
#include "mdtra_histogramDialog.h"
#include "mdtra_histogramPlot.h"

#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QtGui/QFileDialog>
#include <QtGui/QImageWriter>
#include <QtGui/QMessageBox>
#include <QtGui/QColorDialog>

enum {
	BC_SQRT = 0,
	BC_CRT,
	BC_STURGES,
	BC_MAX
};

static const char *szBCFuncName[BC_MAX] = 
{
"Square-root Choice",
"Cubic-root Choice",
"Sturges' Formula"
};

MDTRA_HistogramDialog :: MDTRA_HistogramDialog( QWidget *parent )
					  : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/chart.png") );

	m_pTimer = new QTimer( this );
	m_pTimer->setInterval( 50 );
	m_pTimer->setSingleShot( true );

	m_bBuildStarted = false;
	m_iMaxBands = 0;
	m_iNumBands = 0;
	m_pBands = NULL;
	m_customColor = m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 );
	lblColorValue->setPalette(QPalette(m_customColor));

	// fill band count choices
	aCombo->clear();
	for ( int i = 0; i < BC_MAX; i++ ) {
		aCombo->addItem( QString( szBCFuncName[i] ) );
	}

	int selRCi = m_pMainWindow->getSelectedResultCollectorIndex();
	int selIndex = 0;

	// fill result collector combo
	for (int i = 0; i < m_pMainWindow->getProject()->getResultCount(); i++) {
		MDTRA_Result *pResult = m_pMainWindow->getProject()->fetchResult( i );
		if (pResult && ( pResult->status > 0 )) {
			rCombo->addItem( QIcon(":/png/16x16/result.png"), 
							 tr("RESULT COLLECTOR %1: %2").arg(pResult->index).arg(pResult->name),
							 pResult->index );
			if ( pResult->index == selRCi )
				selIndex = i;
		}
	}
	rCombo->setCurrentIndex(selIndex);
	exec_on_result_change();

	progressBar->setVisible( false );
	progressBar->setValue( 0 );
	progressBar->setMaximum( 100 );

	QGLFormat pixelFormat;
	pixelFormat.setDoubleBuffer(true);
	pixelFormat.setDepth(false);
	pixelFormat.setRgba(true);
	pixelFormat.setSampleBuffers( m_pMainWindow->multisampleAA() );
	QGLFormat::setDefaultFormat(pixelFormat);

	m_pPlot = new MDTRA_HistogramPlot( pixelFormat, parent );
	m_pPlot->setParent( groupBox_3 );
	m_pPlot->setStatusTip( tr("Histogram Plot") );
	m_pPlot->setGeometry(QRect(10, 25, groupBox_3->width() - 20, groupBox_3->height() - 35));
	m_pPlot->setColorMode( MDTRA_HCM_AUTO, m_customColor );
	m_pPlot->setVisible( false );

	exec_on_desired_update();

	connect(rCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_result_change()));
	connect(dCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_rds_change()));
	connect(aCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_desired_update()));
	connect(radioButton, SIGNAL(toggled(bool)), this, SLOT(exec_on_desired_update()));
	connect(rbCAuto, SIGNAL(toggled(bool)), this, SLOT(exec_on_desired_redraw()));
	connect(rbCGray, SIGNAL(toggled(bool)), this, SLOT(exec_on_desired_redraw()));
	connect(rbCCustom, SIGNAL(toggled(bool)), this, SLOT(exec_on_desired_redraw()));
	connect(bandSpin, SIGNAL(valueChanged(int)), this, SLOT(exec_on_desired_update()));
	connect(btnRebuild, SIGNAL(clicked()), this, SLOT(exec_on_rebuild()));
	connect(btnSave, SIGNAL(clicked()), this, SLOT(exec_on_save()));
	connect(btnExport, SIGNAL(clicked()), this, SLOT(exec_on_export()));
	connect(btnClose, SIGNAL(clicked()), this, SLOT(reject()));
	connect(colorChangeButton, SIGNAL(clicked()), this, SLOT(exec_on_colorChangeButton_click()));
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(exec_on_rebuild()));
}

MDTRA_HistogramDialog :: ~MDTRA_HistogramDialog()
{
	if ( m_pTimer ) {
		delete m_pTimer;
		m_pTimer = NULL;
	}
	if ( m_pBands ) {
		delete [] m_pBands;
		m_pBands = NULL;
	}
}

int MDTRA_HistogramDialog :: current_result_index( void )
{
	return rCombo->itemData( rCombo->currentIndex() ).toInt();
}

int MDTRA_HistogramDialog :: current_datasource_index( void )
{
	return dCombo->itemData( dCombo->currentIndex() ).toInt();
}

int MDTRA_HistogramDialog :: current_rds_index( void )
{
	return dCombo->currentIndex();
}

void MDTRA_HistogramDialog :: exec_on_desired_update( void )
{
	if ( !cbAuto->isChecked() )
		return;

	if ( m_bBuildStarted )
		cancel_build();

	if ( m_pTimer )
		m_pTimer->start();
}

void MDTRA_HistogramDialog :: exec_on_desired_redraw( void )
{
	if ( cbAuto->isChecked() ) {
		MDTRA_HistogramColorMode cm = MDTRA_HCM_AUTO;
		if ( rbCGray->isChecked() )
			cm = MDTRA_HCM_GRAYSCALE;
		else if ( rbCCustom->isChecked() )
			cm = MDTRA_HCM_SINGLE;

		if ( m_pPlot->isVisible() ) {
			m_pPlot->setColorMode( cm, m_customColor );
			m_pPlot->repaint();
		}
	}
}

void MDTRA_HistogramDialog :: exec_on_colorChangeButton_click( void )
{
	QColor newColor = QColorDialog::getColor( m_customColor, this, tr("Choose Color") );
	if ( newColor.isValid() ) {
		m_customColor = newColor;
		lblColorValue->setPalette(QPalette(m_customColor));
		exec_on_desired_redraw();
	}
}

void MDTRA_HistogramDialog :: exec_on_result_change( void )
{
	MDTRA_Result *pResult = m_pMainWindow->getProject()->fetchResultByIndex( current_result_index() );
	assert( pResult != NULL );

	// fill result data source combo
	dCombo->clear();
	for (int i = 0; i < pResult->sourceList.count(); i++) {
		MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSourceByIndex( pResult->sourceList.at(i).dataSourceIndex );
		dCombo->addItem( QIcon(":/png/16x16/source.png"), 
						 tr("DATA SOURCE %1: %2").arg(pDS->index).arg(pDS->name),
						 pDS->index );
	}
	dCombo->setCurrentIndex(0);
}

void MDTRA_HistogramDialog :: exec_on_rds_change( void )
{
	int rds = current_rds_index();
	if ( rds < 0 )
		return;

	const MDTRA_Result *pResult = m_pMainWindow->getProject()->fetchResultByIndex( current_result_index() );
	assert( pResult != NULL );

	const MDTRA_DSRef *pDSRef = &pResult->sourceList.at(rds);
	assert( pDSRef != NULL );

	bandSpin->setMaximum( pDSRef->iDataSize );

	exec_on_desired_update();
}

void MDTRA_HistogramDialog :: reject( void )
{
	if (m_bBuildStarted)
		return;

	QDialog::reject();
}

void MDTRA_HistogramDialog :: cancel_build( void )
{
	m_bBuildStarted = false;
	btnRebuild->setText(tr("&Rebuild"));
	btnClose->setEnabled( true );
}

void MDTRA_HistogramDialog :: exec_on_rebuild( void )
{
	if (m_bBuildStarted) {
		cancel_build();
		return;
	}

	m_bBuildStarted = true;
	btnRebuild->setText(tr("S&top"));
	btnClose->setEnabled( false );
	btnSave->setEnabled( false );
	btnExport->setEnabled( false );

	//Show progress info and hide plot
	m_pPlot->setVisible( false );
	groupBox_3->repaint();
	progressBar->setValue( 0 );
	progressBar->setVisible( true );

	// Build histogram
	calc_histogram();

	if (!m_bBuildStarted ) {
		m_pPlot->setVisible( false );
		progressBar->setVisible( false );
		return;
	}

	//Hide progress info and show plot
	progressBar->setVisible( false );
	m_pPlot->setVisible( true );
	exec_on_desired_redraw();
	cancel_build();
	btnSave->setEnabled( true );
	btnExport->setEnabled( true );
	btnClose->setEnabled( true );
}

void MDTRA_HistogramDialog :: exec_on_save( void )
{
	QString imageFilters;
	QString selectedFilter;
	QMap<QString,QString> extMap;
	QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();

	for ( int i = 0; i < imageFormats.count(); i++ ) {
		QString s1 = QString( "%1 Files (*.%2)" ).arg( imageFormats.at(i).toUpper().constData() ).arg( imageFormats.at(i).toLower().constData() );
		extMap[s1] = QString( ".%1" ).arg( imageFormats.at(i).toLower().constData() );
		if (i > 0) imageFilters.append(";;");
		imageFilters.append( s1 );
	}

#ifdef MDTRA_ALLOW_PRINTER
	if (imageFilters.length() > 0) imageFilters.append(";;");
	imageFilters.append("PDF Files (*.pdf);;PostScript Files (*.ps);;");
	extMap["PDF Files (*.pdf)"] = ".pdf";
	extMap["PostScript Files (*.ps)"] = ".ps";
#endif

	if ( imageFilters.length() <= 0 ) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No image formats supported!"));
		return;
	}

	QString fileName = QFileDialog::getSaveFileName( this, tr("Save Histogram Plot"), m_pMainWindow->getCurrentFileDir(), imageFilters, &selectedFilter );
	if (!fileName.isEmpty()) {
		QFileInfo fi( fileName );
		if ( fi.suffix().isEmpty() ) {
			fileName.append( extMap[selectedFilter] );
			fi.setFile( fileName );
		}
		QString fType = fi.suffix().toUpper();
		m_pPlot->saveScreenshot( fileName, fType.toAscii() );
	}
}

void MDTRA_HistogramDialog :: exec_on_export( void )
{
	QString exportFilter = "Text Files (*.txt);;CSV Files (*.csv)";
	QString selectedFilter;
	QMap<QString,QString> extMap;
	extMap["Text Files (*.txt)"] = ".txt";
	extMap["CSV Files (*.csv)"] = ".csv";

	QString fileName = QFileDialog::getSaveFileName( this, tr("Export Histogram Data"), m_pMainWindow->getCurrentFileDir(), exportFilter, &selectedFilter );
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		if ( fi.suffix().isEmpty() ) {
			fileName.append( extMap[selectedFilter] );
			fi.setFile( fileName );
		}
		QFile f(fileName);
		// Open file for writing
		if (!f.open(QFile::WriteOnly | QFile::Truncate)) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open file for writing!\nFile: %1\nReason: %2").arg(fileName,f.errorString()));
			return;
		}

		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QTextStream stream(&f);
		exportResults( fi.suffix(), &stream );
		f.close();
		QApplication::restoreOverrideCursor();
	}
}

void MDTRA_HistogramDialog :: exportResults( const QString &fileSuffix, QTextStream *stream )
{
	const MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSourceByIndex( current_datasource_index() );
	assert( pDS != NULL );

	bool bCSV = false;
	if (fileSuffix.toLower() == "csv") {
		bCSV = true;
	}

	if (bCSV) {
		*stream << QString("\"%1\";\"\";\"\"").arg(pDS->name);
		*stream << endl;
		*stream << QString("\"Min\";\"Max\";\"Count\"");
		*stream << endl;
	} else {
		*stream << QString("\"%1\"\t\"\"\t\"\"").arg(pDS->name);
		*stream << endl;
		*stream << QString("\"Min\"\t\"Max\"\t\"Count\"");
		*stream << endl;
	}

	for (int i = 0; i < m_iNumBands; i++) {
		if (bCSV) {
			*stream << QString("%1;%2;%3")
				.arg(m_flBandMin+m_flBandSize*i, 0, 'f', 6)
				.arg(m_flBandMin+m_flBandSize*(i+1), 0, 'f', 6)
				.arg(m_pBands[i]);
		} else {
			*stream << QString("%1\t%2\t%3")
				.arg(m_flBandMin+m_flBandSize*i, 0, 'f', 6)
				.arg(m_flBandMin+m_flBandSize*(i+1), 0, 'f', 6)
				.arg(m_pBands[i]);
		}
		*stream << endl;
	}
}

void MDTRA_HistogramDialog :: calc_histogram( void )
{
	const MDTRA_Result *pResult = m_pMainWindow->getProject()->fetchResultByIndex( current_result_index() );
	assert( pResult != NULL );

	const MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSourceByIndex( current_datasource_index() );
	assert( pDS != NULL );

	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( pDS->streamIndex );
	assert( pStream != NULL );

	const MDTRA_DSRef *pDSRef = &pResult->sourceList.at(current_rds_index());
	assert( pDSRef != NULL );
	assert( pDSRef->iDataSize >= 0 );

	m_pPlot->setTitle( pDS->name );

	// determine number of bands
	int numBands = 0;
	if ( bandSpin->isEnabled() ) {
		numBands = bandSpin->value();
		if ( numBands < 1 ) {
			numBands = 1;
			bandSpin->setValue( numBands );
		} else if ( numBands > pDSRef->iDataSize ) {
			numBands = pDSRef->iDataSize;
			bandSpin->setValue( numBands );
		}
	} else {
		switch ( aCombo->currentIndex() ) {
		case BC_SQRT:
			numBands = (int)sqrtf( pDSRef->iDataSize );
			break;
		case BC_CRT:
			numBands = (int)powf( pDSRef->iDataSize, 1.0f / 3.0f );
			break;
		case BC_STURGES:
			numBands = (int)ceil( logf(pDSRef->iDataSize)/logf(2.0f) + 1.0f );
			break;
		}
	}

	if ( !m_pBands || numBands > m_iMaxBands ) {
		// reallocate bands
		m_iMaxBands = numBands;
		if ( m_pBands ) delete [] m_pBands;
		m_pBands = new int[m_iMaxBands];
	}
	memset( m_pBands, 0, sizeof(int)*numBands );

	// determine band size
	int updateLimit = pDSRef->iDataSize / 10;
	int updateCounter = 0;

	// calculate min and range (we can't use stat parms, since they can be invalid for user-defined types)
	float fMin = FLT_MAX;
	float fMax = FLT_MIN;
	for ( int i = 0; i < pDSRef->iDataSize; i++, updateCounter++ ) {
		if ( updateCounter >= updateLimit ) {
			// check for cancel, update histogram
			QApplication::processEvents();
			if ( !m_bBuildStarted )
				break;
			progressBar->setValue( progressBar->value() + 5 );
			progressBar->repaint();
			updateCounter = 0;
		}
		if ( pDSRef->pData[i] < fMin ) fMin = pDSRef->pData[i];
		if ( pDSRef->pData[i] > fMax ) fMax = pDSRef->pData[i];
	}

	// check for interrupt
	if ( !m_bBuildStarted )
		return;

	m_iNumBands = numBands;
	m_flBandMin = fMin;
	m_flBandSize = ( fMax - fMin ) / numBands;

	// fill bands
	updateCounter = 0;
	for ( int i = 0; i < pDSRef->iDataSize; i++, updateCounter++ ) {
		if ( updateCounter >= updateLimit ) {
			// check for cancel, update histogram
			QApplication::processEvents();
			if ( !m_bBuildStarted )
				break;
			progressBar->setValue( progressBar->value() + 5 );
			progressBar->repaint();
			updateCounter = 0;
		}
		// accumulate band
		int bandIndex = (int)floor( ( pDSRef->pData[i] - m_flBandMin ) / m_flBandSize );
		if ( bandIndex == numBands ) --bandIndex;
		assert( bandIndex >= 0 && bandIndex < numBands );
		m_pBands[bandIndex]++;
	}

	// check for interrupt
	if ( !m_bBuildStarted )
		return;

	// set histogram data
	m_pPlot->setAxisInfo( pResult->type, pResult->units, pResult->layout, pStream->xscale, pDS->userdata );
	m_pPlot->setStatInfo( pDSRef->stat[MDTRA_SP_ARITHMETIC_MEAN], pDSRef->stat[MDTRA_SP_STDDEV] );
	m_pPlot->setHistogramData( m_flBandMin, m_flBandSize, numBands, m_pBands );
}

