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
#ifndef MDTRA_HBSEARCHRESULTSDIALOG_H
#define MDTRA_HBSEARCHRESULTSDIALOG_H

#define ENABLE_RES_EXPORT

#include <QtGui/QDialog>

#include "ui_hbSearchResultsDialog.h"

class MDTRA_MainWindow;

class MDTRA_HBSearchResultsDialog : public QDialog, public Ui_hbSearchResultsDialog
{
	Q_OBJECT

public:
    MDTRA_HBSearchResultsDialog( QWidget *parent = 0 );
	~MDTRA_HBSearchResultsDialog() {}

public slots:
	void exec_save( void );
	void exec_on_result_add( void );
	void exec_on_header_clicked( int headerIndex );
	void addToNewResultCollectorXY( void );
	void addToNewResultCollectorHY( void );
	void addToNewResultCollectorXHY( void );
	void addToExistingResultCollectorXY( void );
	void addToExistingResultCollectorHY( void );
	void addToExistingResultCollectorXHY( void );
		
private:
	void createPopupMenu( void );
	void exportResults( const QString &fileSuffix, QTextStream *stream );
	void exportResultsToRasMolSelection( QTextStream *stream );
#ifdef ENABLE_RES_EXPORT
	void exportResultsToRES( QTextStream *stream );
#endif
	const struct stMDTRA_DataSource* buildDataSourceDistanceXY( const struct stMDTRA_Stream *pStream, int bondIndex, int aIndex );
	const struct stMDTRA_DataSource* buildDataSourceDistanceHY( const struct stMDTRA_Stream *pStream, int bondIndex, int hIndex, int aIndex );
	const struct stMDTRA_DataSource* buildDataSourceAngleXHY( const struct stMDTRA_Stream *pStream, int bondIndex, int hIndex, int aIndex );
	const struct stMDTRA_Result* buildResultCollector( struct stMDTRA_Result *pHost, int hostType, const struct stMDTRA_DataSource *pDS );

private:
	MDTRA_MainWindow* m_pMainWindow;
	QMenu* m_pPopupMenu;
	QMenu *m_pSubMenu[3];
	int m_iCurrentItem;
};

#endif //MDTRA_HBSEARCHRESULTSDIALOG_H