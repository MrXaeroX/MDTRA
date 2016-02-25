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
//	Implementation of MDTRA_2D_RMSD_Plot

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_2D_RMSD_Plot.h"
#include "mdtra_project.h"
#include "mdtra_colors.h"
#include "mdtra_utils.h"

#include "glext.h"

#include <QtGui/QApplication>
#include <QtGui/QPrinter>

#define LEGEND_TEXTURE_WIDTH	8
#define LEGEND_TEXTURE_HEIGHT	128

static float flXScaleUnit_Scale[MDTRA_XSU_MAX] = 
{
	0.0f,
	1.0f,
	0.001f,
};

static float flXScaleUnit_Bias[MDTRA_XSU_MAX] = 
{
	1.0f,
	0.0f,
	0.0f,
};

MDTRA_2D_RMSD_Plot :: MDTRA_2D_RMSD_Plot( QGLFormat &format, QWidget *parent )
					: MDTRA_Generic_Plot( format, parent )
{
	m_uiTexture = 0;
	m_uiLegend = 0;
	m_pMapPixels = NULL;
	m_bSmoothMap = true;

	m_pLegendBuffer = new byte[LEGEND_TEXTURE_WIDTH*LEGEND_TEXTURE_HEIGHT*4];
	assert( m_pLegendBuffer != NULL );
	m_bShowLegend = true;

	m_iDimension = 100;
}

MDTRA_2D_RMSD_Plot :: ~MDTRA_2D_RMSD_Plot()
{
	if (m_uiTexture) {
		glDeleteTextures( 1, &m_uiTexture );
		m_uiTexture = 0;
	}
	if (m_uiLegend) {
		glDeleteTextures( 1, &m_uiLegend );
		m_uiLegend = 0;
	}
	if (m_pLegendBuffer) {
		delete [] m_pLegendBuffer;
		m_pLegendBuffer = NULL;
	}
}

void MDTRA_2D_RMSD_Plot :: reset( void )
{
	m_bLegendCachedRGB = false;
	m_flLegendCachedMax = -1.0f;
}

void MDTRA_2D_RMSD_Plot :: loadTexture( int width, int height, unsigned char* pixels ) 
{
	makeCurrent();
	if ( !m_uiTexture ) {
		//init texture
		glGenTextures( 1, &m_uiTexture );
		glBindTexture( GL_TEXTURE_2D, m_uiTexture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	} else {
		glBindTexture( GL_TEXTURE_2D, m_uiTexture );
	}
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
	doneCurrent();

	m_iMapWidth = width;
	m_iMapHeight = height;
	m_pMapPixels = pixels;
}

void MDTRA_2D_RMSD_Plot :: updateLegend( float maxValue, bool rgb ) 
{
	if ( m_uiLegend && 
		(m_bLegendCachedRGB == rgb) &&
		(m_flLegendCachedMax == maxValue))
		return;

	m_bLegendCachedRGB = rgb;
	m_flLegendCachedMax = maxValue;

	//generate buffer
	float valueStep = 1.0f / (LEGEND_TEXTURE_HEIGHT-1);
	float dv = 0.0f;
	for (int i = 0; i < LEGEND_TEXTURE_HEIGHT; i++, dv += valueStep) {
		byte rgba[4];
		if (!rgb) {
			rgba[0] = dv * 255;
			rgba[1] = dv * 255;
			rgba[2] = dv * 255;
		} else {
			rgba[0] = UTIL_FloatToColor(0, dv);
			rgba[1] = UTIL_FloatToColor(1, dv);
			rgba[2] = UTIL_FloatToColor(2, dv);
		}
		rgba[3] = 255;

		for (int j = 0; j < LEGEND_TEXTURE_WIDTH; j++)
			memcpy( &m_pLegendBuffer[i*LEGEND_TEXTURE_WIDTH*4+j*4], rgba, 4 );
	}

	makeCurrent();
	if ( !m_uiLegend ) {
		//init texture
		glGenTextures( 1, &m_uiLegend );
		glBindTexture( GL_TEXTURE_2D, m_uiLegend );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	} else {
		glBindTexture( GL_TEXTURE_2D, m_uiLegend );
	}
	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, LEGEND_TEXTURE_WIDTH, LEGEND_TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pLegendBuffer );
	doneCurrent();
}

