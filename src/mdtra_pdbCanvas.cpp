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
//	Implementation of MDTRA_PDB_Canvas

/*
TODO LIST:
----------
Make special PDB class MDTRA_Rendition_PDB_File, something like MDTRA_Compact_PDB_File, but focused on
rendition (must support origin, vdw radius, color, rendermode, connectivity, selection term, centroiding, kabsch alignment)
It must implement function of vertex buffer generation (multiple vertex buffers may be generated, if different parts of the
molecule have different rendermodes).
Connectivity is built only for atoms with LINE or CPK or LICORICE rendermodes (i.e. when it is really needed).
Two vertex buffers must exist: origin (X Y Z R) and color (R G B S) (F means that atom is selected, and color is modulated
by selection color passed in uniform; F should actually be either 0 or 1, and we get rid of branching)
Normal rendition will use both of current frame.
Interpolated rendition will use both, and origin buffer from the next frame (interpolant will be sent as uniform)
MDTRA_Rendition_PDB_File should minimize memory allocations! no freing on reset(), no realloc on load() if not needed!
It must keep "F" (selection) flag on loading new PDB file (or re-assign it).

The only rendermodes for now is POINTS, LINES and NONE (for hidden parts of molecule). Obviously they require no shaders.
But still pass radii to the renderer, there is no overhead.

Make canvas update constantly (use timer), count FPS and print it in a 2d overlay.

Make animation functions. Upon start of playback, half of PDB_RENDITION_CACHE_SIZE frames must be loaded. Then spawn a thread 
that will load more and more frames instead of "passed" frames.
Move cachespot in update function, and then call updateGL() (not in PDB streaming thread!)
Don't forget to be at least ONE frame ahead because of interpolation (if any).
Pause function just pauses update function (and streaming pauses automatically when fulfils the cache).
Stop function stops both update and streaming functions and flushes the cache (it will be no more valid).

Implement selection on any rendition (or, particularly, ALL renditions).
Selected atoms receive "F" flag (and render it immediately to the color vertex buffer).
All subsequent calls to setRenderMode operate on selected atoms.

Neither rendermode nor selection can be changed when playing back animation. Just to make things simple.

Make tweakable:
- cache size
- fov
- near and far
- move speed
- mouse sensitivity
- interpolation
- chase mode
- ortho mode
- vsync
*/

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_math.h"
#include "mdtra_pdbCanvas.h"
#include "mdtra_colors.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_render_pdb.h"
#include "mdtra_utils.h"
#include "mdtra_pdb.h"
#include "mdtra_select.h"

#include "glext.h"

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QMessageBox>

MDTRA_PDB_Canvas :: MDTRA_PDB_Canvas( QGLFormat &format, QWidget *parent )
				  : QGLWidget( format, parent, NULL )
{
	setAutoFillBackground( false );
	setMouseTracking( true );
	setEnabled( false );

	m_rendition.reserve( 8 );
	m_renditionSize = 0;
	m_snapshotIndexDesired = -1;

	m_iSelectionStream = -1;
	m_iSelectionFlags = 0;
	m_iSelectionSize = 0;
	m_pSelectionData = NULL;
	m_bSelectionError = false;
	m_bChaseMode = true;
	//m_bChaseMode = false;
	m_bOrtho = true;
	m_frustumRight = 0.0f;
	m_frustumBottom = 0.0f;
	reset();

	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	m_pSelectionParser = new MDTRA_SelectionParser<MDTRA_PDB_File>;

	m_pUpdateTimer = new QTimer( this );
	m_pUpdateTimer->setInterval( 0 );
	connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(updateActions()));

	m_pDesireTimer = new QTimer( this );
	m_pDesireTimer->setSingleShot( true );
	connect(m_pDesireTimer, SIGNAL(timeout()), this, SLOT(desiredActions()));
}

MDTRA_PDB_Canvas :: ~MDTRA_PDB_Canvas()
{
	if (m_pSelectionParser) {
		delete m_pSelectionParser;
		m_pSelectionParser = NULL;
	}
	if (m_pSelectionData) {
		delete [] m_pSelectionData;
		m_pSelectionData = NULL;
	}
	for ( int i = 0; i < m_rendition.size(); i++ ) {
		MDTRA_PDB_Rendition *pRendition = const_cast<MDTRA_PDB_Rendition*>(&m_rendition.at(i));
		delete pRendition->reference;
		for ( int i = 0; i < PDB_RENDITION_CACHE_SIZE; i++ )
			delete pRendition->cache[i];
	}
}

