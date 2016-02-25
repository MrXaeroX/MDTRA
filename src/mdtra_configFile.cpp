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
#include "mdtra_main.h"
#include "mdtra_utils.h"
#include "mdtra_configFile.h"

#include <QtCore/QVector>

// Purpose:
//  Global config scanning and loading routine
//	Implementation of MDTRA_ConfigFile

static QVector<MDTRA_ConfigFile*> s_configList;

void MDTRA_ScanAndLoadConfigs( void )
{
	s_configList.clear();
	s_configList.reserve( 64 );

	char fullname[8192];
	int numconfigfiles = 0;
	char **configfiles = UTIL_ListFiles( CONFIG_DIRECTORY, "*.cfg", &numconfigfiles );
	for ( int i = 0; i < numconfigfiles; ++i ) {
		memset( fullname, 0, sizeof(fullname) );
		strncat_s( fullname, CONFIG_DIRECTORY, sizeof(fullname)-1 );
		strncat_s( fullname, configfiles[i], sizeof(fullname)-1 );
		MDTRA_ConfigFile *cfg = new MDTRA_ConfigFile( fullname );
		if ( cfg->isValid() ) 
			s_configList.push_back( cfg );
		else
			delete cfg;
	}
	UTIL_FreeFileList( configfiles );
}

void MDTRA_UnloadConfigs( void )
{
	foreach ( MDTRA_ConfigFile *cfg, s_configList )
		delete cfg;
	s_configList.clear();
}

void MDTRA_ForEachConfig( eConfigType type, ConfigCallback_t func )
{
	if ( !func )
		return;
	foreach ( MDTRA_ConfigFile *cfg, s_configList ) {
		if ( type == CONFIG_TYPE_DONTCARE || cfg->isOfType( type ) )
			func( cfg );
	}
}

//////////////////////////////////////////////////////////////////////////

typedef struct {
	const char *signature;
	size_t		siglen;
	eConfigType type;
} ConfigSignatureMap_t;

static ConfigSignatureMap_t s_csm[] = {
	{ "HBparms", 0, CONFIG_TYPE_HBPARMS },
	{ "HBres", 0, CONFIG_TYPE_HBRES }
};

MDTRA_ConfigFile :: MDTRA_ConfigFile( const char *filename )
{
	m_filebase = NULL;
	m_filepos = NULL;
	m_fileend = NULL;
	m_type = CONFIG_TYPE_UNKNOWN;
	m_parseState = CONFIG_PARSE_NORMAL;
	m_tokenReady = false;
	precache( filename );
}

MDTRA_ConfigFile :: ~MDTRA_ConfigFile()
{
	if ( m_filebase )
		free( m_filebase );
	m_filebase = NULL;
	m_filepos = NULL;
	m_fileend = NULL;
}

void MDTRA_ConfigFile :: precache( const char *filename )
{
	FILE *fp;
	if ( fopen_s( &fp, filename, "rb" ) )
		return;

	fseek( fp, 0, SEEK_END );
	long end = ftell( fp );
	::rewind( fp );
	
	m_filebase = (byte*)malloc( end + 1 );
	memset( m_filebase, 0, end + 1 );

	long rv = (long)fread( m_filebase, 1, end, fp );
	if ( end != rv ) {
		free( m_filebase );
		m_filebase = NULL;
		fclose( fp );
		return;
	}

	fclose( fp );

	m_filepos = m_filebase;
	m_fileend = m_filebase + end;
	m_type = CONFIG_TYPE_UNKNOWN;
	m_version = 0.0f;

	if ( parseToken( true ) && m_parseState == CONFIG_PARSE_NORMAL ) {
		if ( m_token[0] == '!' && m_token[1] == '!' ) {
			const size_t sigcount = sizeof(s_csm) / sizeof(s_csm[0]);
			for ( size_t i = 0; i < sigcount; ++i ) {
				if ( !s_csm[i].siglen )
					s_csm[i].siglen = strlen(s_csm[i].signature);
				if ( !_strnicmp( s_csm[i].signature, m_token + 2, s_csm[i].siglen ) ) {
					m_type = s_csm[i].type;
					m_version = atof( m_token + 2 + s_csm[i].siglen );
					break;
				}
			}
		}
	}

	if ( m_type == CONFIG_TYPE_UNKNOWN )
		unparseToken();
}

