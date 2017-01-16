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
#ifndef MDTRA_HISTOGRAM_PLOT_H
#define MDTRA_HISTOGRAM_PLOT_H

#include "mdtra_genericPlot.h"

typedef enum
{
	MDTRA_HCM_AUTO = 0,
	MDTRA_HCM_GRAYSCALE,
	MDTRA_HCM_SINGLE
} MDTRA_HistogramColorMode;

typedef struct stMDTRA_HistogramPlotPaintStruct
{
	QFont			fntTitle;			// device-dependent title font
	QFont			fntNormal;			// device-dependent normal font
	QFont			fntDigits;			// device-dependent digit font
	QRect			rcPlot;				// plot rect
	QRect			rcData;				// data rect
	int				bandWide;			// width of band
	int				bandGroup;			// number of bands in a unit group
	float			bandHeightScale;	// scale applied to band value to get band height
	QString			xAxisTitle;
	QString			yAxisTitle;
	float			scaleFactor;
	float			scaleDimension;
	float			axisScale;
} MDTRA_HistogramPlotPaintStruct;

class MDTRA_HistogramPlot : public MDTRA_Generic_Plot
{
	Q_OBJECT

public:
    MDTRA_HistogramPlot(QGLFormat &format, QWidget *parent = 0);
	virtual ~MDTRA_HistogramPlot();

	void setTitle( const QString& title ) { m_szTitle = title; }
	void setColorMode( MDTRA_HistogramColorMode mode, QColor &color ) { m_CM = mode; m_Color = color; }
	void setAxisInfo( int type, int unit, int layout, float xscale, const QString &userdata ) { m_iAxisType = type; m_iAxisUnit = unit; m_iAxisLayout = layout; m_flXScale = xscale; m_sUserData = userdata; }
	void setStatInfo( float expval, float stddev ) { m_flExpectedValue = expval; m_flInvStdDev = 1.0f / stddev; }
	void setHistogramData( float flMin, float flSize, int iCount, const int *piData );

protected:
	virtual void setupPlotPaintStruct( void );
	virtual bool renderPlot_OpenGL( void );
	virtual bool renderPlot_Generic( QPainter* pPainter );
	virtual void reset( void );

private:
	float calc_normal_distribution( float f );

private:
	MDTRA_HistogramPlotPaintStruct m_PPS;
	MDTRA_HistogramColorMode m_CM;
	QColor m_Color;
	QString m_szTitle;
	float m_flInvStdDev;
	float m_flExpectedValue;
	float m_flBandMin;
	float m_flBandSize;
	float m_flXScale;
	int m_iTotalData;
	int m_iMaxBandData;
	int m_iNumBands;
	int m_iAxisType;
	int m_iAxisUnit;
	int m_iAxisLayout;
	QString m_sUserData;
	const int *m_pBandData;
};

#endif //MDTRA_HISTOGRAM_PLOT_H
