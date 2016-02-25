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
//	Implementation of MDTRA_Plot

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_plot.h"
#include "mdtra_project.h"
#include "mdtra_colors.h"
#include "mdtra_labelDialog.h"
#include "mdtra_pdb.h"
#include "mdtra_waitDialog.h"
#include "mdtra_math.h"
#include "mdtra_utils.h"
#include "glext.h"

#include <QtGui/QMessageBox>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QPrinter>

#define MDTRA_ANGLES_MASK	((1 << MDTRA_YSU_DEGREES) | (1 << MDTRA_YSU_RADIANS))

static const char *szResidueAxisNames[MDTRA_LANG_MAX] = 
{
	"Residue Number",	
	"Íîìåð îñòàòêà"
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

MDTRA_Plot :: MDTRA_Plot( QGLFormat &format, QWidget *parent )
			: QGLWidget( format, parent, NULL )
{
	setAutoFillBackground( false );
	setMouseTracking( true );

	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);
	reset();

	m_bDisabled = true;
	m_szDisabledMsg = tr("No result selected");

	m_iMouseX = -1;
	m_iMouseY = -1;
	m_Layout = MDTRA_LAYOUT_TIME;
	m_DataType = MDTRA_DT_RMSD;
	m_DataScaleUnits = MDTRA_YSU_ANGSTROMS;
	m_pLabels = NULL;

	m_bSelectionStart = false;
	m_bSelectionDrag = false;
	m_bOffsetDrag = false;
	m_bOffsetDragPerformed = false;
	m_fDisplayOffset = 0.0f;
	m_fDisplayScale = 1.0f;

	m_pAddLabelAction = new QAction( tr("Add Label..."), this );
	connect( m_pAddLabelAction, SIGNAL(triggered()), this, SLOT(addLabel()) );
	m_commandMenu.addAction( m_pAddLabelAction );
	m_pEditLabelAction = new QAction( tr("Edit Label..."), this );
	connect( m_pEditLabelAction, SIGNAL(triggered()), this, SLOT(editLabel()) );
	m_commandMenu.addAction( m_pEditLabelAction );
	m_pRemoveLabelAction = new QAction("Remove Label(s)", this);
	connect( m_pRemoveLabelAction, SIGNAL(triggered()), this, SLOT(removeLabel()) );
	m_commandMenu.addAction( m_pRemoveLabelAction );
}

MDTRA_Plot :: ~MDTRA_Plot()
{
}

QSize MDTRA_Plot :: minimumSizeHint() const
{
	return QSize(50, 100);
}

QSize MDTRA_Plot :: sizeHint() const
{
	return QSize(400, 300);
}

void MDTRA_Plot :: reset( void )
{
	m_bDisabled = false;
	m_szDisabledMsg.clear();
	clearPlotData();
}

void MDTRA_Plot :: enable( void )
{
	m_bDisabled = false;
	m_szDisabledMsg.clear();
	updateGL();
}

void MDTRA_Plot :: disable( const QString &disabledMsg )
{
	m_bDisabled = true;
	m_szDisabledMsg = disabledMsg;
	updateGL();
}

void MDTRA_Plot :: setPlotTitle( const QString &title )
{
	m_szTitle = title;
}

void MDTRA_Plot :: setDataType( MDTRA_DataType dt )
{
	m_DataType = dt;
}

void MDTRA_Plot :: setDataScaleUnits( MDTRA_YScaleUnits ysu )
{
	m_DataScaleUnits = ysu;
}

void MDTRA_Plot :: setYAxisTitle( const QString &title )
{
	m_szYAxisTitle = title;
}

void MDTRA_Plot :: setLabels( QList<struct stMDTRA_Label> *pLabels )
{
	m_pLabels = pLabels;
}

void MDTRA_Plot :: clearPlotData( void )
{
	m_iMouseX = -1;
	m_iMouseY = -1;
	m_pLabels = NULL;
	m_PlotData.clear();
	m_bSelectionStart = false;
	m_bSelectionDrag = false;
	m_bOffsetDrag = false;
	m_bOffsetDragPerformed = false;
	m_fDisplayOffset = 0.0f;
	m_fDisplayScale = 1.0f;
}

void MDTRA_Plot :: addPlotData( const QString &title, const MDTRA_DSRef* pref )
{
	MDTRA_PlotData pd;
	pd.title = title;
	pd.pDataRef = pref;
	pd.xOffset = 0;
	if ( m_Layout == MDTRA_LAYOUT_RESIDUE ) {
		pd.xOffset = 1;
		const MDTRA_DataSource *pSrc = m_pMainWindow->getProject()->fetchDataSourceByIndex( pref->dataSourceIndex );
		if ( pSrc ) {
			const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( pSrc->streamIndex );
			if ( pStream && pStream->pdb )
				pd.xOffset = pStream->pdb->getFirstResidue();
		}
	}

	m_PlotData << pd;
}

void MDTRA_Plot :: initializeGL( void ) 
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
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );

	glDisable( GL_LINE_STIPPLE );
	glDisable( GL_MULTISAMPLE );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	//first clear
	qglClearColor( Qt::gray );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void MDTRA_Plot :: resizeGL( int width, int height ) 
{
	if (width <= 0)
		return;

	m_iWidth = width;
	m_iHeight = height;
}

void MDTRA_Plot :: checkDisplayOffset( void )
{
	float minOffset = -m_PPS.rcData.width() * (m_fDisplayScale - 1.0f);
	if ( m_fDisplayOffset < minOffset ) m_fDisplayOffset = minOffset;
	if ( m_fDisplayOffset > 0 ) m_fDisplayOffset = 0;
}

void MDTRA_Plot :: setupPlotPaintStruct( void ) 
{
	// Init fonts
#if defined(WIN32)
	QString fontFace( "Arial" );
#else
	QString fontFace = m_pMainWindow->font().family();
#endif
	m_PPS.fntTitle = QFont( fontFace, 12, QFont::Bold, false );
	m_PPS.fntNormal = QFont( fontFace, 10, QFont::Normal, false );
	m_PPS.fntDigits = QFont( fontFace, 8, QFont::Normal, false );
	m_PPS.fntTitle.setPixelSize( 16 );
	m_PPS.fntNormal.setPixelSize( 14 );
	m_PPS.fntDigits.setPixelSize( 12 );

	if (m_bDisabled)
		return;

	QFontMetrics fmTitle( m_PPS.fntTitle );
	QFontMetrics fmNormal( m_PPS.fntNormal );

	int langId = m_pMainWindow->PlotLanguage();

	// Set defaults
	m_PPS.rcLegend.setRect( 0, 0, 0, 0 );
	m_PPS.legendNumRows = 0;
	m_PPS.legendNumCols = 0;
	m_PPS.siArrow.setWidth( 3 );
	m_PPS.siArrow.setHeight( 8 );
	m_PPS.labelBulletHalfSize = 4;
	m_PPS.labelSelHalfSize = 2;

	if ( m_pMainWindow->plotPolarAngles() && ((1 << m_DataScaleUnits) & MDTRA_ANGLES_MASK) )
		m_PPS.polarCoords = true;
	else
		m_PPS.polarCoords = false;

	// Define initial plot rect
	if ( m_PPS.polarCoords ) {
		m_PPS.plotBottomMargin = 60;
		int wide = m_iWidth - 40;
		int tall = m_iHeight - 40 - m_PPS.plotBottomMargin;
		int dims = MDTRA_MIN( wide, tall );
		m_PPS.rcPlot = QRect( ( m_iWidth - dims ) >> 1, ( m_iHeight + (m_PPS.plotBottomMargin >> 3) - dims ) >> 1, dims, dims );
	} else {
		m_PPS.plotBottomMargin = 50;
		m_PPS.rcPlot = QRect( 40, 60, m_iWidth - 80, m_iHeight - 60 - m_PPS.plotBottomMargin );
	}

	// Calculate legend size, if any
	if (m_pMainWindow->plotShowLegend()) {
		int legendDataRows = 0;
		int maxOddSize = 0;
		int maxEvenSize = 0;
		int legendBottomMargin = 10;

		m_PPS.siBullet.setWidth( 14 );
		m_PPS.siBullet.setHeight( 14 );
		m_PPS.legendXMargin = 10;
		m_PPS.legendYMargin = 10;
		m_PPS.legendRowSpacing = 4;
		m_PPS.legendMidX = 0;

		for (int i = 0; i < m_PlotData.count(); i++) {
			if (!(m_PlotData.at(i).pDataRef->flags & DSREF_FLAG_VISIBLE))
				continue;
			int strWide = fmNormal.width( m_PlotData.at(i).title );
			if (legendDataRows % 2) {
				if (strWide > maxOddSize) maxOddSize = strWide;
			} else {
				if (strWide > maxEvenSize) maxEvenSize = strWide;
			}
			legendDataRows++;
		}

		if ( legendDataRows ) {
			// try to use 2-column display, if possible
			if ( legendDataRows > 1 ) {
				m_PPS.legendNumRows = (legendDataRows + 1) >> 1;
				m_PPS.legendNumCols = 2;
				m_PPS.legendMidX = m_PPS.legendXMargin * 2 + maxEvenSize + m_PPS.siBullet.width();
				m_PPS.rcLegend.setWidth( maxOddSize + maxEvenSize + m_PPS.legendXMargin * 5 + m_PPS.siBullet.width() * 2 );
				m_PPS.rcLegend.setHeight( m_PPS.legendYMargin * 2 + m_PPS.siBullet.height() * m_PPS.legendNumRows + m_PPS.legendRowSpacing * (m_PPS.legendNumRows - 1) );
			}

			if ( !m_PPS.rcLegend.isValid() || m_PPS.rcLegend.width() > m_PPS.rcPlot.width() ) {
				// 2-column legend is not suitable
				m_PPS.legendNumRows = legendDataRows;
				m_PPS.legendNumCols = 1;
				m_PPS.rcLegend.setWidth( MDTRA_MAX( maxOddSize, maxEvenSize ) + m_PPS.legendXMargin * 3 + m_PPS.siBullet.width() );
				m_PPS.rcLegend.setHeight( m_PPS.legendYMargin * 2 + m_PPS.siBullet.height() * m_PPS.legendNumRows + m_PPS.legendRowSpacing * (m_PPS.legendNumRows - 1) );
			}

			// calculate legend position
			m_PPS.rcLegend.moveTo( (m_iWidth - m_PPS.rcLegend.width()) >> 1 , 
									m_iHeight - m_PPS.rcLegend.height() - legendBottomMargin );

			// adjust plot rect
			if ( m_PPS.polarCoords )
				m_PPS.rcPlot.adjust( ((m_PPS.rcLegend.height() + legendBottomMargin)>>1), 0, -((m_PPS.rcLegend.height() + legendBottomMargin)>>1), -(m_PPS.rcLegend.height() + legendBottomMargin) ); 
			else
				m_PPS.rcPlot.adjust( 0, 0, 0, -(m_PPS.rcLegend.height() + legendBottomMargin) ); 
		}
	}

	// Calculate data rect
	if ( m_PPS.polarCoords ) {
		m_PPS.rcData = m_PPS.rcPlot;
	} else {
		m_PPS.rcData.setRect( m_PPS.rcPlot.left(), m_PPS.rcPlot.top() + m_PPS.siArrow.height()*2, m_PPS.rcPlot.width() - m_PPS.siArrow.height()*2, m_PPS.rcPlot.height() - m_PPS.siArrow.height()*2 );
	}
	checkDisplayOffset();

	// Get X axis scale and title
	m_iXScaleUnits = -1;
	if ( m_Layout == MDTRA_LAYOUT_TIME) 
		m_iXScaleUnits = m_pMainWindow->XScaleUnits();
	if ( m_iXScaleUnits < 0 || m_iXScaleUnits >= MDTRA_XSU_MAX )
		m_iXScaleUnits = 0;

	switch (m_Layout) {
	default:
	case MDTRA_LAYOUT_TIME:
		m_szXAxisTitle = QString::fromLocal8Bit(UTIL_GetXAxisTitle(m_iXScaleUnits,langId)); break;
	case MDTRA_LAYOUT_RESIDUE:
		m_szXAxisTitle = QString::fromLocal8Bit(szResidueAxisNames[langId]);
	}

	// Measure data
	m_PPS.dataMin.setX( 0 );
	m_PPS.dataMin.setY( FLT_MAX );
	m_PPS.dataMax.setX( -FLT_MAX );
	m_PPS.dataMax.setY( -FLT_MAX );
	m_PPS.xdataWidth = 0;

	for (int i = 0; i < m_PlotData.count(); i++) {
		const float *pDataPtr = m_PlotData.at(i).pDataRef->pData;
		int iDataSize = m_PlotData.at(i).pDataRef->iActualDataSize;
		if (!pDataPtr || !iDataSize)
			continue;

		for (int j = 0; j < iDataSize; j++) {
			if (pDataPtr[j] > m_PPS.dataMax.y()) m_PPS.dataMax.setY( pDataPtr[j] );
			if (pDataPtr[j] < m_PPS.dataMin.y()) m_PPS.dataMin.setY( pDataPtr[j] );
		}

		if (iDataSize > m_PPS.xdataWidth)
			m_PPS.xdataWidth = iDataSize;

		float flDataSize = (float)iDataSize * (m_PlotData.at(i).pDataRef->xscale * flXScaleUnit_Scale[m_iXScaleUnits] + flXScaleUnit_Bias[m_iXScaleUnits]);
		if (flDataSize > m_PPS.dataMax.x()) 
			m_PPS.dataMax.setX( flDataSize );
	}
	
	if (m_PPS.dataMin.y() > 0) m_PPS.dataMin.setY( 0 );
	if (m_PPS.dataMax.y() < 0) m_PPS.dataMax.setY( 0 );

	if ( m_PPS.dataMin.x() == m_PPS.dataMax.x() )
		m_PPS.dataMax.setX( m_PPS.dataMax.x() + 1 );
	if ( m_PPS.dataMin.y() == m_PPS.dataMax.y() )
		m_PPS.dataMax.setY( m_PPS.dataMax.y() + 1 );

	m_PPS.dataMin.setY( m_PPS.dataMin.y() * 1.1f );
	m_PPS.dataMax.setY( m_PPS.dataMax.y() * 1.1f );

	// Get the most suitable scaling factors
	// Half (well, a bit less, for precision reasons) of absolute maximum (either positive or negative) 
	// should not be greater than the scale checked
	float maxAbsValueY = 0.36f * MDTRA_MAX( fabsf( m_PPS.dataMin.y() ), fabsf( m_PPS.dataMax.y() ) );
	float maxAbsValueX = 0.4f * ( m_PPS.dataMax.x() / m_fDisplayScale );
	// scales:
	// ...0.01 0.02 0.05 0.1 0.2 0.5 1 5 10 20 50 100 200 500 1000 2000 5000 10000...
	float scale = 0.00001f;
	float acceptedScaleX = scale;
	float acceptedScaleY = scale;
	float scaleUp[3] = { 2.0f, 2.5f, 2.0f };
	int scaleCount = 0;
	while ( scale < 1000000 ) {
		if ( (scale >= maxAbsValueX) && (scale >= maxAbsValueY) )
			break;
		if ( scale < maxAbsValueX )
			acceptedScaleX = scale;
		if ( scale < maxAbsValueY )
			acceptedScaleY = scale;
		scale *= scaleUp[scaleCount % 3]; 
		scaleCount++;
	}
	m_PPS.scaleUnit.setX( acceptedScaleX );
	m_PPS.scaleUnit.setY( acceptedScaleY );

	// Setup data scales and biases
	m_PPS.dataScale.setX( m_PPS.rcData.width() * m_fDisplayScale / ( m_PPS.dataMax.x() - m_PPS.dataMin.x() ) );
	m_PPS.dataScale.setY( m_PPS.rcData.height() / ( m_PPS.dataMax.y() - m_PPS.dataMin.y() ) );
	m_PPS.dataBias.setX( m_fDisplayOffset );
	m_PPS.dataBias.setY( 0.0f );

	// Get X-axis Y origin
	m_PPS.yorigin = m_PPS.rcData.bottom() - m_PPS.rcData.height() * ( m_PPS.dataMin.y() / (m_PPS.dataMin.y() - m_PPS.dataMax.y()) );

	// Measure arrows and build arrow vertices
	m_PPS.vXArrow[0].setX( m_PPS.rcPlot.right() - m_PPS.siArrow.height() );
	m_PPS.vXArrow[0].setY( m_PPS.yorigin - m_PPS.siArrow.width() );
	m_PPS.vXArrow[1].setX( m_PPS.rcPlot.right() );
	m_PPS.vXArrow[1].setY( m_PPS.yorigin );
	m_PPS.vXArrow[2].setX( m_PPS.rcPlot.right() - m_PPS.siArrow.height() );
	m_PPS.vXArrow[2].setY( m_PPS.yorigin + m_PPS.siArrow.width() );
	m_PPS.vYArrow[0].setX( m_PPS.rcPlot.left() - m_PPS.siArrow.width() );
	m_PPS.vYArrow[0].setY( m_PPS.rcPlot.top() + m_PPS.siArrow.height() );
	m_PPS.vYArrow[1].setX( m_PPS.rcPlot.left() );
	m_PPS.vYArrow[1].setY( m_PPS.rcPlot.top() );
	m_PPS.vYArrow[2].setX( m_PPS.rcPlot.left() + m_PPS.siArrow.width() );
	m_PPS.vYArrow[2].setY( m_PPS.rcPlot.top() + m_PPS.siArrow.height() );

	// Update plot labels
	if (m_pMainWindow->plotShowLabels() && m_pLabels && m_pLabels->count() > 0) {
		for (int i = 0; i < m_pLabels->count(); i++) {
			MDTRA_Label *pLabel = const_cast<MDTRA_Label*>(&m_pLabels->at(i));
			QStringList labelLines = pLabel->text.split("\n");
			pLabel->wide = 0;
			for (int j = 0; j < labelLines.count(); j++) {
				int wide = fmNormal.width( labelLines.at(j) );
				if (wide > pLabel->wide) pLabel->wide = wide;
			}
			pLabel->wide += 8;
			pLabel->tall = (fmNormal.height() + 2) * labelLines.count() + 4;
		}
	}
}

