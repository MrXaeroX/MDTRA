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
//	Implementation of MDTRA_PDB_Renderer

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_colors.h"
#include "mdtra_pdbCanvas.h"
#include "mdtra_pdbRenderer.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QKeyEvent>

const char *szPDBRenderModeNames[] = {
	"(None)",
	"Points",
	"Lines",
	"Spheres"
};

MDTRA_PDB_Renderer :: MDTRA_PDB_Renderer( QWidget *parent )
					: QWidget( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	QImage img( 16, 16, QImage::Format_RGB888 );
	QColor gray( Qt::lightGray );
	img.fill( 0xffffffff );
	for ( int i = 0; i < 10; i++ ) {
		for ( int j = 0; j < 10; j++ ) {
			img.setPixel( 3 + i, 3 + j, (i && j && i!=9 && j!=9) ? gray.rgb() : 0 );
		}
	}
	m_whiteIcon = QIcon( QPixmap::fromImage( img ) );

	m_streams.reserve( 16 );
	m_iStreamCounter = 0;
	m_currentSelMode = MDTRA_PRM_POINTS;
	m_bPlayback = false;
	m_bUpdatingSnapshot = false;
	createControls();
	enableControls( false );
}

MDTRA_PDB_Renderer :: ~MDTRA_PDB_Renderer()
{

}

bool MDTRA_PDB_Renderer :: event( QEvent *ev )
{
	if (ev->type() == QEvent::KeyPress) {
		QKeyEvent *kev = static_cast<QKeyEvent*>(ev);
		if (kev->key() == Qt::Key_Return) {
			if (m_pSelString->hasFocus()) {
				return true;
			}
		}
	}
	return QWidget::event( ev );
}

void MDTRA_PDB_Renderer :: setActive( bool value )
{
	if ( value ) {
		if ( m_pCanvas )
			m_pCanvas->startUpdateTimer();
	} else {
		if ( m_pCanvas )
			m_pCanvas->stopUpdateTimer();
	}
}