void MDTRA_PDB_Canvas :: reset( void )
{
	m_cameraAngles[0] = 5.0f;
	m_cameraAngles[1] = -5.0f;
	m_cameraAngles[2] = 0.0f;
	m_cameraOrigin[0] = 0.0f;
	m_cameraOrigin[1] = 0.0f;
	m_cameraOrigin[2] = 60.0f;
	if ( m_bChaseMode ) chaseCamera();
	m_fov = PDB_FOV_VALUE;
	m_cameraModified = true;
	m_bLeftDrag = false;
	m_bRightDrag = false;
	m_lastTime = QTime::currentTime();
	m_fpsFrames = 0;
	m_fps = 0;
}

void MDTRA_PDB_Canvas :: startUpdateTimer( void )
{
	m_pUpdateTimer->start();
}

void MDTRA_PDB_Canvas :: stopUpdateTimer( void )
{
	m_pUpdateTimer->stop();
}

static MDTRA_PDB_Canvas *pFuncSrcObject = NULL;

void pdbCanvas_print_f( const QString &msg )
{
	if (!pFuncSrcObject) return;
	pFuncSrcObject->m_sSelectionErrors.append( msg );
	pFuncSrcObject->m_sSelectionErrors.append( "<br>" );
}

void pdbCanvas_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet )
{
	if (!pFuncSrcObject) return;
	pFuncSrcObject->m_iSelectionSize = 0;
	for (int i = 0; i < pSet->realsize(); i++)
		if (pSet->value(i)) pFuncSrcObject->m_iSelectionSize++;

	if (!pFuncSrcObject->m_iSelectionSize)
		return;

	pFuncSrcObject->m_pSelectionData = new int[pFuncSrcObject->m_iSelectionSize];
	for (int i = 0, j = 0; i < pSet->realsize(); i++)
		if (pSet->value(i)) pFuncSrcObject->m_pSelectionData[j++] = i;
}

void MDTRA_PDB_Canvas :: updateCursor( void )
{
	if (m_bLeftDrag) {
		setCursor( Qt::SizeAllCursor );
	} else if (m_bRightDrag) {
		setCursor( m_bChaseMode ? Qt::SizeVerCursor : Qt::SizeAllCursor );
	} else {
		setCursor( Qt::ArrowCursor );
	}
}

void MDTRA_PDB_Canvas :: mousePressEvent( QMouseEvent* pe )
{
	setFocus();
} 

void MDTRA_PDB_Canvas :: mouseReleaseEvent( QMouseEvent* pe )
{
	// Stop dragging
	if (m_bLeftDrag && (pe->button() == Qt::LeftButton)) {
		m_bLeftDrag = false;
		updateCursor();
		pe->accept();
	} 
	if (m_bRightDrag && (pe->button() == Qt::RightButton)) {
		m_bRightDrag = false;
		updateCursor();
		pe->accept();
	}
}

