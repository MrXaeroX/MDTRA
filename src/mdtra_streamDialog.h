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
#ifndef MDTRA_STREAMDIALOG_H
#define MDTRA_STREAMDIALOG_H

#include <QtGui/QDialog>

#include "ui_streamDialog.h"

class MDTRA_MainWindow;

class MDTRA_StreamDialog : public QDialog, public Ui_streamDialog
{
	Q_OBJECT

public:
    MDTRA_StreamDialog( int index, QWidget *parent = 0 );

private slots:
	void exec_on_check_streamInput( void );
	void exec_on_accept( void );
	void add_stream_files( void );
	void add_stream_files_by_mask( void );
	void remove_stream_files( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	QString	m_currentFileDir;
	int m_iStreamIndex;
};

#endif //MDTRA_STREAMDIALOG_H