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
#ifndef MDTRA_PDB_RENDERER_H
#define MDTRA_PDB_RENDERER_H

#include <QtGui/QWidget>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include "mdtra_rendition.h"

class MDTRA_MainWindow;
class MDTRA_PDB_File;
class MDTRA_PDB_Canvas;

class MDTRA_PDB_Renderer: public QWidget
{
	Q_OBJECT

public:
    MDTRA_PDB_Renderer( QWidget *parent = 0 );
	~MDTRA_PDB_Renderer();
	virtual bool event( QEvent *ev );

	void beginDataDesc( void );
	void endDataDesc( void );
	void addDataDescStream( int index );
	void setDataDescSnapshot( int snapshot );
	void setActive( bool value );

private slots:
	void on_start_playback( void );
	void on_stop_playback( void );
	void on_change_snapshot( int snapshot );
	void on_selection_editingFinished( void );

private:
	void createControls( void );
	void enableControls( bool enable );

private:
	MDTRA_MainWindow		*m_pMainWindow;
	MDTRA_PDB_Canvas		*m_pCanvas;
	QPushButton				*m_pPlaybackStart;
	QPushButton				*m_pPlaybackStop;
	QSlider					*m_pPlaybackSlider;
	QSpinBox				*m_pPlaybackSnap;
	QDoubleSpinBox			*m_pPlaybackSpeed;
	QComboBox				*m_pSelMode;
	QComboBox				*m_pSelStream;
	QLineEdit				*m_pSelString;
    QPushButton				*m_pSelButton;
	QString					m_cachedSelectionText;
	int						m_cachedStreamIndex;
	QIcon					m_whiteIcon;
	MDTRA_PDBRenderMode_e	m_currentSelMode;
	QVector<int>			m_streams;
	int						m_iStreamCounter;
	int						m_iDefaultSnapshot;
	int						m_iCurrentSnapshot;
	int						m_iMaxSnaphots;
	bool					m_bPlayback;
	bool					m_bUpdatingSnapshot;
};

#endif //MDTRA_PDB_RENDERER_H