bool MDTRA_ConfigFile :: tokenAvailable( void )
{
	// check parse state
	if ( m_parseState != CONFIG_PARSE_NORMAL )
		return false;

	const byte *search_p = m_filepos;

	// check parse position
	if ( search_p >= m_fileend )
		return false;

	// check for newline
	while ( *search_p <= 0x20 ) {
		if ( *search_p == '\n' )
			return false;
		if ( ++search_p == m_filepos )
			return false;
	}

	// check for comment
	if ( ( *search_p == '/' ) && ( *(search_p + 1) == '/' ) )
		return false;

	return true;
}

void MDTRA_ConfigFile :: skipRestOfLine( void )
{
	// skip to the newline
	while ( tokenAvailable() )
		parseToken( false );
}

void MDTRA_ConfigFile :: unparseToken( void )
{
	assert( m_tokenReady == false );
	m_tokenReady = true;
}

bool MDTRA_ConfigFile :: isSingleChar( byte c )
{
	if ( c == '{' || c == '}' || c == ',' || c == '@' ||
		 c == '(' || c == ')' || c == '=' || c == ';' || 
		 c == '[' || c == ']' || c == ':' )
		return true;

	return false;
}

bool MDTRA_ConfigFile :: parseToken( bool crossline )
{
	if ( m_parseState != CONFIG_PARSE_NORMAL ) {
		m_token[0] = 0;
		return false;
	}

	// is a token already waiting?
	if ( m_tokenReady ) {
		m_tokenReady = false;
		return true;
	}

	m_token[0] = 0;

	if ( m_filepos >= m_fileend ) {
		m_parseState = CONFIG_PARSE_EOF;
		return false;
	}

	// skip space
skipspace:
	while ( *m_filepos <= 0x20 ) {
		if ( m_filepos >= m_fileend ) {
			m_parseState = CONFIG_PARSE_EOF;
			return false;
		}
		if ( *m_filepos++ == '\n' ) {
			if ( !crossline ) {
				--m_filepos;
				return false;
			}
		}
	}

	if ( m_filepos >= m_fileend ) {
		m_parseState = CONFIG_PARSE_EOF;
		return false;
	}

	// '//' comments
	if ( ( *m_filepos == '/' ) && ( *(m_filepos + 1) == '/' ) ) {
		if ( !crossline )
			return false;
		while ( *m_filepos++ != '\n' ) {
			if ( m_filepos >= m_fileend ) {
				m_parseState = CONFIG_PARSE_EOF;
				return false;
			}
		}
		goto skipspace;
	}

	// '/* */' comments
	if ( ( *m_filepos == '/' ) && ( *(m_filepos + 1) == '*' ) ) {
		if ( !crossline )
			return false;
		m_filepos += 2;
		while ( ( *m_filepos != '*' ) || ( *(m_filepos + 1) != '/' ) ) {
			m_filepos++;
			if ( m_filepos >= m_fileend ) {
				m_parseState = CONFIG_PARSE_EOF;
				return false;
			}
		}
		m_filepos += 2;
		goto skipspace;
	}

	// copy token
	char *token_p = m_token;

	if ( *m_filepos == '"' ) {
		// quoted token
		++m_filepos;
		while ( *m_filepos != '"' ) {
			*token_p++ = *m_filepos++;
			if ( m_filepos == m_fileend )
				break;
			if ( token_p == &m_token[CONFIG_MAX_TOKEN] ) {
				m_parseState = CONFIG_PARSE_ERROR;
				return false;
			}
		}
		++m_filepos;
	} else {
		// single character
		byte c = *m_filepos;
		if ( isSingleChar( c ) ) {
			*token_p++ = c;
			*token_p = 0;
			++m_filepos;
			return true;
		}

		// regular token
		while ( *m_filepos > 0x20 ) {
			*token_p++ = *m_filepos++;
			if ( m_filepos == m_fileend )
				break;
			byte c = *m_filepos;
			if ( isSingleChar( c ) )
				break;
			if ( token_p == &m_token[CONFIG_MAX_TOKEN] ) {
				m_parseState = CONFIG_PARSE_ERROR;
				return false;
			}
		}
	}

	*token_p = 0;
	return true;
}

bool MDTRA_ConfigFile :: matchToken( const char *match )
{
	parseToken( true );
	return ( strcmp( m_token, match ) == 0 );
}

void MDTRA_ConfigFile :: rewind( void )
{
	m_filepos = m_filebase;
	m_tokenReady = false;
	m_parseState = CONFIG_PARSE_NORMAL;
}