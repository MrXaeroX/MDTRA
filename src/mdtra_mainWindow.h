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
#ifndef MDTRA_MAINWINDOW_H
#define MDTRA_MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QSystemTrayIcon>
#include "mdtra_threads.h"

#define MENUDESC_DEFAULT			0
#define MENUDESC_PRERECENTFILES		1
#define MENUDESC_POSTRECENTFILES	2

typedef struct stMDTRAMenuDesc
{
	const char *m_ActionName;
	const char *m_Section;
	const char *m_Title;
	const char *m_Image;
	const char *m_Shortcut;
	const char *m_Hint;
	const char *m_SlotName;
	bool m_Checkable;
	int  m_iGroupIndex;
	int  m_Flags;
} MDTRAMenuDesc;

typedef struct stMDTRAToolbarDesc
{
	const char *m_ActionName;
	const char *m_Image;
	const char *m_SlotName;
} MDTRAToolbarDesc;

typedef enum eMDTRA_Languages
{
	MDTRA_LANG_ENGLISH = 0,
	MDTRA_LANG_RUSSIAN,
	MDTRA_LANG_MAX
} MDTRA_Languages;

#define MAX_RECENT_FILES	8

class QLabel;
class QListWidget;
class QSplitter;
class QTableWidget;
class QToolBar;
class QGroupBox;

class MDTRA_Plot;
class MDTRA_Project;
class MDTRA_ColorManager;
class MDTRA_PDB_Renderer;
class MDTRA_Program_Interpreter;

class MDTRA_MainWindow : public QMainWindow
{
	Q_OBJECT

	QList<QAction*> m_pActionList;
	QList<QToolBar*> m_pToolbarList;

public:
    MDTRA_MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~MDTRA_MainWindow();
	bool loadFile( const QString &filename );
	bool saveFile( const QString &filename );

	const QString &getCurrentFileDir( void ) { return m_currentFileDir; }
	MDTRA_Project *getProject( void ) const { return m_pProject; }
	const MDTRA_ColorManager *getColorManager( void ) const { return m_pColorManager; }

	void resetCounters( void );
	int getStreamCounter( void ) const { return m_iStreamCounter; }
	int incStreamCounter( void ) { m_iStreamCounter++; return m_iStreamCounter-1; }
	void setStreamCounter( int c ) { m_iStreamCounter = c; }
	QListWidget *getStreamListWidget( void ) { return m_pStreamList; }
	int getDataSourceCounter( void ) const { return m_iDataSourceCounter; }
	int incDataSourceCounter( void ) { m_iDataSourceCounter++; return m_iDataSourceCounter-1; }
	void setDataSourceCounter( int c ) { m_iDataSourceCounter = c; }
	QListWidget *getDataSourceListWidget( void ) { return m_pDataSourceList; }
	int getResultCounter( void ) const { return m_iResultCounter; }
	int incResultCounter( void ) { m_iResultCounter++; return m_iResultCounter-1; }
	void setResultCounter( int c ) { m_iResultCounter = c; }
	QListWidget *getResultListWidget( void ) { return m_pResultCollectionList; }
	void updateStatusBarPlotXY( float x, float y );
	void updatePlot( void );
	void updateTitleBar( bool modified );
	void viewPDBFiles( int trajectoryPos );
	void addLabel( const struct stMDTRA_Label *pLabel );
	void editLabel( int labelIndex,  const struct stMDTRA_Label *pLabel );
	bool removeLabel( int labelIndex );
	int getSelectedResultCollectorIndex( void );

	void setMultisampleAA( bool value ) { m_bMultisampleAA = value; }
	void setAllowSSE( bool value ) { m_bAllowSSE = value; }
	void setLowPriority( bool value ) { m_bLowPriority = value; }
	void setUseCUDA( bool value );
	void setCUDADevice( int value ) { m_iCUDADevice = value; }
	void setPlotDataFilter( bool value ) { m_bPlotDataFilter = value; }
	void setPlotDataFilterSize( int value ) { m_iPlotDataFilterSize = value; }
	void setPlotPolarAngles( bool value ) { m_bPlotPolarAngles = value; }
	void setNumThreads( int value ) { m_iThreadCount = value; }
	void setXScaleUnits( int value ) { m_xScaleUnits = value; }
	void setViewerType( int value ) { m_iViewer = value; }
	void setRasMolPath( const QString &p ) { m_rasMolPath = p; }
	void setVMDPath( const QString &p ) { m_vmdPath = p; }
	void setPlotLanguage( enum eMDTRA_Languages value ) { m_PlotLanguage = value; }

