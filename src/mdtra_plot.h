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
#ifndef MDTRA_PLOT_H
#define MDTRA_PLOT_H

#include <QtOpenGL/QGLWidget>
#include <QtGui/QMenu>

#include "mdtra_types.h"

class MDTRA_MainWindow;
class QMouseEvent;

typedef struct stMDTRA_PlotData
{
	QString title;
	const struct stMDTRA_DSRef*	pDataRef;
	int xOffset;
} MDTRA_PlotData;

typedef struct stMDTRA_PlotPaintStruct
{
	QFont			fntTitle;			// device-dependent title font
	QFont			fntNormal;			// device-dependent normal font
	QFont			fntDigits;			// device-dependent digit font
	QRect			rcPlot;				// plot rect
	QRect			rcData;				// plot data rect
	QRect			rcLegend;			// legend rect
	QSize			siBullet;			// size of bullet on a legend
	QSize			siArrow;			// size of arrow
	QPoint			vXArrow[3];			// x-axis arrow vertices
	QPoint			vYArrow[3];			// y-axis arrow vertices
	int				yorigin;			// x-axis Y origin
	int				plotBottomMargin;	// bottom plot margin
	int				legendNumRows;		// number of rows in a legend
	int				legendNumCols;		// number of columns in a legend
	int				legendXMargin;		// horizontal margin on a legend
	int				legendYMargin;		// vertical margin on a legend
	int				legendRowSpacing;	// spacing between rows of a legend
	int				legendMidX;			// X of middle of the legend
	int				xdataWidth;			// umremapped width of X data
	int				labelBulletHalfSize;// half size of label's bullet
	int				labelSelHalfSize;	// half size of label's selection mark
	QPointF			scaleUnit;			// axis scale units
	QPointF			dataMin;			// minimum values of plot data
	QPointF			dataMax;			// maximum values of plot data
	QPointF			dataScale;			// raw data scale factor
	QPointF			dataBias;			// raw data bias factor
	bool			polarCoords;
} MDTRA_PlotPaintStruct;

typedef void (*PFNMDTRAPRINTERSETNEXTPAGE)(void);

class MDTRA_Plot : public QGLWidget
{
	Q_OBJECT

public:
    MDTRA_Plot(QGLFormat &format, QWidget *parent = 0);
	~MDTRA_Plot();

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	void enable( void );
	void disable( const QString &disabledMsg );
	void setPlotTitle( const QString &title );
	void setDataType( MDTRA_DataType dt );
	void setDataScaleUnits( MDTRA_YScaleUnits ysu );
	void setYAxisTitle( const QString &title );
	void setXLayout( MDTRA_Layout layout ) { m_Layout = layout; }
	void setLabels( QList<struct stMDTRA_Label> *pLabels );
	void clearPlotData( void );
	void addPlotData( const QString &title, const struct stMDTRA_DSRef* pref );
	bool saveScreenshot( const QString &fileName, const char *fileFormat );
	bool printAllPages( const QString &fileName, int pageCount, PFNMDTRAPRINTERSETNEXTPAGE pfnPrinterSetNextPage );

protected:
	virtual void initializeGL( void );
	virtual void resizeGL( int width, int height );
	virtual void paintGL( void );
	void reset( void );
	void mouseMoveEvent( QMouseEvent* pe );
	void mouseDoubleClickEvent( QMouseEvent* pe );
	void mousePressEvent( QMouseEvent* pe );
	void mouseReleaseEvent( QMouseEvent* pe );
	void wheelEvent( QWheelEvent* pe );
	void contextMenuEvent(QContextMenuEvent * e);

private:
	void setupPlotPaintStruct( void );
	bool renderPlot_OpenGL( void );
	bool renderPlotDataCartesian_OpenGL( void );
	bool renderPlotDataPolar_OpenGL( void );
	bool renderPlot_Generic( QPainter* pPainter );
	bool renderPlotDataCartesian_Generic( QPainter* pPainter );
	bool renderPlotDataPolar_Generic( QPainter* pPainter );
	float sampleData( const float *pDataPtr, int iSample, int iMaxSample );
	void renderSpecialString( int x, int y, const QString& str, const QFont& f, const QFontMetrics& fm, QPainter* pPainter = NULL );
	int getSnapshotIndex( int localX, int localY );
	int getClosestSourceNum( int snapshotIndex, int localY );
	void selectLabelsWithin( const QRect &selectionRect, bool addToSel, bool invertSel );
	void deselectLabels( void );
	bool isWithinSelection( const QPoint &selectionPoint );
	void updateSelectionRect( const QPoint &newp );
	void dragSelectedLabels( const QPoint &newp );
	int countSelection( void );
	int fetchSelectedLabel( void );
	void checkDisplayOffset( void );
	void updateStatusBarXY( int mx, int my );

public slots:
	void addLabel( void );
	void editLabel( void );
	void removeLabel( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	int	m_iWidth;
	int m_iHeight;
	bool m_bDisabled;
	QString m_szTitle;
	QString m_szXAxisTitle;
	QString m_szYAxisTitle;
	QString m_szDisabledMsg;
	MDTRA_Layout m_Layout;
	MDTRA_DataType m_DataType;
	MDTRA_YScaleUnits m_DataScaleUnits;
	QList<MDTRA_PlotData> m_PlotData;
	MDTRA_PlotPaintStruct m_PPS;
	float m_fDisplayOffset;
	float m_fDisplayScale;
	int m_iXScaleUnits;
	int	m_iMouseX;
	int m_iMouseY;
	bool m_bMousePressed;
	bool m_bSelectionStart;
	bool m_bSelectionDrag;
	bool m_bOffsetDrag;
	bool m_bOffsetDragPerformed;
	QPoint m_DragStartPos;
	QPoint m_SelectionStartPos;
	QRect m_SelectionRect;
	QPoint m_CommandPos;
	QMenu m_commandMenu;
	QAction *m_pAddLabelAction;
	QAction *m_pEditLabelAction;
	QAction *m_pRemoveLabelAction;
	QList<struct stMDTRA_Label> *m_pLabels;
};

#endif //MDTRA_PLOT_H