void MDTRA_PDB_Renderer :: createControls( void )
{
	const int numSelModes = sizeof(szPDBRenderModeNames)/sizeof(szPDBRenderModeNames[0]);

	// Pixel format of the canvas
	QGLFormat pixelFormat;
	pixelFormat.setDoubleBuffer(true);
	pixelFormat.setDepth(true);
	pixelFormat.setRgba(true);
	pixelFormat.setSwapInterval(0);	//!TODO: make tweakable ("vsync")
	QGLFormat::setDefaultFormat(pixelFormat);

	// Create canvas
	m_pCanvas = new MDTRA_PDB_Canvas( pixelFormat, parentWidget() );
	m_pCanvas->setParent( this );
	m_pCanvas->setGeometry(QRect(0, 0, 320, 240));
	m_pCanvas->setStatusTip( tr("PDB rendition window") );

	// Create playback controls
	m_pPlaybackStart = new QPushButton( this );
	m_pPlaybackStart->setGeometry(QRect(0, 0, 22, 22));
	m_pPlaybackStart->setFixedSize( 22, 22 );
	m_pPlaybackStart->setIcon( QIcon(":/png/16x16/play.png") );
	m_pPlaybackStart->setStatusTip( tr("Start trajectory animation playback") );
	m_pPlaybackStop = new QPushButton( this );
	m_pPlaybackStop->setGeometry(QRect(0, 0, 22, 22));
	m_pPlaybackStop->setFixedSize( 22, 22 );
	m_pPlaybackStop->setIcon( QIcon(":/png/16x16/stop.png") );
	m_pPlaybackStop->setStatusTip( tr("Stop trajectory animation playback") );
	m_pPlaybackSlider = new QSlider( this );
	m_pPlaybackSlider->setGeometry(QRect(0, 0, 100, 22));
	m_pPlaybackSlider->setOrientation(Qt::Horizontal);
	m_pPlaybackSlider->setMinimum(0);
	m_pPlaybackSlider->setMaximum(99999);
	m_pPlaybackSlider->setValue(0);
	m_pPlaybackSlider->setStatusTip( tr("Current position in the trajectory") );
	m_pPlaybackSnap = new QSpinBox( this );
	m_pPlaybackSnap->setGeometry(QRect(0, 0, 100, 22));
	m_pPlaybackSnap->setMinimum(0);
	m_pPlaybackSnap->setMaximum(99999);
	m_pPlaybackSnap->setValue(0);
	m_pPlaybackSnap->setStatusTip( tr("Current snapshot index of the trajectory") );
	m_pPlaybackSpeed = new QDoubleSpinBox( this );
	m_pPlaybackSpeed->setGeometry(QRect(0, 0, 100, 22));
	m_pPlaybackSpeed->setMinimum(0.01);
	m_pPlaybackSpeed->setMaximum(60);
	m_pPlaybackSpeed->setValue(25);
	m_pPlaybackSpeed->setStatusTip( tr("Playback frame rate (snapshots per second)") );
	connect( m_pPlaybackSlider, SIGNAL(valueChanged(int)), this, SLOT(on_change_snapshot(int)));
	connect( m_pPlaybackSnap, SIGNAL(valueChanged(int)), this, SLOT(on_change_snapshot(int)));
	connect( m_pPlaybackStart, SIGNAL(clicked()), this, SLOT(on_start_playback()));
	connect( m_pPlaybackStop, SIGNAL(clicked()), this, SLOT(on_stop_playback()));

	// Create selection controls
	m_pSelMode = new QComboBox( this );
	m_pSelMode->setGeometry(QRect(0, 0, 40, 20));
	for ( int i = 0; i < numSelModes; i++ )
		m_pSelMode->addItem( szPDBRenderModeNames[i] );
	m_pSelMode->setCurrentIndex( m_currentSelMode );
	m_pSelMode->setStatusTip( tr("Rendition mode of the selection") );
	m_pSelStream = new QComboBox( this );
	m_pSelStream->setGeometry(QRect(0, 0, 60, 20));
	m_pSelStream->addItem( m_whiteIcon, "(all)" );
	m_pSelStream->setMinimumWidth( 140 );
	m_pSelStream->setCurrentIndex( 0 );
	m_pSelStream->setStatusTip( tr("Stream to perform selection within") );
	m_pSelString = new QLineEdit( this );
	m_pSelString->setGeometry(QRect(0, 0, 100, 20));
#if QT_VERSION >= 0x040700
	m_pSelString->setPlaceholderText(tr("> Enter selection term..."));
#endif
	m_pSelString->setStatusTip( tr("Selection command") );
	m_pSelButton = new QPushButton( this );
	m_pSelButton->setIcon( QIcon(":/png/16x16/csel.png") );
	m_pSelButton->setGeometry(QRect(0, 0, 22, 22));
	m_pSelButton->setFixedSize( 22, 22 );
	m_pSelButton->setStatusTip( tr("Execute a selection command") );
	connect(m_pSelString, SIGNAL(editingFinished()), this, SLOT(on_selection_editingFinished()));
	connect(m_pSelButton, SIGNAL(clicked()), this, SLOT(on_selection_editingFinished()));

	// Position controls in layouts
	QVBoxLayout *pvLayout = new QVBoxLayout( this );
	QHBoxLayout *phLayout1 = new QHBoxLayout( this );
	QHBoxLayout *phLayout2 = new QHBoxLayout( this );
	pvLayout->setContentsMargins(1,1,4,1);
	phLayout1->setContentsMargins(0,0,1,0);
	phLayout2->setContentsMargins(0,0,1,0);
	pvLayout->addWidget( m_pCanvas );
	phLayout1->setSpacing( 2 );
	phLayout1->addWidget( m_pPlaybackStart );
	phLayout1->addWidget( m_pPlaybackStop );
	phLayout1->addWidget( m_pPlaybackSlider );
	phLayout1->addWidget( m_pPlaybackSnap );
	phLayout1->addWidget( m_pPlaybackSpeed );
	phLayout2->addWidget( m_pSelMode );
	phLayout2->addWidget( m_pSelStream );
	phLayout2->addWidget( m_pSelString );
	phLayout2->addWidget( m_pSelButton );
	pvLayout->addLayout( phLayout1 );
	pvLayout->addLayout( phLayout2 );
	setLayout( pvLayout );
}

void MDTRA_PDB_Renderer :: enableControls( bool enable )
{
	m_pCanvas->setEnabled( enable );
	m_pPlaybackStart->setEnabled( enable );
	m_pPlaybackStop->setEnabled( enable );
	m_pPlaybackSlider->setEnabled( enable );
	m_pPlaybackSnap->setEnabled( enable );
	m_pPlaybackSpeed->setEnabled( enable );

	m_pSelMode->setEnabled( enable );
	m_pSelStream->setEnabled( enable );
	m_pSelString->setEnabled( enable );
	m_pSelButton->setEnabled( enable );
}

void MDTRA_PDB_Renderer :: beginDataDesc( void )
{
	enableControls( false );
	m_pCanvas->clear();
	m_iMaxSnaphots = 0;
	m_pSelString->clear();
	m_cachedSelectionText.clear();
	m_pSelStream->clear();
	m_pSelStream->addItem( m_whiteIcon, "(all)" );
	m_streams.clear();
	m_iStreamCounter = 0;
	on_stop_playback();
}

