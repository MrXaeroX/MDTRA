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
#ifndef MDTRA_2D_RMSD_PLOT_H
#define MDTRA_2D_RMSD_PLOT_H

#include "mdtra_genericPlot.h"

typedef struct stMDTRA_2D_RMSD_PlotPaintStruct
{
	QFont			fntTitle;			// device-dependent title font
	QFont			fntNormal;			// device-dependent normal font
	QFont			fntDigits;			// device-dependent digit font
	QRect			rcPlot;				// plot rect
	QRect			rcData;				// data rect
	QRect			rcLegend;			// legend rect
	QSize			siArrow;			// size of arrow
	QPoint			vXArrow[3];			// x-axis arrow vertices
	QPoint			vYArrow[3];			// y-axis arrow vertices
	QString			axisTitle;			// title of axes
	float			scaleFactor;
	float			scaleDimension;
	float			axisScale;
	float			legendScaleFactor;
	float			legendAxisScale;
} MDTRA_2D_RMSD_PlotPaintStruct;

class MDTRA_2D_RMSD_Plot : public MDTRA_Generic_Plot
{
	Q_OBJECT

public:
    MDTRA_2D_RMSD_Plot(QGLFormat &format, QWidget *parent = 0);
	virtual ~MDTRA_2D_RMSD_Plot();

	void loadTexture( int width, int height, unsigned char* pixels );
	void updateLegend( float maxValue, bool rgb );
	void setSmoothTexture( bool filter );
	void setShowLegend( bool b );
	void setTitle( const QString& title ) { m_szTitle = title; }
	void setDimension( int d, float f ) { m_iDimension = d; m_flXScale = f; }

protected:
	virtual void setupPlotPaintStruct( void );
	virtual bool renderPlot_OpenGL( void );
	virtual bool renderPlot_Generic( QPainter* pPainter );
	virtual void reset( void );

private:
	MDTRA_2D_RMSD_PlotPaintStruct m_PPS;
	int	m_iMapWidth;
	int m_iMapHeight;
	const unsigned char* m_pMapPixels;
	bool m_bSmoothMap;
	GLuint m_uiTexture;
	GLuint m_uiLegend;
	unsigned char* m_pLegendBuffer;
	bool m_bShowLegend;
	bool m_bLegendCachedRGB;
	float m_flLegendCachedMax;
	int m_iDimension;
	float m_flXScale;
	QString m_szTitle;
};

#endif //MDTRA_2D_RMSD_PLOT_H
