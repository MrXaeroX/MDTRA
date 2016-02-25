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
//	Implementation of MDTRA_Program_Compiler

#include "mdtra_main.h"
#include "mdtra_prog_state.h"
#include "mdtra_prog_compiler.h"

static MDTRA_Program_Compiler* pLocalCompiler = NULL;
static int luaB_compiler_print(lua_State *L) 
{
	const char *msg = lua_tostring( L, -1 );
	int line = 0;
	if (!msg) {
		msg = "(error object is not a string)";
	} else {
		line = atoi(msg);
		while (*msg != ' ') msg++;
		msg++;
	}
	if (pLocalCompiler)
		pLocalCompiler->Message( line, msg );
	return 0;
}

MDTRA_Program_Compiler :: MDTRA_Program_Compiler()
{
	m_pByteCodeBuffer = NULL;
	m_iByteCodeSize = 0;
}

MDTRA_Program_Compiler :: ~MDTRA_Program_Compiler()
{
	if ( m_pByteCodeBuffer ) {
		lua_free( m_pByteCodeBuffer );
		m_pByteCodeBuffer = NULL;
	}
	m_iByteCodeSize = 0;
	Cleanup();
}

void MDTRA_Program_Compiler :: Error( int line, const QString& str )
{
	MDTRA_Compiler_Message msg;
	msg.number = m_Messages.count();
	msg.level = 0;
	msg.line = line;
	msg.string = str;
	m_bSuccess = false;
	m_Messages << msg;
}

void MDTRA_Program_Compiler :: Error( int line, const char* str )
{
	MDTRA_Compiler_Message msg;
	msg.number = m_Messages.count();
	msg.level = 0;
	msg.line = line;
	msg.string = QString( str );
	m_bSuccess = false;
	m_Messages << msg;
}

void MDTRA_Program_Compiler :: Message( int line, const QString& str )
{
	MDTRA_Compiler_Message msg;
	msg.number = m_Messages.count();
	msg.level = 1;
	msg.line = line;
	msg.string = str;
	m_Messages << msg;
}

void MDTRA_Program_Compiler :: Message( int line, const char* str )
{
	MDTRA_Compiler_Message msg;
	msg.number = m_Messages.count();
	msg.level = 1;
	msg.line = line;
	msg.string = QString( str );
	m_Messages << msg;
}

const QString MDTRA_Program_Compiler :: GetMessage( int mNumber ) const
{
	const MDTRA_Compiler_Message& msg = m_Messages.at(mNumber);
	QString htmlMsg( msg.string );
	htmlMsg.replace("<", "&lt;");
	htmlMsg.replace(">", "&gt;");

	if ( msg.line ) 
		return QString("%1 (%2): %3").arg( msg.level ? "Message" : "<b>Error</b>" ).arg(msg.line).arg(htmlMsg);
	else
		return QString("%1: %2").arg( msg.level ? "Message" : "<b>Error</b>" ).arg(htmlMsg);
}

void MDTRA_Program_Compiler :: SortMessages( void )
{
	struct qMessageSortFunctor {
		static bool sortFunc(const MDTRA_Compiler_Message &t1, const MDTRA_Compiler_Message &t2) {
			if (t1.line == t2.line)
				return (t1.number < t2.number);
			else
				return (t1.line < t2.line);
		}
	};
	if (m_Messages.count() > 1)
		qSort(m_Messages.begin(), m_Messages.end(), &(qMessageSortFunctor::sortFunc));
}

void MDTRA_Program_Compiler :: Cleanup( void )
{
	m_Messages.clear();
	m_bSuccess = true;
}

QString MDTRA_Program_Compiler :: Logo( void )
{
	return QString("<b>%1 Compiler</b><br/>%2<br/>%3<br/>%4")
				.arg(LUA_RELEASE)
				.arg(LUA_COPYRIGHT)
				.arg(LUA_AUTHORS)
				.arg("Modified by Alexander V. Popov, 2011-2015");
}

void MDTRA_Program_Compiler :: LuaError( MDTRA_ProgState& pr )
{
	const char *msg = lua_tostring( pr.luaState, -1 );
	int line = 0;
	if (!msg) {
		msg = "(error object is not a string)";
	} else {
		line = atoi(msg);
		while (*msg != ' ') msg++;
		msg++;
	}
	Error( line, msg );
}

bool MDTRA_Program_Compiler :: Compile( const QString& source )
{
	// global cleanup
	Cleanup();

	// check for empty code string
	if ( !source.length() ) {
		Error( 0, "program is empty" );
		return false;
	}

	// make an editable copy of code string
	QByteArray sourceBytes = source.toAscii();
	int parserNumBytes = sourceBytes.count() + 1;
	char* parserBytes = new char[parserNumBytes];
	memcpy( parserBytes, sourceBytes.constData(), sourceBytes.count() );
	parserBytes[parserNumBytes-1] = '\0';

	// open program state
	MDTRA_ProgState pr;
	memset( &pr, 0, sizeof(pr) );
	float dataval = 0;
	float resval = 0;
	PR_OpenState( pr, NULL, MDTRA_PSIF_COMPILE );
	pr.dataPos = 1;
	pr.dataSize = 1;
	pr.dataPtr = &dataval;
	pr.resSize = 1;
	pr.resPtr = &resval;
	lua_setuserdata( pr.luaState, &pr );
	luaB_printevent( luaB_compiler_print );
	pLocalCompiler = this;

	// load lua code
	if ( luaL_loadbuffer( pr.luaState, parserBytes, parserNumBytes-1, NULL ) ) {
		LuaError( pr );
		m_bSuccess = false;
	} else if ( lua_pcall( pr.luaState, 0, LUA_MULTRET, 0 ) ) {
		LuaError( pr );
		m_bSuccess = false;
	} else {
		lua_getglobal( pr.luaState, "main" );
		if ( !lua_isfunction( pr.luaState, -1 ) ) {
			lua_pop( pr.luaState, 1 );
			Error( 0, "entry function 'main' was not defined" );
			m_bSuccess = false;
		} else if (lua_pcall( pr.luaState, 0, LUA_MULTRET, 0 )) {
			LuaError( pr );
			m_bSuccess = false;
		}
		lua_getglobal( pr.luaState, "reduce" );
		if ( lua_isfunction( pr.luaState, -1 ) ) {
			PR_ReduceState( &pr, 1 );
			if (lua_pcall( pr.luaState, 0, LUA_MULTRET, 0 )) {
				LuaError( pr );
				m_bSuccess = false;
			}
		}
	}

	if (m_bSuccess) {
		m_iByteCodeSize = lua_compile( pr.luaState, parserBytes, &m_pByteCodeBuffer, 1 );
		if ( !m_iByteCodeSize ) {
			lua_free( m_pByteCodeBuffer );
			m_pByteCodeBuffer = NULL;
			m_bSuccess = false;
		}
	}

	PR_CloseState( pr );

	// sort compiler messages
	SortMessages();

	// local cleanup
	delete [] parserBytes;
	pLocalCompiler = NULL;

	return m_bSuccess;
}
