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
//	Implementation of MDTRA_PrepWaterShellDialog

#include "mdtra_main.h"
#include "mdtra_pipe.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_prepWaterShellDialog.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QKeyEvent>

#if defined(WIN32)
static const QString s_OSName( "Windows" );
#elif defined(LINUX)
static const QString s_OSName( "Linux" );
#else
static const QString s_OSName( "Unknown OS" );
#endif

#define PREP_PROGRAM_NAME		"wbr"

MDTRA_PrepWaterShellDialog :: MDTRA_PrepWaterShellDialog( QWidget *parent )
							: QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/water.png") );

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

	memset( m_szTempName, 0, sizeof(m_szTempName) );
	m_pPipe = NULL;
	m_bInterrupt = false;

	connect(sCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_stream_change()));
	connect(txtOutputPDB, SIGNAL(textChanged(const QString&)), this, SLOT(exec_on_check_input()));
	connect(btnOutputPDB, SIGNAL(clicked()), this, SLOT(exec_on_browse_output()));
	connect(btnRun, SIGNAL(clicked()), this, SLOT(exec_on_prep()));
	connect(btnClose, SIGNAL(clicked()), this, SLOT(reject()));
}

MDTRA_PrepWaterShellDialog :: ~MDTRA_PrepWaterShellDialog()
{
}

void MDTRA_PrepWaterShellDialog :: exec_on_stream_change( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	eIndex->setMaximum( pStream->files.count() );
#if QT_VERSION >= 0x040700
	QString streamPDB = QFileInfo( pStream->files.at( 0 ) ).completeBaseName();
	txtOutputPDB->setPlaceholderText( streamPDB.append( ".wbr.pdb" ) );
#endif
}

void MDTRA_PrepWaterShellDialog :: exec_on_browse_output( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );
	if ( !pStream )
		return;

	QString srcDir = pStream->files.at( 0 );
	if ( !txtOutputPDB->text().isEmpty() )
		srcDir = txtOutputPDB->text();

	QString pdbName = QFileDialog::getSaveFileName( this, QString(), srcDir, "PDB Files (*.pdb)"  );
	if ( !pdbName.isEmpty() ) {
		QFileInfo fi(pdbName);
		if ( fi.suffix().isEmpty() )
			pdbName.append( ".pdb" );
		txtOutputPDB->setText( pdbName );
	}
}

void MDTRA_PrepWaterShellDialog :: exec_on_check_input( void )
{
#if QT_VERSION >= 0x040700
	btnRun->setEnabled( !txtOutputPDB->text().isEmpty() || !txtOutputPDB->placeholderText().isEmpty() );
#else
	btnRun->setEnabled( !txtOutputPDB->text().isEmpty() );
#endif
}

void MDTRA_PrepWaterShellDialog :: reject( void )
{
	if ( m_pPipe ) {
		if ( QMessageBox::No == QMessageBox::warning( this, tr(APPLICATION_TITLE_SMALL),
			tr( "External program is still running.\nWould you like to interrupt it?" ), QMessageBox::Yes | QMessageBox::No ) ) {
			return;	
		}
		m_bInterrupt = true;
		m_pPipe->stop();
	}
	QDialog::reject();
}

void MDTRA_PrepWaterShellDialog :: exec_on_prep( void )
{
	int streamIndex = sCombo->itemData( sCombo->currentIndex() ).toInt();
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( streamIndex );

	if (!pStream || !pStream->pdb) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Bad stream(s) selected!"));
		return;
	}

	txtOutput->clear();
	btnRun->setEnabled( false );

	char tempBuf[512];
	int errNo = 0;
	FILE *tempFp = NULL;

	if ( !*m_szTempName )
		tmpnam_s( m_szTempName );

	if ( ( errNo = (int)fopen_s( &tempFp, m_szTempName, "w" ) ) != 0 ) {
		strerror_s( tempBuf, sizeof(tempBuf), errNo );
		QString cmdErr = QString( "* Could not open temp file:\n   %1\n* %2 gave the error message:\n   \"%3\"\n" ).arg( m_szTempName ).arg( s_OSName ).arg( QString::fromLocal8Bit( tempBuf ) );
		txtOutput->appendPlainText( cmdErr );
		btnRun->setEnabled( true );
		if ( tempFp ) 
			fclose( tempFp );
		return;
	}

	int trajectoryMin = 1;
	int trajectoryMax = pStream->files.count();
	if (trajectoryRange->isChecked()) {
		trajectoryMin = MDTRA_MAX( trajectoryMin, sIndex->value() );
		trajectoryMax = MDTRA_MIN( trajectoryMax, eIndex->value());
	}

	// write stream data
	for ( int i = trajectoryMin - 1; i < trajectoryMax; ++i )
		fprintf_s( tempFp, "%s\n", pStream->files.at( i ).toLocal8Bit().data() );

	fclose( tempFp );

	// build command line
	QString cmdName, cmdArgs, cmdOutFile;
#if defined(WIN32)
	cmdName = "./utils/" PREP_PROGRAM_NAME ".exe";
#else
	cmdName = "./utils/" PREP_PROGRAM_NAME;
#endif
	cmdOutFile = txtOutputPDB->text();
#if QT_VERSION >= 0x040700
	if ( cmdOutFile.isEmpty() )
		cmdOutFile = txtOutputPDB->placeholderText();
#endif
	cmdArgs = QString( "-i \"%1\" -o \"%2\"" ).arg( m_szTempName ).arg( cmdOutFile );

	QString cmdReport = QString( "** Executing...\n** Command: %1\n** Parameters: %2\n" ).arg( cmdName ).arg( cmdArgs );
	txtOutput->appendPlainText( cmdReport );

	// execute prep program
	m_pPipe = new MDTRA_Pipe( cmdName, cmdArgs, txtOutput );
	if ( m_pPipe ) {
		if ( m_pPipe->exec() ) {
			if ( m_bInterrupt )
				txtOutput->appendPlainText( QString( "** Interrupted by user.\n" ) );
			else
				txtOutput->appendPlainText( QString( "** Finished!\n" ) );
		}
		delete m_pPipe;
		m_pPipe = NULL;
	}

	m_bInterrupt = false;

#if defined(WIN32)
	_unlink( m_szTempName );
#else
	unlink( m_szTempName );
#endif
	btnRun->setEnabled( true );
}
