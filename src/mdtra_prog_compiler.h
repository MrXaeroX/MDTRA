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
#ifndef MDTRA_PROG_COMPILER_H
#define MDTRA_PROG_COMPILER_H

#include <QtCore/QList>
#include <QtCore/QString>

typedef struct stMDTRA_Compiler_Message {
	int						number;
	int						level;
	int						line;
	QString					string;
} MDTRA_Compiler_Message;

class MDTRA_Program_Compiler
{
public:
	MDTRA_Program_Compiler();
	~MDTRA_Program_Compiler();

	bool Compile( const QString& source );
	QString Logo( void );

public:
	void Error( int line, const QString& str );
	void Error( int line, const char* str );
	void Message( int line, const QString& str );
	void Message( int line, const char* str );

	int GetMessageCount( void ) const { return m_Messages.count(); };
	const QString GetMessage( int mNumber ) const;
	unsigned int GetMessageTag( int mNumber ) const { return (m_Messages.at(mNumber).level << 16)|(m_Messages.at(mNumber).line & 0xFFFF); }

	int GetByteCodeSize( void ) const { return m_iByteCodeSize; }
	const byte* GetByteCode( void ) const { return (byte*)m_pByteCodeBuffer; }

private:
	void Cleanup( void );
	void SortMessages( void );
	void LuaError( struct stMDTRA_ProgState& pr );

private:
	QList<MDTRA_Compiler_Message> m_Messages;
	char*			m_pByteCodeBuffer;
	int				m_iByteCodeSize;
	bool			m_bSuccess;
};


#endif //MDTRA_PROG_COMPILER_H