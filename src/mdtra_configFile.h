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
#ifndef MDTRA_CONFIGFILE_H
#define MDTRA_CONFIGFILE_H

typedef enum {
	CONFIG_TYPE_UNKNOWN = 0,
	CONFIG_TYPE_DONTCARE,
	CONFIG_TYPE_HBPARMS,
	CONFIG_TYPE_HBRES
} eConfigType;

typedef enum {
	CONFIG_PARSE_NORMAL = 0,
	CONFIG_PARSE_ERROR,
	CONFIG_PARSE_EOF
} eConfigParse;

#define CONFIG_MAX_TOKEN	512

class MDTRA_ConfigFile
{
public:
	explicit MDTRA_ConfigFile( const char *filename );
	~MDTRA_ConfigFile();

	bool isOfType( eConfigType type ) const { return ( m_type == type ); }
	bool isValid( void ) const { return ( m_filebase != NULL ); };
	bool isEOF( void ) const { return ( m_filepos == NULL ); }
	
	const char *getToken( void ) const { return m_token; }
	float getVersion( void ) const { return m_version; }

	bool parseToken( bool crossline );
	void unparseToken( void );
	bool matchToken( const char *match );
	bool tokenAvailable( void );
	void skipRestOfLine( void );
	void rewind( void );

private:
	void precache( const char *filename );
	bool isSingleChar( byte c );

private:
	eConfigType		m_type;
	float			m_version;
	eConfigParse	m_parseState;
	bool			m_tokenReady;
	byte			*m_filebase;
	byte			*m_filepos;
	byte			*m_fileend;
	char			m_token[CONFIG_MAX_TOKEN];
};

typedef void (*ConfigCallback_t)( MDTRA_ConfigFile* );

extern void MDTRA_ScanAndLoadConfigs( void );
extern void MDTRA_UnloadConfigs( void );
extern void MDTRA_ForEachConfig( eConfigType type, ConfigCallback_t func );

#endif //!MDTRA_CONFIGFILE_H
