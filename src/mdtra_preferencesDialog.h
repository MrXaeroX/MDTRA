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
#ifndef MDTRA_PREFERENCESDIALOG_H
#define MDTRA_PREFERENCESDIALOG_H

#include <QtGui/QDialog>
#include "ui_preferencesDialog.h"

class MDTRA_MainWindow;
class QListWidgetItem;

class MDTRA_PreferencesDialog : public QDialog, public Ui_preferencesDialog
{
	Q_OBJECT

public:
    MDTRA_PreferencesDialog( QWidget *parent = 0 );
	void savePreferences( void );
	void discardPreferences( void );

private slots:
	void exec_on_accept( void );
	void exec_on_colorList_itemClicked( QListWidgetItem *item );
	void exec_on_colorList_itemDblClicked( QListWidgetItem *item );
	void exec_on_colorChangeButton_click( void );
	void exec_on_formatList_selChange( void );
	void exec_on_format_setDefault( void );
	void exec_on_format_add( void );
	void exec_on_format_edit( void );
	void exec_on_format_delete( void );
	void browse_RasMol( void );
	void browse_VMD( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	QList<QColor> m_ColorList;
};

#endif //MDTRA_PREFERENCESDIALOG_H