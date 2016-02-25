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
#ifndef MDTRA_FORCESEARCHRESULTSDIALOG_H
#define MDTRA_FORCESEARCHRESULTSDIALOG_H

#include <QtGui/QDialog>

#include "ui_forceSearchResultsDialog.h"

class MDTRA_MainWindow;

class MDTRA_ForceSearchResultsDialog : public QDialog, public Ui_forceSearchResultsDialog
{
	Q_OBJECT

public:
    MDTRA_ForceSearchResultsDialog( QWidget *parent = 0 );

private slots:
	void exec_save( void );
	void exec_on_result_add( void );
	void exec_on_header_clicked( int headerIndex );
	void addToNewResultCollector( void );
	void addToExistingResultCollector( void );

private:
	void createPopupMenu( void );
	const struct stMDTRA_DataSource* buildDataSource( const struct stMDTRA_Stream *pStream, int streamIndex, int pairIndex );
	const struct stMDTRA_Result* buildResultCollector( struct stMDTRA_Result *pHost, const struct stMDTRA_DataSource *pDS1, const struct stMDTRA_DataSource *pDS2, int pairIndex );
	void exportResults( const QString &fileSuffix, QTextStream *stream );

private:
	MDTRA_MainWindow* m_pMainWindow;
	QMenu* m_pPopupMenu;
	int m_iCurrentItem;
};

#endif //MDTRA_FORCESEARCHRESULTSDIALOG_H