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
//	Implementation of MDTRA_Pipe

#include "mdtra_main.h"
#include "mdtra_pipe.h"

#include <QtGui/QApplication>
#include <QtGui/QPlainTextEdit>

#if defined(WIN32)
static const QString s_OSName( "Windows" );
#elif defined(LINUX)
static const QString s_OSName( "Linux" );
#else
static const QString s_OSName( "Unknown OS" );
#endif

#if !defined(WIN32)
static FILE *custom_popen_read( char *cmd, long *tid ) 
{ 
	static char cmdbuf[8192];
	int p[2]; 
	FILE *fp; 

	memset( cmdbuf, 0, sizeof(cmdbuf) );
	strncpy_s( cmdbuf, cmd, sizeof(cmdbuf)-1 );

	const int cmdArgcMax = 127;
	int cmdArgc = 0;
	char *cmdArgv[cmdArgcMax+1];
	char *lpCmdLine = cmdbuf;

	// Extract argc and argv
	memset( cmdArgv, 0, sizeof(cmdArgv) );
	cmdArgc = 0;

	bool in_quote = false;
	while ( *lpCmdLine && ( cmdArgc < cmdArgcMax ) ) {
		while ( *lpCmdLine && ( (byte)(*lpCmdLine) <= 32 ) )
			++lpCmdLine;

		if ( *lpCmdLine ) {
			if ( *lpCmdLine == '"' ) {
				in_quote = !in_quote;
				++lpCmdLine;
			}

			cmdArgv[cmdArgc] = lpCmdLine;
			cmdArgc++;

			while ( *lpCmdLine && ( ( (byte)(*lpCmdLine) > 32 ) || in_quote ) ) {
				if ( in_quote && ( *lpCmdLine == '"' ) ) {
					in_quote = !in_quote;
					break;
				}
				++lpCmdLine;
			}

			if ( *lpCmdLine ) {
				*lpCmdLine = 0;
				++lpCmdLine;
			}
		}
	}

	cmdArgv[cmdArgc] = NULL;

	//ensure cmd exists
	if (fopen_s( &fp, cmdArgv[0], "rb" ) )
		return NULL;
	fclose( fp );

	if ( pipe( p ) < 0 ) 
		return NULL; 

	if ( ( *tid = fork() ) > 0 ) {
		close( p[1] ); 
		fp = fdopen( p[0], "r" ); 
		return fp; 
	} else if ( *tid == 0 ) { 
		setpgid( 0, 0 ); 
		fflush( stdout ); 
		fflush( stderr );
		close( 1 ); 
		if ( dup( p[1] ) < 0 ) 
			perror( "dup of write side of pipe failed" );
		close( 2 );
		if ( dup( p[1] ) < 0 ) 
			perror("dup of write side of pipe failed");

		close( p[0] );
		close( p[1] );
		execve( cmdArgv[0], cmdArgv, environ );
	} else {
		close( p[0] ); 
		close( p[1] ); 
	} 
	return NULL; 
}
#endif

MDTRA_Pipe :: MDTRA_Pipe( const QString &cmdName, const QString &cmdArgs, QPlainTextEdit *pOutput )
{
	m_cmdName = cmdName;
	m_cmdArgs = cmdArgs;
	m_pOutput = pOutput;
#if defined(WIN32)
	m_hProcess = INVALID_HANDLE_VALUE;
#else
	m_hProcess = 0;
#endif
}

