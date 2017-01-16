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
//	Implementation of MDTRA_HistogramPlot

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_histogramPlot.h"
#include "mdtra_project.h"
#include "mdtra_colors.h"
#include "mdtra_utils.h"
#include "mdtra_math.h"

#include "glext.h"

#include <QtGui/QApplication>
#include <QtGui/QPrinter>

static const char *szResidueAxisNames[MDTRA_LANG_MAX] = 
{
	"Residue Count",	
	"Кол-во остатков"
};


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

static int iXScaleUnit_WidthOffset[MDTRA_XSU_MAX] = 
{
	30,
	30,
	0,
};

MDTRA_HistogramPlot :: MDTRA_HistogramPlot( QGLFormat &format, QWidget *parent )
					 : MDTRA_Generic_Plot( format, parent )
{
}

MDTRA_HistogramPlot :: ~MDTRA_HistogramPlot()
{
}

void MDTRA_HistogramPlot :: reset( void )
{
	m_iNumBands = 0;
	m_pBandData = NULL;
}

float MDTRA_HistogramPlot :: calc_normal_distribution( float f )
{
	const float scale = 1.0f / sqrt( 2.0f * M_PI_F );

	float x = f - m_flExpectedValue;
	return scale * m_flInvStdDev * exp(-0.5f*x*x*m_flInvStdDev*m_flInvStdDev);
}

void MDTRA_HistogramPlot :: setHistogramData( float flMin, float flSize, int iCount, const int *piData )
{
	assert( iCount > 0 );
	assert( piData != NULL );

	m_flBandMin = flMin;
	m_flBandSize = flSize;
	m_iNumBands = iCount;
	m_pBandData = piData;

	m_iMaxBandData = -9999999;
	m_iTotalData = 0;
	for ( int i = 0; i < iCount; i++ ) {
		m_iTotalData += piData[i];
		if ( piData[i] > m_iMaxBandData )
			m_iMaxBandData = piData[i];
	}
}

void MDTRA_HistogramPlot :: setupPlotPaintStruct( void ) 
{
	// Init fonts
	m_PPS.fntTitle = QFont( "Arial", 10, QFont::Bold, false );
	m_PPS.fntTitle.setPixelSize( 14 );
	m_PPS.fntNormal = QFont( "Arial", 8, QFont::Normal, false );
	m_PPS.fntNormal.setPixelSize( 12 );
	m_PPS.fntDigits = QFont( "Arial", 8, QFont::Normal, false );
	m_PPS.fntDigits.setPixelSize( 12 );

	int langId = m_pMainWindow->PlotLanguage();

	if ( m_iAxisType == MDTRA_DT_USER ) {
		m_PPS.xAxisTitle = m_sUserData;
	} else {
		m_PPS.xAxisTitle = QString("%1 %2").arg( QString::fromLocal8Bit( UTIL_GetYAxisTitle( m_iAxisType, langId ) ) ).arg( QString::fromLocal8Bit( UTIL_GetYAxisUnitTitle( m_iAxisUnit, langId ) ) );
	}

	// Get axis scale and title
	int iScaleUnits;
	if ( m_iAxisLayout == MDTRA_LAYOUT_RESIDUE ) {
		iScaleUnits = 0;
		m_PPS.yAxisTitle = QString::fromLocal8Bit( szResidueAxisNames[langId] );
		m_PPS.scaleDimension = m_iMaxBandData;
	} else {
		iScaleUnits = m_pMainWindow->XScaleUnits();
		if (iScaleUnits < 0 || iScaleUnits >= MDTRA_XSU_MAX)
			iScaleUnits = 0;
		m_PPS.yAxisTitle = QString::fromLocal8Bit( UTIL_GetXAxisTitle( iScaleUnits, langId ) );
		m_PPS.scaleDimension = m_iMaxBandData * (m_flXScale * flXScaleUnit_Scale[iScaleUnits] + flXScaleUnit_Bias[iScaleUnits]);
	}

	// Set defaults
	int plotWide = m_iWidth - 60 - iXScaleUnit_WidthOffset[iScaleUnits];
	int plotTall = m_iHeight - 80;

	// Adjust plotWide
	m_PPS.bandWide = (int)floor( (float)plotWide / m_iNumBands );
	if ( m_PPS.bandWide < 1 ) m_PPS.bandWide = 1;
	if ( m_PPS.bandWide > 30 ) m_PPS.bandWide = 32;
	plotWide = m_PPS.bandWide * m_iNumBands;

	// Set X and Y
	int plotX = ((m_iWidth - plotWide) >> 1) - (m_PPS.bandWide >> 2);
	int plotY = (m_iHeight - plotTall) >> 1;

	// define initial plot rect
	m_PPS.rcPlot = QRect( plotX, plotY, plotWide, plotTall );

	// Define data rect
	m_PPS.rcData = m_PPS.rcPlot.adjusted( 0, 4, 0, 0 );

	// Calc band scale
	m_PPS.bandHeightScale = (float)m_PPS.rcData.height() / m_iMaxBandData;

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
	m_PPS.axisScale = (float)m_PPS.rcData.height() / m_PPS.scaleDimension;

	// Group bands
	m_PPS.bandGroup = (int)ceil( 32.0f / m_PPS.bandWide ) << 1;
	if ( m_PPS.bandGroup > m_iNumBands )
		m_PPS.bandGroup = m_iNumBands;
}

