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
#ifndef MDTRA_OCCLUDERDIALOG_H
#define MDTRA_OCCLUDERDIALOG_H

#include <QtGui/QDialog>

#include "ui_occluderDialog.h"

class MDTRA_MainWindow;
class MDTRA_PDB_File;
template<typename T> class MDTRA_SelectionSet;
template<typename T> class MDTRA_SelectionParser;

class MDTRA_OccluderDialog : public QDialog, public Ui_occluderDialog
{
	Q_OBJECT

	friend void occluder_print_f( const QString &msg );
	friend void occluder_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet );

public:
    MDTRA_OccluderDialog( int streamIndex, const struct stMDTRA_Selection* selData, QWidget *mainwindow, QWidget *parent = 0 );
	virtual ~MDTRA_OccluderDialog();
	virtual bool event( QEvent *ev );
	void getSelection( QString* pString, int* pFlags, int* pSize, int** pData );

private slots:
	void exec_on_selection_editingFinished( void );
	void exec_on_selection_showWholeSelection( void );
	void exec_on_check_input( void );

private:
	void update_selection( void );
	void display_selection( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	MDTRA_SelectionParser<MDTRA_PDB_File>* m_pSelectionParser;
	bool m_bSelectionError;
	int	 m_iSelectionFlags;
	int  m_iSelectionSize;
	int* m_pSelectionData;
	QString m_cachedSelectionText;
	int	 m_streamIndex;
};

#endif //MDTRA_OCCLUDERDIALOG_H