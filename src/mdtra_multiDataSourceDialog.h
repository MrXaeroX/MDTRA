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
#ifndef MDTRA_MULTIDATASOURCEDIALOG_H
#define MDTRA_MULTIDATASOURCEDIALOG_H

#include <QtGui/QDialog>

#include "ui_multiDataSourceDialog.h"
#include "mdtra_types.h"

class MDTRA_MainWindow;
class MDTRA_PDB_File;
template<typename T> class MDTRA_SelectionSet;
template<typename T> class MDTRA_SelectionParser;

class MDTRA_MultiDataSourceDialog : public QDialog, public Ui_multiDataSourceDialog
{
	Q_OBJECT

	friend void multidatasource_print_f( const QString &msg );
	friend void multidatasource_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet );
	friend void multidatasource_select2_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet );

public:
    MDTRA_MultiDataSourceDialog( QWidget *parent = 0 );
	virtual ~MDTRA_MultiDataSourceDialog();
	virtual bool event( QEvent *ev );

private slots:
	void exec_on_update_stream( void );
	void exec_on_check_dataSourceInput( void );
	void exec_on_check_args_enabled( void );
	void exec_on_accept( void );
	void exec_on_update_residue_a( void );
	void exec_on_update_residue_b( void );
	void exec_on_update_residue_c( void );
	void exec_on_update_residue_d( void );
	void exec_on_update_residue_e( void );
	void exec_on_update_residue_f( void );
	void exec_on_update_atom_a( void );
	void exec_on_update_atom_b( void );
	void exec_on_update_atom_c( void );
	void exec_on_update_atom_d( void );
	void exec_on_update_atom_e( void );
	void exec_on_update_atom_f( void );
	void exec_on_selection_editingFinished( void );
	void exec_on_selection_showWholeSelection( void );
	void exec_on_advanced( void );

private:
	bool use_selection( void );
	bool use_selection2( void );
	bool use_advanced( void );
	void update_selection( int listIndex );
	void display_selection( void );
	void update_stream_data_decl( const struct stMDTRA_Stream *pStream, QComboBox *pResidueCombo, QComboBox *pAtomCombo, QLabel *pAtomLabel );
	void update_residue( const struct stMDTRA_Stream *pStream, QComboBox *pResidueCombo, QComboBox *pAtomCombo, QLabel *pAtomLabel );
	void update_atom( QComboBox *pAtomCombo, QLabel *pAtomLabel );
	int  get_atom_index( const struct stMDTRA_Stream *pStream, QComboBox *pResidueCombo, QComboBox *pAtomCombo );
	void update_selection2( int listIndex );
	void exec_on_advanced_occluderInfo( void );
	void exec_on_advanced_program( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	MDTRA_SelectionParser<MDTRA_PDB_File>* m_pSelectionParser;
	bool m_bSelectionError;
	int  m_iDataSourceIndex;
	int  m_iCachedStreamIndex;
	bool m_bFillingCombos;
	int	 m_iSelectionFlags;
	int  m_iSelectionSize;
	int* m_pSelectionData;
	QString m_cachedSelectionText;
	QString m_Selection2;
	int	 m_iSelection2Flags;
	int  m_iSelection2Size;
	int* m_pSelection2Data;
	int	 m_userFlags;
	QString m_userData;
	QString m_ProgSource;
	byte* m_ProgBytes;
	int	 m_ProgSize;
};

#endif //MDTRA_MULTIDATASOURCEDIALOG_H