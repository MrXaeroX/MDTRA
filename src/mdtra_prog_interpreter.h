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
#ifndef MDTRA_PROG_INTERPRETER_H
#define MDTRA_PROG_INTERPRETER_H

#include <QtCore/QString>
#include "mdtra_types.h"

class MDTRA_Program_Interpreter
{
public:
	MDTRA_Program_Interpreter( const struct stMDTRA_DataSource* ds );
	~MDTRA_Program_Interpreter();

	bool IsValid( void ) const { return m_valid; }
	bool IsDataModified( void ) const { return m_datamodified; }
	bool ShouldReduce( void ) const { return m_reduce; }

	bool Init( void* pdbFile, int dataSize );
	bool Main( int threadnum, int num, void* pdbFile, float* presult );
	bool Reduce( float* pvalues, dword* presultmask, float* presults );
	void SetResidueBuffer( int threadnum, float *res, int rescount );

protected:
	void RunError( struct lua_State* L );

private:
	struct stMDTRA_ProgState* m_states;
	int	m_args[MAX_DATA_SOURCE_ARGS];
	bool m_valid;
	bool m_datamodified;
	bool m_reduce;
	const char* m_pcode;
	int	m_isize;
	QString m_dsName;
	int m_dsIndex;
};

#endif //MDTRA_PROG_INTERPRETER_H