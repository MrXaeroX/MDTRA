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
#ifndef MDTRA_PROGRESSDIALOG_H
#define MDTRA_PROGRESSDIALOG_H

#include <QtGui/QDialog>

#include "ui_progressDialog.h"

class MDTRA_MainWindow;

class MDTRA_ProgressDialog : public QDialog, public Ui_progressDialog
{
	Q_OBJECT

public:
    MDTRA_ProgressDialog( QWidget *parent = 0 );
	void setStreamCount( int value );
	void setFileCount( int value );
	void setCurrentStream( int value );
	void setCurrentFile( int value );
	void advanceCurrentFile( int value );
	void setProgressAtMax( void );
	void updateGUI( void );
	bool checkInterrupt( void ) { return m_bInterrupt; }
	bool needUpdateGUI( void ) { return m_bUpdateGUI; }

private slots:
	void set_interrupt( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	int m_iStreamCount;
	int m_iFileCount;
	int m_iCurrentStream;
	int m_iCurrentFile;
	int m_iCurrentPercent;
	bool m_bInterrupt;
	bool m_bUpdateGUI;
};

class MDTRA_ProgressBarWrapper
{
public:
	MDTRA_ProgressBarWrapper() { m_pBar = NULL; m_bUpdateGUI = false; }
	void setProgressBar( QProgressBar* pBar ) { m_pBar = pBar; }
	void setSingleThreaded( bool b ) { m_bSingleThreaded = b; }
	bool needUpdateGUI( void ) { return m_bUpdateGUI; }
	void setMaximumValue( int value ) { m_iMaxValue = value; }
	void resetValue( int value ) { m_iValue = value; m_iCurrentPercent = -1; }
	void setValue( int value );
	void updateGUI( void );

private:
	QProgressBar *m_pBar;
	int m_iValue;
	int m_iMaxValue;
	int m_iCurrentPercent;
	bool m_bUpdateGUI;
	bool m_bSingleThreaded;
};

#endif //MDTRA_PROGRESSDIALOG_H