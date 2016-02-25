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
#ifndef MDTRA_MAIN_H
#define MDTRA_MAIN_H

// load config
#include "mdtra_config.h"

// define types
#include "mdtra_types.h"

// thread info
#include "mdtra_threads.h"

// load includes
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>
#include <cfloat>

#define MDTRA_ALLOW_SSE
#define MDTRA_ALLOW_CUDA
#define MDTRA_ALLOW_PRINTER

#if defined(WIN32)

// load Win32-specific includes
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <direct.h>
#include <io.h>

#elif defined(LINUX)

// load Linux-specific includes
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "mdtra_secure_crt_impl.h"

#else
#error Unsupported OS (only Win32 and Linux are supported)
#endif

// load Lua includes
extern "C" 
{
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

#if defined(LINUX) && !defined(_stricmp)
#define _stricmp	strcasecmp
#define _strnicmp	strncasecmp
#define _strnicmp	strncasecmp 
#define _vsnprintf	vsnprintf
#define _snprintf	snprintf
#endif
#if defined(LINUX) && !defined(_unlink)
#define _unlink		unlink
#endif

#define MDTRA_MAX( x, y )		( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )
#define MDTRA_MIN( x, y )		( ( ( x ) < ( y ) ) ? ( x ) : ( y ) )
#define MDTRA_ROUND( x )		( floor( x + 0.5f ) )

#define MAX_FOUND_FILES			8192

extern class QApplication *g_pApp;

#endif //MDTRA_MAIN_H