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
//	Implementation of MDTRA_GPUInfoDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_cuda.h"
#include "mdtra_gpuInfoDialog.h"

#if defined(MDTRA_ALLOW_CUDA)
#include <cuda.h>
#include <cuda_runtime_api.h>
#endif

MDTRA_GPUInfoDialog :: MDTRA_GPUInfoDialog( QWidget *parent )
					   : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowFlags( Qt::Drawer | Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint );
	setWindowIcon( QIcon(":/png/16x16/nvidia.png") );
	label->setPixmap( QPixmap(":/png/nvidia.png" ) );

	m_pDeviceProps = NULL;

#if defined(MDTRA_ALLOW_CUDA)
	//fill GPU list
	comboBox->clear();

	int deviceCount;
	if ( cudaGetDeviceCount( &deviceCount ) != cudaSuccess )
		return;
	if ( !deviceCount )
		return;

	m_pDeviceProps = new cudaDeviceProp[deviceCount];
	memset( m_pDeviceProps, 0, sizeof(cudaDeviceProp)*deviceCount );
	cudaDeviceProp* pProp = (cudaDeviceProp*)m_pDeviceProps;

	for ( int i = 0; i < deviceCount; i++, pProp++ ) {
		if ( cudaGetDeviceProperties( pProp, i ) != cudaSuccess )
			continue;
		comboBox->addItem( QString("DEVICE %1: %2").arg(i).arg(pProp->name), i );
		if ( i == m_pMainWindow->getCUDADevice() ) {
			comboBox->setCurrentIndex( i );
			checkBox->setChecked( true );
			PrintDeviceInfo( i );
		}
	}

	connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_gpu_change()));
#endif
	m_skipCheckboxEvent = false;
	connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(exec_on_checkbox_change()));
}

MDTRA_GPUInfoDialog :: ~MDTRA_GPUInfoDialog()
{
#if defined(MDTRA_ALLOW_CUDA)
	if (m_pDeviceProps) {
		delete [] ((cudaDeviceProp*)m_pDeviceProps);
		m_pDeviceProps = NULL;
	}
#endif
}

void MDTRA_GPUInfoDialog :: PrintDeviceInfo( int deviceIndex )
{
#if defined(MDTRA_ALLOW_CUDA)
	cudaDeviceProp* pProp = (cudaDeviceProp*)m_pDeviceProps + deviceIndex;
	QString strDeviceInfo = QString( 
		"<b>General Information</b><br/>"
		"Name: %1<br/>"
		"Type: %2<br/>"
		"Compute Capability: %3.%4<br/>"
		"ALU Clock Rate: %5 MHz<br/>"
		"Device Overlap: %6<br/>"
		"Kernel Timeout: %7<br/>"
		"Concurrent Kernels: %8<br/><br/>"
		"<b>Memory Information</b><br/>"
		"Global Memory: %9 Mb<br/>"
		"Constant Memory: %10 kb<br/>"
		"Shared Memory (per MP): %11 kb<br/>"
		"Bus Width: %12-bit<br/>"
		"Texture Alignment: %13<br/>"
		"Unified Addressing: %14<br/>"
		"Host Memory Mapping: %15<br/><br/>"
		"<b>Multiprocessor Information</b><br/>"
		"Multiprocessor Count: %16<br/>"
		"Warp Size: %17<br/>"
		"Register Count (per MP): %18<br/>"
		"Maximum Threads (per block): %19<br/>"
		)
		.arg(pProp->name)
		.arg(pProp->integrated?"Integrated":"Discrete")
		.arg(pProp->major)
		.arg(pProp->minor)
		.arg((dword)ceil(pProp->clockRate/1000.0f))
		.arg(pProp->deviceOverlap?"yes":"no")
		.arg(pProp->kernelExecTimeoutEnabled?"yes":"no")
		.arg(pProp->concurrentKernels?"yes":"no")
		.arg((dword)ceil(pProp->totalGlobalMem / (1024.0f*1024.0f)))
		.arg((dword)ceil(pProp->totalConstMem / 1024.0f))
		.arg((dword)ceil(pProp->sharedMemPerBlock / 1024.0f))
		.arg(pProp->memoryBusWidth)
		.arg(pProp->textureAlignment)
		.arg(pProp->unifiedAddressing?"yes":"no")
		.arg(pProp->canMapHostMemory?"yes":"no")
		.arg(pProp->multiProcessorCount)
		.arg(pProp->warpSize)
		.arg(pProp->regsPerBlock)
		.arg(pProp->maxThreadsPerBlock)
		;

	textEdit->setHtml(strDeviceInfo);
#endif
}

void MDTRA_GPUInfoDialog :: exec_on_gpu_change( void )
{
	PrintDeviceInfo( comboBox->itemData( comboBox->currentIndex(), Qt::UserRole ).toInt() );
	exec_on_checkbox_change();
}

void MDTRA_GPUInfoDialog :: exec_on_checkbox_change( void )
{
	if ( !m_skipCheckboxEvent ) {
		bool bChecked = false;
		if ( comboBox->count() > 0 && 
			 comboBox->itemData( comboBox->currentIndex(), Qt::UserRole ).toInt() == m_pMainWindow->getCUDADevice() )
			 bChecked = true;

		m_skipCheckboxEvent = true;
		checkBox->setChecked( bChecked );
		m_skipCheckboxEvent = false;
	}
}