void MDTRA_Plot :: renderSpecialString( int x, int y, const QString& str, const QFont& f, const QFontMetrics& fm, QPainter* pPainter ) 
{
	if (pPainter)
		pPainter->setFont( f );

	// Check for fast case (no special chars)
	if (!str.contains('^') && !str.contains('|') && !str.contains('°')) {
		if (pPainter) {
			pPainter->drawText( x, y, str );
		} else {
			renderText( x, y, str, f );
		}
		return;
	}

	QFont smallerFont( f );
	smallerFont.setPixelSize( f.pixelSize() * 0.75f );
	QFontMetrics fmSmallerFont( smallerFont );
	bool sup = false;
	bool sub = false;
	int stepsize = 0;
	int yoffset = 0;

	for (int i = 0; i < str.length(); i++) {
		QString ch( str.at(i) );
		if ( str.at(i) == '^' ) {
			if ( i < str.length()-1 ) {
				if ((str.at(i+1) >= '0' && str.at(i+1) <= '9') || (str.at(i+1) == '-') || (str.at(i+1) == '+') || (str.at(i+1) == 'o')) {
					sup = true;
					sub = false;
					continue;
				}
			}
		} else if ( str.at(i) == '|' ) {
			if ( i < str.length()-1 ) {
				if ((str.at(i+1) >= '0' && str.at(i+1) <= '9') || (str.at(i+1) == '-') || (str.at(i+1) == '+') || (str.at(i+1) == 'o')) {
					sup = false;
					sub = true;
					continue;
				}
			}
		} else if ( str.at(i) == '°' ) {
			stepsize-=2;
			x -= stepsize;
			yoffset-=5;
		} else if ( (sup || sub) && ( str.at(i) < '0' || str.at(i) > '9' ) 
								 && ( str.at(i) != '-' )
								 && ( str.at(i) != '+' )
								 && ( str.at(i) != 'o' )) {
			sup = false;
			sub = false;
		}

		if ( sup ) {
			if (pPainter) {
				pPainter->setFont( smallerFont );
				pPainter->drawText( x, y - (fm.height() * 0.3f) + yoffset, ch );
				pPainter->setFont( f );
			} else {
				renderText( x, y - (fm.height() * 0.3f) + yoffset, ch, smallerFont );
			}
			if ( ch.at(0) != '°' )
				stepsize = fmSmallerFont.width( ch );
		} else if ( sub ) {
			if (pPainter) {
				pPainter->setFont( smallerFont );
				pPainter->drawText( x, y + (fm.height() * 0.3f) + yoffset, ch );
				pPainter->setFont( f );
			} else {
				renderText( x, y + (fm.height() * 0.3f) + yoffset, ch, smallerFont );
			}
			if ( ch.at(0) != '°' )
				stepsize = fmSmallerFont.width( ch );
		} else {
			if (pPainter) {
				pPainter->drawText( x, y + yoffset, ch );
			} else {
				renderText( x, y + yoffset, ch, f );
			}
			if ( ch.at(0) != '°' )
				stepsize = fm.width( ch );
		}
		x += stepsize;
		yoffset = 0;
	}
}

float MDTRA_Plot :: sampleData( const float *pDataPtr, int iSample, int iMaxSample )
{
	if (!m_pMainWindow->plotDataFilter() || (m_pMainWindow->plotDataFilterSize() <= 1))
		return pDataPtr[iSample];

	// Apply CMA low-pass filter
	int sampleMin = iSample - (m_pMainWindow->plotDataFilterSize() >> 1);
	int sampleMax = iSample + (((m_pMainWindow->plotDataFilterSize()-1) >> 1) + 1);

	if ( sampleMin < 0 )
		sampleMin = 0;
	if ( sampleMax > iMaxSample )
		sampleMax = iMaxSample;

	float fSample = 0.0f;

	switch ( m_DataScaleUnits ) {
	case MDTRA_YSU_DEGREES:
		{
			const float sampleRef = pDataPtr[iSample];
			for ( int i = sampleMin; i < sampleMax; i++ ) {
				float sampleCurrent = pDataPtr[i];
				float sampleDelta = fabsf( sampleCurrent - sampleRef );
				if ( fabsf( sampleDelta - 360 ) < sampleDelta ) {
					if ( sampleCurrent > sampleRef )
						sampleCurrent -= 360;
					else
						sampleCurrent += 360;
				}
				fSample += sampleCurrent;
			}
		}
		break;
	case MDTRA_YSU_RADIANS:
		{
			const float sampleRef = pDataPtr[iSample];
			for ( int i = sampleMin; i < sampleMax; i++ ) {
				float sampleCurrent = pDataPtr[i];
				float sampleDelta = fabsf( sampleCurrent - sampleRef );
				if ( fabsf( sampleDelta - M_PI_F*2.0f ) < sampleDelta ) {
					if ( sampleCurrent > sampleRef )
						sampleCurrent -= M_PI_F*2.0f;
					else
						sampleCurrent += M_PI_F*2.0f;
				}
				fSample += sampleCurrent;
			}
		}
		break;
	default:
		{
			for ( int i = sampleMin; i < sampleMax; i++ )
				fSample += pDataPtr[i];
		}
		break;
	}

	return ( fSample / (float)(sampleMax - sampleMin) );
}


bool MDTRA_Plot :: renderPlotDataCartesian_OpenGL( void ) 
{
	// Init fonts
	QFontMetrics fmTitle( m_PPS.fntTitle );
	QFontMetrics fmNormal( m_PPS.fntNormal );
	QFontMetrics fmDigits( m_PPS.fntDigits );

	// Draw blended interval estimator areas
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
	for (int i = 0; i < m_PlotData.count(); i++) {
		if (!(m_PlotData.at(i).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;

		int iPE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_PE_SHIFT ) & DSREF_PE_MASK) - 1;
		if ( iPE >= 0 ) {
			float flPEValue = m_PlotData.at(i).pDataRef->stat[iPE];
			if ( flPEValue != -FLT_MAX ) {
				int iIE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IE_SHIFT ) & DSREF_IE_MASK) - 1;
				int iIS = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IS_SHIFT ) & DSREF_IS_MASK);
				if ( iIE >= 0 && iIS > 0 ) {
					float flIEValue = m_PlotData.at(i).pDataRef->stat[iIE] * (float)iIS;
					if ( flIEValue != -FLT_MAX ) {
						float yvalue1 = m_PPS.yorigin - (flPEValue + flIEValue) * m_PPS.dataScale.y() + m_PPS.dataBias.y();
						float yvalue2 = m_PPS.yorigin - (flPEValue - flIEValue) * m_PPS.dataScale.y() + m_PPS.dataBias.y();
						if ( yvalue1 < m_PPS.rcData.top() ) yvalue1 = m_PPS.rcData.top();
						if ( yvalue2 > m_PPS.rcData.bottom() ) yvalue2 = m_PPS.rcData.bottom();
	
						QColor clr = m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) );
						glColor4f( clr.redF(), clr.greenF(), clr.blueF(), 0.15f );
						glBegin(GL_QUADS);
							glVertex2f( m_PPS.rcData.left(), yvalue1 );
							glVertex2f( m_PPS.rcData.right(), yvalue1 );
							glVertex2f( m_PPS.rcData.right(), yvalue2 );
							glVertex2f( m_PPS.rcData.left(), yvalue2 );
						glEnd();
					}
				}
			}
		}
	}
	glDisable( GL_BLEND );

	// Draw data
	glLineStipple( 2, 0xE4E4 );
	for (int i = 0; i < m_PlotData.count(); i++) {
		if (!(m_PlotData.at(i).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;

		const float *pDataPtr = m_PlotData.at(i).pDataRef->pData;
		int iDataSize = m_PlotData.at(i).pDataRef->iActualDataSize;
		if (!pDataPtr || !iDataSize)
			continue;

		qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ) );
		if (m_pMainWindow->multisampleAA()) glEnable( GL_MULTISAMPLE );
		glBegin( GL_LINE_STRIP );
			for (int j = 0; j < iDataSize; j++) {
				int realj = j + m_PlotData.at(i).xOffset;
				float xvalue = realj * (m_PlotData.at(i).pDataRef->xscale * flXScaleUnit_Scale[m_iXScaleUnits] + flXScaleUnit_Bias[m_iXScaleUnits]);
				float xdataofs = xvalue * m_PPS.dataScale.x() + m_PPS.dataBias.x();
				if ( xdataofs < 0 ) continue;
				if ( xdataofs > m_PPS.rcData.width() ) break;
				glVertex2f( m_PPS.rcData.left() + xdataofs,
							m_PPS.yorigin - sampleData( pDataPtr, j, iDataSize ) * m_PPS.dataScale.y() + m_PPS.dataBias.y() );
			}
		glEnd();
		if (m_pMainWindow->multisampleAA())	glDisable( GL_MULTISAMPLE );

		// draw estimators
		int iPE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_PE_SHIFT ) & DSREF_PE_MASK) - 1;
		if ( iPE >= 0 ) {
			float flPEValue = m_PlotData.at(i).pDataRef->stat[iPE];
			if ( flPEValue != -FLT_MAX ) {
				float yvalue = m_PPS.yorigin - flPEValue * m_PPS.dataScale.y() + m_PPS.dataBias.y();
				if ( (yvalue >= m_PPS.rcData.top()) && (yvalue <= m_PPS.rcData.bottom()) ) {
					glBegin( GL_LINES );
						glVertex2f( m_PPS.rcData.left(), yvalue );
						glVertex2f( m_PPS.rcData.right(), yvalue );
					glEnd();
				}
				int iIE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IE_SHIFT ) & DSREF_IE_MASK) - 1;
				int iIS = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IS_SHIFT ) & DSREF_IS_MASK);
				if ( iIE >= 0 && iIS > 0 ) {
					float flIEValue = m_PlotData.at(i).pDataRef->stat[iIE] * (float)iIS;
					if ( flIEValue != -FLT_MAX ) {
						float yvalue1 = m_PPS.yorigin - (flPEValue + flIEValue) * m_PPS.dataScale.y() + m_PPS.dataBias.y();
						float yvalue2 = m_PPS.yorigin - (flPEValue - flIEValue) * m_PPS.dataScale.y() + m_PPS.dataBias.y();
						if ( (yvalue1 >= m_PPS.rcData.top()) || (yvalue2 <= m_PPS.rcData.bottom()) ) {
							glEnable( GL_LINE_STIPPLE );
							glBegin(GL_LINES);
								if ( yvalue1 >= m_PPS.rcData.top() ) {
									glVertex2f( m_PPS.rcData.left(), yvalue1 );
									glVertex2f( m_PPS.rcData.right(), yvalue1 );
								}
								if ( yvalue2 <= m_PPS.rcData.bottom() ) {
									glVertex2f( m_PPS.rcData.left(), yvalue2 );
									glVertex2f( m_PPS.rcData.right(), yvalue2 );
								}
							glEnd();
							glDisable( GL_LINE_STIPPLE );
						}
					}
				}
			}
		}
	}

	// Draw axes
	qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ) );
	glBegin(GL_LINES);
		glVertex2f( m_PPS.rcPlot.left(), m_PPS.yorigin );
		glVertex2f( m_PPS.rcPlot.right() - m_PPS.siArrow.height(), m_PPS.yorigin );
		glVertex2f( m_PPS.rcPlot.left(), m_PPS.rcPlot.top() + m_PPS.siArrow.height() );
		glVertex2f( m_PPS.rcPlot.left(), m_PPS.rcPlot.bottom() );
	glEnd();

	// Draw grid, if any
	if (m_pMainWindow->plotShowGrid()) {
		glLineStipple( 2, 0xCCCC );
		glEnable( GL_LINE_STIPPLE );
		glBegin( GL_LINES );
		// Positive Y-axis marks
		for ( float fpos = m_PPS.scaleUnit.y(); fpos <= m_PPS.dataMax.y(); fpos += m_PPS.scaleUnit.y() ) {
			int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y();
			glVertex2f( m_PPS.rcPlot.left(), ipos );
			glVertex2f( m_PPS.rcPlot.right() - m_PPS.siArrow.height(), ipos );
		}
		// Negative Y-axis marks
		for ( float fpos = -m_PPS.scaleUnit.y(); fpos >= m_PPS.dataMin.y(); fpos -= m_PPS.scaleUnit.y() ) {
			int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y();
			glVertex2f( m_PPS.rcPlot.left(), ipos );
			glVertex2f( m_PPS.rcPlot.right() - m_PPS.siArrow.height(), ipos );
		}
		glEnd();
		glDisable( GL_LINE_STIPPLE );
	}

	// Draw axis scale marks
	glBegin(GL_LINES);
	// Positive X-axis marks
	for ( float fpos = m_PPS.scaleUnit.x(); fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
		int ipos = fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x();
		if ( ipos < 0 ) continue;
		if ( ipos > m_PPS.rcData.width() ) break;
		glVertex2f( m_PPS.rcData.left() + ipos, m_PPS.yorigin - 3 );
		glVertex2f( m_PPS.rcData.left() + ipos, m_PPS.yorigin + 4 );
	}
	// Positive X-axis halfmarks
	for ( float fpos = m_PPS.scaleUnit.x()*0.5f; fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
		int ipos = fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x();
		if ( ipos < 0 ) continue;
		if ( ipos > m_PPS.rcData.width() ) break;
		glVertex2f( m_PPS.rcData.left() + ipos, m_PPS.yorigin - 2 );
		glVertex2f( m_PPS.rcData.left() + ipos, m_PPS.yorigin + 3 );
	}
	// Positive Y-axis marks
	for ( float fpos = 0; fpos <= m_PPS.dataMax.y(); fpos += m_PPS.scaleUnit.y() ) {
		int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y();
		glVertex2f( m_PPS.rcPlot.left() - 4, ipos );
		glVertex2f( m_PPS.rcPlot.left() + 3, ipos );
	}
	// Positive Y-axis halfmarks
	for ( float fpos = m_PPS.scaleUnit.y()*0.5f; fpos <= m_PPS.dataMax.y(); fpos += m_PPS.scaleUnit.y() ) {
		int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y();
		glVertex2f( m_PPS.rcPlot.left() - 3, ipos );
		glVertex2f( m_PPS.rcPlot.left() + 2, ipos );
	}
	// Negative Y-axis marks
	for ( float fpos = -m_PPS.scaleUnit.y(); fpos >= m_PPS.dataMin.y(); fpos -= m_PPS.scaleUnit.y() ) {
		int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y();
		glVertex2f( m_PPS.rcPlot.left() - 4, ipos );
		glVertex2f( m_PPS.rcPlot.left() + 3, ipos );
	}
	// Negative Y-axis half marks
	for ( float fpos = -m_PPS.scaleUnit.y()*0.5f; fpos >= m_PPS.dataMin.y(); fpos -= m_PPS.scaleUnit.y() ) {
		int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y();
		glVertex2f( m_PPS.rcPlot.left() - 3, ipos );
		glVertex2f( m_PPS.rcPlot.left() + 2, ipos );
	}
	glEnd();

	// Draw arrows
	if (m_pMainWindow->multisampleAA()) glEnable( GL_MULTISAMPLE );
	glBegin(GL_TRIANGLES);
		glVertex2f( m_PPS.vXArrow[0].x(), m_PPS.vXArrow[0].y() );
		glVertex2f( m_PPS.vXArrow[1].x(), m_PPS.vXArrow[1].y() );
		glVertex2f( m_PPS.vXArrow[2].x(), m_PPS.vXArrow[2].y() );
		glVertex2f( m_PPS.vYArrow[0].x(), m_PPS.vYArrow[0].y() );
		glVertex2f( m_PPS.vYArrow[1].x(), m_PPS.vYArrow[1].y() );
		glVertex2f( m_PPS.vYArrow[2].x(), m_PPS.vYArrow[2].y() );
	glEnd();
	if (m_pMainWindow->multisampleAA())	glDisable( GL_MULTISAMPLE );

	// Draw axis scale titles
	// Positive X-axis marks
	for ( float fpos = m_PPS.scaleUnit.x(); fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
		QString s = QString::number( fpos );
		int ipos = fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x() - (fmDigits.width(s)>>1);
		if ( ipos < 0 ) continue;
		if ( ipos > m_PPS.rcData.width() ) break;
		renderText( m_PPS.rcData.left() + ipos, m_PPS.yorigin + fmDigits.height() + 6, s, m_PPS.fntDigits );
	}
	// Positive Y-axis marks
	for ( float fpos = 0; fpos <= m_PPS.dataMax.y(); fpos += m_PPS.scaleUnit.y() ) {
		QString s = QString::number( fpos );
		int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y() + (fmDigits.height()>>1) - 2;
		renderText( (m_PPS.rcPlot.left() - fmDigits.width( s )) >> 1, ipos, s, m_PPS.fntDigits );
	}
	// Negative Y-axis marks
	for ( float fpos = -m_PPS.scaleUnit.y(); fpos >= m_PPS.dataMin.y(); fpos -= m_PPS.scaleUnit.y() ) {
		QString s = QString::number( fpos );
		int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y() + (fmDigits.height()>>1) - 2;
		renderText( (m_PPS.rcPlot.left() - fmDigits.width( s )) >> 1, ipos, s, m_PPS.fntDigits );
	}

	// Draw X axis title
	renderSpecialString( (m_iWidth - fmNormal.width( m_szXAxisTitle )) >> 1, m_PPS.rcPlot.bottom() + ((m_PPS.plotBottomMargin + fmNormal.height()) >> 1) + 6, m_szXAxisTitle, m_PPS.fntNormal, fmNormal );

	// Draw Y axis title
	renderSpecialString( m_PPS.rcPlot.left() >> 2, (m_PPS.rcPlot.top() >> 1) + fmTitle.height(), m_szYAxisTitle, m_PPS.fntNormal, fmNormal );

	// Draw plot labels
	if (m_pMainWindow->plotShowLabels() && m_pLabels && m_pLabels->count() > 0) {
		glLineStipple( 1, 0xAAAA );
		for (int i = 0; i < m_pLabels->count(); i++) {
			const MDTRA_Label *pLabel = &m_pLabels->at(i);
			if (!(m_PlotData.at(pLabel->sourceNum).pDataRef->flags & DSREF_FLAG_VISIBLE))
				continue;

			float* pDataPtr = m_PlotData.at(pLabel->sourceNum).pDataRef->pData;
			int iDataSize = m_PlotData.at(pLabel->sourceNum).pDataRef->iActualDataSize;
			if (!pDataPtr || !iDataSize)
				continue;

			QStringList labelLines = pLabel->text.split("\n");
			int labelLineHeight = fmNormal.height() + 2;

			int snapNum = (m_Layout != MDTRA_LAYOUT_TIME) ? (pLabel->snapshotNum-m_PlotData.at(pLabel->sourceNum).xOffset) : pLabel->snapshotNum;
			int realSnapNum = ( m_Layout == MDTRA_LAYOUT_TIME ) ? snapNum : (snapNum+m_PlotData.at(pLabel->sourceNum).xOffset);

			float xvalue = realSnapNum * (m_PlotData.at(pLabel->sourceNum).pDataRef->xscale * flXScaleUnit_Scale[m_iXScaleUnits] + flXScaleUnit_Bias[m_iXScaleUnits]);
			float yvalue = sampleData( pDataPtr, snapNum, iDataSize );

			int xpos = xvalue * m_PPS.dataScale.x() + m_PPS.dataBias.x();
			if ( xpos < 0 ) continue;
			if ( xpos > m_PPS.rcData.width() ) continue;
			xpos += m_PPS.rcData.left();

			int ypos = m_PPS.yorigin - yvalue * m_PPS.dataScale.y() + m_PPS.dataBias.y();

			QRect labelRect( pLabel->x, pLabel->y, pLabel->wide, pLabel->tall );

			// Draw vertical line
			if ( pLabel->flags & PLOTLABEL_FLAG_VERTLINE ) {
				if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
					qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
				else
					qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
				glBegin(GL_LINES);
					glVertex2f( xpos, m_PPS.yorigin );
					glVertex2f( xpos, ypos );
				glEnd();
			}

			// Draw background
			if (!(pLabel->flags & PLOTLABEL_FLAG_TRANSPARENT)) {
				qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BACKGROUND ) );
				glBegin(GL_QUADS);
					glVertex2f( labelRect.left(), labelRect.top() );
					glVertex2f( labelRect.right(), labelRect.top() );
					glVertex2f( labelRect.right(), labelRect.bottom() );
					glVertex2f( labelRect.left(), labelRect.bottom() );
				glEnd();
			}

			// Draw border
			if ( pLabel->flags & (PLOTLABEL_FLAG_BORDER|PLOTLABEL_FLAG_SELECTED) ) {
				if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
					qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
				else
					qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
				glBegin(GL_QUADS);
					glVertex2f( labelRect.left(), labelRect.top() );
					glVertex2f( labelRect.right(), labelRect.top() );
					glVertex2f( labelRect.right(), labelRect.bottom() );
					glVertex2f( labelRect.left(), labelRect.bottom() );
				glEnd();
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			}

			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED ) {
				qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
				glBegin(GL_QUADS);
					int iSelBulletX = pLabel->x;
					int iSelBulletY = pLabel->y;
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					
					iSelBulletX = pLabel->x + pLabel->wide - 1;
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					
					iSelBulletY = pLabel->y + pLabel->tall - 1;
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					
					iSelBulletX = pLabel->x;
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
				glEnd();
			}

			// Draw connector
			if ( pLabel->flags & PLOTLABEL_FLAG_CONNECTION ) {
				float nx, ny;
				if (abs(pLabel->x - xpos) < abs(pLabel->x + pLabel->wide - xpos)) 
					nx = pLabel->x;
				else
					nx = pLabel->x + pLabel->wide;

				if (abs(pLabel->y - ypos) < abs(pLabel->y + pLabel->tall - ypos)) 
					ny = pLabel->y;
				else
					ny = pLabel->y + pLabel->tall;

				if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
					qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
				else
					qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
				glEnable( GL_LINE_STIPPLE );
				glBegin(GL_LINES);
					glVertex2f( xpos, ypos );
					glVertex2f( nx, ny );
				glEnd();
				glDisable( GL_LINE_STIPPLE );
			}

			// Draw bullet
			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
				qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
			else
				qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (pLabel->sourceNum % NUM_DATA_COLORS) ) );
			glBegin(GL_QUADS);
				glVertex2f( xpos - m_PPS.labelBulletHalfSize, ypos );
				glVertex2f( xpos, ypos - m_PPS.labelBulletHalfSize );
				glVertex2f( xpos + m_PPS.labelBulletHalfSize, ypos );
				glVertex2f( xpos, ypos + m_PPS.labelBulletHalfSize );
			glEnd();

			// Draw text
			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
				qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
			else
				qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ) );

			int hpos = pLabel->y + labelLineHeight - 2;
			for (int j = 0; j < labelLines.count(); j++, hpos += labelLineHeight) {
				renderSpecialString( pLabel->x + 4, hpos, labelLines.at(j), m_PPS.fntNormal, fmNormal );
			}
		}
	}

	return true;
}

