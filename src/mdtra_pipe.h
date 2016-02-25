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
#ifndef MDTRA_PIPE_H
#define MDTRA_PIPE_H

#include <QtCore/QString>

class QPlainTextEdit;

#define PIPE_BUFFER_SIZE	4096

class MDTRA_Pipe
{
public:
	MDTRA_Pipe( const QString &cmdName, const QString &cmdArgs, QPlainTextEdit *pOutput );
	bool exec( void );
	void stop( void );

private:
	QPlainTextEdit *m_pOutput;
	QString	m_cmdName;
	QString	m_cmdArgs;
#if defined(_WIN32)
	HANDLE m_hProcess;
#else
	long m_hProcess;
#endif
	char m_chBuf[PIPE_BUFFER_SIZE];
};

#endif //MDTRA_PIPE_H