bool MDTRA_HistogramPlot :: renderPlot_OpenGL( void ) 
{
	// Init fonts
	QFontMetrics fmTitle( m_PPS.fntTitle );
	QFontMetrics fmNormal( m_PPS.fntNormal );
	QFontMetrics fmDigits( m_PPS.fntDigits );

	// Clear
	qglClearColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BACKGROUND ) );
	glClear( GL_COLOR_BUFFER_BIT );

	// Draw bands
	if ( m_CM == MDTRA_HCM_SINGLE )
		qglColor( m_Color );

	for ( int i = 0; i < m_iNumBands; i++ ) {
		float height = m_PPS.bandHeightScale * m_pBandData[i];
		if ( m_CM == MDTRA_HCM_AUTO )
			qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ) );
		else if ( m_CM == MDTRA_HCM_GRAYSCALE ) {
			int gray = qGray( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ).rgb() );
			qglColor( qRgb( gray, gray, gray ) );
		}

		glBegin( GL_QUADS );
			glVertex2f( m_PPS.rcData.left() + m_PPS.bandWide*i, m_PPS.rcData.bottom() );
			glVertex2f( m_PPS.rcData.left() + m_PPS.bandWide*i, m_PPS.rcData.bottom() - height );
			glVertex2f( m_PPS.rcData.left() + m_PPS.bandWide*(i+1), m_PPS.rcData.bottom() - height );
			glVertex2f( m_PPS.rcData.left() + m_PPS.bandWide*(i+1), m_PPS.rcData.bottom() );
		glEnd();
	}

	// Draw borders of bands
	qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
	for ( int i = 0; i < m_iNumBands; i++ ) {
		float height = m_PPS.bandHeightScale * m_pBandData[i];
		glBegin( GL_LINES );
			glVertex2f( m_PPS.rcData.left() + m_PPS.bandWide*i, m_PPS.rcData.bottom() );
			glVertex2f( m_PPS.rcData.left() + m_PPS.bandWide*i, m_PPS.rcData.bottom() - height );
			glVertex2f( m_PPS.rcData.left() + m_PPS.bandWide*(i+1), m_PPS.rcData.bottom() );
			glVertex2f( m_PPS.rcData.left() + m_PPS.bandWide*(i+1), m_PPS.rcData.bottom() - height );
			glVertex2f( m_PPS.rcData.left() + m_PPS.bandWide*i-1, m_PPS.rcData.bottom() - height );
			glVertex2f( m_PPS.rcData.left() + m_PPS.bandWide*(i+1), m_PPS.rcData.bottom() - height );
		glEnd();
	}

	// Draw normal distribution