bool MDTRA_Plot :: renderPlotDataPolar_OpenGL( void ) 
{
	// Init fonts
	QFontMetrics fmTitle( m_PPS.fntTitle );
	QFontMetrics fmNormal( m_PPS.fntNormal );
	QFontMetrics fmDigits( m_PPS.fntDigits );

	int plotDataHalfWidth = m_PPS.rcData.width() >> 1;
	int plotCenterX = (m_PPS.rcPlot.left() + m_PPS.rcPlot.right()) >> 1;
	int plotCenterY = (m_PPS.rcPlot.top() + m_PPS.rcPlot.bottom()) >> 1;
	float plotCircleAngleStep = M_PI_F * 0.05f;

	// Draw blended interval estimator areas
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );
	for (int i = 0; i < m_PlotData.count(); i++) {
		if (!(m_PlotData.at(i).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;

		int iPE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_PE_SHIFT ) & DSREF_PE_MASK) - 1;
		if ( iPE >= 0 ) {
			float flPEValue = m_PlotData.at(i).pDataRef->stat[iPE];
			if ( flPEValue != -FLT_MAX ) {
				int iIE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IE_SHIFT ) & DSREF_IE_MASK) - 1;
				int iIS = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IS_SHIFT ) & DSREF_IS_MASK);
				if ( iIE >= 0 && iIS > 0 ) {
					float flIEValue = m_PlotData.at(i).pDataRef->stat[iIE] * (float)iIS;
					if ( flIEValue != -FLT_MAX ) {
						float yvalue1 = (flPEValue + flIEValue);
						float yvalue2 = (flPEValue - flIEValue);
						if ( m_DataScaleUnits == MDTRA_YSU_DEGREES ) {
							yvalue1 = UTIL_deg2rad( yvalue1 );
							yvalue2 = UTIL_deg2rad( yvalue2 );
						}
						if ( yvalue2 < yvalue1 ) {
							float temp = yvalue2;
							yvalue2 = yvalue1;
							yvalue1 = temp;
						}
						if ( yvalue2 - yvalue1 > 2.0f*M_PI_F )
							yvalue2 = yvalue1 + 2.0f*M_PI_F;
	
						QColor clr = m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) );
						glColor4f( clr.redF(), clr.greenF(), clr.blueF(), 0.15f );
						glBegin(GL_TRIANGLE_FAN);
							glVertex2f( plotCenterX, plotCenterY );
							for ( float k = yvalue1; k < yvalue2 + plotCircleAngleStep; k += plotCircleAngleStep ) {
								float k2 = ( k > yvalue2 ) ? yvalue2 : k;
								glVertex2f( plotCenterX + plotDataHalfWidth*cosf(k2), plotCenterY - plotDataHalfWidth*sinf(k2) );
							}
						glEnd();
					}
				}
			}
		}
	}
	glDisable( GL_BLEND );

	// Draw data
	glLineStipple( 2, 0xE4E4 );
	for (int i = 0; i < m_PlotData.count(); i++) {
		if (!(m_PlotData.at(i).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;

		const float *pDataPtr = m_PlotData.at(i).pDataRef->pData;
		int iDataSize = m_PlotData.at(i).pDataRef->iActualDataSize;
		if (!pDataPtr || !iDataSize)
			continue;

		qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ) );
		if (m_pMainWindow->multisampleAA()) glEnable( GL_MULTISAMPLE );
		glBegin( GL_LINE_STRIP );
			float prevangle = FLT_MIN;
			for (int j = 0; j < iDataSize; j++) {
				int realj = j + m_PlotData.at(i).xOffset;
				float xvalue = realj * (m_PlotData.at(i).pDataRef->xscale * flXScaleUnit_Scale[m_iXScaleUnits] + flXScaleUnit_Bias[m_iXScaleUnits]);
				float xdataofs = ( xvalue * m_PPS.dataScale.x() + m_PPS.dataBias.x() ) * 0.5f;
				if ( xdataofs < 0 ) continue;
				if ( xdataofs > plotDataHalfWidth ) break;

				float angle = sampleData( pDataPtr, j, iDataSize );
				if ( m_DataScaleUnits == MDTRA_YSU_DEGREES )
					angle = UTIL_deg2rad( angle );

				if ( prevangle == FLT_MIN )
					prevangle = angle;
				while ( fabsf( angle - prevangle ) > M_PI_F ) {
					if ( prevangle > angle )
						angle += M_PI_F * 2.0f;
					else
						angle -= M_PI_F * 2.0f;
				}
	
				if ( prevangle < angle ) {				
					for ( float k = prevangle; k < angle+plotCircleAngleStep; k += plotCircleAngleStep ) {
						float k2 = ( k > angle ) ? angle : k;
						glVertex2f( plotCenterX + xdataofs*cosf(k2), plotCenterY - xdataofs*sinf(k2) );
					}
				} else {
					for ( float k = prevangle; k > angle-plotCircleAngleStep; k -= plotCircleAngleStep ) {
						float k2 = ( k < angle ) ? angle : k;
						glVertex2f( plotCenterX + xdataofs*cosf(k2), plotCenterY - xdataofs*sinf(k2) );
					}
				}

				prevangle = angle;
			}
		glEnd();
		if (m_pMainWindow->multisampleAA())	glDisable( GL_MULTISAMPLE );

		// draw estimators
		int iPE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_PE_SHIFT ) & DSREF_PE_MASK) - 1;
		if ( iPE >= 0 ) {
			float flPEValue = m_PlotData.at(i).pDataRef->stat[iPE];
			if ( flPEValue != -FLT_MAX ) {
				float angle = flPEValue;
				if ( m_DataScaleUnits == MDTRA_YSU_DEGREES )
					angle = UTIL_deg2rad( angle );

				glBegin( GL_LINES );
					glVertex2f( plotCenterX, plotCenterY );
					glVertex2f( plotCenterX + plotDataHalfWidth*cosf(angle), plotCenterY - plotDataHalfWidth*sinf(angle) );
				glEnd();
				
				int iIE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IE_SHIFT ) & DSREF_IE_MASK) - 1;
				int iIS = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IS_SHIFT ) & DSREF_IS_MASK);
				if ( iIE >= 0 && iIS > 0 ) {
					float flIEValue = m_PlotData.at(i).pDataRef->stat[iIE] * (float)iIS;
					if ( flIEValue != -FLT_MAX ) {
						float yvalue1 = (flPEValue + flIEValue);
						float yvalue2 = (flPEValue - flIEValue);
						if ( m_DataScaleUnits == MDTRA_YSU_DEGREES ) {
							yvalue1 = UTIL_deg2rad( yvalue1 );
							yvalue2 = UTIL_deg2rad( yvalue2 );
						}
						glEnable( GL_LINE_STIPPLE );
						glBegin(GL_LINES);
							glVertex2f( plotCenterX, plotCenterY );
							glVertex2f( plotCenterX + plotDataHalfWidth*cosf(yvalue1), plotCenterY - plotDataHalfWidth*sinf(yvalue1) );
							glVertex2f( plotCenterX, plotCenterY );
							glVertex2f( plotCenterX + plotDataHalfWidth*cosf(yvalue2), plotCenterY - plotDataHalfWidth*sinf(yvalue2) );
						glEnd();
						glDisable( GL_LINE_STIPPLE );
					}
				}
			}
		}
	}

	// Draw axes
	qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ) );
	glBegin( GL_LINES );
		glVertex2f( m_PPS.rcPlot.left(), plotCenterY );
		glVertex2f( m_PPS.rcPlot.right(), plotCenterY );
		glVertex2f( plotCenterX, m_PPS.rcPlot.top() );
		glVertex2f( plotCenterX, m_PPS.rcPlot.bottom() );
	glEnd();

	// Draw grid, if any
	if (m_pMainWindow->plotShowGrid()) {
		glLineStipple( 2, 0xCCCC );
		glEnable( GL_LINE_STIPPLE );
		for ( float fpos = m_PPS.scaleUnit.x(); fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
			int ipos = (fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x()) * 0.5f;
			if ( ipos < 0 ) continue;
			if ( ipos > plotDataHalfWidth ) break;
			glBegin( GL_LINE_LOOP );
			for ( float angle = 0.0f; angle < 2.0f*M_PI_F; angle += plotCircleAngleStep )
				glVertex2f( plotCenterX + ipos*cosf(angle), plotCenterY - ipos*sinf(angle) );
			glEnd();
		}
		glDisable( GL_LINE_STIPPLE );
	}

	// Draw axis scale marks
	glBegin( GL_LINES );
	// marks
	for ( float fpos = m_PPS.scaleUnit.x(); fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
		int ipos = (fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x()) * 0.5f;
		if ( ipos < 0 ) continue;
		if ( ipos > plotDataHalfWidth ) break;
		glVertex2f( plotCenterX + ipos, plotCenterY - 3 );
		glVertex2f( plotCenterX + ipos, plotCenterY + 4 );
		glVertex2f( plotCenterX - ipos, plotCenterY - 3 );
		glVertex2f( plotCenterX - ipos, plotCenterY + 4 );
		glVertex2f( plotCenterX - 4, plotCenterY + ipos );
		glVertex2f( plotCenterX + 3, plotCenterY + ipos );
		glVertex2f( plotCenterX - 4, plotCenterY - ipos );
		glVertex2f( plotCenterX + 3, plotCenterY - ipos );
	}
	// halfmarks
	for ( float fpos = m_PPS.scaleUnit.x()*0.5f; fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
		int ipos = (fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x()) * 0.5f;
		if ( ipos < 0 ) continue;
		if ( ipos > plotDataHalfWidth ) break;
		glVertex2f( plotCenterX + ipos, plotCenterY - 2 );
		glVertex2f( plotCenterX + ipos, plotCenterY + 3 );
		glVertex2f( plotCenterX - ipos, plotCenterY - 2 );
		glVertex2f( plotCenterX - ipos, plotCenterY + 3 );
		glVertex2f( plotCenterX - 3, plotCenterY + ipos );
		glVertex2f( plotCenterX + 2, plotCenterY + ipos );
		glVertex2f( plotCenterX - 3, plotCenterY - ipos );
		glVertex2f( plotCenterX + 2, plotCenterY - ipos );
	}
	glEnd();

	// Draw axis scale titles
	for ( float fpos = m_PPS.scaleUnit.x(); fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
		QString s = QString::number( fpos );
		int swide = fmDigits.width(s);
		int shalfwide = swide >> 1;
		int shalftall = fmDigits.height() >> 1;
		int ipos = (fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x()) * 0.5f;
		if ( ipos < 0 ) continue;
		if ( ipos > plotDataHalfWidth ) break;

		qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BACKGROUND ) );
		glBegin( GL_QUADS );
			glVertex2f( plotCenterX + ipos - shalfwide - 2, plotCenterY + 10 );
			glVertex2f( plotCenterX + ipos - shalfwide - 2, plotCenterY + fmDigits.height() + 8 );
			glVertex2f( plotCenterX + ipos + shalfwide + 2, plotCenterY + fmDigits.height() + 8 );
			glVertex2f( plotCenterX + ipos + shalfwide + 2, plotCenterY + 10 );
			glVertex2f( plotCenterX - ipos - shalfwide - 2, plotCenterY + 10 );
			glVertex2f( plotCenterX - ipos - shalfwide - 2, plotCenterY + fmDigits.height() + 8 );
			glVertex2f( plotCenterX - ipos + shalfwide + 2, plotCenterY + fmDigits.height() + 8 );
			glVertex2f( plotCenterX - ipos + shalfwide + 2, plotCenterY + 10 );
			glVertex2f( plotCenterX - swide - 6, plotCenterY - ipos - shalftall + 2 );
			glVertex2f( plotCenterX - swide - 6, plotCenterY - ipos + shalftall + 2 );
			glVertex2f( plotCenterX - 2, plotCenterY - ipos + shalftall + 2 );
			glVertex2f( plotCenterX - 2, plotCenterY - ipos - shalftall + 2 );
			glVertex2f( plotCenterX - swide - 6, plotCenterY + ipos - shalftall + 2 );
			glVertex2f( plotCenterX - swide - 6, plotCenterY + ipos + shalftall + 2 );
			glVertex2f( plotCenterX - 2, plotCenterY + ipos + shalftall + 2 );
			glVertex2f( plotCenterX - 2, plotCenterY + ipos - shalftall + 2 );
		glEnd();

		qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ) );
		renderText( plotCenterX + ipos - shalfwide, plotCenterY + fmDigits.height() + 6, s, m_PPS.fntDigits );
		renderText( plotCenterX - ipos - shalfwide, plotCenterY + fmDigits.height() + 6, s, m_PPS.fntDigits );
		renderText( plotCenterX - swide - 4, plotCenterY + ipos + shalftall, s, m_PPS.fntDigits );
		renderText( plotCenterX - swide - 4, plotCenterY - ipos + shalftall, s, m_PPS.fntDigits );
	}
	
	// Draw X axis title
	renderSpecialString( (m_iWidth - fmNormal.width( m_szXAxisTitle )) >> 1, m_PPS.rcPlot.bottom() + fmDigits.height() + (fmNormal.height() >> 1) + 16, m_szXAxisTitle, m_PPS.fntNormal, fmNormal );

	// Draw angle titles
	if ( m_DataScaleUnits == MDTRA_YSU_DEGREES ) {
		renderSpecialString( m_PPS.rcPlot.right() + 8, plotCenterY + (fmDigits.height() >> 1), "0^o", m_PPS.fntDigits, fmDigits );
		renderSpecialString( m_PPS.rcPlot.left() - 8 - fmDigits.width( "180o" ), plotCenterY + (fmDigits.height() >> 1), "180^o", m_PPS.fntDigits, fmDigits );
		renderSpecialString( plotCenterX - (fmDigits.width( "90o" ) >> 1), m_PPS.rcPlot.top() - 8, "90^o", m_PPS.fntDigits, fmDigits );
		renderSpecialString( plotCenterX - (fmDigits.width( "270o" ) >> 1), m_PPS.rcPlot.bottom() + 8 + fmDigits.height(), "270^o", m_PPS.fntDigits, fmDigits );
	} else {
		renderSpecialString( m_PPS.rcPlot.right() + 8, plotCenterY + (fmDigits.height() >> 1), "0", m_PPS.fntDigits, fmDigits );
		renderSpecialString( m_PPS.rcPlot.left() - 8 - fmDigits.width( "pi" ), plotCenterY + (fmDigits.height() >> 1), "pi", m_PPS.fntDigits, fmDigits );
		renderSpecialString( plotCenterX - (fmDigits.width( "pi/2" ) >> 1), m_PPS.rcPlot.top() - 8, "pi/2", m_PPS.fntDigits, fmDigits );
		renderSpecialString( plotCenterX - (fmDigits.width( "3pi/2" ) >> 1), m_PPS.rcPlot.bottom() + 8 + fmDigits.height(), "3pi/2", m_PPS.fntDigits, fmDigits );
	}

	// Draw plot labels
	if (m_pMainWindow->plotShowLabels() && m_pLabels && m_pLabels->count() > 0) {
		glLineStipple( 1, 0xAAAA );
		for (int i = 0; i < m_pLabels->count(); i++) {
			const MDTRA_Label *pLabel = &m_pLabels->at(i);
			if (!(m_PlotData.at(pLabel->sourceNum).pDataRef->flags & DSREF_FLAG_VISIBLE))
				continue;

			float* pDataPtr = m_PlotData.at(pLabel->sourceNum).pDataRef->pData;
			int iDataSize = m_PlotData.at(pLabel->sourceNum).pDataRef->iActualDataSize;
			if (!pDataPtr || !iDataSize)
				continue;

			QStringList labelLines = pLabel->text.split("\n");
			int labelLineHeight = fmNormal.height() + 2;

			int snapNum = (m_Layout != MDTRA_LAYOUT_TIME) ? (pLabel->snapshotNum-1) : pLabel->snapshotNum;
			int realSnapNum = ( m_Layout == MDTRA_LAYOUT_TIME ) ? snapNum : (snapNum+m_PlotData.at(pLabel->sourceNum).xOffset);

			float xvalue = realSnapNum * (m_PlotData.at(pLabel->sourceNum).pDataRef->xscale * flXScaleUnit_Scale[m_iXScaleUnits] + flXScaleUnit_Bias[m_iXScaleUnits]);
			float yvalue = sampleData( pDataPtr, snapNum, iDataSize );
			if ( m_DataScaleUnits == MDTRA_YSU_DEGREES )
				yvalue = UTIL_deg2rad( yvalue );

			float xdataofs = ( xvalue * m_PPS.dataScale.x() + m_PPS.dataBias.x() ) * 0.5f;
			if ( xdataofs < 0 ) continue;
			if ( xdataofs > plotDataHalfWidth ) continue;
			
			int xpos = plotCenterX + xdataofs*cosf(yvalue);
			int ypos = plotCenterY - xdataofs*sinf(yvalue);

			QRect labelRect( pLabel->x, pLabel->y, pLabel->wide, pLabel->tall );

			// Draw vertical line
			// NON-SENSE FOR POLAR PLOTS!

			// Draw background
			if (!(pLabel->flags & PLOTLABEL_FLAG_TRANSPARENT)) {
				qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BACKGROUND ) );
				glBegin(GL_QUADS);
					glVertex2f( labelRect.left(), labelRect.top() );
					glVertex2f( labelRect.right(), labelRect.top() );
					glVertex2f( labelRect.right(), labelRect.bottom() );
					glVertex2f( labelRect.left(), labelRect.bottom() );
				glEnd();
			}

			// Draw border
			if ( pLabel->flags & (PLOTLABEL_FLAG_BORDER|PLOTLABEL_FLAG_SELECTED) ) {
				if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
					qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
				else
					qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
				glBegin(GL_QUADS);
					glVertex2f( labelRect.left(), labelRect.top() );
					glVertex2f( labelRect.right(), labelRect.top() );
					glVertex2f( labelRect.right(), labelRect.bottom() );
					glVertex2f( labelRect.left(), labelRect.bottom() );
				glEnd();
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			}

			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED ) {
				qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
				glBegin(GL_QUADS);
					int iSelBulletX = pLabel->x;
					int iSelBulletY = pLabel->y;
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					
					iSelBulletX = pLabel->x + pLabel->wide - 1;
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					
					iSelBulletY = pLabel->y + pLabel->tall - 1;
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					
					iSelBulletX = pLabel->x;
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
					glVertex2f( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize );
				glEnd();
			}

			// Draw connector
			if ( pLabel->flags & PLOTLABEL_FLAG_CONNECTION ) {
				float nx, ny;
				if (abs(pLabel->x - xpos) < abs(pLabel->x + pLabel->wide - xpos)) 
					nx = pLabel->x;
				else
					nx = pLabel->x + pLabel->wide;

				if (abs(pLabel->y - ypos) < abs(pLabel->y + pLabel->tall - ypos)) 
					ny = pLabel->y;
				else
					ny = pLabel->y + pLabel->tall;

				if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
					qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
				else
					qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
				glEnable( GL_LINE_STIPPLE );
				glBegin(GL_LINES);
					glVertex2f( xpos, ypos );
					glVertex2f( nx, ny );
				glEnd();
				glDisable( GL_LINE_STIPPLE );
			}

			// Draw bullet
			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
				qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
			else
				qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (pLabel->sourceNum % NUM_DATA_COLORS) ) );
			glBegin(GL_QUADS);
				glVertex2f( xpos - m_PPS.labelBulletHalfSize, ypos );
				glVertex2f( xpos, ypos - m_PPS.labelBulletHalfSize );
				glVertex2f( xpos + m_PPS.labelBulletHalfSize, ypos );
				glVertex2f( xpos, ypos + m_PPS.labelBulletHalfSize );
			glEnd();

			// Draw text
			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
				qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
			else
				qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ) );

			int hpos = pLabel->y + labelLineHeight - 2;
			for (int j = 0; j < labelLines.count(); j++, hpos += labelLineHeight) {
				renderSpecialString( pLabel->x + 4, hpos, labelLines.at(j), m_PPS.fntNormal, fmNormal );
			}
		}
	}

	return true;
}

