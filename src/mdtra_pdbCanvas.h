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
#ifndef MDTRA_PDB_CANVAS_H
#define MDTRA_PDB_CANVAS_H

#include <QtCore/QTime>
#include <QtOpenGL/QGLWidget>
#include "mdtra_rendition.h"

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class QTimer;

class MDTRA_MainWindow;
class MDTRA_PDB_File;
class MDTRA_Render_PDB_File;
template<typename T> class MDTRA_SelectionSet;
template<typename T> class MDTRA_SelectionParser;

#define PDB_RENDITION_CACHE_SIZE	16
#define PDB_ZNEAR_VALUE				0.75f
#define PDB_ZFAR_VALUE				200.0f
#define PDB_FOV_VALUE				80.0f
#define PDB_WORLD_SIZE				1024.0f

typedef struct stMDTRA_PDB_Rendition {
	const struct stMDTRA_Stream	*stream;
	MDTRA_Render_PDB_File *reference;
	MDTRA_Render_PDB_File *cache[PDB_RENDITION_CACHE_SIZE];
	int	cacheSpot;
} MDTRA_PDB_Rendition;

class MDTRA_PDB_Canvas : public QGLWidget
{
	Q_OBJECT

	friend void pdbCanvas_print_f( const QString &msg );
	friend void pdbCanvas_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet );

public:
    MDTRA_PDB_Canvas(QGLFormat &format, QWidget *parent = 0);
	~MDTRA_PDB_Canvas();

	void reset( void );
	void clear( void );
	void startUpdateTimer( void );
	void stopUpdateTimer( void );
	void addStream( const struct stMDTRA_Stream *pStream );
	void setPlaybackSpeed( float value );
	void setSnapshotIndex( int value );
	void setDesiredSnapshotIndex( int value );
	void setRenderMode( MDTRA_PDBRenderMode_e value );
	void setSelection( int stream, const QString& selection );

private slots:
	void updateActions( void );
	void desiredActions( void );

protected:
	virtual void initializeGL( void );
	virtual void resizeGL( int width, int height );
	virtual void paintGL( void );
	virtual void mousePressEvent( QMouseEvent* pe );
	virtual void mouseReleaseEvent( QMouseEvent* pe );
	virtual void mouseMoveEvent( QMouseEvent* pe );
	virtual void wheelEvent( QWheelEvent* pe );
	virtual void keyPressEvent( QKeyEvent * pe );
	virtual void keyReleaseEvent( QKeyEvent * pe );

private:
	void loadCacheFile( int cacheSpot, int snapshotIndex );
	void paint3D( void );
	void paint2D( void );
	void paintAxes( void );
	void paintStructure( MDTRA_Render_PDB_File *pCurrent, MDTRA_Render_PDB_File *pPrev );
	void chaseCamera( void );
	void boundCamera( void );
	void updateCamera( void );
	void updateCursor( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	MDTRA_SelectionParser<MDTRA_PDB_File>* m_pSelectionParser;
	bool	m_bSelectionError;
	int		m_iSelectionStream;
	int		m_iSelectionFlags;
	int		m_iSelectionSize;
	int*	m_pSelectionData;
	QString m_sSelectionErrors;
	int		m_iWidth;
	int		m_iHeight;
	QFont	m_fntTitle;
	QFont	m_fntNormal;
	QTimer	*m_pUpdateTimer;
	QTimer	*m_pDesireTimer;
	QTime	m_lastTime;

	QVector<MDTRA_PDB_Rendition> m_rendition;
	int		m_renditionSize;
	int		m_snapshotIndex;
	int		m_snapshotIndexDesired;
	float	m_playbackSpeed;

	bool	m_bOrtho;
	float	m_fov;
	float	m_frustumRight;
	float	m_frustumBottom;
	bool	m_cameraModified;
	float	m_cameraOrigin[3];
	float	m_cameraAngles[3];
	float	m_cameraForward[3];
	float	m_cameraRight[3];
	float	m_cameraUp[3];
	float	m_cameraDistance;
	float	m_projMatrix[4][4];
	float	m_viewMatrix[4][4];
	float	m_fps;
	int		m_fpsFrames;

	bool	m_bChaseMode;
	bool	m_bLeftDrag;
	bool	m_bRightDrag;
	QPoint	m_centerPos;
};

#endif //MDTRA_PDB_CANVAS_H