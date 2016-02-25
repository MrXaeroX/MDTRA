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
//	Selection parser functions
//	Implementation of MDTRA_SelectionSet
#include "mdtra_main.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb.h"
#include "mdtra_select.h"

SelectionParms g_SelectionParms;
QList<SelectionError> g_SelectionParseErrors;

char *MDTRA_Select_CopyString( const char *s )
{
	if (!s) return NULL;
	int l = strlen(s);
	char *ns = new char[l+1];
	memset(ns, 0, l+1);
	strcpy_s(ns, l+1, s);
	return ns;
}

void MDTRA_Select_ParseError( const char *s, int pos ) 
{
	SelectionError err;
	err.s = MDTRA_Select_CopyString( s );
	err.pos = pos;
	g_SelectionParseErrors << err;
}