bool MDTRA_Plot :: renderPlot_OpenGL( void ) 
{
	// Init fonts
	QFontMetrics fmTitle( m_PPS.fntTitle );
	QFontMetrics fmNormal( m_PPS.fntNormal );

	if (m_bDisabled) {
		// Draw disabled message and exit
		qglClearColor( Qt::gray );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		qglColor( Qt::white );
		renderText( (m_iWidth - fmTitle.width( m_szDisabledMsg )) * 0.5f, (m_iHeight - fmTitle.height()) * 0.5f, m_szDisabledMsg, m_PPS.fntTitle );
		return true;
	}

	// Clear
	qglClearColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BACKGROUND ) );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Draw border
	qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
	glBegin(GL_LINE_LOOP);
		glVertex2f( 1, 0 );
		glVertex2f( m_iWidth, 0 );
		glVertex2f( m_iWidth, m_iHeight-1 );
		glVertex2f( 0, m_iHeight-1 );
	glEnd();

	// Draw title
	qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ) );
	renderSpecialString( (m_iWidth - fmTitle.width( m_szTitle )) >> 1, fmTitle.height() + 6, m_szTitle, m_PPS.fntTitle, fmTitle );

	if ( m_PPS.polarCoords ) {
		if (!renderPlotDataPolar_OpenGL())
			return false;
	} else {
		if (!renderPlotDataCartesian_OpenGL())
			return false;
	}

	// Draw legend, if any
	if ( m_PPS.rcLegend.isValid() ) {
		qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
		glBegin(GL_LINE_LOOP);
			glVertex2f( m_PPS.rcLegend.left(), m_PPS.rcLegend.top() );
			glVertex2f( m_PPS.rcLegend.right(), m_PPS.rcLegend.top() );
			glVertex2f( m_PPS.rcLegend.right(), m_PPS.rcLegend.bottom() );
			glVertex2f( m_PPS.rcLegend.left()-1, m_PPS.rcLegend.bottom() );
		glEnd();

		int row = 0;
		int col = 0;

		for (int i = 0; i < m_PlotData.count(); i++) {
			if (!(m_PlotData.at(i).pDataRef->flags & DSREF_FLAG_VISIBLE))
				continue;

			int bulletPosX = m_PPS.rcLegend.left() + (col ? m_PPS.legendMidX : 0) + m_PPS.legendXMargin;
			int bulletPosY = m_PPS.rcLegend.top() + m_PPS.legendYMargin + (m_PPS.siBullet.height() + m_PPS.legendRowSpacing) * row;

			qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ) );
			glBegin(GL_QUADS);
				glVertex2f( bulletPosX, bulletPosY );
				glVertex2f( bulletPosX + m_PPS.siBullet.width(), bulletPosY );
				glVertex2f( bulletPosX + m_PPS.siBullet.width(), bulletPosY + m_PPS.siBullet.height() );
				glVertex2f( bulletPosX, bulletPosY + m_PPS.siBullet.height() );
			glEnd();

			qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
			glBegin(GL_LINE_LOOP);
				glVertex2f( bulletPosX, bulletPosY );
				glVertex2f( bulletPosX + m_PPS.siBullet.width(), bulletPosY );
				glVertex2f( bulletPosX + m_PPS.siBullet.width(), bulletPosY + m_PPS.siBullet.height() );
				glVertex2f( bulletPosX-1, bulletPosY + m_PPS.siBullet.height() );
			glEnd();

			qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ) );
			renderSpecialString(bulletPosX + m_PPS.legendXMargin + m_PPS.siBullet.width(),
								bulletPosY + (m_PPS.siBullet.height()>>1) + (fmNormal.height()>>2) + 2,
								m_PlotData.at(i).title, m_PPS.fntNormal, fmNormal );

			col++;
			if ( col >= m_PPS.legendNumCols ) {
				col = 0;
				row++;
			}
		}
	}

	//OpenGL-specific rendering
	if (m_pMainWindow->plotMouseTrack()) {
		if ( m_iMouseX >= 0 && m_iMouseY >= 0 ) {
			qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ) );
			glLineStipple( 1, 0xCCCC );
			glEnable( GL_LINE_STIPPLE );
			glBegin(GL_LINES);
				glVertex2f( 0, m_iMouseY );
				glVertex2f( m_iWidth, m_iMouseY );
				glVertex2f( m_iMouseX, 0 );
				glVertex2f( m_iMouseX, m_iHeight );
			glEnd();
			glDisable( GL_LINE_STIPPLE );

			qglColor( Qt::red );
			glBegin(GL_QUADS);
				glVertex2f( m_iMouseX-3, m_iMouseY-3 );
				glVertex2f( m_iMouseX-3, m_iMouseY+3 );
				glVertex2f( m_iMouseX+3, m_iMouseY+3 );
				glVertex2f( m_iMouseX+3, m_iMouseY-3 );
			glEnd();
		}
	}

	if (m_bSelectionStart) {
		qglColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
		glLineStipple( 2, 0xCCCC );
		glEnable( GL_LINE_STIPPLE );
		glBegin(GL_LINE_LOOP);
			glVertex2f( m_SelectionRect.topLeft().x() - 1, m_SelectionRect.topLeft().y() );
			glVertex2f( m_SelectionRect.topRight().x(), m_SelectionRect.topRight().y() );
			glVertex2f( m_SelectionRect.bottomRight().x(), m_SelectionRect.bottomRight().y() );
			glVertex2f( m_SelectionRect.bottomLeft().x(), m_SelectionRect.bottomLeft().y() );
		glEnd();
		glDisable( GL_LINE_STIPPLE );
	}
	
	return true;
}

