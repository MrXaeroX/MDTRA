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
#ifndef MDTRA_LABELDIALOG_H
#define MDTRA_LABELDIALOG_H

#include <QtGui/QDialog>

#include "ui_labelDialog.h"

class MDTRA_MainWindow;

class MDTRA_LabelDialog : public QDialog, public Ui_labelDialog
{
	Q_OBJECT

public:
    MDTRA_LabelDialog( bool bEditMode, QWidget *parent = 0 );
	void addDataSourceRef( int index, const QString &title );
	void setLabelData( struct stMDTRA_Label* pLabel );

private slots:
	void exec_on_accept( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	struct stMDTRA_Label* m_pLabelData;
};

#endif //MDTRA_LABELDIALOG_H