void MDTRA_PDB_Renderer :: endDataDesc( void )
{
	assert( m_iCurrentSnapshot < m_iMaxSnaphots );
	m_pSelStream->setCurrentIndex( 0 );
	m_pPlaybackSlider->setMaximum( m_iMaxSnaphots-1 );
	m_pPlaybackSnap->setMaximum( m_iMaxSnaphots-1 );
	setDataDescSnapshot( 0 );
	enableControls( true );
}

void MDTRA_PDB_Renderer :: addDataDescStream( int index )
{
	// don't add existing stream
	int i = m_streams.indexOf( index );
	if ( i != -1 ) {
		++m_iStreamCounter;
		return;
	}
	// register stream
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( index );
	assert( pStream != NULL );
	m_streams.push_back( index );
	if ( pStream->files.size() > m_iMaxSnaphots )
		m_iMaxSnaphots = pStream->files.size();

	// add to canvas
	m_pCanvas->addStream( pStream );

	// get stream color
	const QColor &streamColor = m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (m_iStreamCounter % NUM_DATA_COLORS) );
	unsigned int rgbColor = streamColor.rgb();
	QImage img( 16, 16, QImage::Format_RGB888 );
	img.fill( 0xffffffff );
	for ( int i = 0; i < 10; i++ ) {
		for ( int j = 0; j < 10; j++ ) {
			img.setPixel( 3 + i, 3 + j, (i && j && i!=9 && j!=9) ? rgbColor : 0 );
		}
	}
	m_pSelStream->addItem( QIcon(QPixmap::fromImage( img )), pStream->name );

	++m_iStreamCounter;
}

void MDTRA_PDB_Renderer :: setDataDescSnapshot( int snapshot )
{
	on_stop_playback();
	m_iDefaultSnapshot = snapshot;
	m_iCurrentSnapshot = snapshot;
	m_bUpdatingSnapshot = true;
	m_pPlaybackSnap->setValue( m_iCurrentSnapshot );
	m_pPlaybackSlider->setValue( m_iCurrentSnapshot );
	m_bUpdatingSnapshot = false;
	m_pCanvas->setSnapshotIndex( m_iCurrentSnapshot );
}

void MDTRA_PDB_Renderer :: on_change_snapshot( int snapshot )
{
	if ( m_bUpdatingSnapshot )
		return;

	m_bUpdatingSnapshot = true;

	if ( m_pPlaybackSnap->value() != snapshot )
		m_pPlaybackSnap->setValue( snapshot );
	if ( m_pPlaybackSlider->value() != snapshot )
		m_pPlaybackSlider->setValue( snapshot );

	if ( m_pCanvas && m_pCanvas->isEnabled() )
		m_pCanvas->setDesiredSnapshotIndex( snapshot );

	m_bUpdatingSnapshot = false;
}

void MDTRA_PDB_Renderer :: on_start_playback( void )
{
	if ( m_bPlayback ) {
		// pause
		m_pPlaybackStart->setIcon( QIcon(":/png/16x16/play.png") );
		m_bPlayback = false;
	} else {
		// play
		m_pPlaybackStart->setIcon( QIcon(":/png/16x16/pause.png") );
		m_bPlayback = true;
		m_pSelMode->setEnabled( false );
		m_pSelStream->setEnabled( false );
		m_pSelString->setEnabled( false );
		m_pSelButton->setEnabled( false );
	}
}

void MDTRA_PDB_Renderer :: on_stop_playback( void )
{
	m_pSelMode->setEnabled( true );
	m_pSelStream->setEnabled( true );
	m_pSelString->setEnabled( true );
	m_pSelButton->setEnabled( true );

	if ( m_bPlayback ) {
		// stop
		m_iCurrentSnapshot = m_iDefaultSnapshot;
		m_pPlaybackStart->setIcon( QIcon(":/png/16x16/play.png") );
		m_bPlayback = false;
	}
}

void MDTRA_PDB_Renderer :: on_selection_editingFinished( void )
{
	if ((m_cachedSelectionText == m_pSelString->text()) && (m_cachedStreamIndex == m_pSelStream->currentIndex()))
		return;

	m_cachedSelectionText = m_pSelString->text();
	m_cachedStreamIndex = m_pSelStream->currentIndex();

	if ( m_pCanvas && m_cachedSelectionText.length() )
		m_pCanvas->setSelection( m_cachedStreamIndex - 1, m_cachedSelectionText );
}