bool MDTRA_Plot :: renderPlotDataCartesian_Generic( QPainter* pPainter ) 
{
#ifdef MDTRA_ALLOW_PRINTER
	// Init fonts
	QFontMetrics fmTitle( m_PPS.fntTitle );
	QFontMetrics fmNormal( m_PPS.fntNormal );
	QFontMetrics fmDigits( m_PPS.fntDigits );

	// Init rects, brushes, pens, etc.
	QBrush estimatorBrush( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ) );
	QBrush axisBrush( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ) );
	QPen estimatorPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ), 1, Qt::CustomDashLine, Qt::SquareCap, Qt::MiterJoin );
	QPen dataPen( Qt::black, 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin );
	QPen borderPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );
	QPen axisPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );
	QPen gridPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ), 1, Qt::CustomDashLine, Qt::SquareCap, Qt::MiterJoin );
	QPen textPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );

	QVector<qreal> dashes;
	qreal dash = 4;
	qreal space = 6;
	dashes << dash << space << dash << space;
	gridPen.setDashPattern(dashes);
	dashes.clear();
	dash = 4;
	space = 6;
	dashes << dash << space << 1 << space;
	estimatorPen.setDashPattern(dashes);

	// Draw blended interval estimator areas
	for (int i = 0; i < m_PlotData.count(); i++) {
		if (!(m_PlotData.at(i).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;

		int iPE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_PE_SHIFT ) & DSREF_PE_MASK) - 1;
		if ( iPE >= 0 ) {
			float flPEValue = m_PlotData.at(i).pDataRef->stat[iPE];
			if ( flPEValue != -FLT_MAX ) {
				int iIE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IE_SHIFT ) & DSREF_IE_MASK) - 1;
				int iIS = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IS_SHIFT ) & DSREF_IS_MASK);
				if ( iIE >= 0 && iIS > 0 ) {
					float flIEValue = m_PlotData.at(i).pDataRef->stat[iIE] * (float)iIS;
					if ( flIEValue != -FLT_MAX ) {
						float yvalue1 = m_PPS.yorigin - (flPEValue + flIEValue) * m_PPS.dataScale.y() + m_PPS.dataBias.y();
						float yvalue2 = m_PPS.yorigin - (flPEValue - flIEValue) * m_PPS.dataScale.y() + m_PPS.dataBias.y();
						if ( yvalue1 < m_PPS.rcData.top() ) yvalue1 = m_PPS.rcData.top();
						if ( yvalue2 > m_PPS.rcData.bottom() ) yvalue2 = m_PPS.rcData.bottom();
						QRect rcEstimator( m_PPS.rcData.left(), yvalue1, m_PPS.rcData.width(), yvalue2-yvalue1 );
	
						QColor clr( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ) );
						clr.setAlphaF( 0.15f );
						estimatorBrush.setColor( clr );
						pPainter->fillRect( rcEstimator, estimatorBrush );
					}
				}
			}
		}
	}

	// Draw data
	for (int i = 0; i < m_PlotData.count(); i++) {
		if (!(m_PlotData.at(i).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;

		const float *pDataPtr = m_PlotData.at(i).pDataRef->pData;
		int iDataSize = m_PlotData.at(i).pDataRef->iActualDataSize;
		if (!pDataPtr || !iDataSize)
			continue;

		QPainterPath path;
		bool pathStarted = false;
		for (int j = 0; j < iDataSize; j++) {
			int realj = j + m_PlotData.at(i).xOffset;
			float xvalue = realj * (m_PlotData.at(i).pDataRef->xscale * flXScaleUnit_Scale[m_iXScaleUnits] + flXScaleUnit_Bias[m_iXScaleUnits]);
			float xdataofs = xvalue * m_PPS.dataScale.x() + m_PPS.dataBias.x();
			if ( xdataofs < 0 ) continue;
			if ( xdataofs > m_PPS.rcData.width() ) break;
			if ( pathStarted ) {
				path.lineTo( m_PPS.rcData.left() + xdataofs,
							 m_PPS.yorigin - sampleData( pDataPtr, j, iDataSize ) * m_PPS.dataScale.y() + m_PPS.dataBias.y() );
			} else {
				pathStarted = true;
				path.moveTo( m_PPS.rcData.left() + xdataofs, 
							 m_PPS.yorigin - sampleData( pDataPtr, j, iDataSize ) * m_PPS.dataScale.y() + m_PPS.dataBias.y() );
			}
		}

		dataPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ) );
		pPainter->setPen( dataPen );
		pPainter->drawPath( path );

		// draw estimators
		int iPE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_PE_SHIFT ) & DSREF_PE_MASK) - 1;
		if ( iPE >= 0 ) {
			float flPEValue = m_PlotData.at(i).pDataRef->stat[iPE];
			if ( flPEValue != -FLT_MAX ) {
				float yvalue = m_PPS.yorigin - flPEValue * m_PPS.dataScale.y() + m_PPS.dataBias.y();
				if ( (yvalue >= m_PPS.rcData.top()) && (yvalue <= m_PPS.rcData.bottom()) )
					pPainter->drawLine( m_PPS.rcData.left(), yvalue, m_PPS.rcData.right(), yvalue );

				int iIE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IE_SHIFT ) & DSREF_IE_MASK) - 1;
				int iIS = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IS_SHIFT ) & DSREF_IS_MASK);
				if ( iIE >= 0 && iIS > 0 ) {
					float flIEValue = m_PlotData.at(i).pDataRef->stat[iIE] * (float)iIS;
					if ( flIEValue != -FLT_MAX ) {
						float yvalue1 = m_PPS.yorigin - (flPEValue + flIEValue) * m_PPS.dataScale.y() + m_PPS.dataBias.y();
						float yvalue2 = m_PPS.yorigin - (flPEValue - flIEValue) * m_PPS.dataScale.y() + m_PPS.dataBias.y();
						if ( (yvalue1 >= m_PPS.rcData.top()) || (yvalue2 <= m_PPS.rcData.bottom()) ) {
							estimatorPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ) );
							pPainter->setPen( estimatorPen );
							if ( yvalue1 >= m_PPS.rcData.top() )
								pPainter->drawLine( m_PPS.rcData.left(), yvalue1, m_PPS.rcData.right(), yvalue1 );
							if ( yvalue2 <= m_PPS.rcData.bottom() )
								pPainter->drawLine( m_PPS.rcData.left(), yvalue2, m_PPS.rcData.right(), yvalue2 );
						}
					}
				}
			}
		}
	}

	// Draw axes
	pPainter->setPen( axisPen );
	pPainter->drawLine( m_PPS.rcPlot.left(), m_PPS.yorigin,
						m_PPS.rcPlot.right() - m_PPS.siArrow.height(),	m_PPS.yorigin );
	pPainter->drawLine( m_PPS.rcPlot.left(), m_PPS.rcPlot.top() + m_PPS.siArrow.height(),
						m_PPS.rcPlot.left(), m_PPS.rcPlot.bottom() );

	// Draw grid, if any
	if (m_pMainWindow->plotShowGrid()) {
		pPainter->setPen( gridPen );
		// Positive Y-axis marks
		for ( float fpos = m_PPS.scaleUnit.y(); fpos <= m_PPS.dataMax.y(); fpos += m_PPS.scaleUnit.y() ) {
			int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y();
			pPainter->drawLine( m_PPS.rcPlot.left() - 1, ipos, m_PPS.rcPlot.right() - m_PPS.siArrow.height(), ipos );
		}
		// Negative Y-axis marks
		for ( float fpos = -m_PPS.scaleUnit.y(); fpos >= m_PPS.dataMin.y(); fpos -= m_PPS.scaleUnit.y() ) {
			int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y();
			pPainter->drawLine( m_PPS.rcPlot.left() - 1, ipos, m_PPS.rcPlot.right() - m_PPS.siArrow.height(), ipos );
		}
	}

	// Draw axis scale marks
	pPainter->setPen( axisPen );
	// Positive X-axis marks
	for ( float fpos = m_PPS.scaleUnit.x(); fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
		int ipos = fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x();
		if ( ipos < 0 ) continue;
		if ( ipos > m_PPS.rcData.width() ) break;
		pPainter->drawLine( m_PPS.rcData.left() + ipos, m_PPS.yorigin - 3, m_PPS.rcData.left() + ipos, m_PPS.yorigin + 3 );
	}
	// Positive X-axis halfmarks
	for ( float fpos = m_PPS.scaleUnit.x()*0.5f; fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
		int ipos = fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x();
		if ( ipos < 0 ) continue;
		if ( ipos > m_PPS.rcData.width() ) break;
		pPainter->drawLine( m_PPS.rcData.left() + ipos, m_PPS.yorigin - 2, m_PPS.rcData.left() + ipos, m_PPS.yorigin + 2 );
	}
	// Positive Y-axis marks
	for ( float fpos = 0; fpos <= m_PPS.dataMax.y(); fpos += m_PPS.scaleUnit.y() ) {
		int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y();
		pPainter->drawLine( m_PPS.rcPlot.left() - 3, ipos, m_PPS.rcPlot.left() + 3, ipos );
	}
	// Positive Y-axis halfmarks
	for ( float fpos = m_PPS.scaleUnit.y()*0.5f; fpos <= m_PPS.dataMax.y(); fpos += m_PPS.scaleUnit.y() ) {
		int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y();
		pPainter->drawLine( m_PPS.rcPlot.left() - 2, ipos, m_PPS.rcPlot.left() + 2, ipos );
	}
	// Negative Y-axis marks
	for ( float fpos = -m_PPS.scaleUnit.y(); fpos >= m_PPS.dataMin.y(); fpos -= m_PPS.scaleUnit.y() ) {
		int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y();
		pPainter->drawLine( m_PPS.rcPlot.left() - 3, ipos, m_PPS.rcPlot.left() + 3, ipos );
	}
	// Negative Y-axis half marks
	for ( float fpos = -m_PPS.scaleUnit.y()*0.5f; fpos >= m_PPS.dataMin.y(); fpos -= m_PPS.scaleUnit.y() ) {
		int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y();
		pPainter->drawLine( m_PPS.rcPlot.left() - 2, ipos, m_PPS.rcPlot.left() + 2, ipos );
	}

	// Draw arrows
	pPainter->setBrush( axisBrush );
	pPainter->drawPolygon( m_PPS.vXArrow, 3 );
	pPainter->drawPolygon( m_PPS.vYArrow, 3 );

	// Draw axis scale titles
	pPainter->setPen( textPen );
	pPainter->setFont( m_PPS.fntDigits );
	// Positive X-axis marks
	for ( float fpos = m_PPS.scaleUnit.x(); fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
		QString s = QString::number( fpos );
		int ipos = fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x() - (fmDigits.width(s)>>1);
		if ( ipos < 0 ) continue;
		if ( ipos > m_PPS.rcData.width() ) break;
		pPainter->drawText( m_PPS.rcData.left() + ipos, m_PPS.yorigin + fmDigits.height() + 6, s );
	}
	// Positive Y-axis marks
	for ( float fpos = 0; fpos <= m_PPS.dataMax.y(); fpos += m_PPS.scaleUnit.y() ) {
		QString s = QString::number( fpos );
		int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y() + (fmDigits.height()>>1) - 2;
		pPainter->drawText( (m_PPS.rcPlot.left() - fmDigits.width( s )) >> 1, ipos, s );
	}
	// Negative Y-axis marks
	for ( float fpos = -m_PPS.scaleUnit.y(); fpos >= m_PPS.dataMin.y(); fpos -= m_PPS.scaleUnit.y() ) {
		QString s = QString::number( fpos );
		int ipos = m_PPS.yorigin - fpos * m_PPS.dataScale.y() + m_PPS.dataBias.y() + (fmDigits.height()>>1) - 2;
		pPainter->drawText( (m_PPS.rcPlot.left() - fmDigits.width( s )) >> 1, ipos, s );
	}

	// Draw X axis title
	renderSpecialString( (m_iWidth - fmNormal.width( m_szXAxisTitle )) >> 1, m_PPS.rcPlot.bottom() + ((m_PPS.plotBottomMargin + fmNormal.height()) >> 1) + 6, m_szXAxisTitle, m_PPS.fntNormal, fmNormal, pPainter );

	// Draw Y axis title
	renderSpecialString( m_PPS.rcPlot.left() >> 2, (m_PPS.rcPlot.top() >> 1) + fmTitle.height(), m_szYAxisTitle, m_PPS.fntNormal, fmNormal, pPainter );

	// Draw plot labels
	if (m_pMainWindow->plotShowLabels() && m_pLabels && m_pLabels->count() > 0) {
		QBrush labelBulletBrush( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ) );
		QPen labelConnectorPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ), 1, Qt::DotLine, Qt::SquareCap, Qt::MiterJoin );

		for (int i = 0; i < m_pLabels->count(); i++) {
			const MDTRA_Label *pLabel = &m_pLabels->at(i);
			if (!(m_PlotData.at(pLabel->sourceNum).pDataRef->flags & DSREF_FLAG_VISIBLE))
				continue;

			float* pDataPtr = m_PlotData.at(pLabel->sourceNum).pDataRef->pData;
			int iDataSize = m_PlotData.at(pLabel->sourceNum).pDataRef->iActualDataSize;
			if (!pDataPtr || !iDataSize)
				continue;

			QStringList labelLines = pLabel->text.split("\n");
			int labelLineHeight = fmNormal.height() + 2;

			int snapNum = (m_Layout != MDTRA_LAYOUT_TIME) ? (pLabel->snapshotNum-1) : pLabel->snapshotNum;
			int realSnapNum = ( m_Layout == MDTRA_LAYOUT_TIME ) ? snapNum : (snapNum+m_PlotData.at(pLabel->sourceNum).xOffset);

			float xvalue = realSnapNum * (m_PlotData.at(pLabel->sourceNum).pDataRef->xscale * flXScaleUnit_Scale[m_iXScaleUnits] + flXScaleUnit_Bias[m_iXScaleUnits]);
			float yvalue = sampleData( pDataPtr, snapNum, iDataSize );

			int xpos = xvalue * m_PPS.dataScale.x() + m_PPS.dataBias.x();
			if ( xpos < 0 ) continue;
			if ( xpos > m_PPS.rcData.width() ) continue;
			xpos += m_PPS.rcData.left();
			
			int ypos = m_PPS.yorigin - yvalue * m_PPS.dataScale.y() + m_PPS.dataBias.y();

			QRect labelRect( pLabel->x, pLabel->y, pLabel->wide, pLabel->tall );

			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
				borderPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
			else
				borderPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );

			// Draw vertical line
			if ( pLabel->flags & PLOTLABEL_FLAG_VERTLINE ) {
				pPainter->setBrush( Qt::NoBrush );
				pPainter->setPen( borderPen );
				pPainter->drawLine( xpos, m_PPS.yorigin, xpos, ypos );
			}

			// Draw background
			if (!(pLabel->flags & PLOTLABEL_FLAG_TRANSPARENT))
				pPainter->fillRect( labelRect, m_pMainWindow->getColorManager()->color( COLOR_PLOT_BACKGROUND ) );

			// Draw border
			if ( pLabel->flags & (PLOTLABEL_FLAG_BORDER|PLOTLABEL_FLAG_SELECTED) ) {
				pPainter->setBrush( Qt::NoBrush );
				pPainter->setPen( borderPen );
				pPainter->drawRect( labelRect );
			}

			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED ) {
				labelBulletBrush.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
				pPainter->setBrush( labelBulletBrush );

				QPolygon poly( 4 );
				int iSelBulletX = pLabel->x;
				int iSelBulletY = pLabel->y;
				poly.clear();
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				pPainter->drawPolygon( poly );
			
				iSelBulletX = pLabel->x + pLabel->wide;
				poly.clear();
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				pPainter->drawPolygon( poly );
					
				iSelBulletY = pLabel->y + pLabel->tall;
				poly.clear();
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				pPainter->drawPolygon( poly );
					
				iSelBulletX = pLabel->x;
				poly.clear();
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				pPainter->drawPolygon( poly );
			}

			// Draw connector
			if ( pLabel->flags & PLOTLABEL_FLAG_CONNECTION ) {
				float nx, ny;
				if (abs(pLabel->x - xpos) < abs(pLabel->x + pLabel->wide - xpos)) 
					nx = pLabel->x;
				else
					nx = pLabel->x + pLabel->wide;

				if (abs(pLabel->y - ypos) < abs(pLabel->y + pLabel->tall - ypos)) 
					ny = pLabel->y;
				else
					ny = pLabel->y + pLabel->tall;

				if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
					labelConnectorPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
				else
					labelConnectorPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
				
				pPainter->setBrush( Qt::NoBrush );
				pPainter->setPen( labelConnectorPen );
				pPainter->drawLine( xpos, ypos, nx, ny );
			}

			// Draw bullet
			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
				labelBulletBrush.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
			else
				labelBulletBrush.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (pLabel->sourceNum % NUM_DATA_COLORS) ) );
			pPainter->setBrush( labelBulletBrush );
			pPainter->setPen( Qt::NoPen );
			QPolygon poly( 4 );
			poly.clear();
			poly.append( QPoint( xpos - m_PPS.labelBulletHalfSize, ypos ) );
			poly.append( QPoint( xpos, ypos - m_PPS.labelBulletHalfSize ) );
			poly.append( QPoint( xpos + m_PPS.labelBulletHalfSize, ypos ) );
			poly.append( QPoint( xpos, ypos + m_PPS.labelBulletHalfSize ) );
			pPainter->drawPolygon( poly );

			// Draw text
			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
				borderPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
			else
				borderPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ) );
			pPainter->setPen( borderPen );
			int hpos = pLabel->y + labelLineHeight - 2;
			for (int j = 0; j < labelLines.count(); j++, hpos += labelLineHeight) {
				renderSpecialString( pLabel->x + 4, hpos, labelLines.at(j), m_PPS.fntNormal, fmNormal, pPainter );
			}
		}
	}