/*	qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
	glBegin( GL_LINE_STRIP );
	float bandScale = m_flBandSize / m_PPS.bandWide;
	for ( int i = 0; i < m_PPS.rcData.width(); i += 4 ) {
		float f = m_flBandMin + i * bandScale;
		float height = m_PPS.rcData.height() * calc_normal_distribution( f );
		float d = calc_normal_distribution( f );
		glVertex2f( m_PPS.rcData.left() + i, m_PPS.rcData.bottom() - height );
	}
	glEnd();*/

	// Draw axes
	qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ) );
	glBegin(GL_LINES);
		glVertex2f( m_PPS.rcPlot.left(), m_PPS.rcPlot.bottom() );
		glVertex2f( m_PPS.rcPlot.right() + (m_PPS.bandWide >> 1), m_PPS.rcPlot.bottom() );
		glVertex2f( m_PPS.rcPlot.left(), m_PPS.rcPlot.top() );
		glVertex2f( m_PPS.rcPlot.left(), m_PPS.rcPlot.bottom() );
	glEnd();

	// Draw unit marks
	glBegin(GL_LINES);
	for (int i = 0; i <= m_iNumBands; i += m_PPS.bandGroup) {
		glVertex2f( m_PPS.rcPlot.left() + m_PPS.bandWide*i, m_PPS.rcPlot.bottom() + 4 );
		glVertex2f( m_PPS.rcPlot.left() + m_PPS.bandWide*i, m_PPS.rcPlot.bottom() - 3 );
	}
	for (int i = (m_PPS.bandGroup >> 1); i <= m_iNumBands; i += m_PPS.bandGroup) {
		glVertex2f( m_PPS.rcPlot.left() + m_PPS.bandWide*i, m_PPS.rcPlot.bottom() + 3 );
		glVertex2f( m_PPS.rcPlot.left() + m_PPS.bandWide*i, m_PPS.rcPlot.bottom() - 2 );
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

	// Draw title
	qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ) );
	renderSpecialString( m_PPS.rcPlot.left() + ((m_PPS.rcPlot.width() - fmTitle.width( m_szTitle )) >> 1), 20 - (fmTitle.height() >> 2), m_szTitle, m_PPS.fntTitle, fmTitle );

	// Draw scale titles
	for (int i = 0; i <=m_iNumBands; i += m_PPS.bandGroup) {
		QString s = QString::number( m_flBandMin + i*m_flBandSize, 'f', 2 );
		renderText( m_PPS.rcPlot.left() + m_PPS.bandWide*i - (fmDigits.width(s)>>1), m_PPS.rcPlot.bottom() + fmDigits.height(), s, m_PPS.fntDigits );
	}
	for (float i = m_PPS.scaleFactor; i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		QString s = QString::number( i );
		renderText( m_PPS.rcPlot.left() - fmDigits.width(s) - 6, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale + (fmDigits.height()>>1), s, m_PPS.fntDigits );
	}

	// Draw X axis title
	renderSpecialString( m_PPS.rcPlot.left() + ((m_PPS.rcPlot.width() - fmNormal.width( m_PPS.xAxisTitle )) >> 1), m_iHeight - (fmNormal.height()>>1), m_PPS.xAxisTitle, m_PPS.fntNormal, fmNormal );

	// Draw Y axis title
	renderSpecialString( MDTRA_MAX( 4, m_PPS.rcData.left() - fmNormal.width( m_PPS.yAxisTitle ) - 2 ), m_PPS.rcPlot.top() - 4, m_PPS.yAxisTitle, m_PPS.fntNormal, fmNormal );

	return true;
}