void MDTRA_PDB_Canvas :: mouseMoveEvent( QMouseEvent* pe )
{
	//!TODO: make tweakable
	const int mouse_sensitivity = 2;
	const int mouse_speed = 10;

	// Set proper drag action
	if (!m_bLeftDrag && (pe->buttons() & Qt::LeftButton)) {
		QCursor::setPos( mapToGlobal( m_centerPos ) );
		m_bLeftDrag = true;
		updateCursor();
		return;
	}
	if (!m_bRightDrag && (pe->buttons() & Qt::RightButton)) {
		QCursor::setPos( mapToGlobal( m_centerPos ) );
		m_bRightDrag = true;
		updateCursor();
		return;
	}

	// Do dragging
	if ( m_bLeftDrag && m_bRightDrag ) {
		if ( m_bChaseMode ) {
			m_cameraAngles[1] -= (pe->x() - m_centerPos.x()) * 0.4f * mouse_sensitivity;
			m_cameraOrigin[0] += m_cameraForward[0] * (-pe->y() + m_centerPos.y()) * 0.1f * mouse_speed;
			m_cameraOrigin[1] += m_cameraForward[1] * (-pe->y() + m_centerPos.y()) * 0.1f * mouse_speed;
			m_cameraOrigin[2] += m_cameraForward[2] * (-pe->y() + m_centerPos.y()) * 0.1f * mouse_speed;
			chaseCamera();
		} else {
			m_cameraOrigin[0] += m_cameraRight[0] * (pe->x() - m_centerPos.x()) * 0.1f * mouse_speed;
			m_cameraOrigin[1] += m_cameraRight[1] * (pe->x() - m_centerPos.x()) * 0.1f * mouse_speed;
			m_cameraOrigin[2] += m_cameraRight[2] * (pe->x() - m_centerPos.x()) * 0.1f * mouse_speed;
			m_cameraOrigin[0] += m_cameraForward[0] * (-pe->y() + m_centerPos.y()) * 0.1f * mouse_speed;
			m_cameraOrigin[1] += m_cameraForward[1] * (-pe->y() + m_centerPos.y()) * 0.1f * mouse_speed;
			m_cameraOrigin[2] += m_cameraForward[2] * (-pe->y() + m_centerPos.y()) * 0.1f * mouse_speed;
		}
	} else if ( m_bLeftDrag ) {
		if ( m_bChaseMode ) {
			m_cameraAngles[1] -= (pe->x() - m_centerPos.x()) * 0.4f * mouse_sensitivity;
			m_cameraAngles[0] -= (pe->y() - m_centerPos.y()) * 0.4f * mouse_sensitivity;
			chaseCamera();
		} else {
			m_cameraAngles[1] -= (pe->x() - m_centerPos.x()) * 0.4f * mouse_sensitivity;
			m_cameraAngles[0] -= (pe->y() - m_centerPos.y()) * 0.4f * mouse_sensitivity;
		}
	} else if ( m_bRightDrag ) {
		if ( m_bChaseMode ) {
			m_cameraOrigin[0] += m_cameraForward[0] * (-pe->y() + m_centerPos.y()) * 0.1f * mouse_speed;
			m_cameraOrigin[1] += m_cameraForward[1] * (-pe->y() + m_centerPos.y()) * 0.1f * mouse_speed;
			m_cameraOrigin[2] += m_cameraForward[2] * (-pe->y() + m_centerPos.y()) * 0.1f * mouse_speed;
			chaseCamera();
		} else {
			m_cameraOrigin[0] += m_cameraRight[0] * (pe->x() - m_centerPos.x()) * 0.1f * mouse_speed;
			m_cameraOrigin[1] += m_cameraRight[1] * (pe->x() - m_centerPos.x()) * 0.1f * mouse_speed;
			m_cameraOrigin[2] += m_cameraRight[2] * (pe->x() - m_centerPos.x()) * 0.1f * mouse_speed;
			m_cameraOrigin[0] += m_cameraUp[0] * (-pe->y() + m_centerPos.y()) * 0.1f * mouse_speed;
			m_cameraOrigin[1] += m_cameraUp[1] * (-pe->y() + m_centerPos.y()) * 0.1f * mouse_speed;
			m_cameraOrigin[2] += m_cameraUp[2] * (-pe->y() + m_centerPos.y()) * 0.1f * mouse_speed;
		}
	}
	if ( m_bLeftDrag || m_bRightDrag ) {
		m_cameraModified = true;
		QCursor::setPos( mapToGlobal( m_centerPos ) );
		pe->accept();
	}
}

void MDTRA_PDB_Canvas :: wheelEvent( QWheelEvent* pe )
{
	//!TODO: make tweakable
	const int mouse_speed = 10;

	m_cameraOrigin[0] += m_cameraForward[0] * ((pe->delta() > 0) ? 1.0f : -1.0f) * mouse_speed;
	m_cameraOrigin[1] += m_cameraForward[1] * ((pe->delta() > 0) ? 1.0f : -1.0f) * mouse_speed;
	m_cameraOrigin[2] += m_cameraForward[2] * ((pe->delta() > 0) ? 1.0f : -1.0f) * mouse_speed;
	m_cameraModified = true;
	pe->accept();
}

void MDTRA_PDB_Canvas :: keyPressEvent( QKeyEvent * pe )
{
	switch ( pe->key() ) {
	case Qt::Key_A:
		m_bOrtho = true;
		m_cameraModified = true;
		resizeGL( m_iWidth, m_iHeight );
		break;
	case Qt::Key_S:
		m_bOrtho = false;
		m_cameraModified = true;
		resizeGL( m_iWidth, m_iHeight );
		break;
	}
}

void MDTRA_PDB_Canvas :: keyReleaseEvent( QKeyEvent * pe )
{
	//!TODO
}

