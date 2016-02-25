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
#ifndef MDTRA_SECURE_CRT_IMPL_H
#define MDTRA_SECURE_CRT_IMPL_H
#if !defined(WIN32)

#define sprintf_s(buffer, buffer_size, stringbuffer, ...)	sprintf(buffer, stringbuffer, __VA_ARGS__)
#define fprintf_s(stream, format, ...)				fprintf(stream, format, __VA_ARGS__)
#define tmpnam_s(str)						tmpnam(str)

extern int fopen_s( FILE **f, const char *name, const char *mode );
extern int strcpy_s( char *strDestination, size_t numberOfElements, const char *strSource );
extern int strcpy_s( char *strDestination, const char *strSource );
extern int strncpy_s( char *strDestination, size_t numberOfElements, const char *strSource, size_t count );
extern int strncpy_s( char *strDestination, const char *strSource, size_t count );
extern int strcat_s( char *strDestination, size_t numberOfElements, const char *strSource );
extern int strcat_s( char *strDestination, const char *strSource );
extern int strncat_s( char *strDestination, size_t numberOfElements, const char *strSource, size_t count );
extern int strncat_s( char *strDestination, const char *strSource, size_t count );
extern int strerror_s( char *buf, size_t size, int errnumber );

#endif //!WIN32
#endif //MDTRA_SECURE_CRT_IMPL_H