#endif //MDTRA_ALLOW_PRINTER
	return true;
}

bool MDTRA_Plot :: renderPlotDataPolar_Generic( QPainter* pPainter ) 
{
#ifdef MDTRA_ALLOW_PRINTER
	// Init fonts
	QFontMetrics fmTitle( m_PPS.fntTitle );
	QFontMetrics fmNormal( m_PPS.fntNormal );
	QFontMetrics fmDigits( m_PPS.fntDigits );

	// Init rects, brushes, pens, etc.
	QBrush backgroundBrush( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BACKGROUND ) );
	QBrush estimatorBrush( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ) );
	QBrush axisBrush( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ) );
	QPen estimatorPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ), 1, Qt::CustomDashLine, Qt::SquareCap, Qt::MiterJoin );
	QPen dataPen( Qt::black, 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin );
	QPen borderPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );
	QPen axisPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );
	QPen gridPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_AXES ), 1, Qt::CustomDashLine, Qt::SquareCap, Qt::MiterJoin );
	QPen textPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );

	QVector<qreal> dashes;
	qreal dash = 4;
	qreal space = 6;
	dashes << dash << space << dash << space;
	gridPen.setDashPattern(dashes);
	dashes.clear();
	dash = 4;
	space = 6;
	dashes << dash << space << 1 << space;
	estimatorPen.setDashPattern(dashes);

	int plotDataHalfWidth = m_PPS.rcData.width() >> 1;
	int plotCenterX = (m_PPS.rcPlot.left() + m_PPS.rcPlot.right()) >> 1;
	int plotCenterY = (m_PPS.rcPlot.top() + m_PPS.rcPlot.bottom()) >> 1;
	float plotCircleAngleStep = M_PI_F * 0.05f;

	// Draw blended interval estimator areas
	pPainter->setPen( Qt::NoPen );
	for (int i = 0; i < m_PlotData.count(); i++) {
		if (!(m_PlotData.at(i).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;

		int iPE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_PE_SHIFT ) & DSREF_PE_MASK) - 1;
		if ( iPE >= 0 ) {
			float flPEValue = m_PlotData.at(i).pDataRef->stat[iPE];
			if ( flPEValue != -FLT_MAX ) {
				int iIE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IE_SHIFT ) & DSREF_IE_MASK) - 1;
				int iIS = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IS_SHIFT ) & DSREF_IS_MASK);
				if ( iIE >= 0 && iIS > 0 ) {
					float flIEValue = m_PlotData.at(i).pDataRef->stat[iIE] * (float)iIS;
					if ( flIEValue != -FLT_MAX ) {
						float yvalue1 = (flPEValue + flIEValue);
						float yvalue2 = (flPEValue - flIEValue);
						if ( m_DataScaleUnits == MDTRA_YSU_DEGREES ) {
							yvalue1 = UTIL_deg2rad( yvalue1 );
							yvalue2 = UTIL_deg2rad( yvalue2 );
						}
						if ( yvalue2 < yvalue1 ) {
							float temp = yvalue2;
							yvalue2 = yvalue1;
							yvalue1 = temp;
						}
						if ( yvalue2 - yvalue1 > 2.0f*M_PI_F )
							yvalue2 = yvalue1 + 2.0f*M_PI_F;

						QColor clr( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ) );
						clr.setAlphaF( 0.15f );
						estimatorBrush.setColor( clr );
						pPainter->setBrush( estimatorBrush );
						pPainter->drawPie( m_PPS.rcData.left(), m_PPS.rcData.top(), m_PPS.rcData.width()-1, m_PPS.rcData.height()-1, UTIL_rad2deg( yvalue1 ) * 16, UTIL_rad2deg( yvalue2-yvalue1 ) * 16 );
					}
				}
			}
		}
	}

	pPainter->setBrush( Qt::NoBrush );

	// Draw data
	for (int i = 0; i < m_PlotData.count(); i++) {
		if (!(m_PlotData.at(i).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;

		const float *pDataPtr = m_PlotData.at(i).pDataRef->pData;
		int iDataSize = m_PlotData.at(i).pDataRef->iActualDataSize;
		if (!pDataPtr || !iDataSize)
			continue;

		QPainterPath path;
		bool pathStarted = false;
		float prevangle = FLT_MIN;
		for (int j = 0; j < iDataSize; j++) {
			int realj = j + m_PlotData.at(i).xOffset;
			float xvalue = realj * (m_PlotData.at(i).pDataRef->xscale * flXScaleUnit_Scale[m_iXScaleUnits] + flXScaleUnit_Bias[m_iXScaleUnits]);
			float xdataofs = ( xvalue * m_PPS.dataScale.x() + m_PPS.dataBias.x() ) * 0.5f;
			if ( xdataofs < 0 ) continue;
			if ( xdataofs > plotDataHalfWidth ) break;

			float angle = sampleData( pDataPtr, j, iDataSize );
			if ( m_DataScaleUnits == MDTRA_YSU_DEGREES )
				angle = UTIL_deg2rad( angle );

			if ( prevangle == FLT_MIN )
				prevangle = angle;
			while ( fabsf( angle - prevangle ) > M_PI_F ) {
				if ( prevangle > angle )
					angle += M_PI_F * 2.0f;
				else
					angle -= M_PI_F * 2.0f;
			}
	
			if ( prevangle < angle ) {				
				for ( float k = prevangle; k < angle+plotCircleAngleStep; k += plotCircleAngleStep ) {
					float k2 = ( k > angle ) ? angle : k;
					if ( pathStarted ) {
						path.lineTo( plotCenterX + xdataofs*cosf(k2), plotCenterY - xdataofs*sinf(k2) );
					} else {
						pathStarted = true;
						path.moveTo( plotCenterX + xdataofs*cosf(k2), plotCenterY - xdataofs*sinf(k2) );
					}			
				}
			} else {
				for ( float k = prevangle; k > angle-plotCircleAngleStep; k -= plotCircleAngleStep ) {
					float k2 = ( k < angle ) ? angle : k;
					if ( pathStarted ) {
						path.lineTo( plotCenterX + xdataofs*cosf(k2), plotCenterY - xdataofs*sinf(k2) );
					} else {
						pathStarted = true;
						path.moveTo( plotCenterX + xdataofs*cosf(k2), plotCenterY - xdataofs*sinf(k2) );
					}
				}
			}

			prevangle = angle;
		}

		dataPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ) );
		pPainter->setPen( dataPen );
		pPainter->drawPath( path );

		// draw estimators
		int iPE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_PE_SHIFT ) & DSREF_PE_MASK) - 1;
		if ( iPE >= 0 ) {
			float flPEValue = m_PlotData.at(i).pDataRef->stat[iPE];
			if ( flPEValue != -FLT_MAX ) {
				float angle = flPEValue;
				if ( m_DataScaleUnits == MDTRA_YSU_DEGREES )
					angle = UTIL_deg2rad( angle );

				pPainter->drawLine( plotCenterX, plotCenterY, plotCenterX + plotDataHalfWidth*cosf(angle), plotCenterY - plotDataHalfWidth*sinf(angle) );
				
				int iIE = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IE_SHIFT ) & DSREF_IE_MASK) - 1;
				int iIS = (( m_PlotData.at(i).pDataRef->flags >> DSREF_IS_SHIFT ) & DSREF_IS_MASK);
				if ( iIE >= 0 && iIS > 0 ) {
					float flIEValue = m_PlotData.at(i).pDataRef->stat[iIE] * (float)iIS;
					if ( flIEValue != -FLT_MAX ) {
						float yvalue1 = (flPEValue + flIEValue);
						float yvalue2 = (flPEValue - flIEValue);
						if ( m_DataScaleUnits == MDTRA_YSU_DEGREES ) {
							yvalue1 = UTIL_deg2rad( yvalue1 );
							yvalue2 = UTIL_deg2rad( yvalue2 );
						}

						estimatorPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ) );
						pPainter->setPen( estimatorPen );
						pPainter->drawLine( plotCenterX, plotCenterY, plotCenterX + 1 + plotDataHalfWidth*cosf(yvalue1), plotCenterY + 1 - plotDataHalfWidth*sinf(yvalue1) );
						pPainter->drawLine( plotCenterX, plotCenterY, plotCenterX + 1 + plotDataHalfWidth*cosf(yvalue2), plotCenterY + 1 - plotDataHalfWidth*sinf(yvalue2) );
					}
				}
			}
		}
	}

	// Draw axes
	pPainter->setPen( axisPen );
	pPainter->drawLine( m_PPS.rcPlot.left(), plotCenterY, m_PPS.rcPlot.right(), plotCenterY );
	pPainter->drawLine( plotCenterX, m_PPS.rcPlot.top(), plotCenterX, m_PPS.rcPlot.bottom() );

	// Draw grid, if any
	if (m_pMainWindow->plotShowGrid()) {
		pPainter->setPen( gridPen );
		for ( float fpos = m_PPS.scaleUnit.x(); fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
			int ipos = (fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x()) * 0.5f;
			if ( ipos < 0 ) continue;
			if ( ipos > plotDataHalfWidth ) break;
			pPainter->drawEllipse( plotCenterX-ipos, plotCenterY-ipos, ipos*2, ipos*2 );
		}
	}

	// Draw axis scale marks
	pPainter->setPen( axisPen );
	// marks
	for ( float fpos = m_PPS.scaleUnit.x(); fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
		int ipos = (fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x()) * 0.5f;
		if ( ipos < 0 ) continue;
		if ( ipos > plotDataHalfWidth ) break;
		pPainter->drawLine( plotCenterX + ipos, plotCenterY - 3, plotCenterX + ipos, plotCenterY + 3 );
		pPainter->drawLine( plotCenterX - ipos, plotCenterY - 3, plotCenterX - ipos, plotCenterY + 3 );
		pPainter->drawLine( plotCenterX - 3, plotCenterY + ipos, plotCenterX + 3, plotCenterY + ipos );
		pPainter->drawLine( plotCenterX - 3, plotCenterY - ipos, plotCenterX + 3, plotCenterY - ipos );
	}
	// halfmarks
	for ( float fpos = m_PPS.scaleUnit.x()*0.5f; fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
		int ipos = (fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x()) * 0.5f;
		if ( ipos < 0 ) continue;
		if ( ipos > plotDataHalfWidth ) break;
		pPainter->drawLine( plotCenterX + ipos, plotCenterY - 2, plotCenterX + ipos, plotCenterY + 2 );
		pPainter->drawLine( plotCenterX - ipos, plotCenterY - 2, plotCenterX - ipos, plotCenterY + 2 );
		pPainter->drawLine( plotCenterX - 2, plotCenterY + ipos, plotCenterX + 2, plotCenterY + ipos );
		pPainter->drawLine( plotCenterX - 2, plotCenterY - ipos, plotCenterX + 2, plotCenterY - ipos );
	}

	// Draw axis scale titles
	pPainter->setBackground( backgroundBrush );
	pPainter->setBackgroundMode( Qt::OpaqueMode );
	pPainter->setPen( textPen );
	pPainter->setFont( m_PPS.fntDigits );
	for ( float fpos = m_PPS.scaleUnit.x(); fpos <= m_PPS.dataMax.x(); fpos += m_PPS.scaleUnit.x() ) {
		QString s = QString::number( fpos );
		int swide = fmDigits.width(s);
		int shalfwide = swide >> 1;
		int shalftall = fmDigits.height() >> 1;
		int ipos = (fpos * m_PPS.dataScale.x() + m_PPS.dataBias.x()) * 0.5f;
		if ( ipos < 0 ) continue;
		if ( ipos > plotDataHalfWidth ) break;

		pPainter->drawText( plotCenterX + ipos - shalfwide, plotCenterY + fmDigits.height() + 6, s );
		pPainter->drawText( plotCenterX - ipos - shalfwide, plotCenterY + fmDigits.height() + 6, s );
		pPainter->drawText( plotCenterX - swide - 4, plotCenterY + ipos + shalftall, s );
		pPainter->drawText( plotCenterX - swide - 4, plotCenterY - ipos + shalftall, s );
	}
	pPainter->setBackgroundMode( Qt::TransparentMode );
	
	// Draw X axis title
	renderSpecialString( (m_iWidth - fmNormal.width( m_szXAxisTitle )) >> 1, m_PPS.rcPlot.bottom() + fmDigits.height() + (fmNormal.height() >> 1) + 16, m_szXAxisTitle, m_PPS.fntNormal, fmNormal, pPainter );

	// Draw angle titles
	if ( m_DataScaleUnits == MDTRA_YSU_DEGREES ) {
		renderSpecialString( m_PPS.rcPlot.right() + 8, plotCenterY + (fmDigits.height() >> 1), "0^o", m_PPS.fntDigits, fmDigits, pPainter );
		renderSpecialString( m_PPS.rcPlot.left() - 8 - fmDigits.width( "180o" ), plotCenterY + (fmDigits.height() >> 1), "180^o", m_PPS.fntDigits, fmDigits, pPainter );
		renderSpecialString( plotCenterX - (fmDigits.width( "90o" ) >> 1), m_PPS.rcPlot.top() - 8, "90^o", m_PPS.fntDigits, fmDigits, pPainter );
		renderSpecialString( plotCenterX - (fmDigits.width( "270o" ) >> 1), m_PPS.rcPlot.bottom() + 8 + fmDigits.height(), "270^o", m_PPS.fntDigits, fmDigits, pPainter );
	} else {
		renderSpecialString( m_PPS.rcPlot.right() + 8, plotCenterY + (fmDigits.height() >> 1), "0", m_PPS.fntDigits, fmDigits, pPainter );
		renderSpecialString( m_PPS.rcPlot.left() - 8 - fmDigits.width( "pi" ), plotCenterY + (fmDigits.height() >> 1), "pi", m_PPS.fntDigits, fmDigits, pPainter );
		renderSpecialString( plotCenterX - (fmDigits.width( "pi/2" ) >> 1), m_PPS.rcPlot.top() - 8, "pi/2", m_PPS.fntDigits, fmDigits, pPainter );
		renderSpecialString( plotCenterX - (fmDigits.width( "3pi/2" ) >> 1), m_PPS.rcPlot.bottom() + 8 + fmDigits.height(), "3pi/2", m_PPS.fntDigits, fmDigits, pPainter );
	}


	// Draw plot labels
	if (m_pMainWindow->plotShowLabels() && m_pLabels && m_pLabels->count() > 0) {
		QBrush labelBulletBrush( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ) );
		QPen labelConnectorPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ), 1, Qt::DotLine, Qt::SquareCap, Qt::MiterJoin );

		for (int i = 0; i < m_pLabels->count(); i++) {
			const MDTRA_Label *pLabel = &m_pLabels->at(i);
			if (!(m_PlotData.at(pLabel->sourceNum).pDataRef->flags & DSREF_FLAG_VISIBLE))
				continue;

			float* pDataPtr = m_PlotData.at(pLabel->sourceNum).pDataRef->pData;
			int iDataSize = m_PlotData.at(pLabel->sourceNum).pDataRef->iActualDataSize;
			if (!pDataPtr || !iDataSize)
				continue;

			QStringList labelLines = pLabel->text.split("\n");
			int labelLineHeight = fmNormal.height() + 2;

			int snapNum = (m_Layout != MDTRA_LAYOUT_TIME) ? (pLabel->snapshotNum-1) : pLabel->snapshotNum;
			int realSnapNum = ( m_Layout == MDTRA_LAYOUT_TIME ) ? snapNum : (snapNum+m_PlotData.at(pLabel->sourceNum).xOffset);

			float xvalue = realSnapNum * (m_PlotData.at(pLabel->sourceNum).pDataRef->xscale * flXScaleUnit_Scale[m_iXScaleUnits] + flXScaleUnit_Bias[m_iXScaleUnits]);
			float yvalue = sampleData( pDataPtr, snapNum, iDataSize );
			if ( m_DataScaleUnits == MDTRA_YSU_DEGREES )
				yvalue = UTIL_deg2rad( yvalue );

			float xdataofs = ( xvalue * m_PPS.dataScale.x() + m_PPS.dataBias.x() ) * 0.5f;
			if ( xdataofs < 0 ) continue;
			if ( xdataofs > plotDataHalfWidth ) continue;

			int xpos = plotCenterX + xdataofs*cosf(yvalue);
			int ypos = plotCenterY - xdataofs*sinf(yvalue);

			QRect labelRect( pLabel->x, pLabel->y, pLabel->wide, pLabel->tall );

			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
				borderPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
			else
				borderPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );

			// Draw vertical line
			// NON-SENSE FOR POLAR PLOTS!

			// Draw background
			if (!(pLabel->flags & PLOTLABEL_FLAG_TRANSPARENT))
				pPainter->fillRect( labelRect, m_pMainWindow->getColorManager()->color( COLOR_PLOT_BACKGROUND ) );

			// Draw border
			if ( pLabel->flags & (PLOTLABEL_FLAG_BORDER|PLOTLABEL_FLAG_SELECTED) ) {
				pPainter->setBrush( Qt::NoBrush );
				pPainter->setPen( borderPen );
				pPainter->drawRect( labelRect );
			}

			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED ) {
				labelBulletBrush.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
				pPainter->setBrush( labelBulletBrush );

				QPolygon poly( 4 );
				int iSelBulletX = pLabel->x;
				int iSelBulletY = pLabel->y;
				poly.clear();
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				pPainter->drawPolygon( poly );
			
				iSelBulletX = pLabel->x + pLabel->wide;
				poly.clear();
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				pPainter->drawPolygon( poly );
					
				iSelBulletY = pLabel->y + pLabel->tall;
				poly.clear();
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				pPainter->drawPolygon( poly );
					
				iSelBulletX = pLabel->x;
				poly.clear();
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY - m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX + m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				poly.append( QPoint( iSelBulletX - m_PPS.labelSelHalfSize, iSelBulletY + m_PPS.labelSelHalfSize ) );
				pPainter->drawPolygon( poly );
			}

			// Draw connector
			if ( pLabel->flags & PLOTLABEL_FLAG_CONNECTION ) {
				float nx, ny;
				if (abs(pLabel->x - xpos) < abs(pLabel->x + pLabel->wide - xpos)) 
					nx = pLabel->x;
				else
					nx = pLabel->x + pLabel->wide;

				if (abs(pLabel->y - ypos) < abs(pLabel->y + pLabel->tall - ypos)) 
					ny = pLabel->y;
				else
					ny = pLabel->y + pLabel->tall;

				if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
					labelConnectorPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
				else
					labelConnectorPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ) );
				
				pPainter->setBrush( Qt::NoBrush );
				pPainter->setPen( labelConnectorPen );
				pPainter->drawLine( xpos, ypos, nx, ny );
			}

			// Draw bullet
			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
				labelBulletBrush.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
			else
				labelBulletBrush.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (pLabel->sourceNum % NUM_DATA_COLORS) ) );
			pPainter->setBrush( labelBulletBrush );
			pPainter->setPen( Qt::NoPen );
			QPolygon poly( 4 );
			poly.clear();
			poly.append( QPoint( xpos - m_PPS.labelBulletHalfSize, ypos ) );
			poly.append( QPoint( xpos, ypos - m_PPS.labelBulletHalfSize ) );
			poly.append( QPoint( xpos + m_PPS.labelBulletHalfSize, ypos ) );
			poly.append( QPoint( xpos, ypos + m_PPS.labelBulletHalfSize ) );
			pPainter->drawPolygon( poly );

			// Draw text
			if ( pLabel->flags & PLOTLABEL_FLAG_SELECTED )
				borderPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_SELECTION ) );
			else
				borderPen.setColor( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ) );
			pPainter->setPen( borderPen );
			int hpos = pLabel->y + labelLineHeight - 2;
			for (int j = 0; j < labelLines.count(); j++, hpos += labelLineHeight) {
				renderSpecialString( pLabel->x + 4, hpos, labelLines.at(j), m_PPS.fntNormal, fmNormal, pPainter );
			}
		}
	}