void MDTRA_PDB_Canvas :: clear( void )
{
	m_renditionSize = 0;
	for ( int i = 0; i < m_rendition.size(); i++ ) {
		MDTRA_PDB_Rendition *pRendition = const_cast<MDTRA_PDB_Rendition*>(&m_rendition.at(i));
		pRendition->stream = NULL;
		pRendition->cacheSpot = 0;
	}
}

void MDTRA_PDB_Canvas :: addStream( const MDTRA_Stream *pStream )
{
	assert( pStream != NULL );

	if ( m_renditionSize == m_rendition.size() ) {
		MDTRA_PDB_Rendition newRendition;
		for ( int i = 0; i < PDB_RENDITION_CACHE_SIZE; i++ )
			newRendition.cache[i] = new MDTRA_Render_PDB_File;
		newRendition.reference = new MDTRA_Render_PDB_File;
		newRendition.cacheSpot = 0;
		newRendition.stream = pStream;
		m_rendition << newRendition;
	} else {
		MDTRA_PDB_Rendition *pRendition = const_cast<MDTRA_PDB_Rendition*>(&m_rendition.at(m_renditionSize));
		pRendition->reference->reset();
		pRendition->cacheSpot = 0;
		pRendition->stream = pStream;
	}
	++m_renditionSize;
}

void MDTRA_PDB_Canvas :: setPlaybackSpeed( float value )
{
	m_playbackSpeed = value;
}

void MDTRA_PDB_Canvas :: setSnapshotIndex( int value )
{
	m_snapshotIndex = value;

	// prepare the snapshot
	MDTRA_Render_PDB_File *pFirstPDB = NULL;
	for ( int i = 0; i < m_renditionSize; i++ ) {
		MDTRA_PDB_Rendition *pRendition = const_cast<MDTRA_PDB_Rendition*>(&m_rendition.at(i));
		// reset cache spot
		pRendition->cacheSpot = 0;
		// check reference
		if ( pRendition->reference->getAtomCount() <= 0 ) {
			pRendition->reference->load( 0, pRendition->stream->format_identifier, pRendition->stream->files.at(0).toAscii(), pRendition->stream->flags );
			if ( pRendition->reference->getAtomCount() <= 0 )
				continue;
			pRendition->reference->move_to_centroid();
			if ( pFirstPDB )
				pRendition->reference->align_kabsch( pFirstPDB );
			else
				pFirstPDB = pRendition->reference;
		}
	}

	loadCacheFile( 0, m_snapshotIndex );
}

void MDTRA_PDB_Canvas :: setDesiredSnapshotIndex( int value )
{
	m_snapshotIndexDesired = value;
	m_pDesireTimer->start( 100 );
}

void MDTRA_PDB_Canvas :: setRenderMode( MDTRA_PDBRenderMode_e value )
{
	//!TODO: apply rendermode to selected atoms (if any)
}

void MDTRA_PDB_Canvas :: loadCacheFile( int cacheSpot, int snapshotIndex )
{
	assert( cacheSpot >= 0 && cacheSpot < PDB_RENDITION_CACHE_SIZE );

	// cache the snapshot
	MDTRA_Render_PDB_File *pFirstPDB = NULL;
	for ( int i = 0; i < m_renditionSize; i++ ) {
		MDTRA_PDB_Rendition *pRendition = const_cast<MDTRA_PDB_Rendition*>(&m_rendition.at(i));
		assert( pRendition->cache[cacheSpot] != NULL );

		if ( pRendition->reference->getAtomCount() <= 0 ) {
			// pdb not valid
			pRendition->cache[cacheSpot]->reset();
			continue;
		}
		if ( pRendition->stream->files.count() <= snapshotIndex ) {
			// snapshot out of range
			pRendition->cache[cacheSpot]->reset();
			continue;
		}

		// load file
		pRendition->cache[cacheSpot]->load( 0, pRendition->stream->format_identifier, pRendition->stream->files.at(snapshotIndex).toAscii(), pRendition->stream->flags );
		if ( pRendition->cache[cacheSpot]->getAtomCount() <= 0 )
			continue;

		// align PDB
		pRendition->cache[cacheSpot]->move_to_centroid();
		if ( snapshotIndex > 0 )
			pRendition->cache[cacheSpot]->align_kabsch( pRendition->reference );

		// apply selection
		if ( m_iSelectionSize ) {
			if ( m_iSelectionStream >= 0 && m_iSelectionStream != i )
				continue;
			pRendition->cache[cacheSpot]->set_selection_flag( m_iSelectionSize, m_pSelectionData );
		}
	}
}

