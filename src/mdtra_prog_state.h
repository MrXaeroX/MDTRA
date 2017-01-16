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
#ifndef MDTRA_PROG_STATE_H
#define MDTRA_PROG_STATE_H

class MDTRA_PDB_File;

#define MDTRA_PSIF_COMPILE			(1<<0)	//Lua functions are compiled, not executed -- skip runtime checks
#define MDTRA_PSIF_REDUCE			(1<<1)	//Executing a reduce function
#define MDTRA_PSOF_DATAMODIFIED		(1<<0)	//"datawrite" was called
#define MDTRA_PSOF_ALIGNED			(1<<1)	//pdb was aligned to ref at this frame

typedef struct stMDTRA_ProgState {
	int			index;
	lua_State*	luaState;
	void*		pdbFile;
	void*		pdbRef;
	void*		dataSrc;
	float*		dataPtr;
	int			dataPos;
	int			dataSize;
	float*		resPtr;
	int			resSize;
	int			args[MAX_DATA_SOURCE_ARGS];
	int			inputFlags;
	int			outputFlags;
} MDTRA_ProgState;

extern void PR_OpenState( MDTRA_ProgState& state, int* args, int flags );
extern void PR_CloseState( MDTRA_ProgState& state );
extern void PR_ReduceState( MDTRA_ProgState* threadstates, int numthreadstates );

#endif //MDTRA_PROG_STATE_H