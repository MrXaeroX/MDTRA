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
//	Implementation of MDTRA_Program_Interpreter

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_progressDialog.h"
#include "mdtra_prog_state.h"
#include "mdtra_prog_interpreter.h"

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

//must not be enabled! only for debug!!
#define SHOW_ERROR_MSG

MDTRA_Program_Interpreter :: MDTRA_Program_Interpreter( const MDTRA_DataSource* ds )
{
	m_pcode = (const char*)ds->prog.byteCode;
	m_isize = ds->prog.byteCodeSize;
	m_states = NULL;
	m_valid = false;
	m_datamodified = false;
	m_dsName = ds->name;
	m_dsIndex = ds->index;

	for (int i = 0; i < MAX_DATA_SOURCE_ARGS; i++)
		m_args[i] = ds->arg[i].atomIndex;
}

MDTRA_Program_Interpreter :: ~MDTRA_Program_Interpreter()
{
	if ( m_states ) {
		for ( int i = 0; i < CountThreads(); i++ )
			PR_CloseState( m_states[i] );
		delete [] m_states;
	}
}

bool MDTRA_Program_Interpreter :: Init( void* pdbFile, int dataSize )
{
	m_valid = false;
	m_reduce = false;

	if ( !m_pcode || !m_isize )
		return false;

	// allocate states
	lua_State* L;
	m_states = new MDTRA_ProgState[ CountThreads() ];
	assert( m_states != NULL );
	memset( m_states, 0, sizeof(MDTRA_ProgState)*CountThreads() );

	for ( int i = 0; i < CountThreads(); i++ ) {
		m_states[i].index = i;
		PR_OpenState( m_states[i], m_args, 0 );
		L = m_states[i].luaState;
		lua_setuserdata( L, &m_states[i] );
		m_states[i].pdbRef = pdbFile;
		m_states[i].dataSize = dataSize;

		if ( luaL_loadbinary( L, m_pcode, m_isize, NULL ) ) {
			QMessageBox::warning( NULL, QString(APPLICATION_TITLE_SMALL), "Failed to load program");
			return false;
		}
		if ( lua_pcall(L, 0, LUA_MULTRET, 0) ) {
			QMessageBox::warning( NULL, QString(APPLICATION_TITLE_SMALL), "Failed to initialize program");
			return false;
		}
	}

	L = m_states[0].luaState;
	lua_getglobal( L, "reduce" );
	if ( lua_isfunction( L, -1 ) )
		m_reduce = true;

	lua_settop(L, 0);
	m_valid = true;
	return true;
}

void MDTRA_Program_Interpreter :: RunError( lua_State* L )
{
	extern MDTRA_ProgressDialog *pProgressDialog;

	ThreadLock();
	m_valid = false;

#ifdef SHOW_ERROR_MSG
	const char *msg = lua_tostring( L, -1 );
	int line = 0;
	if (!msg) {
		msg = "(error object is not a string)";
	} else {
		line = atoi(msg);
		while (*msg != ' ') msg++;
		msg++;
	}
	QString msgstring = QString("Error in user-defined program!\n\nData Source name: %1\nData Source index: %2\nError message: %3").arg(m_dsName).arg(m_dsIndex).arg(msg);
#else
	QString msgstring = QString("Error in user-defined program!\n\nData Source name: %1\nData Source index: %2").arg(m_dsName).arg(m_dsIndex);
#endif
	ThreadUnlock();

	QMessageBox::warning( pProgressDialog, APPLICATION_TITLE_SMALL, msgstring );
}

bool MDTRA_Program_Interpreter :: Main( int threadnum, int num, void* pdbFile, float* presult )
{
	//setup current state
	m_states[threadnum].pdbFile = pdbFile;
	m_states[threadnum].dataPos = num + 1;
	m_states[threadnum].outputFlags = 0;

	lua_State* L = m_states[threadnum].luaState;
	lua_getglobal( L, "main" );
	if ( lua_pcall(L, 0, 1, 0) ) {
		RunError( L );
		if ( presult ) *presult = 0.0f;
		return false;
	}

	if ( presult ) *presult = (float)lua_tonumber(L, -1);
	lua_settop(L, 0);
	return true;
}

bool MDTRA_Program_Interpreter :: Reduce( float* pvalues, dword* presultmask, float* presults )
{
	//HARD LIMIT: 6 parms
	const int cMaxUserResults = 6;

	dword outMask = 0;
	float outResults[cMaxUserResults];	

	//setup current state
	m_datamodified = false;
	m_states[0].pdbFile = NULL;
	m_states[0].outputFlags = 0;
	m_states[0].resPtr = NULL;
	m_states[0].resSize = 0;
	memset( outResults, 0, sizeof(outResults) );

	//collect globals from all threads
	PR_ReduceState( m_states, CountThreads() );
	m_states[0].dataPtr = pvalues;

	lua_State* L = m_states[0].luaState;
	lua_getglobal( L, "reduce" );
	if ( lua_pcall(L, 0, cMaxUserResults, 0) ) {
		RunError( L );
		if ( presultmask ) *presultmask = outMask;
		if ( presults ) memcpy( presults, outResults, sizeof(outResults) );
		return false;
	}

	if (m_states[0].outputFlags & MDTRA_PSOF_DATAMODIFIED)
		m_datamodified = true;

	//get all results
	for ( int i = cMaxUserResults-1; i >= 0; i--) {
		if ( !lua_isnumber( L, -1 ) ) {
			lua_pop(L, 1);
			continue;
		}
		outResults[i] = lua_tonumber( L, -1 );
		lua_pop(L, 1);
		outMask |= (1 << (i));
	}

	lua_settop(L, 0);

	if ( presultmask ) *presultmask = outMask;
	if ( presults ) memcpy( presults, outResults, sizeof(outResults) );
	return true;
}

void MDTRA_Program_Interpreter :: SetResidueBuffer( int threadnum, float *res, int rescount )
{
	m_states[threadnum].resPtr = res;
	m_states[threadnum].resSize = rescount;
}