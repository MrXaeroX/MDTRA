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
//	Implementation of MDTRA_ProgramSyntexHighlighter

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_colors.h"
#include "mdtra_prog_syntaxHighlight.h"

static const char* s_szKeywords[] = {
"and", "break", "do", "else", "elseif", "end", "false", "for", "function", "if", 
"in", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while",
"vec2", "vec3", "mat3"
};

static const char* s_szFunctions[] = {
"tonumber", "tostring", "type", "print", "pairs", "ipairs",
"datapos", "datasize", "dataread", "datawrite", "rboutput",
"coord", "radius", "residue", "atomflags", "checkflags", "sqdev", "numatoms", "numresidues",
"rmsd", "sas", "distance", "angle", "torsion", "utorsion", "dihedral", "force", "resultant",
"dot", "length", "normalize", "cross", "get_residue_transform", "build_rotation_matrix",
"hbenergy", "serial"
};

static const char* s_szArguments[] = {
"pi", "huge",
"arg1", "arg2", "arg3", "arg4", "arg5", "arg6"
};

MDTRA_ProgramSyntaxHighlighter :: MDTRA_ProgramSyntaxHighlighter( QWidget *mainwindow, QTextDocument *document )
								: QSyntaxHighlighter( document )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(mainwindow);
	assert(m_pMainWindow != NULL);

	QTextCharFormat keywordFormat;
	keywordFormat.setForeground( m_pMainWindow->getColorManager()->color( COLOR_PROG_KEYWORD ) );
	setFormatFor( PC_KEYWORD, keywordFormat );

	QTextCharFormat functionFormat;
	functionFormat.setForeground( m_pMainWindow->getColorManager()->color( COLOR_PROG_FUNCTION ) );
	functionFormat.setFontWeight( QFont::Bold );
	setFormatFor( PC_FUNCTION, functionFormat );

	QTextCharFormat argumentFormat;
	argumentFormat.setForeground( m_pMainWindow->getColorManager()->color( COLOR_PROG_ARGUMENT ) );
	argumentFormat.setFontWeight( QFont::Bold );
	setFormatFor( PC_ARGUMENT, argumentFormat );

	QTextCharFormat stringFormat;
	stringFormat.setForeground( m_pMainWindow->getColorManager()->color( COLOR_PROG_STRING ) );
	setFormatFor( PC_STRING, stringFormat );
    
	QTextCharFormat commentFormat;
	commentFormat.setForeground( m_pMainWindow->getColorManager()->color( COLOR_PROG_COMMENT ));
	setFormatFor( PC_COMMENT, commentFormat );
}

void MDTRA_ProgramSyntaxHighlighter :: highlightBlock( const QString &text )
{
	int state = previousBlockState();
	int start = 0;
	int numKeywords = sizeof(s_szKeywords) / sizeof(s_szKeywords[0]);
	int numFunctions = sizeof(s_szFunctions) / sizeof(s_szFunctions[0]);
	int numArgs = sizeof(s_szArguments) / sizeof(s_szArguments[0]);
    
	for (int i = 0; i < text.length(); ++i) {
		if ( state == PBS_INSIDE_COMMENT ) {
			if (text.mid(i, 2) == "*/") {
				state = PBS_NORMAL;
				setFormat( start, i - start + 2, formatFor(PC_COMMENT) );
			}
		} else if ( state == PBS_INSIDE_SQSTRING ) {
			if (text.at(i) == '\'') {
				state = PBS_NORMAL;
				setFormat( start, i - start + 1, formatFor(PC_STRING) );
			}
		} else if ( state == PBS_INSIDE_DQSTRING ) {
			if (text.at(i) == '"') {
				state = PBS_NORMAL;
				setFormat( start, i - start + 1, formatFor(PC_STRING) );
			}
		} else if ( state == PBS_NORMAL ) {
			if (text.mid(i, 2) == "//") {
				setFormat( i, text.length() - i, formatFor(PC_COMMENT) );
				break;
			} else if (text.mid(i, 2) == "/*") {
				start = i;
				state = PBS_INSIDE_COMMENT;
			} else if (text.at(i) == '\'') {
				start = i;
				state = PBS_INSIDE_SQSTRING;
			} else if (text.at(i) == '"') {
				start = i;
				state = PBS_INSIDE_DQSTRING;
			} else {
				QChar ch = text.at(i);
				if  ( ch == '_' || 
					( ch >= 'A' && ch <= 'Z') || 
					( ch >= 'a' && ch <= 'z')) {
					start = i;
					state = PBS_INSIDE_IDENTIFIER;
				}
			}
		} else {	//PBS_INSIDE_IDENTIFIER
			QChar ch = text.at(i);
			if (!( ch == '_' || 
				 ( ch >= 'A' && ch <= 'Z') || 
				 ( ch >= 'a' && ch <= 'z') || 
				 ( ch >= '0' && ch <= '9'))) {
				QByteArray identifierByteArray = text.mid(start, i - start).toAscii();
				const char* pszIdentifier = identifierByteArray.data();
				bool isKnownIdentifier = false;
				//check if it is a valid keyword
				for ( int j = 0; j < numKeywords; j++ ) {
					if ( !strcmp( pszIdentifier, s_szKeywords[j] ) ) {
						setFormat( start, i - start, formatFor(PC_KEYWORD) );
						isKnownIdentifier = false;
						break;
					}
				}
				if (!isKnownIdentifier) {
					//check if it is a valid function
					for ( int j = 0; j < numFunctions; j++ ) {
						if ( !strcmp( pszIdentifier, s_szFunctions[j] ) ) {
							setFormat( start, i - start, formatFor(PC_FUNCTION) );
							isKnownIdentifier = true;
							break;
						}
					}
				}
				if (!isKnownIdentifier) {
					//check if it is a valid predefined argument
					for ( int j = 0; j < numArgs; j++ ) {
						if ( !strcmp( pszIdentifier, s_szArguments[j] ) ) {
							setFormat( start, i - start, formatFor(PC_ARGUMENT) );
							isKnownIdentifier = true;
							break;
						}
					}
				}
				--i;
				state = PBS_NORMAL;
			}	
		}
	}

	if ( state == PBS_INSIDE_IDENTIFIER ) {
		QByteArray identifierByteArray = text.mid(start, text.length()).toAscii();
		const char* pszIdentifier = identifierByteArray.data();
		bool isKnownIdentifier = false;
		//check if it is a valid keyword
		for ( int j = 0; j < numKeywords; j++ ) {
			if ( !strcmp( pszIdentifier, s_szKeywords[j] ) ) {
				setFormat( start, text.length() - start, formatFor(PC_KEYWORD) );
				isKnownIdentifier = false;
				break;
			}
		}
		if (!isKnownIdentifier) {
			//check if it is a valid function
			for ( int j = 0; j < numFunctions; j++ ) {
				if ( !strcmp( pszIdentifier, s_szFunctions[j] ) ) {
					setFormat( start, text.length() - start, formatFor(PC_FUNCTION) );
					isKnownIdentifier = true;
					break;
				}
			}
		}
		if (!isKnownIdentifier) {
			//check if it is a valid predefined argument
			for ( int j = 0; j < numArgs; j++ ) {
				if ( !strcmp( pszIdentifier, s_szArguments[j] ) ) {
					setFormat( start, text.length() - start, formatFor(PC_ARGUMENT) );
					isKnownIdentifier = true;
					break;
				}
			}
		}
		state = PBS_NORMAL;
	} else if ( state == PBS_INSIDE_COMMENT )
		setFormat( start, text.length() - start, formatFor(PC_COMMENT) );
    
	setCurrentBlockState( state );
}
