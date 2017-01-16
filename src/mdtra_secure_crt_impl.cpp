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
#if !defined(WIN32)

#include "mdtra_main.h"
#include "mdtra_secure_crt_impl.h"

int fopen_s( FILE **f, const char *name, const char *mode ) 
{
    int ret = 0;
    *f = fopen(name, mode);
    if (!*f) ret = errno;
    return ret;
}

int strcpy_s( char *strDestination, size_t numberOfElements, const char *strSource )
{
	strcpy( strDestination, strSource );
	return 0;
}

int strcpy_s( char *strDestination, const char *strSource )
{
	strcpy( strDestination, strSource );
	return 0;
}

int strncpy_s( char *strDestination, size_t numberOfElements, const char *strSource, size_t count )
{
	strncpy( strDestination, strSource, count );
	return 0;
}

int strncpy_s( char *strDestination, const char *strSource, size_t count )
{
	strncpy( strDestination, strSource, count );
	return 0;
}

int strcat_s( char *strDestination, size_t numberOfElements, const char *strSource )
{
	strcat( strDestination, strSource );
	return 0;
}

int strcat_s( char *strDestination, const char *strSource )
{
	strcat( strDestination, strSource );
	return 0;
}

int strncat_s( char *strDestination, size_t numberOfElements, const char *strSource, size_t count )
{
	strncat( strDestination, strSource, count );
	return 0;
}

int strncat_s( char *strDestination, const char *strSource, size_t count )
{
	strncat( strDestination, strSource, count );
	return 0;
}

int strerror_s( char *buf, size_t size, int errnumber )
{
	memset( buf, 0, size );
	strncpy( buf, strerror( errnumber ), size - 1 );
	return errnumber;
}

#endif //!WIN32
