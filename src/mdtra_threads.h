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
#ifndef MDTRA_THREADS_H
#define MDTRA_THREADS_H

#if defined(WIN32)
#define USE_WIN32_THREADS
#elif defined(LINUX)
#define USE_POSIX_THREADS
#else
//single-threaded otherwise
#endif

#define MDTRA_MAX_THREADS	16

typedef void (*MDTRA_ThreadFunc)(int, int);

extern void ThreadSetDefault( int count, int priority );
extern void ThreadLock( void );
extern void ThreadUnlock( void );

extern void RunThreadsOn(int workcnt, MDTRA_ThreadFunc func);
extern void RunThreadsOnIndividual( int workcnt, MDTRA_ThreadFunc func );
extern void InterruptThreads( void );
extern int  CountThreads( void );

#endif //MDTRA_THREADS_H
