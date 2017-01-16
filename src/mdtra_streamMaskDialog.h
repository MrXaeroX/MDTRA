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
#ifndef MDTRA_STREAMMASKDIALOG_H
#define MDTRA_STREAMMASKDIALOG_H

#include <QtGui/QDialog>

#include "ui_streamMaskDialog.h"

class MDTRA_MainWindow;

class MDTRA_StreamMaskDialog : public QDialog, public Ui_streamMaskDialog
{
	Q_OBJECT

public:
    MDTRA_StreamMaskDialog( QWidget *mainwindow, QWidget *parent = 0 );
	QStringList getFilesByMask( void );

private slots:
	void set_mask( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	QString	m_currentFileDir;
};

#endif //MDTRA_STREAMMASKDIALOG_H