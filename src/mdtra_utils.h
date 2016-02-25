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
#ifndef MDTRA_UTILS_H
#define MDTRA_UTILS_H

extern void* UTIL_AlignedMalloc( size_t size );
extern void* UTIL_AlignedRealloc( void *baseptr, size_t size, size_t oldsize );
extern void  UTIL_AlignedFree( void *baseptr );
extern char *UTIL_Strdup( const char *s );

extern void UTIL_PlaneFromPoints( const float *v1, const float *v2, const float *v3, float *n, float *d );

extern const char *UTIL_GetXAxisTitle( int dataId, int langId );
extern const char *UTIL_GetYAxisTitle( int dataId, int langId );
extern const char *UTIL_GetYAxisUnitTitle( int dataId, int langId );
extern MDTRA_DataType UTIL_GetDataSourceTypeId( int id );
extern const char *UTIL_GetDataSourceTypeName( int id );
extern const char *UTIL_GetDataSourceShortTypeName( int id );
extern const char *UTIL_GetDataSourceTypeName( MDTRA_DataType dt );
extern bool UTIL_IsDataSourceLayoutChangeable( MDTRA_DataType dt );
extern MDTRA_Layout UTIL_GetDataSourceDefaultLayout( MDTRA_DataType dt ); 

extern bool UTIL_IsSharedPlotType( int type1, int type2 );
extern const char *UTIL_GetStatParmName( int index );
extern const char *UTIL_GetStatParmShortName( int index );
extern const char *UTIL_GetStatParmLabel( int index );

extern const char *UTIL_SymbolForAtomTitle( const char *trimmed_title );
extern bool UTIL_IsElementTitle( const char *element_title );
extern bool UTIL_SymbolMatch( const char *symbol, const char *element_title );

extern bool UTIL_strfilter(const char *filter, const char *name, int casesensitive);

extern bool UTIL_ResidueGroupMatch( const char *check, const char *ref );

extern int UTIL_BestPowerOf2( int x, int limit );
extern unsigned char UTIL_FloatToColor( int channel, float v );

extern int UTIL_Atoi( const char *str );
extern float UTIL_Atof( const char *str );

// Simple profiler
inline qword MDTRA_AppCycles( void )
{
#if defined(LINUX)
	register long long result asm("eax");
	__asm__ __volatile__(".byte 15, 49" : : : "eax", "edx");
#elif defined(WIN32)
	qword result;
	__asm
	{
		xor eax,eax
		xor edx,edx
		rdtsc                 
		mov	DWORD PTR[result+0], eax   //Read low
		mov	DWORD PTR[result+4], edx   //Read high
	}
#else
	result = 0;
#endif
	return result;
}
extern void Prof_Log( const char *func, qword cycles );

#define MDTRA_PROF_CLOCK(x)					{ x = MDTRA_AppCycles(); }
#define MDTRA_PROF_UNCLOCK(x)				{ x = MDTRA_AppCycles() - x; }

#define MDTRA_PROF_START(f)					{ const char *__func_name__ = #f; qword __prof_time__ = MDTRA_AppCycles(); 
#define MDTRA_PROF_END()					 __prof_time__ = MDTRA_AppCycles() - __prof_time__; Prof_Log( __func_name__, __prof_time__ ); }

extern const float *UTIL_Color4Sym( const char* symbol );

extern char **UTIL_ListFiles( const char *directory, const char *extension, int *numfiles );
extern void UTIL_FreeFileList( char **list );

#endif //MDTRA_UTILS_H