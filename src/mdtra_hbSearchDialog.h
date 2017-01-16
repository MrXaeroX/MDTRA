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
#ifndef MDTRA_HBSEARCHDIALOG_H
#define MDTRA_HBSEARCHDIALOG_H

#include <QtGui/QDialog>

#include "ui_hbSearchDialog.h"

class MDTRA_MainWindow;
class MDTRA_PDB_File;

class MDTRA_HBSearchDialog : public QDialog, public Ui_hbSearchDialog
{
	Q_OBJECT

public:
    MDTRA_HBSearchDialog( QWidget *parent = 0 );
	virtual ~MDTRA_HBSearchDialog();

	const struct stMDTRA_HBSearchInfo* getHBSearchInfo( void ) const { return pHBsInfo; }

private slots:
	void exec_on_stream_change( void );
	void exec_on_accept( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	int	 m_cachedStreamIndex;
	struct stMDTRA_HBSearchInfo *pHBsInfo;
};

#endif //MDTRA_HBSEARCHDIALOG_H