#endif //MDTRA_ALLOW_PRINTER
	return true;
}

bool MDTRA_Plot :: renderPlot_Generic( QPainter* pPainter ) 
{
#ifdef MDTRA_ALLOW_PRINTER
	// Init fonts
	QFontMetrics fmTitle( m_PPS.fntTitle );
	QFontMetrics fmNormal( m_PPS.fntNormal );

	// Init rects, brushes, pens, etc.
	QRect wholeRect( 0, 0, m_iWidth, m_iHeight );
	QPen borderPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_BORDER ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );
	QPen textPen( m_pMainWindow->getColorManager()->color( COLOR_PLOT_TEXT ), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin );

	// Clear
	pPainter->fillRect( wholeRect, m_pMainWindow->getColorManager()->color( COLOR_PLOT_BACKGROUND ) );

	// Draw border
	pPainter->setBrush( Qt::NoBrush );
	pPainter->setPen( borderPen );
	pPainter->drawRect( wholeRect );

	// Draw title
	pPainter->setPen( textPen );
	renderSpecialString( (m_iWidth - fmTitle.width( m_szTitle )) >> 1, fmTitle.height() + 6, m_szTitle, m_PPS.fntTitle, fmTitle, pPainter );

	if ( m_PPS.polarCoords ) {
		if (!renderPlotDataPolar_Generic( pPainter ))
			return false;
	} else {
		if (!renderPlotDataCartesian_Generic( pPainter ))
			return false;
	}

	// Draw legend, if any
	if ( m_PPS.rcLegend.isValid() ) {
		pPainter->setBrush( Qt::NoBrush );
		pPainter->setPen( borderPen );
		pPainter->drawRect( m_PPS.rcLegend );

		int row = 0;
		int col = 0;

		for (int i = 0; i < m_PlotData.count(); i++) {
			if (!(m_PlotData.at(i).pDataRef->flags & DSREF_FLAG_VISIBLE))
				continue;

			int bulletPosX = m_PPS.rcLegend.left() + (col ? m_PPS.legendMidX : 0) + m_PPS.legendXMargin;
			int bulletPosY = m_PPS.rcLegend.top() + m_PPS.legendYMargin + (m_PPS.siBullet.height() + m_PPS.legendRowSpacing) * row;
			QRect bulletRect( bulletPosX, bulletPosY, m_PPS.siBullet.width(), m_PPS.siBullet.height() );

			pPainter->fillRect( bulletRect, m_pMainWindow->getColorManager()->color( COLOR_PLOT_DATA1 + (i % NUM_DATA_COLORS) ) );

			pPainter->setBrush( Qt::NoBrush );
			pPainter->setPen( borderPen );
			pPainter->drawRect( bulletRect );
		
			pPainter->setPen( textPen );
			renderSpecialString(bulletPosX + m_PPS.legendXMargin + m_PPS.siBullet.width(),
								bulletPosY + (m_PPS.siBullet.height()>>1) + (fmNormal.height()>>2) + 2,
								m_PlotData.at(i).title, m_PPS.fntNormal, fmNormal, pPainter );

			col++;
			if ( col >= m_PPS.legendNumCols ) {
				col = 0;
				row++;
			}
		}
	}

#endif
	return true;
}

void MDTRA_Plot :: paintGL( void ) 
{
	// Set viewport
	glViewport( 0, 0, m_iWidth, m_iHeight );

	// Switch to orthogonal projection and setup near/far
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, m_iWidth, m_iHeight, 0, -1.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Fill paintstruct and render
	setupPlotPaintStruct();
	renderPlot_OpenGL();
}

void MDTRA_Plot :: deselectLabels( void )
{
	if ( !m_pLabels || m_pLabels->count() <= 0 )
		return;

	for ( int i = 0; i < m_pLabels->count(); i++ ) {
		MDTRA_Label *pLabel = const_cast<MDTRA_Label*>(&m_pLabels->at(i));
		pLabel->flags &= ~PLOTLABEL_FLAG_SELECTED;
	}
}

