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
#ifndef MDTRA_HISTOGRAMDIALOG_H
#define MDTRA_HISTOGRAMDIALOG_H

#include <QtGui/QDialog>

#include "mdtra_types.h"
#include "ui_histogramDialog.h"

class QTimer;

class MDTRA_MainWindow;
class MDTRA_HistogramPlot;

class MDTRA_HistogramDialog : public QDialog, public Ui_histogramDialog
{
	Q_OBJECT

public:
    MDTRA_HistogramDialog( QWidget *parent = 0 );
	virtual ~MDTRA_HistogramDialog();

private slots:
	virtual void reject( void );
	void exec_on_colorChangeButton_click( void );
	void exec_on_desired_update( void );
	void exec_on_desired_redraw( void );
	void exec_on_result_change( void );
	void exec_on_rds_change( void );
	void exec_on_rebuild( void );
	void exec_on_save( void );
	void exec_on_export( void );

private:
	int current_result_index( void );
	int current_datasource_index( void );
	int current_rds_index( void );
	void cancel_build( void );
	void exportResults( const QString &fileSuffix, QTextStream *stream );
	void calc_histogram( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	MDTRA_HistogramPlot* m_pPlot;
	QTimer *m_pTimer;
	QColor m_customColor;
	bool m_bBuildStarted;
	int m_iMaxBands;
	int m_iNumBands;
	int *m_pBands;
	float m_flBandMin;
	float m_flBandSize;
};

#endif //MDTRA_HISTOGRAMDIALOG_H