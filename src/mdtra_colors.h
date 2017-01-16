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
#ifndef MDTRA_COLORS_H
#define MDTRA_COLORS_H

#include <QtGui/QColor>

class QSettings;
class QListWidget;

#define NUM_DATA_COLORS		16

typedef enum {
	COLOR_PLOT_BACKGROUND = 0,
	COLOR_PLOT_BORDER,
	COLOR_PLOT_AXES,
	COLOR_PLOT_DATA1,
	COLOR_PLOT_DATA2,
	COLOR_PLOT_DATA3,
	COLOR_PLOT_DATA4,
	COLOR_PLOT_DATA5,
	COLOR_PLOT_DATA6,
	COLOR_PLOT_DATA7,
	COLOR_PLOT_DATA8,
	COLOR_PLOT_DATA9,
	COLOR_PLOT_DATA10,
	COLOR_PLOT_DATA11,
	COLOR_PLOT_DATA12,
	COLOR_PLOT_DATA13,
	COLOR_PLOT_DATA14,
	COLOR_PLOT_DATA15,
	COLOR_PLOT_DATA16,
	COLOR_PLOT_TEXT,
	COLOR_PLOT_SELECTION,
	COLOR_3D_BACKGROUND,
	COLOR_3D_TEXT,
	COLOR_3D_SELECTION,
	COLOR_PROG_KEYWORD,
	COLOR_PROG_FUNCTION,
	COLOR_PROG_ARGUMENT,
	COLOR_PROG_STRING,
	COLOR_PROG_COMMENT,
} MDTRA_Color;

typedef struct stMDTRA_ColorDef {
	int index;
	const char *name;
	const char *title;
	QColor colorRef;
} MDTRA_ColorDef;

class MDTRA_ColorManager
{
public:
    MDTRA_ColorManager();
	~MDTRA_ColorManager() {}

	void saveSettings( QSettings &settings );
	void restoreSettings( QSettings &settings );

	const QColor &color( int colorIndex ) const;
	const QColor &color( const char *name ) const;
	const QColor &black( void ) const { return m_blackColor; }
	const QColor &white( void ) const { return m_whiteColor; }
	const QString colorTitle( int colorIndex ) const;
	int numColors( void ) const;

	void fillPreferencesTable( QListWidget *pList ) const;
	void applyColorChanges( QList<QColor> &newColors ) const;

private:
	QColor m_blackColor;
	QColor m_whiteColor;
};

#endif //MDTRA_COLORS_H