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
#include "mdtra_main.h"
#include "mdtra_cuda.h"
#include "mdtra_mainWindow.h"
#include <QtGui/QMessageBox>

#if defined(MDTRA_ALLOW_CUDA)
#include <cuda.h>
#include <cuda_runtime_api.h>
#endif

bool g_bAllowCUDA = false;
bool g_bSupportsCUDA = false;
int g_iComputeDevice = -1;
float g_flComputeVersionCUDA = 0.0f;

#if defined(LINUX)
bool g_bSASWarningPrinted = false;
#endif

void MDTRA_CUDA_CheckSupport( void )
{
	g_bSupportsCUDA = false;

#if defined(MDTRA_ALLOW_CUDA)
	int dc = 0;
	if ( cudaGetDeviceCount( &dc ) == cudaSuccess )
		g_bSupportsCUDA = ( dc > 0 );
#endif
}

void MDTRA_CUDA_InitDevice( int deviceNumber )
{
	g_iComputeDevice = deviceNumber;
	g_flComputeVersionCUDA = 0.0f;

#if defined(MDTRA_ALLOW_CUDA)
	cudaDeviceProp prop;
	if ( cudaGetDeviceProperties( &prop, deviceNumber ) == cudaSuccess ) {
		g_flComputeVersionCUDA = prop.major + prop.minor / 10.0f;
	}
#endif
}

void MDTRA_CUDA_ErrorMessage( const char* s, const char* f, int l )
{
	ThreadLock();
	QMessageBox::warning( NULL, "CUDA Error", QString("Reason: %1\nFile: %2\nLine: %3").arg(s).arg(f).arg(l) );
	ThreadUnlock();
}

bool MDTRA_CUDA_CanCalculateSAS( int numAtoms )
{
#if defined(MDTRA_ALLOW_CUDA)
	//warning if CM < 1.2
#if defined(LINUX)
	if ( g_flComputeVersionCUDA < 1.2 && !g_bSASWarningPrinted ) {
		g_bSASWarningPrinted = true;
		printf("WARNING: Selected CUDA device has low compute capability (%.1f)\n"
		       "         GPGPU performance may be lower than expected.\n"
			   "         It is recommended to use device with compute capability 1.2 or higher.\n",
			   g_flComputeVersionCUDA);
	}
#endif

	return ( g_bAllowCUDA && ( numAtoms <= MDTRA_CUDA_ATOMS_MAX ) );
#else
	return false;
#endif
}