void MDTRA_2D_RMSD_Plot :: setSmoothTexture( bool filter ) 
{
	if ( m_uiTexture ) {
		makeCurrent();
		glBindTexture( GL_TEXTURE_2D, m_uiTexture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter ? GL_LINEAR : GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter ? GL_LINEAR : GL_NEAREST );
		doneCurrent();
	}
	m_bSmoothMap = filter;
}

void MDTRA_2D_RMSD_Plot :: setShowLegend( bool b )
{
	m_bShowLegend = b;
}

void MDTRA_2D_RMSD_Plot :: setupPlotPaintStruct( void ) 
{
	// Init fonts
	m_PPS.fntTitle = QFont( "Arial", 10, QFont::Bold, false );
	m_PPS.fntTitle.setPixelSize( 14 );
	m_PPS.fntNormal = QFont( "Arial", 8, QFont::Normal, false );
	m_PPS.fntNormal.setPixelSize( 12 );
	m_PPS.fntDigits = QFont( "Arial", 8, QFont::Normal, false );
	m_PPS.fntDigits.setPixelSize( 12 );
	m_PPS.siArrow.setWidth( 3 );
	m_PPS.siArrow.setHeight( 8 );

	QFontMetrics fmTitle( m_PPS.fntTitle );

	int langId = m_pMainWindow->PlotLanguage();

	// Set defaults
	int plotWide = MDTRA_MIN(m_iWidth, m_iHeight) - 70;
	int plotTall = MDTRA_MIN(m_iWidth, m_iHeight) - 70;
	int plotX = (m_iWidth - plotWide) >> 1;
	int plotY = ((m_iHeight - plotTall) >> 1) - 5;

	// define initial plot rect
	m_PPS.rcPlot = QRect( plotX, plotY, plotWide, plotTall );

	// Define legend rect
	m_PPS.rcLegend.setRect( m_iWidth - 90, plotY + 16, 24, plotTall - 16 );

	if ( m_bShowLegend )
		m_PPS.rcPlot.moveLeft( (m_PPS.rcLegend.left() - plotWide) >> 1 );

	// Define data rect
	m_PPS.rcData = m_PPS.rcPlot.adjusted( 0, 15, -16, 0 );

	// Measure arrows and build arrow vertices
	m_PPS.vXArrow[0].setX( m_PPS.rcPlot.right() - m_PPS.siArrow.height() );
	m_PPS.vXArrow[0].setY( m_PPS.rcPlot.bottom() - m_PPS.siArrow.width() );
	m_PPS.vXArrow[1].setX( m_PPS.rcPlot.right() );
	m_PPS.vXArrow[1].setY( m_PPS.rcPlot.bottom() );
	m_PPS.vXArrow[2].setX( m_PPS.rcPlot.right() - m_PPS.siArrow.height() );
	m_PPS.vXArrow[2].setY( m_PPS.rcPlot.bottom() + m_PPS.siArrow.width() );
	m_PPS.vYArrow[0].setX( m_PPS.rcPlot.left() - m_PPS.siArrow.width() );
	m_PPS.vYArrow[0].setY( m_PPS.rcPlot.top() + m_PPS.siArrow.height() );
	m_PPS.vYArrow[1].setX( m_PPS.rcPlot.left() );
	m_PPS.vYArrow[1].setY( m_PPS.rcPlot.top() );
	m_PPS.vYArrow[2].setX( m_PPS.rcPlot.left() + m_PPS.siArrow.width() );
	m_PPS.vYArrow[2].setY( m_PPS.rcPlot.top() + m_PPS.siArrow.height() );

	// Get axis scale and title
	int iScaleUnits = m_pMainWindow->XScaleUnits();
	if (iScaleUnits < 0 || iScaleUnits >= MDTRA_XSU_MAX)
		iScaleUnits = 0;
	m_PPS.axisTitle = QString::fromLocal8Bit(UTIL_GetXAxisTitle(iScaleUnits,langId));
	m_PPS.scaleDimension = m_iDimension * (m_flXScale * flXScaleUnit_Scale[iScaleUnits] + flXScaleUnit_Bias[iScaleUnits]);

	// Get scale factor
	float fScaleFactor = 1.0f;
	if (m_PPS.scaleDimension > 10) {
		int plotDataSize = m_PPS.scaleDimension;
		while ( 1 ) {
			plotDataSize /= 10;
			fScaleFactor *= 10;
			if (plotDataSize <= 10) break;		
		}
	}
	else if (m_PPS.scaleDimension < 2) {
		int plotDataSize = (int)(1.0f / m_PPS.scaleDimension);
		while ( 1 ) {
			plotDataSize /= 10;
			fScaleFactor *= 0.1f;
			if (plotDataSize <= 1) break;		
		}
	}
	m_PPS.scaleFactor = fScaleFactor;
	m_PPS.axisScale = (float)m_PPS.rcData.width() / m_PPS.scaleDimension;

	// Get legend scales
	if ( m_bShowLegend )
	{
		m_PPS.legendAxisScale = (float)m_PPS.rcLegend.height() / (float)m_flLegendCachedMax;
		if ( m_flLegendCachedMax <= 2.0f )
			m_PPS.legendScaleFactor = 0.25f;
		else if ( m_flLegendCachedMax <= 10.0f )
			m_PPS.legendScaleFactor = 0.5f;
		else if ( m_flLegendCachedMax <= 30.0f )
			m_PPS.legendScaleFactor = 1.0f;
		else if ( m_flLegendCachedMax <= 60.0f )
			m_PPS.legendScaleFactor = 2.0f;
		else if ( m_flLegendCachedMax <= 100.0f ) 
			m_PPS.legendScaleFactor = 4.0f;
		else 
			m_PPS.legendScaleFactor = 10.0f;
	}
}