void MDTRA_PDB_Canvas :: setSelection( int stream, const QString& selection )
{
	m_sSelectionErrors.clear();
	QApplication::processEvents();
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	m_iSelectionStream = stream;
	pFuncSrcObject = this;
	for ( int i = 0; i < m_renditionSize; i++ ) {
		MDTRA_PDB_Rendition *pRendition = const_cast<MDTRA_PDB_Rendition*>(&m_rendition.at(i));
		// don't mess with unloaded PDB files
		if ( pRendition->cache[pRendition->cacheSpot]->getAtomCount() <= 0 )
			continue;
		if ( m_iSelectionStream >= 0 && m_iSelectionStream != i )
			continue;

		if (!m_pSelectionParser->parse(pRendition->stream->pdb, selection.toAscii(), pdbCanvas_select_f)) {
			m_pSelectionParser->printErrors( pdbCanvas_print_f );
			m_bSelectionError = true;
			break;
		} else {
			pRendition->cache[pRendition->cacheSpot]->set_selection_flag( m_iSelectionSize, m_pSelectionData );
		}
	}
	pFuncSrcObject = NULL;
	QApplication::restoreOverrideCursor();

	if ( m_bSelectionError && m_sSelectionErrors.length() )
		QMessageBox::warning(m_pMainWindow, tr("Selection error"), m_sSelectionErrors);
}

void MDTRA_PDB_Canvas :: updateActions( void )
{
	// update canvas
	update();
}

void MDTRA_PDB_Canvas :: desiredActions( void )
{
	// apply desired actions and disable desired timer
	if ( m_snapshotIndexDesired >= 0 ) {
		setSnapshotIndex( m_snapshotIndexDesired );
		m_snapshotIndexDesired = -1;
	}
	m_pDesireTimer->stop();
}

void MDTRA_PDB_Canvas :: chaseCamera( void )
{
	float flCameraDist = sqrtf( m_cameraOrigin[0]*m_cameraOrigin[0] + m_cameraOrigin[1]*m_cameraOrigin[1] + m_cameraOrigin[2]*m_cameraOrigin[2] );
	if ( flCameraDist < 1.0f )
		flCameraDist = 1.0f;

	// bound angles
	for (int i = 0; i < 3; i++)	{
		if (m_cameraAngles[i] < -180)
			m_cameraAngles[i] += 360;
		if (m_cameraAngles[i] > 180)
			m_cameraAngles[i] -= 360;
	}
	// bound pitch
	if (m_cameraAngles[0] < -90)
		m_cameraAngles[0] = -90;
	if (m_cameraAngles[0] > 90)
		m_cameraAngles[0] = 90;

	float angle_x = UTIL_deg2rad(-m_cameraAngles[0]);
	float angle_y = UTIL_deg2rad(-m_cameraAngles[1]);

	float vecCameraDir[3];
	vecCameraDir[0] = -sin(angle_y)*cos(angle_x);
	vecCameraDir[1] = sin(angle_x);
	vecCameraDir[2] = cos(angle_y)*cos(angle_x);

	m_cameraOrigin[0] = vecCameraDir[0] * flCameraDist;
	m_cameraOrigin[1] = vecCameraDir[1] * flCameraDist;
	m_cameraOrigin[2] = vecCameraDir[2] * flCameraDist;
}

void MDTRA_PDB_Canvas :: boundCamera( void )
{
	// bound origin
	for (int i = 0; i < 3; i++) {
		if (m_cameraOrigin[i] < -PDB_WORLD_SIZE)
			m_cameraOrigin[i] = -PDB_WORLD_SIZE;
		if (m_cameraOrigin[i] > PDB_WORLD_SIZE)
			m_cameraOrigin[i] = PDB_WORLD_SIZE;
	}
	// bound angles
	for (int i = 0; i < 3; i++) {
		if (m_cameraAngles[i] < -180)
			m_cameraAngles[i] += 360;
		if (m_cameraAngles[i] > 180)
			m_cameraAngles[i] -= 360;
	}
	// bound pitch
	if (m_cameraAngles[0] < -90)
		m_cameraAngles[0] = -90;
	if (m_cameraAngles[0] > 90)
		m_cameraAngles[0] = 90;
}

