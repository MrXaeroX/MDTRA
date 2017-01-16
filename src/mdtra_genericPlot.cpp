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
//	Implementation of MDTRA_Generic_Plot

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_genericPlot.h"

#include "glext.h"

#include <QtGui/QApplication>
#include <QtGui/QPrinter>

MDTRA_Generic_Plot :: MDTRA_Generic_Plot( QGLFormat &format, QWidget *parent )
					: QGLWidget( format, parent, NULL )
{
	m_bMultisampleContext = false;
	setAutoFillBackground( false );

	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);
	reset();
}

void MDTRA_Generic_Plot :: reset( void )
{
}

void MDTRA_Generic_Plot :: initializeGL( void ) 
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

	if ( format().sampleBuffers() ) {
		m_bMultisampleContext = true;
		glDisable( GL_MULTISAMPLE );
	}

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	//first clear
	qglClearColor( Qt::gray );
	glClear( GL_COLOR_BUFFER_BIT );
}

void MDTRA_Generic_Plot :: resizeGL( int width, int height ) 
{
	if (width <= 0)
		return;

	m_iWidth = width;
	m_iHeight = height;
}

void MDTRA_Generic_Plot :: renderSpecialString( int x, int y, const QString& str, const QFont& f, const QFontMetrics& fm, QPainter* pPainter ) 
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

void MDTRA_Generic_Plot :: paintGL( void ) 
{
	//Set viewport
	glViewport( 0, 0, m_iWidth, m_iHeight );

	//Switch to orthogonal projection and setup near/far
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, m_iWidth, m_iHeight, 0, -1.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Fill paintstruct and render
	setupPlotPaintStruct();
	renderPlot_OpenGL();
}

bool MDTRA_Generic_Plot :: saveScreenshot( const QString &fileName, const char *fileFormat )
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