bool MDTRA_Pipe :: exec( void )
{
	QString fullCommand;

	if ( m_cmdName.contains( ' ' ) )
		fullCommand = QString( "\"%1\" %2" ).arg( m_cmdName ).arg( m_cmdArgs );
	else
		fullCommand = QString( "%1 %2" ).arg( m_cmdName ).arg( m_cmdArgs );

	QByteArray fullCommandString = fullCommand.toLocal8Bit();
	QByteArray fullCommandProg = m_cmdName.toLocal8Bit();
	bool bSuccess = false;

#if defined(WIN32)
	SECURITY_ATTRIBUTES saAttr;
	PROCESS_INFORMATION pi; 
	STARTUPINFO si;
	HANDLE hChildStdinRd, hChildStdinWr, hChildStdoutRd, hChildStdoutWr, hChildStderrWr;
	DWORD dwCount, dwRead;

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL;

	if ( CreatePipe( &hChildStdoutRd, &hChildStdoutWr, &saAttr, 0 ) ) {
		if ( CreatePipe( &hChildStdinRd, &hChildStdinWr, &saAttr, 0 ) ) {
			if ( DuplicateHandle( GetCurrentProcess(), hChildStdoutWr, GetCurrentProcess(), &hChildStderrWr, 0, TRUE, DUPLICATE_SAME_ACCESS ) ) {
				// Create the child process
				memset( &pi, 0, sizeof(pi) );
				memset( &si, 0, sizeof(si) );
				si.cb = sizeof(si);
				si.dwFlags = STARTF_USESTDHANDLES;
				si.hStdInput = hChildStdinRd;
				si.hStdError = hChildStderrWr;
				si.hStdOutput = hChildStdoutWr;
				if ( CreateProcess( fullCommandProg.data(), fullCommandString.data(), NULL, NULL, TRUE, 
					DETACHED_PROCESS | IDLE_PRIORITY_CLASS, NULL, NULL, &si, &pi ) ) {
						m_hProcess = pi.hProcess;
						// Pump through the pipe
						bool bDone = false;
						for ( ;; ) {
							dwCount = 0;
							dwRead = 0;
							PeekNamedPipe( hChildStdoutRd, NULL, 0, NULL, &dwCount, NULL );
							if ( dwCount ) {
								dwCount = MDTRA_MIN( dwCount, sizeof(m_chBuf)-1 );
								ReadFile( hChildStdoutRd, m_chBuf, dwCount, &dwRead, NULL );
							}
							if ( dwRead ) {
								m_chBuf[dwRead] = '\0';
								if ( m_pOutput ) {
									m_pOutput->moveCursor( QTextCursor::End );
									m_pOutput->insertPlainText( m_chBuf );
									m_pOutput->moveCursor( QTextCursor::End );
								}
								g_pApp->processEvents();
							} else if ( WaitForSingleObject( m_hProcess, 100 ) != WAIT_TIMEOUT ) {
								// process termination
								if ( bDone ) break;
								bDone = true; // next time we get it
							} else {
								g_pApp->processEvents();
							}
						} 
						bSuccess = true;
				} else if ( m_pOutput ) {
					FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, m_chBuf, sizeof(m_chBuf), NULL );
					char *p = strchr( m_chBuf, '\r' );	// get rid of \r\n
					if (p) p[0] = 0;
					QString cmdErr = QString( "* Could not execute the command:\n   %1\n* %2 gave the error message:\n   \"%3\"\n" ).arg( fullCommand ).arg( s_OSName ).arg( QString::fromLocal8Bit( m_chBuf ) );
					m_pOutput->appendPlainText( cmdErr );
				}
				CloseHandle( hChildStderrWr );
			}
			CloseHandle( hChildStdinRd );
			CloseHandle( hChildStdinWr );
		}
		CloseHandle( hChildStdoutRd );
		CloseHandle( hChildStdoutWr );
	}
#else
	FILE *fpPipe = custom_popen_read( fullCommandString.data(), &m_hProcess );
	if ( fpPipe ) {
		// Read pipe until end of file, or an error occurs
		while ( fgets( m_chBuf, sizeof(m_chBuf), fpPipe ) ) {
			if ( m_pOutput ) {
				m_pOutput->moveCursor( QTextCursor::End );
				m_pOutput->insertPlainText( m_chBuf );
				m_pOutput->moveCursor( QTextCursor::End );
			}
			g_pApp->processEvents();
		}
		// Close pipe
		if ( feof( fpPipe ) ) {
			bSuccess = true;
		}
		pclose( fpPipe );
	} else if ( m_pOutput ) {
		strerror_s( m_chBuf, sizeof(m_chBuf), errno );
		QString cmdErr = QString( "* Could not execute the command:\n   %1\n* %2 gave the error message:\n   \"%3\"\n" ).arg( fullCommand ).arg( s_OSName ).arg( QString::fromLocal8Bit( m_chBuf ) );
		m_pOutput->appendPlainText( cmdErr );
	}
	m_hProcess = 0;
#endif
	return bSuccess;
}

void MDTRA_Pipe :: stop( void )
{
#if defined(WIN32)
	if ( m_hProcess != INVALID_HANDLE_VALUE )
		TerminateProcess( m_hProcess, 1 );
#else
	if ( m_hProcess )
		kill( -m_hProcess, SIGINT );
#endif
}
