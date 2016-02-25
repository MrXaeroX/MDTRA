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
#ifndef MDTRA_RESULTDIALOG_H
#define MDTRA_RESULTDIALOG_H

#include <QtGui/QDialog>

#include "ui_resultDialog.h"
#include "mdtra_types.h"

class MDTRA_MainWindow;

class MDTRA_ResultDialog : public QDialog, public Ui_resultDialog
{
	Q_OBJECT

public:
    MDTRA_ResultDialog( int index, QWidget *parent = 0 );

private slots:
	void exec_on_update_layout_and_scale_units( void );
	void exec_on_check_resultInput( void );
	void exec_on_accept( void );
	void exec_on_add_result_data_source( void );
	void exec_on_add_multiple_result_data_sources( void );
	void exec_on_edit_result_data_source( void );
	void exec_on_remove_result_data_source( void );
	void exec_on_up_result_data_source( void );
	void exec_on_down_result_data_source( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	int m_iResultIndex;
	QList<MDTRA_DSRef> m_dsRefList;
};

#endif //MDTRA_RESULTDIALOG_H