bool MDTRA_HistogramPlot :: renderPlot_Generic( QPainter* pPainter ) 
{
#ifdef MDTRA_ALLOW_PRINTER
	// Init fonts
	QFontMetrics fmTitle( m_PPS.fntTitle );
	QFontMetrics fmNormal( m_PPS.fntNormal );
	QFontMetrics fmDigits( m_PPS.fntDigits );

	// Init rects, brushes, pens, etc.
	QRect wholeRect( 0, 0, m_iWidth, m_iHeight );
	QPen borderPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );
	QPen axisPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );
	QPen textPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );

	// Clear
	pPainter->fillRect( wholeRect, m_pMainWindow->getColorManager()->color( COLOR_PLOT_BACKGROUND ) );

	// Draw bands
	if ( m_CM == MDTRA_HCM_SINGLE )
		pPainter->setBrush( QBrush( m_Color ) );

	for ( int i = 0; i < m_iNumBands; i++ ) {
		int height = (int)floor(m_PPS.bandHeightScale * m_pBandData[i] );
		if ( m_CM == MDTRA_HCM_AUTO )
			pPainter->setBrush( QBrush( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ) ) );
		else if ( m_CM == MDTRA_HCM_GRAYSCALE ) {
			int gray = qGray( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ).rgb() );
			pPainter->setBrush( QBrush( qRgb( gray, gray, gray ) ) );
		}
		pPainter->drawRect( m_PPS.rcData.left() + m_PPS.bandWide*i, m_PPS.rcData.bottom() - height, m_PPS.bandWide, height );
	}

	// Draw borders of bands
	pPainter->setBrush( Qt::NoBrush );
	pPainter->setPen( borderPen );
	for ( int i = 0; i < m_iNumBands; i++ ) {
		int height = (int)floor( m_PPS.bandHeightScale * m_pBandData[i] );
		pPainter->drawRect( m_PPS.rcData.left() + m_PPS.bandWide*i, m_PPS.rcData.bottom() - height, m_PPS.bandWide, height );
	}

	// Draw axes
	pPainter->setPen( axisPen );
	pPainter->drawLine( m_PPS.rcPlot.left(), m_PPS.rcPlot.bottom(), m_PPS.rcPlot.right() + (m_PPS.bandWide >> 1), m_PPS.rcPlot.bottom() );
	pPainter->drawLine( m_PPS.rcPlot.left(), m_PPS.rcPlot.top(), m_PPS.rcPlot.left(), m_PPS.rcPlot.bottom() );

	// Draw unit marks
	for (int i = 0; i <= m_iNumBands; i += m_PPS.bandGroup) {
		pPainter->drawLine( m_PPS.rcPlot.left() + m_PPS.bandWide*i, m_PPS.rcPlot.bottom() + 4, m_PPS.rcPlot.left() + m_PPS.bandWide*i, m_PPS.rcPlot.bottom() - 3 );
	}
	for (int i = (m_PPS.bandGroup >> 1); i <= m_iNumBands; i += m_PPS.bandGroup) {
		pPainter->drawLine( m_PPS.rcPlot.left() + m_PPS.bandWide*i, m_PPS.rcPlot.bottom() + 3, m_PPS.rcPlot.left() + m_PPS.bandWide*i, m_PPS.rcPlot.bottom() - 2 );
	}
	for (float i = m_PPS.scaleFactor; i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		pPainter->drawLine( m_PPS.rcPlot.left() + 3, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale, m_PPS.rcPlot.left() - 4, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale );
	}
	for (float i = (m_PPS.scaleFactor*0.5f); i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		pPainter->drawLine( m_PPS.rcPlot.left() + 2, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale, m_PPS.rcPlot.left() - 3, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale );
	}

	// Draw title
	pPainter->setPen( textPen );
	renderSpecialString( m_PPS.rcPlot.left() + ((m_PPS.rcPlot.width() - fmTitle.width( m_szTitle )) >> 1), 20 - (fmTitle.height() >> 2), m_szTitle, m_PPS.fntTitle, fmTitle, pPainter );

	// Draw scale titles
	pPainter->setFont( m_PPS.fntDigits );
	for (int i = 0; i <=m_iNumBands; i += m_PPS.bandGroup) {
		QString s = QString::number( m_flBandMin + i*m_flBandSize, 'f', 2 );
		pPainter->drawText( m_PPS.rcPlot.left() + m_PPS.bandWide*i - (fmDigits.width(s)>>1), m_PPS.rcPlot.bottom() + fmDigits.height(), s );
	}
	for (float i = m_PPS.scaleFactor; i <= m_PPS.scaleDimension; i += m_PPS.scaleFactor) {
		QString s = QString::number( i );
		pPainter->drawText( m_PPS.rcPlot.left() - fmDigits.width(s) - 6, m_PPS.rcPlot.bottom() - i*m_PPS.axisScale + (fmDigits.height()>>1), s );
	}

	// Draw X axis title
	renderSpecialString( m_PPS.rcPlot.left() + ((m_PPS.rcPlot.width() - fmNormal.width( m_PPS.xAxisTitle )) >> 1), m_iHeight - (fmNormal.height()>>1), m_PPS.xAxisTitle, m_PPS.fntNormal, fmNormal, pPainter );

	// Draw Y axis title
	renderSpecialString( MDTRA_MAX( 4, m_PPS.rcData.left() - fmNormal.width( m_PPS.yAxisTitle ) - 2 ), m_PPS.rcPlot.top() - 4, m_PPS.yAxisTitle, m_PPS.fntNormal, fmNormal, pPainter );


#endif
	return true;
}
