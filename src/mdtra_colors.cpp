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
//	Implementation of MDTRA_ColorManager

#include "mdtra_main.h"
#include "mdtra_colors.h"

#include <QtCore/QSettings>
#include <QtGui/QListWidget>

#define MDTRA_COLORDEF(x, y, r, g, b)	{ x, #x, y, QColor(r,g,b) }

//Our global color table
static MDTRA_ColorDef g_ColorDef[] = 
{
MDTRA_COLORDEF( COLOR_PLOT_BACKGROUND, "Plot Background", 255, 255, 255 ),
MDTRA_COLORDEF( COLOR_PLOT_BORDER, "Plot Border", 0, 0, 0 ),
MDTRA_COLORDEF( COLOR_PLOT_AXES, "Plot Axes", 0, 0, 0 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA1, "Plot Data 1", 0, 0, 128 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA2, "Plot Data 2", 255, 0, 128 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA3, "Plot Data 3", 0, 128, 255 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA4, "Plot Data 4", 128, 0, 255 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA5, "Plot Data 5", 0, 255, 128 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA6, "Plot Data 6", 128, 0, 0 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA7, "Plot Data 7", 0, 128, 0 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA8, "Plot Data 8", 255, 128, 128 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA9, "Plot Data 9", 128, 255, 0 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA10, "Plot Data 10", 128, 255, 128 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA11, "Plot Data 11", 255, 128, 0 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA12, "Plot Data 12", 128, 128, 255 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA13, "Plot Data 13", 180, 255, 255 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA14, "Plot Data 14", 180, 0, 128 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA15, "Plot Data 15", 200, 0, 0 ),
MDTRA_COLORDEF( COLOR_PLOT_DATA16, "Plot Data 16", 255, 255, 200 ),
MDTRA_COLORDEF( COLOR_PLOT_TEXT, "Plot Text", 0, 0, 0 ),
MDTRA_COLORDEF( COLOR_PLOT_SELECTION, "Plot Selection", 255, 0, 0 ),
MDTRA_COLORDEF( COLOR_3D_BACKGROUND, "3D Background", 100, 100, 100 ),
MDTRA_COLORDEF( COLOR_3D_TEXT, "3D Text", 255, 255, 255 ),
MDTRA_COLORDEF( COLOR_3D_SELECTION, "3D Selection", 0, 255, 0 ),
MDTRA_COLORDEF( COLOR_PROG_KEYWORD, "Program Keyword", 0, 0, 255 ),
MDTRA_COLORDEF( COLOR_PROG_FUNCTION, "Program Function", 0, 0, 128 ),
MDTRA_COLORDEF( COLOR_PROG_ARGUMENT, "Program Predefined Argument", 0, 0, 0 ),
MDTRA_COLORDEF( COLOR_PROG_STRING, "Program String", 163, 21, 21 ),
MDTRA_COLORDEF( COLOR_PROG_COMMENT, "Program Comment", 0, 128, 0 ),
};

MDTRA_ColorManager :: MDTRA_ColorManager() 
					: m_blackColor( 0, 0, 0 ), m_whiteColor( 255, 255, 255 )
{
}

void MDTRA_ColorManager :: saveSettings( QSettings &settings )
{
	//Save color values
	int numColors = sizeof(g_ColorDef) / sizeof(g_ColorDef[0]);
	for ( int i = 0; i < numColors; i++ )
		settings.setValue( QString("Colors/").append(QString(g_ColorDef[i].name)), g_ColorDef[i].colorRef );
}

void MDTRA_ColorManager :: restoreSettings( QSettings &settings )
{
	//Load color values
	int numColors = sizeof(g_ColorDef) / sizeof(g_ColorDef[0]);
	for ( int i = 0; i < numColors; i++ ) {
		QString settingName = QString("Colors/").append(QString(g_ColorDef[i].name));
		if ( settings.contains( settingName ) )
			g_ColorDef[i].colorRef = settings.value( settingName ).value<QColor>();
	}
}

const QColor &MDTRA_ColorManager :: color( int colorIndex ) const
{
	//Get color by index (e.g. COLOR_PLOT_BACKGROUND)
	int numColors = sizeof(g_ColorDef) / sizeof(g_ColorDef[0]);
	for ( int i = 0; i < numColors; i++ )
		if (g_ColorDef[i].index == colorIndex)
			return g_ColorDef[i].colorRef;
	return m_blackColor;
}

const QColor &MDTRA_ColorManager :: color( const char *name ) const
{
	//Get color by name (e.g. "COLOR_PLOT_BACKGROUND")
	int numColors = sizeof(g_ColorDef) / sizeof(g_ColorDef[0]);
	for ( int i = 0; i < numColors; i++ )
		if (!strcmp(g_ColorDef[i].name, name))
			return g_ColorDef[i].colorRef;
	return m_blackColor;
}

const QString MDTRA_ColorManager :: colorTitle( int colorIndex ) const
{
	//Get color title by index (e.g. COLOR_PLOT_BACKGROUND)
	int numColors = sizeof(g_ColorDef) / sizeof(g_ColorDef[0]);
	for ( int i = 0; i < numColors; i++ )
		if (g_ColorDef[i].index == colorIndex)
			return g_ColorDef[i].title;
	return "";
}

int MDTRA_ColorManager :: numColors( void ) const
{
	return sizeof(g_ColorDef) / sizeof(g_ColorDef[0]);
}

void MDTRA_ColorManager :: fillPreferencesTable( QListWidget *pList ) const
{
	//Insert colors into preference table
	pList->clear();

	int numColors = sizeof(g_ColorDef) / sizeof(g_ColorDef[0]);
	for ( int i = 0; i < numColors; i++ ) {
		pList->addItem( g_ColorDef[i].title );
		pList->item(i)->setData(Qt::UserRole, i);
	}
}

void MDTRA_ColorManager :: applyColorChanges( QList<QColor> &newColors ) const
{
	int numColors = sizeof(g_ColorDef) / sizeof(g_ColorDef[0]);
	assert( numColors == newColors.count() );
	for ( int i = 0; i < numColors; i++ ) {
		g_ColorDef[i].colorRef = newColors[i];
	}
}