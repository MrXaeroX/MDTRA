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
#ifndef MDTRA_DISTANCESEARCHDIALOG_H
#define MDTRA_DISTANCESEARCHDIALOG_H

#include <QtGui/QDialog>

#include "ui_distanceSearchDialog.h"

class MDTRA_MainWindow;
class MDTRA_PDB_File;
template<typename T> class MDTRA_SelectionSet;
template<typename T> class MDTRA_SelectionParser;

class MDTRA_DistanceSearchDialog : public QDialog, public Ui_distanceSearchDialog
{
	Q_OBJECT

	friend void distancesearch_print_f( const QString &msg );
	friend void distancesearch_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet );
	friend void distancesearch_select2_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet );

public:
    MDTRA_DistanceSearchDialog( QWidget *parent = 0 );
	virtual ~MDTRA_DistanceSearchDialog();
	virtual bool event( QEvent *ev );

	const struct stMDTRA_DistanceSearchInfo* getDistanceSearchInfo( void ) const { return pDsInfo; }

private slots:
	void exec_on_stream_change( void );
	void exec_on_selection_editingFinished( void );
	void exec_on_selection_showWholeSelection( void );
	void exec_on_accept( void );

private:
	void update_selection( void );
	void update_selection2( void );
	void display_selection( void );
	void build_common_atom_set( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	MDTRA_SelectionParser<MDTRA_PDB_File>* m_pSelectionParser;
	bool m_bSelectionError;
	int	 m_iSelectionFlags;
	int  m_iSelectionSize;
	int* m_pSelectionData;
	bool m_bSelectionError2;
	int  m_iSelectionSize2;
	int* m_pSelectionData2;
	QString m_cachedSelectionText;
	int	 m_cachedStreamIndex;
	struct stMDTRA_DistanceSearchInfo *pDsInfo;
};

#endif //MDTRA_DISTANCESEARCHDIALOG_H