bool MDTRA_2D_RMSD_Plot :: renderPlot_OpenGL( void ) 
{
	// Init fonts
	QFontMetrics fmTitle( m_PPS.fntTitle );
	QFontMetrics fmNormal( m_PPS.fntNormal );
	QFontMetrics fmDigits( m_PPS.fntDigits );

	// Clear
	qglClearColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BACKGROUND ) );
	glClear( GL_COLOR_BUFFER_BIT );

	// Draw RMSD map
	qglColor( Qt::white );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, m_uiTexture );
	glBegin( GL_QUADS );
		glTexCoord2f(0,1);
		glVertex2f( m_PPS.rcData.left(), m_PPS.rcData.top() );
		glTexCoord2f(1,1);
		glVertex2f( m_PPS.rcData.right(), m_PPS.rcData.top() );
		glTexCoord2f(1,0);
		glVertex2f( m_PPS.rcData.right(), m_PPS.rcData.bottom() );
		glTexCoord2f(0,0);
		glVertex2f( m_PPS.rcData.left(), m_PPS.rcData.bottom() );
	glEnd();
	glDisable( GL_TEXTURE_2D );

	// Draw axes
	qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ) );
	glBegin(GL_LINES);
		glVertex2f( m_PPS.rcPlot.left(), m_PPS.rcPlot.bottom() );
		glVertex2f( m_PPS.rcPlot.right() - m_PPS.siArrow.height(), m_PPS.rcPlot.bottom() );
		glVertex2f( m_PPS.rcPlot.left(), m_PPS.rcPlot.top() + m_PPS.siArrow.height() );
		glVertex2f( m_PPS.rcPlot.left(), m_PPS.rcPlot.bottom() );
	glEnd();

	//Draw unit marks
	glBegin(GL_LINES);
	for (float i = 0; i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		glVertex2f( m_PPS.rcPlot.left() + i*m_PPS.axisScale, m_PPS.rcPlot.bottom() + 4 );
		glVertex2f( m_PPS.rcPlot.left() + i*m_PPS.axisScale, m_PPS.rcPlot.bottom() - 3 );
	}
	for (float i = (m_PPS.scaleFactor*0.5f); i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		glVertex2f( m_PPS.rcPlot.left() + i*m_PPS.axisScale, m_PPS.rcPlot.bottom() + 3 );
		glVertex2f( m_PPS.rcPlot.left() + i*m_PPS.axisScale, m_PPS.rcPlot.bottom() - 2 );
	}
	for (float i = m_PPS.scaleFactor; i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		glVertex2f( m_PPS.rcPlot.left() + 3, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale );
		glVertex2f( m_PPS.rcPlot.left() - 4, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale );
	}
	for (float i = (m_PPS.scaleFactor*0.5f); i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		glVertex2f( m_PPS.rcPlot.left() + 2, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale );
		glVertex2f( m_PPS.rcPlot.left() - 3, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale );
	}
	glEnd();

	// Draw arrows
	if ( m_bMultisampleContext ) glEnable( GL_MULTISAMPLE );
	glBegin(GL_TRIANGLES);
		glVertex2f( m_PPS.vXArrow[0].x(), m_PPS.vXArrow[0].y() );
		glVertex2f( m_PPS.vXArrow[1].x(), m_PPS.vXArrow[1].y() );
		glVertex2f( m_PPS.vXArrow[2].x(), m_PPS.vXArrow[2].y() );
		glVertex2f( m_PPS.vYArrow[0].x(), m_PPS.vYArrow[0].y() );
		glVertex2f( m_PPS.vYArrow[1].x(), m_PPS.vYArrow[1].y() );
		glVertex2f( m_PPS.vYArrow[2].x(), m_PPS.vYArrow[2].y() );
	glEnd();
	if ( m_bMultisampleContext ) glDisable( GL_MULTISAMPLE );

	// Draw title
	qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ) );
	renderSpecialString( m_PPS.rcPlot.left() + ((m_PPS.rcPlot.width() - fmTitle.width( m_szTitle )) >> 1), 20 - (fmTitle.height() >> 2), m_szTitle, m_PPS.fntTitle, fmTitle );

	// Draw X axis title
	renderSpecialString( m_PPS.rcPlot.left() + ((m_PPS.rcPlot.width() - fmNormal.width( m_PPS.axisTitle )) >> 1), m_iHeight - (fmNormal.height()>>1), m_PPS.axisTitle, m_PPS.fntNormal, fmNormal );

	// Draw Y axis title
	renderSpecialString( m_PPS.rcPlot.left() - 8 - fmNormal.width( m_PPS.axisTitle ), 20, m_PPS.axisTitle, m_PPS.fntNormal, fmNormal );

	// Draw scale titles
	for (float i = 0; i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		QString s = QString::number( i );
		renderText( m_PPS.rcPlot.left() + i*m_PPS.axisScale - (fmDigits.width(s)>>1), m_PPS.rcPlot.bottom() + fmDigits.height(), s, m_PPS.fntDigits );
	}
	for (float i = m_PPS.scaleFactor; i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		QString s = QString::number( i );
		renderText( m_PPS.rcPlot.left() - fmDigits.width(s) - 6, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale + (fmDigits.height()>>1), s, m_PPS.fntDigits );
	}

	if (m_bShowLegend) {
		// Draw texture
		qglColor( Qt::white );
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, m_uiLegend );
		glBegin( GL_QUADS );
			glTexCoord2f(0,1);
			glVertex2f( m_PPS.rcLegend.left(), m_PPS.rcLegend.top() );
			glTexCoord2f(1,1);
			glVertex2f( m_PPS.rcLegend.right(), m_PPS.rcLegend.top() );
			glTexCoord2f(1,0);
			glVertex2f( m_PPS.rcLegend.right(), m_PPS.rcLegend.bottom() );
			glTexCoord2f(0,0);
			glVertex2f( m_PPS.rcLegend.left(), m_PPS.rcLegend.bottom() );
		glEnd();
		glDisable( GL_TEXTURE_2D );

		// Draw frame
		qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glBegin( GL_QUADS );
			glVertex2f( m_PPS.rcLegend.left(), m_PPS.rcLegend.top() );
			glVertex2f( m_PPS.rcLegend.right(), m_PPS.rcLegend.top() );
			glVertex2f( m_PPS.rcLegend.right(), m_PPS.rcLegend.bottom() );
			glVertex2f( m_PPS.rcLegend.left(), m_PPS.rcLegend.bottom() );
		glEnd();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

		// Draw unit marks
		glBegin(GL_LINES);
		for (float i = 0; i < m_flLegendCachedMax; i += m_PPS.legendScaleFactor) {
			glVertex2f( m_PPS.rcLegend.left() - 1, m_PPS.rcLegend.bottom() - i*m_PPS.legendAxisScale );
			glVertex2f( m_PPS.rcLegend.left() + 3, m_PPS.rcLegend.bottom() - i*m_PPS.legendAxisScale );
			glVertex2f( m_PPS.rcLegend.right() - 4, m_PPS.rcLegend.bottom() - i*m_PPS.legendAxisScale );
			glVertex2f( m_PPS.rcLegend.right(), m_PPS.rcLegend.bottom() - i*m_PPS.legendAxisScale);
		}
		glEnd();

		// Draw scale titles
		qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ) );
		for (float i = 0; i <= m_flLegendCachedMax; i += m_PPS.legendScaleFactor) {
			QString s = QString::number( i );
			renderText( m_PPS.rcLegend.right() + 10, m_PPS.rcLegend.bottom() - i*m_PPS.legendAxisScale + (fmDigits.height()>>1) - 2, s, m_PPS.fntDigits );
		}

		//Draw legend title
		QString szLegendTitle("RMSD [A°]");
		renderSpecialString( m_PPS.rcLegend.left() + ((m_PPS.rcLegend.width() - fmNormal.width(szLegendTitle))>>1), 20, szLegendTitle, m_PPS.fntNormal, fmNormal );
	}
	return true;
}

