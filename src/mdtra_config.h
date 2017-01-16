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
#ifndef MDTRA_CONFIG_H
#define MDTRA_CONFIG_H

//Titles
#define APPLICATION_TITLE_SMALL	"MDTRA"
#define APPLICATION_TITLE_FULL	"MD Trajectory Reader & Analyzer"
#define APPLICATION_EXT			"mdtra"
#define TOOLBAR_TITLE			"Main Toolbar"
#define APPLICATION_VERSION		"1.2"

//Paths and filenames
#define CONFIG_DIRECTORY		"conf/"
#define SETTINGS_FILENAME		"settings.ini"
#define LAYOUT_FILENAME			"layout.dat"
#define FORMATS_FILENAME		"formats.dat"
#define HELP_FILENAME			"help/mdtra.pdf"

// Build without internal PDB viewer
#define NO_INTERNAL_PDB_RENDERER

#endif //MDTRA_CONFIG_H