	bool multisampleContext( void ) const { return m_bMultisampleContext; }
	bool multisampleAA( void ) const { return m_bMultisampleAA; }
	bool allowSSE( void ) const { return m_bAllowSSE; }
	bool lowPriority( void ) const { return m_bLowPriority; }
	bool useCUDA( void ) const { return m_bUseCUDA; }
	int  getCUDADevice( void ) const { return m_iCUDADevice; }
	bool plotDataFilter( void ) const { return m_bPlotDataFilter; }
	int  plotDataFilterSize( void ) const { return m_iPlotDataFilterSize; }
	int  numThreads( void ) const { return m_iThreadCount; }
	bool plotShowGrid( void ) const { return m_bPlotShowGrid; }
	bool plotShowLabels( void ) const { return m_bPlotShowLabels; }
	bool plotShowLegend( void ) const { return m_bPlotShowLegend; }
	bool plotMouseTrack( void ) const { return m_bPlotMouseTrack; }
	bool plotPolarAngles( void ) const { return m_bPlotPolarAngles; }
	int XScaleUnits( void ) const { return m_xScaleUnits; }
	int ViewerType( void ) const { return m_iViewer; }
	enum eMDTRA_Languages PlotLanguage( void ) const { return m_PlotLanguage; }
	const QString &getRasMolPath( void ) const { return m_rasMolPath; }
	const QString &getVMDPath( void ) const { return m_vmdPath; }
	bool allowProfiling( void ) const { return m_bProfilingEnabled; }
	void advanceResultCollector( void );

protected:
	void closeEvent(QCloseEvent *ev);
	void showEvent(QShowEvent *ev);

private slots:
	void fileNew( void );
	bool fileOpen( void );
	bool fileSave( void );
	bool fileSaveAs( void );
	void openRecentFile( void );
	void setStatusText( const QString &msg );
	void showReferenceManual( void );
	void showAbout( void );
	void showCmdList( void );
	void showGPUInfo( void );
	void add_data_stream( void );
	void edit_data_stream( void );
	void remove_data_stream( void );
	void add_data_source( void );
	void add_multiple_data_sources( void );
	void edit_data_source( void );
	void remove_data_source( void );
	void add_result_collector( void );
	void edit_result_collector( void );
	void invalidate_result_collector( void );
	void remove_result_collector( void );
	void remove_selection( void );
	void export_result( void );
	void export_stats( void );
	void build_result_collectors( void );
	void rebuild_all_result_collectors( void );
	void select_result_collector( void );
	void editPreferences( void );
	bool plotSaveImage( void );
	bool plotSavePDF( void );
	void plotToggleGrid( void );
	void plotToggleLabels( void );
	void plotToggleLegend( void );
	void plotToggleMouseTrack( void );
	void plotToggleDataVisibility( void );
	void toolsSelectAtoms( void );
	void toolsDistanceSearch( void );
	void toolsTorsionSearch( void );
	void toolsForceSearch( void );
	void toolsHBSearch( void );
	void toolsPCA( void );
	void tools2DRMSD( void );
	void toolsDDM( void );
	void toolsHistogram( void );
	void prepWaterShell( void );
	void exec_on_cell_dblclicked( int row, int column );
	void updatePanelVisibility( void );
	void toggle_toolbar( void );
	void switch_to_plot( void );
	void switch_to_pdb_renderer( void );
	void toggle_pdb_renderer( void );
	void messageClickEvent( void );
	void iconActivateEvent( QSystemTrayIcon::ActivationReason reason );

private:
	void setupMenuBar( void );
	void setupToolBars( void );
	void setupStatusBar( void );
	void setupToolBar( const QString &toolbarName, const MDTRAToolbarDesc *pToolbarDesc, int numActions );
	void createWidgets( void );
	bool okToContinue( void );
	void saveLayout( void );
	void restoreLayout( void );
	void saveSettings( void );
	void restoreSettings( void );
	void updateRecentFiles( void );
	void addRecentFile( const QString &filename );
	bool generateTemporaryPDBFiles( int resultIndex, int trajectoryPos, QStringList &pdbList, QList<int> &indexList );
	bool runExecutable( const QString &exeName, const QStringList &args );
	void updatePanelVisibility( const QString &name, bool bVis );
	void update_toolbars( void );
	void enablePlotControls( bool bEnable );
	
private:
	QSystemTrayIcon *m_pTrayIcon;
	QString			m_currentFileName;
	QString			m_currentFileDir;
	QLabel*			m_pLblStatusMessage;
	QLabel*			m_pLblPlotCoords;
	MDTRA_Project*	m_pProject;
	MDTRA_ColorManager *m_pColorManager;
	QAction*		m_pRecentFileActions[MAX_RECENT_FILES];
	QAction*		m_pRecentFileSeparator;
	QStringList		m_RecentFileList;
	QSplitter*		m_pHSplitter;
	QSplitter*		m_pVSplitter;
	QSplitter*		m_pVSplitter2;
	QListWidget*	m_pStreamList;
	QListWidget*	m_pDataSourceList;
	QListWidget*	m_pResultCollectionList;
	QTableWidget*	m_pResultTable;
	QTableWidget*	m_pStatTable;
	QGroupBox*		m_pPlotBox;
	MDTRA_Plot*		m_pPlot;
	MDTRA_PDB_Renderer* m_pPDBRenderer;
	enum eMDTRA_Languages m_PlotLanguage;
	bool			m_bMultisampleContext;
	int				m_iStreamCounter;
	int				m_iDataSourceCounter;
	int				m_iResultCounter;
	bool			m_bMultisampleAA;
	bool			m_bAllowSSE;
	bool			m_bLowPriority;
	bool			m_bPlotDataFilter;
	int				m_iPlotDataFilterSize;
	int				m_iThreadCount;
	bool			m_bPlotShowGrid;
	bool			m_bPlotShowLabels;
	bool			m_bPlotShowLegend;
	bool			m_bPlotMouseTrack;
	bool			m_bPlotPolarAngles;
	int				m_xScaleUnits;
	int				m_iViewer;
	int				m_iCUDADevice;
	bool			m_bUseCUDA;
	bool			m_bProfilingEnabled;
	bool			m_bEnableBalloonTips;
	QString			m_rasMolPath;
	QString			m_vmdPath;
	QStringList		m_tempFiles;
};

extern MDTRA_MainWindow *g_pMainWindow;

#endif //MDTRA_MAINWINDOW_H
