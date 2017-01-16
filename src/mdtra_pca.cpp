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
//	Principal component analysis implementation

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb.h"
#include "mdtra_progressDialog.h"
#include "mdtra_waitDialog.h"
#include "mdtra_pca.h"

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

float *g_pEigenVectors;
float *g_pEigenValues;
int g_iNumEigens;
int g_iNumDisplayEigens;

static MDTRA_MainWindow *s_pMainWindow;
static MDTRA_PCAData s_lpcad;
static float *s_memoryHunk = NULL;
static float *s_pCovarianceMatrix;
static float *s_pMeans;
static float *s_pJacobiTemp;

extern MDTRA_ProgressDialog *pProgressDialog;
static MDTRA_WaitDialog *pWaitDialog = NULL;

static void f_BuildCovarianceMatrix( int threadnum, int num )
{
	//Load PDB file
	MDTRA_PDB_File *pPdbFile;
	if ( (s_lpcad.workStart + num) == 0) {
		pPdbFile = s_lpcad.pStream->pdb;
	} else {
		pPdbFile = s_lpcad.tempPDB[threadnum];
		pPdbFile->load( threadnum, s_lpcad.pStream->format_identifier, s_lpcad.pStream->files.at(s_lpcad.workStart + num).toAscii(), s_lpcad.pStream->flags );
		pPdbFile->move_to_centroid();
		pPdbFile->align_kabsch( s_lpcad.pStream->pdb );
	}

	//Set PCA flag
	pPdbFile->set_flag( s_lpcad.selectionSize, s_lpcad.selectionData, PDB_FLAG_PCA );

	//s_pMeans is thread-safe
	//s_pCovarianceMatrix is NOT (we cannot allocate such huge amount of memory for each thread, sorry)
	float *pMeans = s_pMeans + s_lpcad.selectionSize*3*threadnum;

	//First, calculate partial sums into pMeans
	for (int i = 0; i < pPdbFile->getAtomCount(); i++) {
		const MDTRA_PDB_Atom *pAt = pPdbFile->fetchAtomByIndex( i );
		if ( pAt->atomFlags & PDB_FLAG_PCA ) {
			pMeans[0] += pAt->xyz[0];
			pMeans[1] += pAt->xyz[1];
			pMeans[2] += pAt->xyz[2];
			pMeans += 3;
		}
	}

	//Sync and calculate covariance for an upper-right matrix part
	ThreadLock();
	int ii = 0;
	for (int i = 0; i < pPdbFile->getAtomCount(); i++) {
		const MDTRA_PDB_Atom *pAt1 = pPdbFile->fetchAtomByIndex( i );
		if (!( pAt1->atomFlags & PDB_FLAG_PCA ))
			continue;
	
		int jj = ii;
		for (int j = i; j < pPdbFile->getAtomCount(); j++) {
			const MDTRA_PDB_Atom *pAt2 = pPdbFile->fetchAtomByIndex( j );
			if (!( pAt2->atomFlags & PDB_FLAG_PCA ))
				continue;

			for (int k = 0; k < 3; k++) {
				for (int l = k; l < 3; l++) {
					int offset = (jj*3+k)*s_lpcad.selectionSize*3 + ii*3 + l;
					assert( offset < g_iNumEigens*g_iNumEigens );
					float *pCov = s_pCovarianceMatrix + offset;
					*pCov += pAt1->xyz[k]*pAt2->xyz[l];
				}
			}
			jj++;
		}
		ii++;
	}
	ThreadUnlock();

	if (pProgressDialog) {
		pProgressDialog->advanceCurrentFile( num+1 );
		if (pProgressDialog->checkInterrupt()) {
			InterruptThreads();
		}
	}
}

static void f_FinalizeCovarianceMatrix( void )
{
	float fInvNumSnapshots = 1.0f / (float)s_lpcad.workCount;

	//Reduce mean values
	int numThreads = CountThreads();
	for (int i = 0; i < s_lpcad.selectionSize*3; i++) {
		for (int j = 1; j < numThreads; j++) {
			s_pMeans[i] += s_pMeans[i + s_lpcad.selectionSize*3*j];
		}
		s_pMeans[i] *= fInvNumSnapshots;
	}
	
	//Finalize covariance matrix
	for (int i = 0; i < s_lpcad.selectionSize; i++) {
		for (int j = i; j < s_lpcad.selectionSize; j++) {
			for (int k = 0; k < 3; k++) {
				for (int l = k; l < 3; l++) {
					float *pSrcCov = s_pCovarianceMatrix + (j*3+k)*s_lpcad.selectionSize*3 + i*3 + l;
					float fMean1 = *(s_pMeans + i*3 + l);
					float fMean2 = *(s_pMeans + j*3 + k);
					*pSrcCov = (*pSrcCov) * fInvNumSnapshots - fMean1*fMean2;
					if ( i != j || k != l ) {
						float *pDstCov = s_pCovarianceMatrix + (i*3+l)*s_lpcad.selectionSize*3 + j*3 + k;
						assert( *pDstCov == 0 );
						*pDstCov = *pSrcCov;
					}
				}
			}
		}
	}

/*	char buf[256];
	OutputDebugString("Covariance matrix\n");
	for (int i = 0; i < s_lpcad.selectionSize*s_lpcad.selectionSize*9; i++) {
		sprintf_s(buf,sizeof(buf),"%f ", s_pCovarianceMatrix[i]);
		OutputDebugString(buf);
	}
	OutputDebugString("\n");*/
}