void MDTRA_Plot :: selectLabelsWithin( const QRect &selectionRect, bool addToSel, bool invertSel )
{
	if ( !m_pMainWindow->plotShowLabels() || !m_pLabels || m_pLabels->count() <= 0 )
		return;

	for ( int i = 0; i < m_pLabels->count(); i++ ) {
		MDTRA_Label *pLabel = const_cast<MDTRA_Label*>(&m_pLabels->at(i));
		if (!(m_PlotData.at(pLabel->sourceNum).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;

		QRect labelRect = QRect( QPoint( pLabel->x, pLabel->y ), QSize( pLabel->wide, pLabel->tall ) );
		if (labelRect.intersects( selectionRect )) {
			if (invertSel)
				pLabel->flags ^= PLOTLABEL_FLAG_SELECTED;
			else
				pLabel->flags |= PLOTLABEL_FLAG_SELECTED;
		}
		else if (!addToSel && !invertSel)
			pLabel->flags &= ~PLOTLABEL_FLAG_SELECTED;
	}
}

bool MDTRA_Plot :: isWithinSelection( const QPoint &selectionPoint )
{
	if ( !m_pMainWindow->plotShowLabels() || !m_pLabels || m_pLabels->count() <= 0 )
		return false;

	for ( int i = 0; i < m_pLabels->count(); i++ ) {
		MDTRA_Label *pLabel = const_cast<MDTRA_Label*>(&m_pLabels->at(i));
		if (!(m_PlotData.at(pLabel->sourceNum).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;
		if (!(pLabel->flags & PLOTLABEL_FLAG_SELECTED))
			continue;

		QRect labelRect = QRect( QPoint( pLabel->x, pLabel->y ), QSize( pLabel->wide, pLabel->tall ) );
		if (labelRect.contains( selectionPoint ))
			return true;
	}

	return false;
}

int MDTRA_Plot :: fetchSelectedLabel( void )
{
	if ( !m_pMainWindow->plotShowLabels() || !m_pLabels || m_pLabels->count() <= 0 )
		return -1;

	for ( int i = 0; i < m_pLabels->count(); i++ ) {
		MDTRA_Label *pLabel = const_cast<MDTRA_Label*>(&m_pLabels->at(i));
		if (!(m_PlotData.at(pLabel->sourceNum).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;
		if (!(pLabel->flags & PLOTLABEL_FLAG_SELECTED))
			continue;

		return i;
	}

	return -1;
}

int MDTRA_Plot :: countSelection( void )
{
	if ( !m_pMainWindow->plotShowLabels() || !m_pLabels || m_pLabels->count() <= 0 )
		return 0;

	int c = 0;

	for ( int i = 0; i < m_pLabels->count(); i++ ) {
		MDTRA_Label *pLabel = const_cast<MDTRA_Label*>(&m_pLabels->at(i));
		if (!(m_PlotData.at(pLabel->sourceNum).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;
		if (!(pLabel->flags & PLOTLABEL_FLAG_SELECTED))
			continue;

		c++;
	}

	return c;
}

void MDTRA_Plot :: dragSelectedLabels( const QPoint &newp )
{
	if ( !m_pMainWindow->plotShowLabels() || !m_pLabels || m_pLabels->count() <= 0 )
		return;

	int ofsX = newp.x() - m_SelectionStartPos.x();
	int ofsY = newp.y() - m_SelectionStartPos.y();
	int numdrag = 0;
	int fixX = 0;
	int fixY = 0;
	int fixX2 = m_iWidth;
	int fixY2 = m_iHeight;

	for ( int i = 0; i < m_pLabels->count(); i++ ) {
		MDTRA_Label *pLabel = const_cast<MDTRA_Label*>(&m_pLabels->at(i));
		if (!(m_PlotData.at(pLabel->sourceNum).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;
		if (!(pLabel->flags & PLOTLABEL_FLAG_SELECTED))
			continue;

		pLabel->x += ofsX;
		if (pLabel->x < fixX) fixX = pLabel->x;
		if (pLabel->x + pLabel->wide > fixX2) fixX2 = pLabel->x + pLabel->wide;

		pLabel->y += ofsY;
		if (pLabel->y < fixY) fixY = pLabel->y;
		if (pLabel->y + pLabel->tall > fixY2) fixY2 = pLabel->y + pLabel->tall;

		numdrag++;
	}

	if (fixX != 0 || fixY != 0 || fixX2 != m_iWidth || fixY2 != m_iHeight) {
		for ( int i = 0; i < m_pLabels->count(); i++ ) {
			MDTRA_Label *pLabel = const_cast<MDTRA_Label*>(&m_pLabels->at(i));
			if (!(m_PlotData.at(pLabel->sourceNum).pDataRef->flags & DSREF_FLAG_VISIBLE))
				continue;
			if (!(pLabel->flags & PLOTLABEL_FLAG_SELECTED))
				continue;

			pLabel->x -= fixX;
			pLabel->x -= (fixX2 - m_iWidth);

			pLabel->y -= fixY;
			pLabel->y -= (fixY2 - m_iHeight);
		}
	}

	if (numdrag)
		updateGL();
}

void MDTRA_Plot :: updateStatusBarXY( int mx, int my )
{
	float xRemap, yRemap;

	if ( m_PPS.polarCoords ) {
		int plotCenterX = (m_PPS.rcPlot.left() + m_PPS.rcPlot.right()) >> 1;
		int plotCenterY = (m_PPS.rcPlot.top() + m_PPS.rcPlot.bottom()) >> 1;

		float r = 2.0f * sqrtf((float)((mx - plotCenterX)*(mx - plotCenterX) + (my - plotCenterY)*(my - plotCenterY)));
		if ( r > m_PPS.rcData.width() ) {
			m_pMainWindow->updateStatusBarPlotXY( -1, 0 );
			return;
		}

		xRemap = (float)(r - m_fDisplayOffset) / (float)(m_PPS.rcData.width() * m_fDisplayScale);
		yRemap = atan2f( plotCenterY - my, mx - plotCenterX );
		if ( yRemap < 0 ) yRemap += M_PI_F*2.0f;
		if ( m_DataScaleUnits == MDTRA_YSU_DEGREES ) yRemap = UTIL_rad2deg( yRemap );

	} else {
		if ( mx < m_PPS.rcData.left() ||
			 mx > m_PPS.rcData.right() ||
			 my < m_PPS.rcData.top() ||
			 my > m_PPS.rcData.bottom() ) {
			m_pMainWindow->updateStatusBarPlotXY( -1, 0 );
			return;
		}

		xRemap = (mx - m_PPS.rcData.left() - m_fDisplayOffset) / (float)(m_PPS.rcData.width() * m_fDisplayScale);
		yRemap = (m_PPS.yorigin - my) / (float)(m_PPS.rcData.height());
		yRemap *= (m_PPS.dataMax.y()-m_PPS.dataMin.y());
	}

	if ( xRemap > 1.0f ) xRemap = 1.0f;
	xRemap *= (m_PPS.dataMax.x()-m_PPS.dataMin.x());

	//Update status bar coords mapped to the plot
	m_pMainWindow->updateStatusBarPlotXY( xRemap, yRemap );
}

void MDTRA_Plot :: mouseMoveEvent( QMouseEvent* pe )
{
	if (m_bDisabled) {
		m_pMainWindow->updateStatusBarPlotXY( -1, 0 );
		if (m_iMouseX >= 0 || m_iMouseY >= 0) {
			m_iMouseX = -1;
			m_iMouseY = -1;
			if (m_pMainWindow->plotMouseTrack()) updateGL();
		}
		return;
	}

	if (m_bSelectionStart)
		updateSelectionRect( pe->pos() );

	if (m_bSelectionDrag) {
		dragSelectedLabels( pe->pos() );
		m_SelectionStartPos = pe->pos();
	}

	if (m_bOffsetDrag) {
		int xofs = pe->pos().x() - m_DragStartPos.x();
		if ( xofs != 0 ) {
			if ( m_PPS.polarCoords && (m_DragStartPos.x() < ((m_PPS.rcPlot.left() + m_PPS.rcPlot.right()) >> 1)) ) {
				m_fDisplayOffset -= pe->pos().x() - m_DragStartPos.x();
			} else {
				m_fDisplayOffset += pe->pos().x() - m_DragStartPos.x();
			}
			m_DragStartPos = pe->pos();
			if ( !m_bOffsetDragPerformed ) {
				m_bOffsetDragPerformed = true;
				setCursor( QCursor( Qt::ClosedHandCursor ) );
			}
		}
	}

	updateStatusBarXY( pe->x(), pe->y() );

	if ( pe->x() < m_PPS.rcData.left() ||
		 pe->x() > m_PPS.rcData.right() ||
		 pe->y() < m_PPS.rcData.top() ||
		 pe->y() > m_PPS.rcData.bottom() ) {
		if (m_iMouseX >= 0 || m_iMouseY >= 0) {
			m_iMouseX = -1;
			m_iMouseY = -1;
			if (m_bSelectionStart || m_pMainWindow->plotMouseTrack()) updateGL();
		} else if (m_bSelectionStart) 
			updateGL();
		return;
	}
	
	m_iMouseX = pe->x();
	m_iMouseY = pe->y();
	if (m_bSelectionStart || m_bOffsetDragPerformed || m_pMainWindow->plotMouseTrack()) updateGL();
}

int MDTRA_Plot :: getSnapshotIndex( int localX, int localY )
{
	float xRemap;

	if ( m_PPS.polarCoords ) {
		int plotCenterX = (m_PPS.rcPlot.left() + m_PPS.rcPlot.right()) >> 1;
		int plotCenterY = (m_PPS.rcPlot.top() + m_PPS.rcPlot.bottom()) >> 1;

		float r = 2.0f * sqrtf((float)((localX - plotCenterX)*(localX - plotCenterX) + (localY - plotCenterY)*(localY - plotCenterY)));
		if ( r > m_PPS.rcData.width() )
			return -1;

		xRemap = (float)(r - m_fDisplayOffset) / (float)(m_PPS.rcData.width() * m_fDisplayScale);
	} else {
		if ( localX < m_PPS.rcData.left() ||
			 localX > m_PPS.rcData.right() ||
			 localY < m_PPS.rcData.top() ||
			 localY > m_PPS.rcData.bottom() ) {
			return -1;
		}
		xRemap = (float)(localX - m_PPS.rcData.left() - m_fDisplayOffset) / (float)(m_PPS.rcData.width() * m_fDisplayScale);
	}

	if ( xRemap > 1.0f ) xRemap = 1.0f;
	xRemap *= m_PPS.xdataWidth;

	int sid = (int)MDTRA_ROUND(xRemap);
	if ( sid > m_PPS.xdataWidth )
		return -1;
		
	return sid;
}

int MDTRA_Plot :: getClosestSourceNum( int snapshotIndex, int localY )
{
	int iClosest = -1;
	float flBestDist = m_iHeight;

	for (int i = 0; i < m_PlotData.count(); i++) {
		if (!(m_PlotData.at(i).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;

		int iDataSize = m_PlotData.at(i).pDataRef->iActualDataSize;
		const float *pDataPtr = m_PlotData.at(i).pDataRef->pData;
		if (!iDataSize || !pDataPtr)
			continue;

		int localSnapshotIndex = snapshotIndex;
		if (m_Layout != MDTRA_LAYOUT_TIME) localSnapshotIndex-=m_PlotData.at(i).xOffset;
		if ( localSnapshotIndex < 0 )
			continue;

		float testData;
		float sampledData = sampleData( pDataPtr, localSnapshotIndex, iDataSize );
		
		if ( m_PPS.polarCoords ) {
			float xvalue = localSnapshotIndex * (m_PlotData.at(i).pDataRef->xscale * flXScaleUnit_Scale[m_iXScaleUnits] + flXScaleUnit_Bias[m_iXScaleUnits]);
			float xdataofs = ( xvalue * m_PPS.dataScale.x() + m_PPS.dataBias.x() ) * 0.5f;
			if ( xdataofs < 0 ) continue;
			if ( xdataofs > (m_PPS.rcData.width() >> 1) ) continue;
			if ( m_DataScaleUnits == MDTRA_YSU_DEGREES ) sampledData = UTIL_deg2rad( sampledData );
			testData = ((m_PPS.rcPlot.top() + m_PPS.rcPlot.bottom()) >> 1) - xdataofs * sinf( sampledData );
		} else {
			testData = m_PPS.yorigin - sampledData * m_PPS.dataScale.y() + m_PPS.dataBias.y();
		}

		float flDist = fabsf( testData - localY );
		if (flDist < flBestDist) {
			flBestDist = flDist;
			iClosest = i;
		}
	}
	return iClosest;
}

void MDTRA_Plot :: mouseDoubleClickEvent( QMouseEvent* pe )
{
	if (m_bDisabled)
		return;

	m_bSelectionStart = false;
	m_bSelectionDrag = false;
	setCursor( QCursor( Qt::ArrowCursor ) );

	if ( m_pMainWindow->plotShowLabels() && m_pLabels && m_pLabels->count() > 0 ) {
		if (countSelection() == 1 && isWithinSelection( pe->pos() )) {
			editLabel();
			return;
		}
	}

	if (m_Layout != MDTRA_LAYOUT_TIME)
		return;

	if (!m_pMainWindow->ViewerType())
		return;

	int newX = getSnapshotIndex( pe->x(), pe->y() );
	if (newX < 0)
		return;

	int r = QMessageBox::warning( this, tr("Confirm"), tr("View trajectory file(s) at snapshot #%1?").arg(newX), 
								  QMessageBox::Yes | QMessageBox::Default, QMessageBox::No );

	if (r == QMessageBox::Yes)
		m_pMainWindow->viewPDBFiles( newX );
}

void MDTRA_Plot :: updateSelectionRect( const QPoint &newp )
{
	m_SelectionRect = QRect( m_SelectionStartPos, newp ).normalized();
	if (m_SelectionRect.x() < 0) m_SelectionRect.setX( 0 );
	if (m_SelectionRect.y() < 0) m_SelectionRect.setY( 0 );
	if (m_SelectionRect.x() > m_iWidth - m_SelectionRect.width()) m_SelectionRect.setWidth( m_iWidth - m_SelectionRect.x());
	if (m_SelectionRect.y() > m_iHeight - m_SelectionRect.height()) m_SelectionRect.setHeight( m_iHeight - m_SelectionRect.y());
}

void MDTRA_Plot :: mousePressEvent( QMouseEvent* pe )
{
	if ( m_bDisabled ) {
		return;
	}

	m_bMousePressed = true;

	if ( pe->button() == Qt::LeftButton ) {
		m_SelectionStartPos = pe->pos();
		if (isWithinSelection( pe->pos() )) {
			m_bSelectionDrag = true;
			setCursor( QCursor( Qt::SizeAllCursor ) );
		} else {
			m_bSelectionStart = true;
			updateSelectionRect( QPoint(pe->pos().x()+1, pe->pos().y()+1) );
		}
	} else if ( pe->button() == Qt::RightButton ) {
		m_DragStartPos = pe->pos();
		m_bOffsetDrag = true;
		m_bOffsetDragPerformed = false;
	}
}

void MDTRA_Plot :: mouseReleaseEvent( QMouseEvent* pe )
{
	if ( m_bDisabled || !m_bMousePressed ) {
		return;
	}

	m_bMousePressed = false;

	if ( pe->button() == Qt::LeftButton ) {
		if ( m_bSelectionDrag ) {
			m_bSelectionDrag = false;
			setCursor( QCursor( Qt::ArrowCursor ) );
		} else if ( m_bSelectionStart ) {
			updateSelectionRect( pe->pos() );

			//select all labels within selection rectangle
			selectLabelsWithin( m_SelectionRect, pe->modifiers() & Qt::ControlModifier, pe->modifiers() & Qt::ShiftModifier );
			
			m_bSelectionStart = false;
			updateGL();
		}
	} else if ( pe->button() == Qt::RightButton ) {
		if ( m_bOffsetDragPerformed ) {
			m_bOffsetDrag = false;
			m_bOffsetDragPerformed = false;
			setCursor( QCursor( Qt::ArrowCursor ) );
		} else {
			if ( m_bOffsetDrag ) m_bOffsetDrag = false;
			int numSel = countSelection();
			m_pEditLabelAction->setEnabled( numSel == 1 );
			m_pRemoveLabelAction->setEnabled( numSel >= 1 );
			m_CommandPos = pe->pos();
			m_commandMenu.exec( mapToGlobal( m_CommandPos ) );
		}
	}
}

void MDTRA_Plot :: wheelEvent( QWheelEvent* pe )
{
	if ( m_bDisabled )
		return;

	if ( pe->delta() != 0 ) {
		float fNewScale = m_fDisplayScale + pe->delta() * 0.001f;
		if ( fNewScale < 1.0f ) fNewScale = 1.0f;

		// zoom at cursor position
		if ( m_PPS.polarCoords ) {
			int plotCenterX = (m_PPS.rcPlot.left() + m_PPS.rcPlot.right()) >> 1;
			int plotCenterY = (m_PPS.rcPlot.top() + m_PPS.rcPlot.bottom()) >> 1;
			float r = 2.0f * sqrtf((float)((pe->x() - plotCenterX)*(pe->x() - plotCenterX) + (pe->y() - plotCenterY)*(pe->y() - plotCenterY)));
			float posX = (m_fDisplayOffset - r) / m_fDisplayScale;
			m_fDisplayOffset = posX * fNewScale + r;
		} else {
			int iMouseX = pe->x() - m_PPS.rcData.left();
			float posX = (m_fDisplayOffset - iMouseX) / m_fDisplayScale;
			m_fDisplayOffset = posX * fNewScale + iMouseX;
		}
	
		m_fDisplayScale = fNewScale;
		checkDisplayOffset();

		updateStatusBarXY( pe->x(), pe->y() );
		updateGL();
	}

	pe->accept();
}

void MDTRA_Plot :: addLabel( void )
{
	int newX = getSnapshotIndex( m_CommandPos.x(), m_CommandPos.y() );
	if (newX < 0)
		return;

	int srcNum = getClosestSourceNum( newX, m_CommandPos.y() );
	if (srcNum < 0)
		return;

	MDTRA_Label lbl;

	switch (m_Layout) {
	default:
	case MDTRA_LAYOUT_TIME: lbl.text = QString("Snapshot %1").arg(newX); break;
	case MDTRA_LAYOUT_RESIDUE: 
		{
			MDTRA_DataSource *pDS = m_pMainWindow->getProject()->fetchDataSourceByIndex(m_PlotData.at(srcNum).pDataRef->dataSourceIndex);
			assert( pDS != NULL );
			MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex(pDS->streamIndex);
			assert( pStream != NULL );
			if ( pStream->pdb != NULL ) {
				const MDTRA_PDB_Atom *pAtom = pStream->pdb->fetchAtomByResidueSerial(newX);
				assert( pAtom != NULL );
				lbl.text = QString("%1-%2").arg(pAtom->trimmed_residue).arg(pAtom->residuenumber);
			} else
				lbl.text = QString("Residue %1").arg(newX);
			break;
		}
	}

	lbl.x = m_CommandPos.x();
	lbl.y = m_CommandPos.y();
	lbl.wide = 1;
	lbl.tall = 1;
	lbl.sourceNum = srcNum;
	lbl.snapshotNum = newX;
	lbl.flags = 0xFF & ~(PLOTLABEL_FLAG_TRANSPARENT|PLOTLABEL_FLAG_VERTLINE);

	MDTRA_LabelDialog dialog( false, m_pMainWindow );
	for (int i = 0; i < m_PlotData.count(); i++) {
		const MDTRA_DataSource *pSrc = m_pMainWindow->getProject()->fetchDataSourceByIndex( m_PlotData.at(i).pDataRef->dataSourceIndex );
		dialog.addDataSourceRef( m_PlotData.at(i).pDataRef->dataSourceIndex, pSrc->name );
	}
	dialog.setLabelData( &lbl );
	if (dialog.exec()) {
		deselectLabels();
		m_pMainWindow->addLabel( &lbl );
		updateGL();
	}
}

void MDTRA_Plot :: editLabel( void )
{
	int iSelLabel = fetchSelectedLabel();
	if (iSelLabel < 0)
		return;

	const MDTRA_Label *pLabel = &m_pLabels->at(iSelLabel);

	MDTRA_Label lbl;
	lbl.text = pLabel->text;
	lbl.x = pLabel->x;
	lbl.y = pLabel->y;
	lbl.sourceNum = pLabel->sourceNum;
	lbl.snapshotNum = pLabel->snapshotNum;
	lbl.flags = pLabel->flags;

	MDTRA_LabelDialog dialog( true, m_pMainWindow );
	for (int i = 0; i < m_PlotData.count(); i++) {
		const MDTRA_DataSource *pSrc = m_pMainWindow->getProject()->fetchDataSourceByIndex( m_PlotData.at(i).pDataRef->dataSourceIndex );
		dialog.addDataSourceRef( m_PlotData.at(i).pDataRef->dataSourceIndex, pSrc->name );
	}
	dialog.setLabelData( &lbl );
	if (dialog.exec()) {
		QStringList labelLines = lbl.text.split("\n");
		lbl.wide = 0;
		for (int j = 0; j < labelLines.count(); j++) {
			int wide = fontMetrics().width( labelLines.at(j) );
			if (wide > lbl.wide) lbl.wide = wide;
		}
		lbl.wide += 4;
		lbl.tall = (fontMetrics().height() + 2) * labelLines.count() + 4;

		m_pMainWindow->editLabel( iSelLabel, &lbl );
		updateGL();
	}
}

void MDTRA_Plot :: removeLabel( void )
{
	int numSel = countSelection();
	if (!numSel)
		return;

	int r = QMessageBox::warning( this, tr("Confirm"), tr("Are you sure to remove %1 label(s)?").arg(numSel), 
								  QMessageBox::Yes | QMessageBox::Default, QMessageBox::No );

	if (r == QMessageBox::No)
		return;

	for ( int i = 0; i < m_pLabels->count(); i++ ) {
		MDTRA_Label *pLabel = const_cast<MDTRA_Label*>(&m_pLabels->at(i));
		if (!(m_PlotData.at(pLabel->sourceNum).pDataRef->flags & DSREF_FLAG_VISIBLE))
			continue;
		if (!(pLabel->flags & PLOTLABEL_FLAG_SELECTED))
			continue;
	
		if (m_pMainWindow->removeLabel( i ))
			i--;
	}

	m_pMainWindow->updateTitleBar( true );
	updateGL();
}

void MDTRA_Plot :: contextMenuEvent(QContextMenuEvent * e)
{
	//it is intended to be empty
}

bool MDTRA_Plot :: saveScreenshot( const QString &fileName, const char *fileFormat )
{
	bool bSaveResult = true;
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

#ifdef MDTRA_ALLOW_PRINTER
	bool bPDF = !_stricmp( fileFormat, "pdf" );
	bool bPS = !_stricmp( fileFormat, "ps" );

	if ( bPDF || bPS ) {
		float pdfWidth = m_iWidth * 0.5f;
		float pdfHeight = m_iHeight * 0.5f;
		float pdfMarginX = pdfWidth * 0.05f;
		float pdfMarginY = pdfHeight * 0.05f;

		QPrinter printer(QPrinter::HighResolution);
		printer.setOutputFormat(bPDF ? QPrinter::PdfFormat : QPrinter::PostScriptFormat);
		printer.setOutputFileName(fileName);
		printer.setFullPage(true);
		printer.setPageSize(QPrinter::Custom);
		printer.setPaperSize(QSizeF(pdfWidth,pdfHeight),QPrinter::Millimeter);
		printer.setPageMargins(pdfMarginX,pdfMarginY,pdfMarginX,pdfMarginY,QPrinter::Millimeter);

		QPainter painter(&printer);
		float scaleX = printer.width() / (float)m_iWidth;
		float scaleY = printer.height() / (float)m_iHeight;
		painter.setTransform(QTransform( scaleX, 0, 0, 0, scaleY, 0, 0, 0, 1.0f ));
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		painter.setRenderHint(QPainter::TextAntialiasing);

		// Fill paintstruct
		setupPlotPaintStruct();

		// Draw the plot
		bSaveResult = renderPlot_Generic( &painter );

		painter.end();
	}
	else 
#endif
	{
		QImage img = grabFrameBuffer();
		bSaveResult = img.save( fileName, fileFormat );
	}

	QApplication::restoreOverrideCursor();
	return bSaveResult;
}

bool MDTRA_Plot :: printAllPages( const QString &fileName, int pageCount, PFNMDTRAPRINTERSETNEXTPAGE pfnPrinterSetNextPage )
{
	if (!pageCount || !pfnPrinterSetNextPage)
		return false;

#ifdef MDTRA_ALLOW_PRINTER
	float pdfWidth = m_iWidth * 0.5f;
	float pdfHeight = m_iHeight * 0.5f;
	float pdfMarginX = pdfWidth * 0.05f;
	float pdfMarginY = pdfHeight * 0.05f;

	MDTRA_WaitDialog dlgWait( m_pMainWindow );
	dlgWait.setMessage( QObject::tr("Printing PDF: 0/%1").arg(pageCount) );
	dlgWait.show();

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QApplication::processEvents();

	QPrinter printer(QPrinter::HighResolution);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName(fileName);
	printer.setFullPage(true);
	printer.setPageSize(QPrinter::Custom);
	printer.setPaperSize(QSizeF(pdfWidth,pdfHeight),QPrinter::Millimeter);
	printer.setPageMargins(pdfMarginX,pdfMarginY,pdfMarginX,pdfMarginY,QPrinter::Millimeter);

	QPainter painter(&printer);
	float scaleX = printer.width() / (float)m_iWidth;
	float scaleY = printer.height() / (float)m_iHeight;
	painter.setTransform(QTransform( scaleX, 0, 0, 0, scaleY, 0, 0, 0, 1.0f ));
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.setRenderHint(QPainter::TextAntialiasing);

	for ( int i = 0; i < pageCount; i++ ) {

		dlgWait.setMessage( QObject::tr("Printing PDF: %1/%2").arg(i+1).arg(pageCount) );
		QApplication::processEvents();

		setupPlotPaintStruct();

		if (!renderPlot_Generic( &painter )) {
			QApplication::restoreOverrideCursor();
			dlgWait.hide();
			return false;
		}

		if ( i < pageCount-1 ) {
			if (!printer.newPage()) {
				QApplication::restoreOverrideCursor();
				dlgWait.hide();
				return false;
			}
		}

		pfnPrinterSetNextPage();
	}

	QApplication::restoreOverrideCursor();
	dlgWait.hide();
#endif
	return true;
}
