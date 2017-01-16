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
#ifndef MDTRA_DNADATAMININGDIALOG_H
#define MDTRA_DNADATAMININGDIALOG_H

#include <QtGui/QDialog>

#include "ui_dnaDataMiningDialog.h"

class MDTRA_MainWindow;
class MDTRA_PDB_File;
class MDTRA_Program_Compiler;
template<typename T> class MDTRA_SelectionSet;
template<typename T> class MDTRA_SelectionParser;

class MDTRA_DNADataMiningDialog : public QDialog, public Ui_dnaDataMiningDialog
{
	Q_OBJECT

	friend void ddm_print_f( const QString &msg );
	friend void ddm_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet );

public:
    MDTRA_DNADataMiningDialog( QWidget *parent = 0 );
	virtual ~MDTRA_DNADataMiningDialog();
	virtual bool event( QEvent *ev );

private slots:
	void exec_on_select_all_or_none( void );
	void exec_on_select_something( void );
	void exec_on_update_stream( void );
	void exec_on_selection_editingFinished( void );
	void exec_on_selection_showWholeSelection( void );
	void exec_on_accept( void );

private:
	void update_selection( int listIndex );
	void display_selection( void );
	QString makeUniqueDataSourceName( const QString& suggestion );
	QString makeUniqueResultName( const QString& suggestion );
	int lookupExistingDataSource( const struct stMDTRA_DataSource& check );
	int findLocalDataSource( int iStart, const QVector<struct stMDTRA_DataSource>& localList, int residueNumber, int dataNumber );
	int findComplementaryResidue( MDTRA_PDB_File *ppdb, QList<int> &selList, int residueNumber, const struct stMDTRA_NucleoBaseDesc **pd1, const struct stMDTRA_NucleoBaseDesc **pd2 );
	const struct stMDTRA_NucleoBaseDesc* lookupNucleoBaseDesc( const char *residueTitle );
	float calc_bond_energy( const struct stMDTRA_PDB_Atom *pAtX, const struct stMDTRA_PDB_Atom *pAtH, const struct stMDTRA_PDB_Atom *pAtY, short xff, short yff );

private:
	MDTRA_MainWindow* m_pMainWindow;
	MDTRA_SelectionParser<MDTRA_PDB_File>* m_pSelectionParser;
	MDTRA_Program_Compiler* m_Compiler;
	bool m_bSelectionError;
	int	 m_iSelectionFlags;
	int  m_iSelectionSize;
	int* m_pSelectionData;
	QString m_cachedSelectionText;
	int	 m_cachedStreamIndex;
	bool m_changingCBState;
};

#endif //MDTRA_DNADATAMININGDIALOG_H