#define JACOBI_MAXSWEEP					50
#define JACOBI_ROTATE(a,siz,i,j,k,l)	{ g=a[j*siz+i]; h=a[l*siz+k]; a[j*siz+i]=g-s*(h+g*tau); a[l*siz+k]=h+s*(g-h*tau); }

static bool f_CalcEigens( float *inputMatrix, int inputMatrixDim, float *tempBuffer, float *outEigenValues, float *outEigenVectors )
{
	float *bTemp = tempBuffer;
	float *zTemp = tempBuffer + inputMatrixDim;
	int inputMatrixDimSq = inputMatrixDim*inputMatrixDim;

	memset( outEigenVectors, 0, sizeof(float)*inputMatrixDim*3 );
	memset( zTemp, 0, sizeof(float)*inputMatrixDim );

    for( int ip = 0; ip < inputMatrixDim; ip++ ) {
		outEigenVectors[ip*inputMatrixDim+ip] = 1.0f;
		bTemp[ip] = outEigenValues[ip] = inputMatrix[ip*inputMatrixDim+ip];
	}

	for ( int i = 1; i <= JACOBI_MAXSWEEP; i++ ) {
		float sm = 0.0;

		for ( int ip = 0; ip < inputMatrixDim-1; ip++ ) {
			for ( int iq = ip+1; iq < inputMatrixDim; iq++ ) 
				sm += fabsf( inputMatrix[iq*inputMatrixDim+ip] );
		}

		if ( sm == 0.0f ) 
			return true;

		float tresh = (i < 4) ? (0.2f * sm/inputMatrixDimSq) : 0.0f;

		for ( int ip = 0; ip < inputMatrixDim-1; ip++ ) {
			for( int iq = ip+1; iq < inputMatrixDim; iq++ ) {
				float g = 100.0f * fabsf(inputMatrix[iq*inputMatrixDim+ip]);
				if ((i > 4) && fabsf(outEigenValues[ip]+g) == fabsf(outEigenValues[ip]) 
						    && fabsf(outEigenValues[iq]+g) == fabsf(outEigenValues[iq])) {
				   inputMatrix[iq*inputMatrixDim+ip] = 0.0f;
				} else if (fabs(inputMatrix[iq*inputMatrixDim+ip]) > tresh) {
					float h = outEigenValues[iq] - outEigenValues[ip];
					float t;
					
					if ((fabsf(h)+g)==fabsf(h)) {
						t = inputMatrix[iq*inputMatrixDim+ip]/h;
					} else {
						float theta = 0.5f*h/inputMatrix[iq*inputMatrixDim+ip];
						t = 1.0f/(fabsf(theta)+sqrtf(1.0f+theta*theta));
						if (theta < 0.0f) t = -t;
					}

					float c = 1.0f/sqrtf(1.0f+t*t); 
					float s = t*c; 
					float tau = s/(1.0f + c); 
					h = t*inputMatrix[iq*inputMatrixDim+ip];

					zTemp[ip] -= h; 
					zTemp[iq] += h; 
					outEigenValues[ip] -= h; 
					outEigenValues[iq] += h;
					inputMatrix[iq*inputMatrixDim+ip] = 0.0f;

					for ( int j = 0; j <= ip-1; j++ )			JACOBI_ROTATE(inputMatrix,inputMatrixDim,j,ip,j,iq);
					for ( int j = ip+1; j <= iq-1; j++ )		JACOBI_ROTATE(inputMatrix,inputMatrixDim,ip,j,j,iq);
					for ( int j = iq+1; j < inputMatrixDim; j++ )JACOBI_ROTATE(inputMatrix,inputMatrixDim,ip,j,iq,j);
					for ( int j = 0; j < inputMatrixDim; j++ )	JACOBI_ROTATE(outEigenVectors,inputMatrixDim,j,ip,j,iq);
				}

				//update wait dialog
				QApplication::processEvents();
				if (pWaitDialog->checkInterrupt())
					return false;
			}
		}

		for ( int ip = 0; ip < inputMatrixDim; ip++ ) {
			bTemp[ip] += zTemp[ip]; 
			outEigenValues[ip] = bTemp[ip]; 
			zTemp[ip] = 0.0f;
		}
    }

	//OutputDebugString("ERROR: jacobi: Too many iterations\n");
	return false;
}