bool MDTRA_2D_RMSD_Plot :: renderPlot_Generic( QPainter* pPainter ) 
{
#ifdef MDTRA_ALLOW_PRINTER
	// Init fonts
	QFontMetrics fmTitle( m_PPS.fntTitle );
	QFontMetrics fmNormal( m_PPS.fntNormal );
	QFontMetrics fmDigits( m_PPS.fntDigits );

	// Init rects, brushes, pens, etc.
	QRect wholeRect( 0, 0, m_iWidth, m_iHeight );
	QBrush axisBrush( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ) );
	QPen borderPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );
	QPen axisPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );
	QPen textPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );

	// Clear
	pPainter->fillRect( wholeRect, m_pMainWindow->getColorManager()->color( COLOR_PLOT_BACKGROUND ) );

	if ( m_pMapPixels ) {
		// Generate QImage for RMSD map
		QImage rmsdMap = QImage( m_iMapWidth, m_iMapHeight, QImage::Format_RGB32 );
		for ( int i = 0; i < m_iMapHeight; i++ ) {
			for ( int j = 0; j < m_iMapWidth; j++ ) {
				byte r = m_pMapPixels[(i*m_iMapWidth+j)*4+0];
				byte g = m_pMapPixels[(i*m_iMapWidth+j)*4+1];
				byte b = m_pMapPixels[(i*m_iMapWidth+j)*4+2];
				unsigned int c = (0xFF << 24) | (r << 16) | (g << 8) | b;
				rmsdMap.setPixel( j, m_iMapHeight-i-1, c );
			}
		}
		// Draw RMSD map
		QRect imgRect( 0, 0, m_iMapWidth, m_iMapHeight );
		pPainter->drawImage( m_PPS.rcData, rmsdMap, imgRect );
	}

	// Draw axes
	pPainter->setPen( axisPen );
	pPainter->drawLine( m_PPS.rcPlot.left(), m_PPS.rcPlot.bottom()+1,
						m_PPS.rcPlot.right() - m_PPS.siArrow.height(),	m_PPS.rcPlot.bottom()+1 );
	pPainter->drawLine( m_PPS.rcPlot.left(), m_PPS.rcPlot.top() + m_PPS.siArrow.height(),
						m_PPS.rcPlot.left(), m_PPS.rcPlot.bottom() );

	//Draw unit marks
	for (float i = 0; i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		pPainter->drawLine( m_PPS.rcPlot.left() + i*m_PPS.axisScale, m_PPS.rcPlot.bottom() + 4, m_PPS.rcPlot.left() + i*m_PPS.axisScale, m_PPS.rcPlot.bottom() - 2 );
	}
	for (float i = (m_PPS.scaleFactor*0.5f); i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		pPainter->drawLine( m_PPS.rcPlot.left() + i*m_PPS.axisScale, m_PPS.rcPlot.bottom() + 3, m_PPS.rcPlot.left() + i*m_PPS.axisScale, m_PPS.rcPlot.bottom() - 1 );
	}
	for (float i = m_PPS.scaleFactor; i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		pPainter->drawLine( m_PPS.rcPlot.left() + 3, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale + 1, m_PPS.rcPlot.left() - 3, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale + 1 );
	}
	for (float i = (m_PPS.scaleFactor*0.5f); i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		pPainter->drawLine( m_PPS.rcPlot.left() + 2, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale + 1, m_PPS.rcPlot.left() - 2, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale + 1 );
	}

	// Draw arrows
	pPainter->setBrush( axisBrush );
	pPainter->drawPolygon( m_PPS.vXArrow, 3 );
	pPainter->drawPolygon( m_PPS.vYArrow, 3 );

	// Draw title
	pPainter->setPen( textPen );
	renderSpecialString( m_PPS.rcPlot.left() + ((m_PPS.rcPlot.width() - fmTitle.width( m_szTitle )) >> 1), 20 - (fmTitle.height() >> 2), m_szTitle, m_PPS.fntTitle, fmTitle, pPainter );

	// Draw X axis title
	renderSpecialString( m_PPS.rcPlot.left() + ((m_PPS.rcPlot.width() - fmNormal.width( m_PPS.axisTitle )) >> 1), m_iHeight - (fmNormal.height()>>1), m_PPS.axisTitle, m_PPS.fntNormal, fmNormal, pPainter  );

	// Draw Y axis title
	renderSpecialString( m_PPS.rcPlot.left() - 8 - fmNormal.width( m_PPS.axisTitle ), 20, m_PPS.axisTitle, m_PPS.fntNormal, fmNormal, pPainter  );

	// Draw scale titles
	pPainter->setFont( m_PPS.fntDigits );
	for (float i = 0; i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		QString s = QString::number( i );
		pPainter->drawText( m_PPS.rcPlot.left() + i*m_PPS.axisScale - (fmDigits.width(s)>>1), m_PPS.rcPlot.bottom() + fmDigits.height(), s );
	}
	for (float i = m_PPS.scaleFactor; i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		QString s = QString::number( i );
		pPainter->drawText( m_PPS.rcPlot.left() - fmDigits.width(s) - 6, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale + (fmDigits.height()>>1), s );
	}

	if (m_bShowLegend) {
		// Generate and draw gradient texture
		QImage gradientMap = QImage( m_PPS.rcLegend.width(), m_PPS.rcLegend.height(), QImage::Format_RGB32 );
		for ( int i = 0; i < m_PPS.rcLegend.height(); i++ ) {
			float dv = 1.0f - (float)i / (float)(m_PPS.rcLegend.height()-1);
			for ( int j = 0; j < m_PPS.rcLegend.width(); j++ ) {
				byte r = m_bLegendCachedRGB ? UTIL_FloatToColor( 0, dv ) : dv * 255;
				byte g = m_bLegendCachedRGB ? UTIL_FloatToColor( 1, dv ) : dv * 255;
				byte b = m_bLegendCachedRGB ? UTIL_FloatToColor( 2, dv ) : dv * 255;
				unsigned int c = (0xFF << 24) | (r << 16) | (g << 8) | b;
				gradientMap.setPixel( j, i, c );
			}
		}
		QRect gradientRect( 0, 0, m_PPS.rcLegend.width(), m_PPS.rcLegend.height() );
		pPainter->drawImage( m_PPS.rcLegend, gradientMap, gradientRect );

		// Draw frame
		pPainter->setBrush( Qt::NoBrush );
		pPainter->setPen( borderPen );
		pPainter->drawRect( m_PPS.rcLegend );
		
		// Draw unit marks
		for (float i = m_PPS.legendScaleFactor; i < m_flLegendCachedMax; i += m_PPS.legendScaleFactor) {
			pPainter->drawLine( m_PPS.rcLegend.left(), m_PPS.rcLegend.bottom() - i*m_PPS.legendAxisScale + 1, m_PPS.rcLegend.left() + 3, m_PPS.rcLegend.bottom() - i*m_PPS.legendAxisScale + 1 );
			pPainter->drawLine( m_PPS.rcLegend.right() - 2, m_PPS.rcLegend.bottom() - i*m_PPS.legendAxisScale + 1, m_PPS.rcLegend.right() + 1, m_PPS.rcLegend.bottom() - i*m_PPS.legendAxisScale + 1);
		}

		// Draw scale titles
		pPainter->setFont( m_PPS.fntDigits );
		pPainter->setPen( textPen );
		for (float i = 0; i <= m_flLegendCachedMax; i += m_PPS.legendScaleFactor) {
			QString s = QString::number( i );
			pPainter->drawText( m_PPS.rcLegend.right() + 10, m_PPS.rcLegend.bottom() - i*m_PPS.legendAxisScale + (fmDigits.height()>>1) - 2, s );
		}

		//Draw legend title
		QString szLegendTitle("RMSD [A°]");
		renderSpecialString( m_PPS.rcLegend.left() + ((m_PPS.rcLegend.width() - fmNormal.width(szLegendTitle))>>1), 20, szLegendTitle, m_PPS.fntNormal, fmNormal, pPainter );
	}

#endif
	return true;
}
