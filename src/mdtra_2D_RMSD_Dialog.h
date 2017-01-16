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
#ifndef MDTRA_2D_RMSD_DIALOG_H
#define MDTRA_2D_RMSD_DIALOG_H

#include <QtGui/QDialog>

#include "mdtra_types.h"
#include "ui_rmsd2dDialog.h"

class MDTRA_MainWindow;
class MDTRA_PDB_File;
class MDTRA_Compact_PDB_File;
class MDTRA_2D_RMSD_Plot;
template<typename T> class MDTRA_SelectionSet;
template<typename T> class MDTRA_SelectionParser;

class MDTRA_2D_RMSD_Dialog : public QDialog, public Ui_rmsd2dDialog
{
	Q_OBJECT

	friend void rmsd2d_print_f( const QString &msg );
	friend void rmsd2d_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet );

public:
    MDTRA_2D_RMSD_Dialog( QWidget *parent = 0 );
	virtual ~MDTRA_2D_RMSD_Dialog();
	virtual bool event( QEvent *ev );

private slots:
	virtual void reject( void );
	void exec_on_stream_change( void );
	void exec_on_selection_editingFinished( void );
	void exec_on_selection_showWholeSelection( void );
	void exec_on_toggle_smooth( void );
	void exec_on_toggle_legend( void );
	void exec_on_update_plot_title( void );
	void exec_on_rebuild( void );
	void exec_on_save( void );

private:
	int current_stream_index( void );
	void update_selection( void );
	void display_selection( void );
	bool load_pdb_files( int trMin, int trMax, bool bSelectionChange );
	void calc_rmsd( int trMin, int trMax );
	void build_texture( int trMin, int trMax );
	void cancel_build( void );
	void profileStart( void );
	void profileEnd( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	MDTRA_SelectionParser<MDTRA_PDB_File>* m_pSelectionParser;
	MDTRA_2D_RMSD_Plot* m_pPlot;
	MDTRA_Compact_PDB_File** m_pPDBFiles;
	int  m_iNumPDBFiles;
	bool m_bSelectionError;
	int	 m_iSelectionFlags;
	int  m_iSelectionSize;
	int* m_pSelectionData;
	QString m_cachedSelectionText;
	QString m_cachedSelectionText2;
	QString m_cachedTitle;
	int	 m_cachedStreamIndex;
	int	 m_cachedPDBStreamIndex;
	int	 m_cachedPDBMinMax[2];
	float* m_pDataBuffer;
	float m_dataMax;
	unsigned char* m_pTextureData;
	int  m_iTextureSize;
	bool m_bBuildStarted;
	bool m_bCachedRGB;
	float m_flCachedPlotMin;
	bool m_bProfiling;
	dword m_profStart;
};

#endif //MDTRA_2D_RMSD_DIALOG_H