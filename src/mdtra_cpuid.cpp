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
#include "mdtra_main.h"

bool g_bSupportsSSE = false;

#if defined(MDTRA_ALLOW_SSE)
bool g_bAllowSSE = false;
#endif

static bool cpuid(unsigned long function, unsigned long& out_eax, unsigned long& out_ebx, unsigned long& out_ecx, unsigned long& out_edx)
{
#if defined(LINUX)
	asm("pushl %%ebx\n\t" "cpuid\n\t" "movl %%ebx,%%esi\n\t" "pop %%ebx": "=a" (out_eax), "=S" (out_ebx), "=c" (out_ecx), "=d" (out_edx) : "a" (function));
	return true;
#elif defined(WIN32)
	bool retval = true;
	unsigned long local_eax, local_ebx, local_ecx, local_edx;
	_asm pushad;

	__try
	{
        _asm
		{
			xor edx, edx
            mov eax, function
            cpuid
            mov local_eax, eax
            mov local_ebx, ebx
            mov local_ecx, ecx
            mov local_edx, edx
		}
    } 
	__except(EXCEPTION_EXECUTE_HANDLER) 
	{ 
		retval = false; 
	}

	out_eax = local_eax;
	out_ebx = local_ebx;
	out_ecx = local_ecx;
	out_edx = local_edx;

	_asm popad

	return retval;
#else
#error Unsupported platform
#endif
}

void CheckCPU( void )
{
	g_bSupportsSSE = false;

    unsigned long eax,ebx,ecx,edx;
    if( !cpuid(1,eax,ebx,ecx,edx) )
		return;

    g_bSupportsSSE = (( edx & 0x2000000L ) != 0);
}