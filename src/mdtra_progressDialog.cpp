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

// Purpose:
//	Implementation of MDTRA_ProgressDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_progressDialog.h"

#include <QtGui/QMessageBox>

MDTRA_ProgressDialog :: MDTRA_ProgressDialog( QWidget *parent )
					  : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/build.png") );

	streamProgress->setValue(0);
	currentProgress->setValue(0);
	currentProgress->setMaximum(100);

	m_iStreamCount = 0;
	m_iFileCount = 0;
	m_iCurrentStream = 0;
	m_iCurrentFile = 0;
	m_iCurrentPercent = -1;
	m_bInterrupt = false;
	m_bUpdateGUI = false;

	connect(pushButton, SIGNAL(clicked()), this, SLOT(set_interrupt()));
}

void MDTRA_ProgressDialog :: setStreamCount( int value )
{
	m_iStreamCount = value;
	lblStreams->setText( tr("Stream: %1/%2").arg(m_iCurrentStream).arg(m_iStreamCount) );
	streamProgress->setMaximum( value );
	QApplication::processEvents();
}

void MDTRA_ProgressDialog :: setFileCount( int value )
{
	m_iFileCount = value;
	m_iCurrentPercent = (m_iCurrentFile * 100 / m_iFileCount);
	lblCurrent->setText( tr("Current Stream: %1%").arg(m_iCurrentPercent) );
	QApplication::processEvents();
}

void MDTRA_ProgressDialog :: setCurrentStream( int value )
{
	m_iCurrentStream = value + 1;
	if (m_iCurrentStream > m_iStreamCount) m_iCurrentStream = m_iStreamCount;
	lblStreams->setText( tr("Stream: %1/%2").arg(m_iCurrentStream).arg(m_iStreamCount) );
	streamProgress->setValue( value );
	QApplication::processEvents();
}

void MDTRA_ProgressDialog :: setCurrentFile( int value )
{
	m_iCurrentFile = value;
	m_iCurrentPercent = (m_iCurrentFile * 100 / m_iFileCount);
	lblCurrent->setText( tr("Current Stream: %1%").arg(m_iCurrentPercent) );
	currentProgress->setValue( m_iCurrentPercent );
	QApplication::processEvents();
}

void MDTRA_ProgressDialog :: setProgressAtMax( void )
{
	lblStreams->setText( tr("Stream: %1/%2").arg(m_iCurrentStream).arg(m_iStreamCount) );
	streamProgress->setValue( m_iCurrentStream );
	m_iCurrentPercent = 100;
	lblCurrent->setText( tr("Current Stream: %1%").arg(m_iCurrentPercent) );
	currentProgress->setValue( m_iCurrentPercent );
	QApplication::processEvents();
}

void MDTRA_ProgressDialog :: advanceCurrentFile( int value )
{
	if (value <= m_iCurrentFile)
		return;

	m_iCurrentFile = value;
	int iPercent = (m_iCurrentFile * 100 / m_iFileCount);
	if (iPercent > m_iCurrentPercent) {
		ThreadLock();
		m_iCurrentPercent = iPercent;
		m_bUpdateGUI = true;
		if (CountThreads() <= 1) {
			updateGUI();
			QApplication::processEvents();
		}
		ThreadUnlock();
	}
}

void MDTRA_ProgressDialog :: set_interrupt( void )
{
	m_bInterrupt = true;
}

void MDTRA_ProgressDialog :: updateGUI( void )
{
	if (!m_bUpdateGUI)
		return;

	lblCurrent->setText( tr("Current Stream: %1%").arg(m_iCurrentPercent) );
	currentProgress->setValue( m_iCurrentPercent );
	m_bUpdateGUI = false;
}

void MDTRA_ProgressBarWrapper :: setValue( int value )
{
	if (value <= m_iValue)
		return;

	m_iValue = value;
	int iPercent = (m_iValue * 100 / m_iMaxValue);
	if (iPercent > m_iCurrentPercent) {
		ThreadLock();
		m_iCurrentPercent = iPercent;
		m_bUpdateGUI = true;
		if (m_bSingleThreaded || CountThreads() <= 1) {
			updateGUI();
			QApplication::processEvents();
		}
		ThreadUnlock();
	}
}

void MDTRA_ProgressBarWrapper :: updateGUI( void )
{
	if (!m_bUpdateGUI || !m_pBar)
		return;

	m_pBar->setValue( m_iCurrentPercent );
	m_bUpdateGUI = false;
}