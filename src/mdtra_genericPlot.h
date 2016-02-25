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
#ifndef MDTRA_GENERIC_PLOT_H
#define MDTRA_GENERIC_PLOT_H

#include <QtOpenGL/QGLWidget>

class MDTRA_MainWindow;

class MDTRA_Generic_Plot : public QGLWidget
{
	Q_OBJECT

public:
    MDTRA_Generic_Plot(QGLFormat &format, QWidget *parent = 0);
	virtual ~MDTRA_Generic_Plot() {}
	bool saveScreenshot( const QString &fileName, const char *fileFormat );

protected:
	void renderSpecialString( int x, int y, const QString& str, const QFont& f, const QFontMetrics& fm, QPainter* pPainter = NULL );
	virtual void setupPlotPaintStruct( void ) = 0;
	virtual bool renderPlot_OpenGL( void ) = 0;
	virtual bool renderPlot_Generic( QPainter* pPainter ) = 0;
	virtual void initializeGL( void );
	virtual void resizeGL( int width, int height );
	virtual void paintGL( void );
	virtual void reset( void );

protected:
	MDTRA_MainWindow* m_pMainWindow;
	int	m_iWidth;
	int m_iHeight;
	bool m_bMultisampleContext;
};

#endif //MDTRA_GENERIC_PLOT_H