void MDTRA_PDB_Canvas :: updateCamera( void )
{
	float angle_x = UTIL_deg2rad( -m_cameraAngles[0] );
	float angle_y = UTIL_deg2rad( -m_cameraAngles[1] );
	float angle_z = UTIL_deg2rad( -m_cameraAngles[2] );

	float *result = &m_viewMatrix[0][0];

	result[0] = (cos(angle_y)*cos(angle_z) - sin(angle_y)*sin(angle_x)*sin(angle_z));
	result[1] = (cos(angle_y)*sin(angle_z) + sin(angle_y)*sin(angle_x)*cos(angle_z));
	result[2] = -sin(angle_y)*cos(angle_x);
	result[3] = 0;
	result[4] = -cos(angle_x)*sin(angle_z);
	result[5] = cos(angle_x)*cos(angle_z);
	result[6] = sin(angle_x);
	result[7] = 0;
	result[8] = (sin(angle_y)*cos(angle_z) + cos(angle_y)*sin(angle_x)*sin(angle_z));
	result[9] = (sin(angle_y)*sin(angle_z) - cos(angle_y)*sin(angle_x)*cos(angle_z));
	result[10] = cos(angle_y)*cos(angle_x);
	result[11] = 0;
	result[12] = ((-m_cameraOrigin[0]*(cos(angle_y)*cos(angle_z) - sin(angle_y)*sin(angle_x)*sin(angle_z)) + m_cameraOrigin[1]*cos(angle_x)*sin(angle_z)) - m_cameraOrigin[2]*(sin(angle_y)*cos(angle_z) + cos(angle_y)*sin(angle_x)*sin(angle_z)));
	result[13] = ((-m_cameraOrigin[0]*(cos(angle_y)*sin(angle_z) + sin(angle_y)*sin(angle_x)*cos(angle_z)) - m_cameraOrigin[1]*cos(angle_x)*cos(angle_z)) - m_cameraOrigin[2]*(sin(angle_y)*sin(angle_z) - cos(angle_y)*sin(angle_x)*cos(angle_z)));
	result[14] = ((m_cameraOrigin[0]*sin(angle_y)*cos(angle_x) - m_cameraOrigin[1]*sin(angle_x)) - m_cameraOrigin[2]*cos(angle_y)*cos(angle_x));
	result[15] = 1;

	m_cameraRight[0] = m_viewMatrix[0][0];
	m_cameraRight[1] = m_viewMatrix[1][0];
	m_cameraRight[2] = m_viewMatrix[2][0];
	m_cameraUp[0] = m_viewMatrix[0][1];
	m_cameraUp[1] = m_viewMatrix[1][1];
	m_cameraUp[2] = m_viewMatrix[2][1];
	m_cameraForward[0] = -m_viewMatrix[0][2];
	m_cameraForward[1] = -m_viewMatrix[1][2];
	m_cameraForward[2] = -m_viewMatrix[2][2];

	if ( m_bOrtho ) {
		m_cameraDistance = sqrtf( m_cameraOrigin[0]*m_cameraOrigin[0] + m_cameraOrigin[1]*m_cameraOrigin[1] + m_cameraOrigin[2]*m_cameraOrigin[2] );
		float flInvCameraDist = 1.0f / m_cameraDistance;

		result[0] *= flInvCameraDist;
		result[1] *= flInvCameraDist;
		result[2] *= flInvCameraDist;
		result[4] *= flInvCameraDist;
		result[5] *= flInvCameraDist;
		result[6] *= flInvCameraDist;
		result[8] *= flInvCameraDist;
		result[9] *= flInvCameraDist;
		result[10] *= flInvCameraDist;
	}
}

void MDTRA_PDB_Canvas :: initializeGL( void ) 
{
	//Setup GL defaults
	glDrawBuffer( GL_BACK );
	glReadBuffer( GL_BACK );

	glShadeModel( GL_SMOOTH );
	glDisable( GL_POLYGON_SMOOTH );
    glDisable( GL_POLYGON_STIPPLE );
    glDisable( GL_DITHER );
	glDisable( GL_LOGIC_OP );
	glDisable( GL_LIGHTING );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LINE_STIPPLE );
	glDisable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );

	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CCW );
	glCullFace( GL_BACK );

	glPointSize( 2.0f );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	// Init fonts
	m_fntTitle = QFont( "Arial", 12, QFont::Bold, false );
	m_fntTitle.setPixelSize( 16 );
	m_fntNormal = QFont( "Arial", 8, QFont::Normal, false );
	m_fntNormal.setPixelSize( 10 );

	// First clear
	qglClearColor( Qt::gray );
	glClear( GL_COLOR_BUFFER_BIT );
}