static bool f_SortEigens( int inputMatrixDim, float *outEigenValues, float *outEigenVectors )
{	
	for ( int i = 0; i < inputMatrixDim-1; i++ ) {
		int k = i;
		float p = outEigenValues[k];

		for( int j = i+1; j < inputMatrixDim; j++ ) {
			if ( outEigenValues[j] >= p ) p = outEigenValues[k=j];
		}
		
		if ( k != i ) {
			outEigenValues[k] = outEigenValues[i]; 
			outEigenValues[i] = p;
			for( int j = 0; j < inputMatrixDim; j++ ) {
				float temp = outEigenVectors[k*inputMatrixDim+j];
				outEigenVectors[k*inputMatrixDim+j] = outEigenVectors[i*inputMatrixDim+j];
				outEigenVectors[i*inputMatrixDim+j] = temp;
			}
		}

		//update wait dialog
		QApplication::processEvents();
		if (pWaitDialog->checkInterrupt())
			return false;
	}

	return true;
}

bool SetupPCA( MDTRA_MainWindow *pMainWindow, const MDTRA_PCAInfo *pInfo, size_t *pOutOfMemSize )
{
	s_pMainWindow = pMainWindow;
	assert( s_pMainWindow != NULL );

	int totalThreads = CountThreads();

	if (pOutOfMemSize) *pOutOfMemSize = 0;

	s_lpcad.workStart = pInfo->trajectoryMin - 1;
	s_lpcad.workCount = pInfo->trajectoryMax - pInfo->trajectoryMin + 1;
	s_lpcad.pStream = s_pMainWindow->getProject()->fetchStreamByIndex( pInfo->streamIndex );
	if (!s_lpcad.pStream || !s_lpcad.pStream->pdb)
		return false;

	s_lpcad.selectionSize = pInfo->selectionSize;
	s_lpcad.selectionData = pInfo->selectionData;

	g_iNumEigens = s_lpcad.selectionSize*3;
	g_iNumDisplayEigens = pInfo->numDisplayPC;
	size_t numAllocFloats = g_iNumEigens*g_iNumEigens + g_iNumEigens*MDTRA_MAX(totalThreads, g_iNumEigens+3);

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	assert( s_memoryHunk == NULL );
	s_memoryHunk = (float*)malloc( numAllocFloats * sizeof(float) );
	if (!s_memoryHunk) {
		if (pOutOfMemSize) *pOutOfMemSize = numAllocFloats*sizeof(float);
		QApplication::restoreOverrideCursor();
		return false;
	}
	memset(s_memoryHunk,0,numAllocFloats*sizeof(float));

	s_pCovarianceMatrix = s_memoryHunk;
	s_pMeans = s_memoryHunk + g_iNumEigens*g_iNumEigens;

	g_pEigenValues = s_memoryHunk + g_iNumEigens*g_iNumEigens;
	g_pEigenVectors = g_pEigenValues + g_iNumEigens;
	s_pJacobiTemp = g_pEigenVectors + g_iNumEigens*g_iNumEigens;

	//allocate thread memory
	for (int i = 0; i < CountThreads(); i++) {
		s_lpcad.tempPDB[i] = new MDTRA_PDB_File;
		if (!s_lpcad.tempPDB[i])
			return false;
	}

	QApplication::restoreOverrideCursor();
	return true;
}

bool PerformPCA( void )
{
	//Build covariance matrix
	MDTRA_ProgressDialog dlgProgress( s_pMainWindow );
	dlgProgress.setStreamCount( 1 );
	dlgProgress.show();
	pProgressDialog = &dlgProgress;

	dlgProgress.setFileCount( s_lpcad.workCount );
	dlgProgress.setCurrentStream( 0 );
	dlgProgress.setCurrentFile( 0 );
	
	RunThreadsOnIndividual( s_lpcad.workCount, f_BuildCovarianceMatrix );

	if (dlgProgress.checkInterrupt())
		return false;

	dlgProgress.setProgressAtMax();

	f_FinalizeCovarianceMatrix();

	dlgProgress.hide();
	QApplication::processEvents();

	MDTRA_WaitDialog dlgWait( s_pMainWindow );
	dlgWait.setMessage( QObject::tr("Calculating eigenvectors and eigenvalues, please wait...") );
	dlgWait.show();
	pWaitDialog = &dlgWait;
	QApplication::processEvents();

	//Get eigen values
	if (!f_CalcEigens( s_pCovarianceMatrix, g_iNumEigens, s_pJacobiTemp, g_pEigenValues, g_pEigenVectors )) {
		dlgWait.hide();
		return false;
	}

	dlgWait.setMessage( QObject::tr("Sorting eigenvectors and eigenvalues, please wait...") );
	QApplication::processEvents();

	if (!f_SortEigens( g_iNumEigens, g_pEigenValues, g_pEigenVectors )) {
		dlgWait.hide();
		return false;
	}

	pWaitDialog = NULL;
	dlgWait.hide();
	QApplication::processEvents();

	return true;
}

void FreePCA( void )
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	for (int i = 0; i < CountThreads(); i++) {
		if (s_lpcad.tempPDB[i]) {
			delete s_lpcad.tempPDB[i];
			s_lpcad.tempPDB[i] = NULL;
		}
	}

	if (s_memoryHunk) {
		free( s_memoryHunk );
		s_memoryHunk = NULL;
	}

	g_pEigenValues = NULL;
	g_pEigenVectors = NULL;
	g_iNumEigens = 0;

	QApplication::restoreOverrideCursor();
}