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
#ifndef MDTRA_PREPWATERSHELLDIALOG_H
#define MDTRA_PREPWATERSHELLDIALOG_H

#include <QtGui/QDialog>

#include "ui_prepWaterShellDialog.h"

class MDTRA_MainWindow;
class MDTRA_Pipe;

class MDTRA_PrepWaterShellDialog : public QDialog, public Ui_prepWaterShellDialog
{
	Q_OBJECT

public:
	MDTRA_PrepWaterShellDialog( QWidget *parent = 0 );
	virtual ~MDTRA_PrepWaterShellDialog();

private slots:
	void reject( void );
	void exec_on_stream_change( void );
	void exec_on_browse_output( void );
	void exec_on_check_input( void );
	void exec_on_prep( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	MDTRA_Pipe* m_pPipe;
	int	 m_cachedStreamIndex;
	bool m_bInterrupt;
	char m_szTempName[512];
};

#endif //MDTRA_PREPWATERSHELLDIALOG_H