void MDTRA_PDB_Canvas :: resizeGL( int width, int height ) 
{
	if (width <= 0)
		return;

	m_iWidth = width;
	m_iHeight = height;

	m_centerPos.setX( m_iWidth >> 1 );
	m_centerPos.setY( m_iHeight >> 1 );

	float flAspect = (float)height/ (float)width;
	m_frustumRight = tanf( UTIL_deg2rad( m_fov ) * 0.5f );
	m_frustumBottom = m_frustumRight * flAspect;

	if ( m_bOrtho ) {
		m_projMatrix[0][0] = 1.0f / m_frustumRight;
		m_projMatrix[0][1] = 0;
		m_projMatrix[0][2] = 0;
		m_projMatrix[0][3] = 0;
		m_projMatrix[1][0] = 0;
		m_projMatrix[1][1] = 1.0f / m_frustumBottom;
		m_projMatrix[1][2] = 0;
		m_projMatrix[1][3] = 0;
		m_projMatrix[2][0] = 0;
		m_projMatrix[2][1] = 0;
		m_projMatrix[2][2] = -2.0f / PDB_ZFAR_VALUE;
		m_projMatrix[2][3] = 0;
		m_projMatrix[3][0] = 0;
		m_projMatrix[3][1] = 0;
		m_projMatrix[3][2] = -1;
		m_projMatrix[3][3] = 1;
	} else {
		m_projMatrix[0][0] = PDB_ZNEAR_VALUE / m_frustumRight;
		m_projMatrix[0][1] = 0;
		m_projMatrix[0][2] = 0;
		m_projMatrix[0][3] = 0;
		m_projMatrix[1][0] = 0;
		m_projMatrix[1][1] = PDB_ZNEAR_VALUE / m_frustumBottom;
		m_projMatrix[1][2] = 0;
		m_projMatrix[1][3] = 0;
		m_projMatrix[2][0] = 0;
		m_projMatrix[2][1] = 0;
		m_projMatrix[2][2] = (PDB_ZNEAR_VALUE + PDB_ZFAR_VALUE) / (PDB_ZNEAR_VALUE - PDB_ZFAR_VALUE);
		m_projMatrix[2][3] = -1;
		m_projMatrix[3][0] = 0;
		m_projMatrix[3][1] = 0;
		m_projMatrix[3][2] = (2.0f * PDB_ZNEAR_VALUE * PDB_ZFAR_VALUE) / (PDB_ZNEAR_VALUE - PDB_ZFAR_VALUE);
		m_projMatrix[3][3] = 0;
	}
}

void MDTRA_PDB_Canvas :: paintGL( void ) 
{
	// check time and update fps
	QTime newTime = QTime::currentTime();
	int timeDiff = m_lastTime.msecsTo( newTime );
	if ( timeDiff >= 1000 ) {
		m_lastTime = newTime;
		m_fps = m_fpsFrames * 1000.0f / (float)timeDiff;
		m_fpsFrames = 0;
	}
	++m_fpsFrames;

	//Set viewport
	glViewport( 0, 0, m_iWidth, m_iHeight );

	// Clear
	qglClearColor( isEnabled() ? m_pMainWindow->getColorManager()->color( COLOR_3D_BACKGROUND ) : Qt::gray );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if ( isEnabled() )
		paint3D();
	
	paint2D();
}

