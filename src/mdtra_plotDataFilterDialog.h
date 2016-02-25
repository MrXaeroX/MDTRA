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
#ifndef MDTRA_PLOTDATAFILTERDIALOG_H
#define MDTRA_PLOTDATAFILTERDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>

#include "ui_plotDataFilter.h"
#include "mdtra_types.h"

class MDTRA_MainWindow;

class MDTRA_PlotDataFilterDialog : public QDialog, public Ui_plotDataFilterDialog
{
	Q_OBJECT

public:
    MDTRA_PlotDataFilterDialog( int index, QWidget *parent = 0 );
	~MDTRA_PlotDataFilterDialog();

public slots:
	virtual void reject( void );

private slots:
	void checkbox_clicked( void );
	void pe_combo_changed( void );
	void ie_combo_changed( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	QCheckBox **m_pCheckBoxes;
	QComboBox **m_pPECombos;
	QComboBox **m_pIECombos;
	QLabel **m_pPELabels;
	QLabel **m_pIELabels;
	int m_iNumCheckBoxes;
	QList<dword> m_InitialFlags;
	QList<MDTRA_DSRef*> m_DataRefList;
};

#endif //MDTRA_PLOTDATAFILTERDIALOG_H