void MDTRA_PDB_Canvas :: paintAxes( void )
{
	float vecAxisOrigin[3];
	float axisSize = 0.1f;
	float axisScale = 1.75f;

	if ( m_bOrtho ) {
		axisScale *= m_cameraDistance * 0.5f;
		axisSize *= m_cameraDistance * 0.5f;
	}

	vecAxisOrigin[0] = m_cameraOrigin[0] - m_cameraRight[0] * m_frustumRight * axisScale - m_cameraUp[0] * m_frustumBottom * axisScale + m_cameraForward[0] * PDB_ZNEAR_VALUE * 2.0f;
	vecAxisOrigin[1] = m_cameraOrigin[1] - m_cameraRight[1] * m_frustumRight * axisScale - m_cameraUp[1] * m_frustumBottom * axisScale + m_cameraForward[1] * PDB_ZNEAR_VALUE * 2.0f;
	vecAxisOrigin[2] = m_cameraOrigin[2] - m_cameraRight[2] * m_frustumRight * axisScale - m_cameraUp[2] * m_frustumBottom * axisScale + m_cameraForward[2] * PDB_ZNEAR_VALUE * 2.0f;

	glBegin( GL_LINES );
		qglColor( Qt::red );
		glVertex3fv( vecAxisOrigin );
		glVertex3f( vecAxisOrigin[0] + axisSize, vecAxisOrigin[1], vecAxisOrigin[2] );
		qglColor( Qt::green );
		glVertex3fv( vecAxisOrigin );
		glVertex3f( vecAxisOrigin[0], vecAxisOrigin[1] + axisSize, vecAxisOrigin[2] );
		qglColor( Qt::blue );
		glVertex3fv( vecAxisOrigin );
		glVertex3f( vecAxisOrigin[0], vecAxisOrigin[1], vecAxisOrigin[2] + axisSize );
	glEnd();
}

void MDTRA_PDB_Canvas :: paintStructure( MDTRA_Render_PDB_File *pCurrent, MDTRA_Render_PDB_File *pPrev ) 
{
	qglColor( Qt::white );

	//!TODO: this is a temporary rendition solution!
	glBegin( GL_POINTS );
	for ( int i = 0; i < pCurrent->getAtomCount(); i++ ) {
		const MDTRA_Render_PDB_Atom *pAtom = pCurrent->fetchAtomByIndex( i );
		assert( pAtom != NULL );

		if ( pAtom->atomFlags & PDB_FLAG_SELECTED )
			qglColor( m_pMainWindow->getColorManager()->color( COLOR_3D_SELECTION ) );
		else
			glColor4fv( pAtom->color );

		glVertex3fv( pAtom->modified_xyz );
	}
	glEnd();
}

void MDTRA_PDB_Canvas :: paint3D( void ) 
{
	// Rebuild camera matrix
	if ( m_cameraModified ) {
		boundCamera();
		updateCamera();
		m_cameraModified = false;
	}

	// Switch to 3D projection
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( &m_projMatrix[0][0] );
	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( &m_viewMatrix[0][0] );

	// Enable depth test
	glEnable( GL_DEPTH_TEST );

	// Draw all snapshots
	for ( int i = 0; i < m_renditionSize; i++ ) {
		MDTRA_PDB_Rendition *pRendition = const_cast<MDTRA_PDB_Rendition*>(&m_rendition.at(i));
		// don't mess with unloaded PDB files
		if ( pRendition->cache[pRendition->cacheSpot]->getAtomCount() <= 0 )
			continue;
		paintStructure( pRendition->cache[pRendition->cacheSpot], NULL );
	}

	// Disable depth test
	glDisable( GL_DEPTH_TEST );

	// Draw axes
	paintAxes();
}

void MDTRA_PDB_Canvas :: paint2D( void ) 
{
	// Init fonts
	QFontMetrics fmNormal( m_fntNormal );
	QFontMetrics fmTitle( m_fntTitle );

	//Switch to 2D projection
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, m_iWidth, m_iHeight, 0, -1.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	if ( !isEnabled() ) {
		// Draw disabled text
		QString szDisabledMsg = tr("No result selected");
		qglColor( Qt::white );
		renderText( (m_iWidth - fmTitle.width( szDisabledMsg )) * 0.5f, (m_iHeight - fmTitle.height()) * 0.5f, szDisabledMsg, m_fntTitle );
		return;
	}

	// Draw camera stats
	int ypos = 16;
	int ystep = fmNormal.height() + 4;

	qglColor( m_pMainWindow->getColorManager()->color( COLOR_3D_TEXT ) );
	renderText( 10, ypos, QString("Camera Origin: %1 %2 %3").arg(m_cameraOrigin[0],6,'f',2).arg(m_cameraOrigin[1],6,'f',2).arg(m_cameraOrigin[2],6,'f',2), m_fntNormal ); ypos += ystep;
	renderText( 10, ypos, QString("Camera Angles: %1 %2 %3").arg(m_cameraAngles[0],6,'f',2).arg(m_cameraAngles[1],6,'f',2).arg(m_cameraAngles[2],6,'f',2), m_fntNormal ); ypos += ystep;

	renderText( m_iWidth - 55, 10, QString("%1 fps").arg(m_fps,6,'f',1), m_fntNormal );
}