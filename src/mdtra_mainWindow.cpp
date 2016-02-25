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

// Purpose:
//	Implementation of MDTRA_MainWindow

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_pdb.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb_format.h"
#include "mdtra_plot.h"
#include "mdtra_project.h"
#include "mdtra_saverestore.h"
#include "mdtra_select.h"
#include "mdtra_configFile.h"
#include "mdtra_streamDialog.h"
#include "mdtra_dataSourceDialog.h"
#include "mdtra_multiDataSourceDialog.h"
#include "mdtra_resultDialog.h"
#include "mdtra_preferencesDialog.h"
#include "mdtra_plotDataFilterDialog.h"
#include "mdtra_selectionDialog.h"
#include "mdtra_distanceSearch.h"
#include "mdtra_distanceSearchDialog.h"
#include "mdtra_distanceSearchResultsDialog.h"
#include "mdtra_torsionSearch.h"
#include "mdtra_torsionSearchDialog.h"
#include "mdtra_torsionSearchResultsDialog.h"
#include "mdtra_forceSearch.h"
#include "mdtra_forceSearchDialog.h"
#include "mdtra_forceSearchResultsDialog.h"
#include "mdtra_hbSearch.h"
#include "mdtra_hbSearchDialog.h"
#include "mdtra_hbSearchResultsDialog.h"
#include "mdtra_dnaDataMiningDialog.h"
#include "mdtra_prepWaterShellDialog.h"
#include "mdtra_pca.h"
#include "mdtra_pcaDialog.h"
#include "mdtra_pcaResultsDialog.h"
#include "mdtra_2D_RMSD_Dialog.h"
#include "mdtra_histogramDialog.h"
#include "mdtra_gpuInfoDialog.h"
#include "mdtra_messageDialog.h"
#include "mdtra_colors.h"
#include "mdtra_cpuid.h"
#include "mdtra_cuda.h"
#include "mdtra_utils.h"
#include "mdtra_hbSearch.h"
#include "mdtra_SAS.h"
#include "mdtra_pdbRenderer.h"
#include "mdtra_prog_interpreter.h"

#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtCore/QProcess>
#include <QtCore/QUrl>

#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include <QtGui/QShowEvent>
#include <QtGui/QDesktopServices>
#include <QtGui/QFileDialog>
#include <QtGui/QImageWriter>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QToolBar>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

//////////////////////////////////////////////////////////////////////
//	MAIN MENU DECLARATION											//
//////////////////////////////////////////////////////////////////////
static MDTRAMenuDesc g_MenuItems[] = 
{
{ "FileNew", "&File", "&New Project", ":/png/16x16/new.png", "Ctrl+N", "Create a new project", SLOT(fileNew()), false, 0, MENUDESC_DEFAULT },
{ "FileOpen", "&File", "&Open Project...", ":/png/16x16/open.png", "Ctrl+O", "Open an existing project", SLOT(fileOpen()), false, 0, MENUDESC_DEFAULT },
{ "FileSave", "&File", "&Save Project", ":/png/16x16/save.png", "Ctrl+S", "Save changes in the project", SLOT(fileSave()), false, 0, MENUDESC_DEFAULT },
{ "FileSaveAs", "&File", "Save Project &As...", NULL, NULL, "Save the project with a different name", SLOT(fileSaveAs()), false, 0, MENUDESC_DEFAULT },
{ NULL, "&File", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_DEFAULT },
{ "FileExport", "&File", "Export &Results...", NULL, NULL, "Export results from the selected result collector", SLOT(export_result()), false, 0, MENUDESC_DEFAULT },
{ "FileExport2", "&File", "Export S&tatistics...", NULL, NULL, "Export statistics from the selected result collector", SLOT(export_stats()), false, 0, MENUDESC_DEFAULT },
{ NULL, "&File", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_PRERECENTFILES },
{ NULL, "&File", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_POSTRECENTFILES },
{ "FileQuit", "&File", "&Quit", NULL, NULL, "Exit " APPLICATION_TITLE_SMALL, SLOT(close()), false, 0, MENUDESC_DEFAULT },

{ "EditAddStream", "&Edit", "Add &Stream...", ":/png/16x16/stream.png", "Ctrl+Alt+S", "Add new data stream", SLOT(add_data_stream()), false, 0, MENUDESC_DEFAULT },
{ "EditAddDatasource", "&Edit", "Add &Data Source...", ":/png/16x16/source.png", "Ctrl+Alt+D", "Add new data source", SLOT(add_data_source()), false, 0, MENUDESC_DEFAULT },
{ "EditAddMultiDatasources", "&Edit", "Add M&ultiple Data Sources...", NULL, "Ctrl+Alt+M", "Add multiple data sources", SLOT(add_multiple_data_sources()), false, 0, MENUDESC_DEFAULT },
{ "EditAddResult", "&Edit", "Add &Result...", ":/png/16x16/result.png", "Ctrl+Alt+R", "Add new result collector", SLOT(add_result_collector()), false, 0, MENUDESC_DEFAULT },
{ "EditAddResult", "&Edit", "&Invalidate Result", ":/png/16x16/result2.png", "Ctrl+Alt+I", "Invalidate selected result collector", SLOT(invalidate_result_collector()), false, 0, MENUDESC_DEFAULT },
{ "EditDelete", "&Edit", "Dele&te Selection", ":/png/16x16/delete.png", "Del", "Delete selected item", SLOT(remove_selection()), false, 0, MENUDESC_DEFAULT },
{ NULL, "&Edit", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_DEFAULT },
{ "EditBuild", "&Edit", "&Build", ":/png/16x16/build2.png", "F7", "Build modified results", SLOT(build_result_collectors()), false, 0, MENUDESC_DEFAULT },
{ "EditBuildAll", "&Edit", "Rebuild &All", ":/png/16x16/build.png", "Ctrl+Alt+F7", "Rebuild all results", SLOT(rebuild_all_result_collectors()), false, 0, MENUDESC_DEFAULT },
{ NULL, "&Edit", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_DEFAULT },
{ "EditPreferences", "&Edit", "&Preferences...", ":/png/16x16/settings.png", "F2", "Edit " APPLICATION_TITLE_SMALL " Preferences", SLOT(editPreferences()), false, 0, MENUDESC_DEFAULT },

#if !defined(NO_INTERNAL_PDB_RENDERER)
{ "ViewSwitchToPlot", "&View", "&Plot", NULL, "Ctrl+1", "Switch to Plot", SLOT(switch_to_plot()), true, 0, MENUDESC_DEFAULT },
{ "ViewSwitchToPDB", "&View", "PDB &Renderer", NULL, "Ctrl+2", "Switch to PDB Renderer", SLOT(switch_to_pdb_renderer()), true, 0, MENUDESC_DEFAULT },
{ NULL, "&View", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_DEFAULT },
#endif
{ TOOLBAR_TITLE, "&View", TOOLBAR_TITLE, NULL, NULL, "Toggle toolbar visibility", SLOT(toggle_toolbar()), true, 0, MENUDESC_DEFAULT },

{ "PlotToggleDataVis", "&Plot", "Plot &Data Visibility...", ":/png/16x16/pdv.png", "Ctrl+D", "Toggle visibility of particular data rows", SLOT(plotToggleDataVisibility()), false, 0, MENUDESC_DEFAULT },
{ NULL, "&Plot", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_DEFAULT },
{ "PlotToggleMouseTrack", "&Plot", "Track &Mouse Cursor", ":/png/16x16/track.png", "Ctrl+M", "Toggle plot mouse cursor tracking", SLOT(plotToggleMouseTrack()), true, 0, MENUDESC_DEFAULT },
{ "PlotToggleGrid", "&Plot", "Show &Grid Lines", ":/png/16x16/grid.png", "Ctrl+G", "Toggle plot grid lines", SLOT(plotToggleGrid()), true, 0, MENUDESC_DEFAULT },
{ "PlotToggleLabels", "&Plot", "Show La&bels", ":/png/16x16/labels.png", "Ctrl+B", "Toggle plot labels", SLOT(plotToggleLabels()), true, 0, MENUDESC_DEFAULT },
{ "PlotToggleLegend", "&Plot", "Show &Legend", ":/png/16x16/legend.png", "Ctrl+L", "Toggle plot legend", SLOT(plotToggleLegend()), true, 0, MENUDESC_DEFAULT },
{ NULL, "&Plot", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_DEFAULT },
{ "PlotSavePic", "&Plot", "&Save Image...", ":/png/16x16/pic.png", "F5", "Save plot to image file", SLOT(plotSaveImage()), false, 0, MENUDESC_DEFAULT },
{ "PlotSavePDF", "&Plot", "Save &All Images to PDF...", NULL, NULL, "Save multiple plots to PDF File", SLOT(plotSavePDF()), false, 0, MENUDESC_DEFAULT },

{ "PrepWaterShell", "&Prep", "&Water Shell Optimization...", ":/png/16x16/water.png", NULL, "Prepare optimized water shell", SLOT(prepWaterShell()), false, 0, MENUDESC_DEFAULT },

{ "ToolsSelection", "&Tools", "&Select Atoms...", ":/png/16x16/csel.png", "F3", "Select atoms by expression", SLOT(toolsSelectAtoms()), false, 0, MENUDESC_DEFAULT },
{ NULL, "&Tools", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_DEFAULT },
{ "ToolsDistanceSearch", "&Tools", "&Distance Search...", ":/png/16x16/ds.png", "F8", "Search for meaningful pairwise distances", SLOT(toolsDistanceSearch()), false, 0, MENUDESC_DEFAULT },
{ "ToolsTorsionSearch", "&Tools", "&Torsion Search...", ":/png/16x16/ts.png", "F9", "Search for meaningful torsion angles", SLOT(toolsTorsionSearch()), false, 0, MENUDESC_DEFAULT },
{ "ToolsForceSearch", "&Tools", "&Force Search...", ":/png/16x16/force.png", NULL, "Search for meaningful force differences (BioPASED format only)", SLOT(toolsForceSearch()), false, 0, MENUDESC_DEFAULT },
{ "ToolsHBSearch", "&Tools", "&H-Bonds Search...", ":/png/16x16/hbond.png", "F10", "Search for meaningful hydrogen bonds along the trajectory", SLOT(toolsHBSearch()), false, 0, MENUDESC_DEFAULT },
{ NULL, "&Tools", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_DEFAULT },
/*{ "ToolsPCA", "&Tools", "&Principal Component Analysis...", ":/png/16x16/pca.png", "F11", "Perform principal component analysis", SLOT(toolsPCA()), false, 0, MENUDESC_DEFAULT },*/
{ "Tools2DRMSD", "&Tools", "Calculate 2D-&RMSD...", ":/png/16x16/rmsd2d.png", "F12", "Calculate two-dimensional RMSD mp", SLOT(tools2DRMSD()), false, 0, MENUDESC_DEFAULT },
{ "ToolsHistogram", "&Tools", "Build &Histogram...", ":/png/16x16/chart.png", "Ctrl+H", "Build histogram for calculated results", SLOT(toolsHistogram()), false, 0, MENUDESC_DEFAULT },
{ NULL, "&Tools", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_DEFAULT },
{ "ToolsDDM", "&Tools", "DNA Data &Mining...", ":/png/16x16/ddm.png", NULL, "Quickly build DNA-specific result collectors", SLOT(toolsDDM()), false, 0, MENUDESC_DEFAULT },

{ "HelpReferenceManual", "&Help", "&Reference Manual", NULL, "F1", "Display " APPLICATION_TITLE_SMALL " reference manual", SLOT(showReferenceManual()),  false, 0, MENUDESC_DEFAULT },
{ "HelpCommands", "&Help", "&Command List...", NULL, NULL, "Display list of commands with shortcuts", SLOT(showCmdList()),  false, 0, MENUDESC_DEFAULT },
{ NULL, "&Help", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_DEFAULT },
#if defined(MDTRA_ALLOW_CUDA)
{ "HelpCUDA", "&Help", "&NVIDIA GPU Information...", ":/png/16x16/nvidia.png", NULL, "Display information on NVIDIA GPU used in CUDA computations", SLOT(showGPUInfo()),  false, 0, MENUDESC_DEFAULT },
{ NULL, "&Help", NULL, NULL, NULL, NULL, NULL, false, 0, MENUDESC_DEFAULT },
#endif //MDTRA_ALLOW_CUDA
{ "HelpAbout", "&Help", "&About " APPLICATION_TITLE_SMALL "...", NULL, NULL, "Display information about " APPLICATION_TITLE_SMALL, SLOT(showAbout()),  false, 0, MENUDESC_DEFAULT },
};

//////////////////////////////////////////////////////////////////////
//	TOOLBAR DECLARATION												//
//////////////////////////////////////////////////////////////////////
static MDTRAToolbarDesc g_ToolbarItems[] = 
{
{ "FileNew", ":/png/24x24/new.png", SLOT(fileNew()) },
{ "FileOpen", ":/png/24x24/open.png", SLOT(fileOpen()) },
{ "FileSave", ":/png/24x24/save.png", SLOT(fileSave()) },
{ "PlotSavePic", ":/png/24x24/pic.png", SLOT(plotSaveImage()) },
{ NULL, NULL },
{ "EditBuild", ":/png/24x24/build2.png", SLOT(build_result_collectors()) },
{ "EditBuildAll", ":/png/24x24/build.png", SLOT(rebuild_all_result_collectors()) },
{ NULL, NULL },
{ "PlotToggleDataVis", ":/png/24x24/pdv.png", SLOT(plotToggleDataVisibility()) },
{ "PlotToggleMouseTrack", ":/png/24x24/track.png", SLOT(plotToggleMouseTrack()) },
{ "PlotToggleGrid", ":/png/24x24/grid.png", SLOT(plotToggleGrid()) },
{ "PlotToggleLabels", ":/png/24x24/labels.png", SLOT(plotToggleLabels()) },
{ "PlotToggleLegend", ":/png/24x24/legend.png", SLOT(plotToggleLegend()) },
#if !defined(NO_INTERNAL_PDB_RENDERER)
{ "ViewSwitchToPDB", ":/png/24x24/pdbview.png", SLOT(toggle_pdb_renderer()) },
#endif
{ NULL, NULL },
{ "ToolsSelection", ":/png/24x24/csel.png", SLOT(toolsSelectAtoms()) },
{ "ToolsDistanceSearch", ":/png/24x24/ds.png", SLOT(toolsDistanceSearch()) },
{ "ToolsTorsionSearch", ":/png/24x24/ts.png", SLOT(toolsTorsionSearch()) },
{ "ToolsForceSearch", ":/png/24x24/force.png", SLOT(toolsForceSearch()) },
{ "ToolsHBSearch", ":/png/24x24/hbond.png", SLOT(toolsHBSearch()) },
/*{ "ToolsPCA", ":/png/24x24/pca.png", SLOT(toolsPCA()) },*/
{ "Tools2DRMSD", ":/png/24x24/rmsd2d.png", SLOT(tools2DRMSD()) },
{ "ToolsHistogram", ":/png/24x24/chart.png", SLOT(toolsHistogram()) },
{ "ToolsDDM", ":/png/24x24/ddm.png", SLOT(toolsDDM()) },
{ NULL, NULL },
{ "EditPreferences", ":/png/24x24/settings.png", SLOT(editPreferences()) },
};

//////////////////////////////////////////////////////////////////////

MDTRA_MainWindow *g_pMainWindow = NULL;

MDTRA_MainWindow :: MDTRA_MainWindow( QWidget *parent, Qt::WindowFlags flags )
				  : QMainWindow( parent, flags )
{
	g_pMainWindow = this;
	setObjectName("MDTRA_MainWindow");
	setWindowTitle(tr(APPLICATION_TITLE_SMALL));
	setWindowIcon(QIcon(":/png/icon.png"));

	m_pTrayIcon = new QSystemTrayIcon( windowIcon() );
	m_pTrayIcon->setToolTip(tr(APPLICATION_TITLE_SMALL " " APPLICATION_VERSION " [Idle]" ));
	connect(m_pTrayIcon, SIGNAL(messageClicked()), this, SLOT(messageClickEvent()));
    connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivateEvent(QSystemTrayIcon::ActivationReason)));
	m_pTrayIcon->show();

	m_currentFileName = "Unnamed";
	m_currentFileDir = ".";

	m_pPlot = NULL;
	m_pPDBRenderer = NULL;
	m_bMultisampleAA = false;
	m_bAllowSSE = true;
	m_bLowPriority = false;
	m_iThreadCount = -1;
	m_bPlotShowGrid = true;
	m_bPlotShowLabels = true;
	m_bPlotShowLegend = true;
	m_bPlotMouseTrack = false;
	m_bPlotPolarAngles = false;
	m_bMultisampleContext = false;
	m_bPlotDataFilter = false;
	m_iPlotDataFilterSize = 10;
	m_PlotLanguage = MDTRA_LANG_ENGLISH;
	m_iViewer = 0;
	m_iCUDADevice = 0;
	m_bUseCUDA = false;
	m_bProfilingEnabled = false;
	m_bEnableBalloonTips = true;

	m_pProject = new MDTRA_Project( this );
	m_pColorManager = new MDTRA_ColorManager;
	
	resetCounters();
	setupMenuBar();
	setupToolBars();
	setupStatusBar();
	restoreSettings();
	createWidgets();
	updateTitleBar( false );
	restoreLayout();

	MDTRA_ScanAndLoadConfigs();

	//!TODO: remove
	HBInitConfigData();

#if defined(WIN32)
	timeBeginPeriod(1);
#endif

	ThreadSetDefault( m_iThreadCount, m_bLowPriority ? 0 : 1 );
	MDTRA_CUDA_InitDevice( getCUDADevice() );
	statusBar()->showMessage(tr("Welcome to " APPLICATION_TITLE_FULL " " APPLICATION_VERSION));
}

MDTRA_MainWindow :: ~MDTRA_MainWindow()
{
	for (int i = 0; i < m_tempFiles.count(); i++)
		_unlink(m_tempFiles.at(i).toAscii());
	m_tempFiles.clear();

	HBFreeConfigData();
	MDTRA_UnloadConfigs();

	if (m_pProject) {
		delete m_pProject;
		m_pProject = NULL;
	}
	if (m_pColorManager) {
		delete m_pColorManager;
		m_pColorManager = NULL;
	}
	g_pMainWindow = NULL;
}

void MDTRA_MainWindow :: showEvent( QShowEvent *ev )
{
	updatePanelVisibility( "ViewSwitchToPDB", false );
	updatePanelVisibility( "ViewSwitchToPlot", true );
	update_toolbars();
}

void MDTRA_MainWindow :: messageClickEvent( void )
{
	setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive );
	raise();
	activateWindow();
}

void MDTRA_MainWindow :: iconActivateEvent( QSystemTrayIcon::ActivationReason reason )
{
	switch (reason) {
	case QSystemTrayIcon::DoubleClick:
		setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
		raise();
		activateWindow();
		break;
	default:
		break;
     }
 }

void MDTRA_MainWindow :: resetCounters( void )
{
	m_iStreamCounter = 1;
	m_iDataSourceCounter = 1;
	m_iResultCounter = 1;
}

void MDTRA_MainWindow :: createWidgets( void )
{
	QGLFormat pixelFormat;
	pixelFormat.setDoubleBuffer(true);
	pixelFormat.setDepth(true);
	pixelFormat.setRgba(true);
	pixelFormat.setSampleBuffers(m_bMultisampleAA);
	QGLFormat::setDefaultFormat(pixelFormat);
	m_bMultisampleContext = m_bMultisampleAA;

	QWidget *pLeftWidget;
	QWidget *pCenterWidget;
	QWidget *pRightWidget;

	m_pHSplitter = new QSplitter( this );
	m_pVSplitter = new QSplitter( this );
	m_pVSplitter2 = new QSplitter( this );
	pLeftWidget = new QWidget( this );
	pCenterWidget = new QWidget( this );
	pRightWidget = new QWidget( this );

	m_pHSplitter->setChildrenCollapsible( false );
	m_pVSplitter->setChildrenCollapsible( false );
	m_pVSplitter->setOrientation( Qt::Vertical );
	m_pVSplitter2->setChildrenCollapsible( false );
	m_pVSplitter2->setOrientation( Qt::Vertical );

	QVBoxLayout *vLayout;
	QVBoxLayout *vGroupLayout;
	QHBoxLayout *hGroupLayout;
	QGroupBox *groupBox;
	QPushButton *pushButton;

	vLayout = new QVBoxLayout( pLeftWidget );
	vLayout->setContentsMargins(4,1,1,1);
	groupBox = new QGroupBox( pLeftWidget );
	groupBox->setTitle( tr( "Streams" ) );
	vGroupLayout = new QVBoxLayout();
	m_pStreamList = new QListWidget( groupBox );
	m_pStreamList->setStatusTip( tr("List of data streams") );
	connect(m_pStreamList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(edit_data_stream()));
	vGroupLayout->addWidget( m_pStreamList );
	hGroupLayout = new QHBoxLayout();
	hGroupLayout->setContentsMargins(4,1,1,1);
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Add...") );
	pushButton->setStatusTip( tr("Add new data stream") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(add_data_stream()));
	hGroupLayout->addWidget( pushButton );
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Edit...") );
	pushButton->setStatusTip( tr("Edit selected data stream") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(edit_data_stream()));
	hGroupLayout->addWidget( pushButton );
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Remove") );
	pushButton->setStatusTip( tr("Remove selected data stream") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(remove_data_stream()));
	hGroupLayout->addWidget( pushButton );
	vGroupLayout->addLayout( hGroupLayout );
	groupBox->setLayout( vGroupLayout );
	m_pVSplitter->addWidget( groupBox );	
	groupBox = new QGroupBox( pLeftWidget );
	groupBox->setTitle( tr( "Data Sources" ) );
	vGroupLayout = new QVBoxLayout();
	m_pDataSourceList = new QListWidget( groupBox );
	m_pDataSourceList->setStatusTip( tr("List of data sources") );
	connect(m_pDataSourceList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(edit_data_source()));
	vGroupLayout->addWidget( m_pDataSourceList );
	hGroupLayout = new QHBoxLayout();
	hGroupLayout->setContentsMargins(4,1,1,1);
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Add...") );
	pushButton->setStatusTip( tr("Add new data source") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(add_data_source()));
	hGroupLayout->addWidget( pushButton );
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Multiple...") );
	pushButton->setStatusTip( tr("Add multiple data sources") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(add_multiple_data_sources()));
	hGroupLayout->addWidget( pushButton );
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Edit...") );
	pushButton->setStatusTip( tr("Edit selected data source") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(edit_data_source()));
	hGroupLayout->addWidget( pushButton );
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Remove") );
	pushButton->setStatusTip( tr("Remove selected data source") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(remove_data_source()));
	hGroupLayout->addWidget( pushButton );
	vGroupLayout->addLayout( hGroupLayout );
	groupBox->setLayout( vGroupLayout );
	m_pVSplitter->addWidget( groupBox );
	vLayout->addWidget( m_pVSplitter );
	pLeftWidget->setLayout( vLayout );

	vLayout = new QVBoxLayout( pCenterWidget );
	vLayout->setContentsMargins(1,1,1,1);
	groupBox = new QGroupBox( pCenterWidget );
	groupBox->setTitle( tr( "Result Collection" ) );
	hGroupLayout = new QHBoxLayout();
	m_pResultCollectionList = new QListWidget( groupBox );
	m_pResultCollectionList->setStatusTip( tr("List of results to get from data sources") );
	connect(m_pResultCollectionList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(edit_result_collector()));
	connect(m_pResultCollectionList, SIGNAL(itemSelectionChanged()), this, SLOT(select_result_collector()));
	hGroupLayout->addWidget( m_pResultCollectionList );
	vGroupLayout = new QVBoxLayout();
	vGroupLayout->setContentsMargins(1,1,1,1);
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Add...") );
	pushButton->setStatusTip( tr("Add new result collector") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(add_result_collector()));
	vGroupLayout->addWidget( pushButton );
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Edit...") );
	pushButton->setStatusTip( tr("Edit selected result collector") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(edit_result_collector()));
	vGroupLayout->addWidget( pushButton );
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Invalidate") );
	pushButton->setStatusTip( tr("Invalidate selected result collector") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(invalidate_result_collector()));
	vGroupLayout->addWidget( pushButton );
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Remove") );
	pushButton->setStatusTip( tr("Remove selected result collector") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(remove_result_collector()));
	vGroupLayout->addWidget( pushButton );
	vGroupLayout->addStretch( 0 );
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Build") );
	pushButton->setStatusTip( tr("Build new and modified result collectors") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(build_result_collectors()));
	vGroupLayout->addWidget( pushButton );
	pushButton = new QPushButton( groupBox );
	pushButton->setText( tr("Rebuild All") );
	pushButton->setStatusTip( tr("Rebuild all result collectors") );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(rebuild_all_result_collectors()));
	vGroupLayout->addWidget( pushButton );
	hGroupLayout->addLayout( vGroupLayout );
	groupBox->setLayout( hGroupLayout );
	vLayout->addWidget( groupBox );
	pCenterWidget->setLayout( vLayout );

	vLayout = new QVBoxLayout( pRightWidget );
	vLayout->setContentsMargins(1,1,4,1);

	groupBox = new QGroupBox( pRightWidget );
	groupBox->setTitle( tr( "Data Table" ) );
	vGroupLayout = new QVBoxLayout();
	m_pResultTable = new QTableWidget( groupBox );
	m_pResultTable->setMouseTracking( true );
	m_pResultTable->setStatusTip( tr("Table of selected result's data (double-click a row to view PDB files)") );
	m_pResultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pResultTable->setSelectionMode(QAbstractItemView::NoSelection);
	m_pResultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pResultTable->verticalHeader()->setDefaultSectionSize(24);
	m_pResultTable->horizontalHeader()->setStretchLastSection( true );
	connect(m_pResultTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(exec_on_cell_dblclicked(int,int)));
	vGroupLayout->addWidget( m_pResultTable );
	groupBox->setLayout( vGroupLayout );
	m_pVSplitter2->addWidget( groupBox );	
	groupBox = new QGroupBox( pRightWidget );
	groupBox->setTitle( tr( "Statistic Table" ) );
	vGroupLayout = new QVBoxLayout();
	m_pStatTable = new QTableWidget( groupBox );
	m_pStatTable->setMouseTracking( true );
	m_pStatTable->setStatusTip( tr("Table of statistic parameters of selected result's data") );
	m_pStatTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pStatTable->setSelectionMode(QAbstractItemView::NoSelection);
	m_pStatTable->verticalHeader()->setDefaultSectionSize(24);
	m_pStatTable->horizontalHeader()->setStretchLastSection( true );
	vGroupLayout->addWidget( m_pStatTable );
	groupBox->setLayout( vGroupLayout );
	m_pVSplitter2->addWidget( groupBox );	
	m_pPlotBox = new QGroupBox( pRightWidget );
	m_pPlotBox->setTitle( tr( "Plot" ) );
	vGroupLayout = new QVBoxLayout();
	m_pPlot = new MDTRA_Plot( pixelFormat, this );
	m_pPlot->setParent( m_pPlotBox );
	m_pPlot->setStatusTip( tr("Plot of selected result's data (double-click to view PDB files)") );
	m_pPlot->setGeometry(QRect(0, 0, 320, 240));
	m_pPlot->setVisible( true );
	if (m_bMultisampleContext && !m_pPlot->format().sampleBuffers()) {
		m_bMultisampleContext = false;
	}
	vGroupLayout->addWidget( m_pPlot );
#if !defined(NO_INTERNAL_PDB_RENDERER)
	m_pPDBRenderer = new MDTRA_PDB_Renderer( this );
	m_pPDBRenderer->setParent( m_pPlotBox );
	m_pPDBRenderer->setStatusTip( tr("PDB renderer") );
	m_pPDBRenderer->setGeometry(QRect(0, 0, 320, 240));
	m_pPDBRenderer->setVisible( false );
	vGroupLayout->addWidget( m_pPDBRenderer );
#endif
	m_pPlotBox->setLayout( vGroupLayout );
	m_pVSplitter2->addWidget( m_pPlotBox );
	vLayout->addWidget( m_pVSplitter2 );
	pRightWidget->setLayout( vLayout );

	m_pHSplitter->addWidget( pLeftWidget );
	m_pHSplitter->addWidget( pCenterWidget );
	m_pHSplitter->addWidget( pRightWidget );
	this->setCentralWidget( m_pHSplitter );
}

void MDTRA_MainWindow :: updatePanelVisibility( void )
{
	QObject *obj = QObject::sender();
	if (QWidget *wobj = qobject_cast<QWidget*>(obj)) {
		for (int i = 0; i < m_pActionList.size(); ++i) {
			if (m_pActionList.at(i)->objectName() == wobj->objectName()) {
				m_pActionList.at(i)->setChecked( wobj->isVisible() );
			}
		}
	}
}

void MDTRA_MainWindow :: updatePanelVisibility( const QString &name, bool bVis )
{
	for (int i = 0; i < m_pActionList.size(); ++i) {
		if (m_pActionList.at(i)->objectName() == name) {
			m_pActionList.at(i)->setChecked( bVis );
		}
	}
}

void MDTRA_MainWindow :: setupMenuBar( void )
{
	//Create main menu
	int iCurrentGroup = 0;
	QAction *pRecentFilesPost = NULL;
	QActionGroup *pCurrentGroup = NULL;
	QMenu *pCurrentMenu = NULL;
	MDTRAMenuDesc *pCurrentMenuDesc = NULL;
	int numActions = sizeof(g_MenuItems) / sizeof(g_MenuItems[0]);

	for (int i = 0; i < numActions; i++) {
		pCurrentMenuDesc = &g_MenuItems[i];
		assert(pCurrentMenuDesc->m_Section);
		if (!pCurrentMenu || strcmp(pCurrentMenu->title().toAscii(), pCurrentMenuDesc->m_Section)) {
			pCurrentMenu = menuBar()->addMenu(tr(pCurrentMenuDesc->m_Section));
			pCurrentMenu->setObjectName(pCurrentMenuDesc->m_Section);
		}
		if (!pCurrentMenuDesc->m_Title) {
			if (pCurrentMenuDesc->m_Flags == MENUDESC_PRERECENTFILES)
				m_pRecentFileSeparator = pCurrentMenu->addSeparator();
			else if (pCurrentMenuDesc->m_Flags == MENUDESC_POSTRECENTFILES)
				pRecentFilesPost = pCurrentMenu->addSeparator();
			else
				pCurrentMenu->addSeparator();
		} else {
			QAction *pAction = new QAction( tr(pCurrentMenuDesc->m_Title), this );
			if (pCurrentMenuDesc->m_iGroupIndex) {
				if (!pCurrentGroup || (pCurrentMenuDesc->m_iGroupIndex != iCurrentGroup))	{
					iCurrentGroup = pCurrentMenuDesc->m_iGroupIndex;
					pCurrentGroup = new QActionGroup( this );
					pCurrentGroup->setExclusive( true );
				}
				pAction->setActionGroup( pCurrentGroup );
			}
			pAction->setCheckable(pCurrentMenuDesc->m_Checkable);
			if (pCurrentMenuDesc->m_Image) pAction->setIcon(QIcon(pCurrentMenuDesc->m_Image));
			if (pCurrentMenuDesc->m_Shortcut) {
				pAction->setShortcut(tr(pCurrentMenuDesc->m_Shortcut));
				if (pCurrentMenuDesc->m_Hint) pAction->setStatusTip(QString("%1 (%2)").arg(tr(pCurrentMenuDesc->m_Hint)).arg(tr(pCurrentMenuDesc->m_Shortcut)));
			} else {
				if (pCurrentMenuDesc->m_Hint) pAction->setStatusTip(tr(pCurrentMenuDesc->m_Hint));
			}
			if (pCurrentMenuDesc->m_SlotName) connect(pAction, SIGNAL(triggered()), this, pCurrentMenuDesc->m_SlotName);
			if (pCurrentMenuDesc->m_ActionName) pAction->setObjectName(pCurrentMenuDesc->m_ActionName);
			pCurrentMenu->addAction(pAction);
			m_pActionList << pAction;
		}
	}

	//create recent file menu
	QMenu *fileMenu = menuBar()->findChild<QMenu*>( "&File" );
	assert( fileMenu != NULL );
	for (int i = 0; i < MAX_RECENT_FILES; i++)
	{
		m_pRecentFileActions[i] = new QAction( this );
		m_pRecentFileActions[i]->setVisible( false );
		connect( m_pRecentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
		fileMenu->insertAction( 	pRecentFilesPost, m_pRecentFileActions[i] );
	}
	m_pRecentFileSeparator->setVisible( false );
}

void MDTRA_MainWindow :: setupToolBar( const QString &toolbarName, const MDTRAToolbarDesc *pToolbarDesc, int numActions )
{
	//Create toolbar
	const MDTRAToolbarDesc *pCurrentToolbarDesc = NULL;
	QToolBar *pToolbar = addToolBar(toolbarName);
	connect(pToolbar, SIGNAL(visibilityChanged(bool)), this, SLOT(updatePanelVisibility()));

	pToolbar->setObjectName( toolbarName );
	pToolbar->setIconSize(QSize(24, 24));

	for (int i = 0; i < numActions; i++) {
		pCurrentToolbarDesc = &pToolbarDesc[i];
		if (!pCurrentToolbarDesc->m_ActionName) {
			pToolbar->addSeparator();
		} else {
			QAction *pRef = NULL;
			for (int i = 0; i < m_pActionList.size(); ++i) {
				if (m_pActionList.at(i)->objectName() == QString(pCurrentToolbarDesc->m_ActionName)) {
					pRef = m_pActionList.at(i);
					break;
				}
			}	
			if (!pRef)
				continue;
			QAction *pToolBarAction = new QAction(pRef->text(), this);
			pToolBarAction->setStatusTip(pRef->statusTip());
			pToolBarAction->setCheckable(pRef->isCheckable());
			pToolBarAction->setObjectName(pRef->objectName());
			pToolBarAction->setActionGroup(pRef->actionGroup());
			if (pCurrentToolbarDesc->m_Image) pToolBarAction->setIcon(QIcon(pCurrentToolbarDesc->m_Image));
			if (pCurrentToolbarDesc->m_SlotName) connect(pToolBarAction, SIGNAL(triggered()), this, pCurrentToolbarDesc->m_SlotName);
			pToolbar->addAction(pToolBarAction);
			m_pActionList << pToolBarAction;
		}
	}
	m_pToolbarList << pToolbar;
}

void MDTRA_MainWindow :: toggle_toolbar( void )
{
	QObject *obj = QObject::sender();
	if (QAction *act = qobject_cast<QAction*>(obj)) {
		for (int i = 0; i < m_pToolbarList.size(); ++i) {
			if (m_pToolbarList.at(i)->objectName() == act->objectName()) {
				if (m_pToolbarList.at(i)->isVisible())
					m_pToolbarList.at(i)->hide();
				else
					m_pToolbarList.at(i)->show();
				updatePanelVisibility(act->objectName(), m_pToolbarList.at(i)->isVisible());
			}
		}
	}
}

void MDTRA_MainWindow :: update_toolbars( void )
{
	for (int i = 0; i < m_pToolbarList.size(); ++i) {
		updatePanelVisibility( m_pToolbarList.at(i)->objectName(), m_pToolbarList.at(i)->isVisible() );
	}
}

void MDTRA_MainWindow :: setupToolBars( void )
{
	setupToolBar( QString(TOOLBAR_TITLE), g_ToolbarItems, sizeof(g_ToolbarItems)/sizeof(g_ToolbarItems[0]) );
}

void MDTRA_MainWindow :: setupStatusBar( void )
{
	//Create status bar
	m_pLblStatusMessage = new QLabel;

	m_pLblPlotCoords = new QLabel;
	m_pLblPlotCoords->setAlignment( Qt::AlignLeft );
	m_pLblPlotCoords->setMinimumWidth( 200 );

	statusBar()->addPermanentWidget( m_pLblStatusMessage, 1 );
	statusBar()->addPermanentWidget( m_pLblPlotCoords );

	connect( statusBar(), SIGNAL(messageChanged(const QString&)), this, SLOT(setStatusText(const QString&)));	
}

void MDTRA_MainWindow :: setStatusText( const QString &msg )
{
	//Set status text onto main label
	m_pLblStatusMessage->setText( msg );
}

void MDTRA_MainWindow :: updateStatusBarPlotXY( float x, float y )
{
	if (x < 0) {
		m_pLblPlotCoords->setText("");
	} else {
		m_pLblPlotCoords->setText(QString("X = %1, Y = %2").arg(x, 0, 'g').arg(y, 0, 'f', 3));
		m_pLblPlotCoords->repaint();
	}
}

void MDTRA_MainWindow :: updateTitleBar( bool modified )
{
	//Maintain caption and modified state
	QString fileName = m_currentFileName;
	if (modified)
		fileName.append("[*]");
	setWindowTitle(tr(APPLICATION_TITLE_SMALL).append(" - %1").arg(fileName));
	setWindowModified(modified);
}

void MDTRA_MainWindow :: fileNew( void )
{
	if (okToContinue()) {
		resetCounters();
		m_pProject->clear();
		m_currentFileName = "Unnamed";
		updateTitleBar( false );
	}
}
bool MDTRA_MainWindow :: fileOpen( void )
{
	if (okToContinue()) {
		QString fileName = QFileDialog::getOpenFileName( this, tr("Open Project File"), m_currentFileDir, APPLICATION_TITLE_SMALL " Projects (*." APPLICATION_EXT ")" );
		if (!fileName.isEmpty())
			return loadFile(fileName);
	}
	return false;
}
bool MDTRA_MainWindow :: fileSave( void )
{
	if (m_currentFileName == "Unnamed")
		return fileSaveAs();

	return saveFile(QString(m_currentFileDir).append("/").append(m_currentFileName));
}
bool MDTRA_MainWindow :: fileSaveAs( void )
{
	QString fileName = QFileDialog::getSaveFileName( this, tr("Save Project File"), m_currentFileDir, APPLICATION_TITLE_SMALL " Projects (*." APPLICATION_EXT ")" );
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		if ( fi.suffix().isEmpty() )
			fileName.append( "." APPLICATION_EXT );
		return saveFile(fileName);
	}
	return false;
}

void MDTRA_MainWindow :: openRecentFile( void )
{
	QObject *obj = QObject::sender();
	if (QAction *act = qobject_cast<QAction*>(obj)) {
		if (okToContinue()) {
			loadFile(act->data().toString());
		}
	}
}

void MDTRA_MainWindow :: updateRecentFiles( void )
{
	QMutableStringListIterator it(m_RecentFileList);
	while (it.hasNext()) {
		if (!QFile::exists(it.next()))
			it.remove();
	}

	for (int i = 0; i < MAX_RECENT_FILES; i++) {
		if (i < m_RecentFileList.count()) {
			QFileInfo fi(m_RecentFileList[i]);
			QString text = QString("&%1 %2").arg(i+1).arg(fi.fileName());
			m_pRecentFileActions[i]->setText(text);
			m_pRecentFileActions[i]->setData(m_RecentFileList[i]);
			m_pRecentFileActions[i]->setVisible(true);
		} else {
			m_pRecentFileActions[i]->setVisible(false);
		}
	}
	m_pRecentFileSeparator->setVisible(!m_RecentFileList.isEmpty());
}

void MDTRA_MainWindow :: addRecentFile( const QString &filename )
{
	m_RecentFileList.removeAll( filename );
	m_RecentFileList.prepend( filename );
	updateRecentFiles();
}

bool MDTRA_MainWindow :: loadFile( const QString &filename )
{
	QFileInfo fi(filename);
	QFile f(filename);

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	statusBar()->showMessage(tr("Loading file: %1").arg(filename));

	//Open file for reading
	if (!f.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open file for reading!\nFile: %1\nReason: %2").arg(filename,f.errorString()));
		statusBar()->showMessage(tr("Error loading file: %1").arg(filename));
		QApplication::restoreOverrideCursor();
        return false;
	}

	QDataStream stream(&f);
	if (!m_pProject->loadFile( fi.canonicalPath(), &stream )) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot read project file!\nFile: %1").arg(filename));
		f.close();
		statusBar()->showMessage(tr("Error loading file: %1").arg(filename));
		QApplication::restoreOverrideCursor();
		return false;
	}

	f.close();
	addRecentFile(filename);

	m_currentFileName = fi.fileName();
	m_currentFileDir = fi.path();
	updateTitleBar( false );
	m_pResultCollectionList->setCurrentRow( 0 );
	statusBar()->showMessage(tr("Loaded file: %1").arg(filename), 2000);
	QApplication::restoreOverrideCursor();
	return true;
}

bool MDTRA_MainWindow :: saveFile( const QString &filename )
{
	QFileInfo fi(filename);
	QFile f(filename);

	//Open file for writing
	if (!f.open(QFile::WriteOnly | QFile::Truncate)) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open file for writing!\nFile: %1\nReason: %2").arg(filename,f.errorString()));
        return false;
	}

	QDataStream stream(&f);
	if (!m_pProject->saveFile( fi.canonicalPath(), &stream )) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot write project file!\nFile: %1").arg(filename));
		f.close();
		return false;
	}

	f.close();
	addRecentFile(filename);

	m_currentFileName = fi.fileName();
	m_currentFileDir = fi.path();
	updateTitleBar( false );
	statusBar()->showMessage(tr("Saved file: %1").arg(filename), 2000);
	return true;
}

bool MDTRA_MainWindow :: okToContinue( void )
{
	if (isWindowModified())	{
		int r = QMessageBox::warning( this, tr("Confirm"), tr("Do you want to save changes in %1?").arg(m_currentFileName), 
									  QMessageBox::Yes | QMessageBox::Default,
									  QMessageBox::No,
									  QMessageBox::Cancel | QMessageBox::Escape );
		if (r == QMessageBox::Yes) {
			return fileSave();
		} else if (r == QMessageBox::Cancel) {
			return false;
		}
	}
	return true;
}

void MDTRA_MainWindow :: closeEvent( QCloseEvent *ev )
{
	if (okToContinue()) {
		FreeHBSearchOnExit();
		MDTRA_ShutdownSAS();
		saveSettings();
		saveLayout();
#if defined(WIN32)
		timeEndPeriod(1);
#endif
		if ( m_pTrayIcon )
			m_pTrayIcon->hide();
		ev->accept();
	} else {
		ev->ignore();
	}
}

void MDTRA_MainWindow :: saveLayout( void )
{
	MDTRA_SaveRestore<QMainWindow> hMainWindowSaveRestore( this );
	MDTRA_SaveRestore<QSplitter> hHSplitterSaveRestore( m_pHSplitter );
	MDTRA_SaveRestore<QSplitter> hVSplitterSaveRestore( m_pVSplitter );
	MDTRA_SaveRestore<QSplitter> hVSplitter2SaveRestore( m_pVSplitter2 );

	QString layoutFileName = QApplication::applicationDirPath().append("/").append(CONFIG_DIRECTORY LAYOUT_FILENAME);

	QFile file(layoutFileName);
    if (!file.open(QFile::WriteOnly)) {
        QString msg = tr("Failed to open %1\n%2").arg(layoutFileName).arg(file.errorString());
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), msg);
        return;
    }

	if (!hMainWindowSaveRestore.save( file ) ||
		!hHSplitterSaveRestore.save( file ) ||
		!hVSplitterSaveRestore.save( file ) ||
		!hVSplitter2SaveRestore.save( file ))
	{
		file.close();
        QString msg = tr("Error writing %1\n%2").arg(layoutFileName).arg(file.errorString());
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), msg);
        return;
    }

	file.close();
}

void MDTRA_MainWindow :: restoreLayout( void )
{
	MDTRA_SaveRestore<QMainWindow> hMainWindowSaveRestore( this );
	MDTRA_SaveRestore<QSplitter> hHSplitterSaveRestore( m_pHSplitter );
	MDTRA_SaveRestore<QSplitter> hVSplitterSaveRestore( m_pVSplitter );
	MDTRA_SaveRestore<QSplitter> hVSplitter2SaveRestore( m_pVSplitter2 );

	QString layoutFileName = QApplication::applicationDirPath().append("/").append(CONFIG_DIRECTORY LAYOUT_FILENAME);
	QFile file(layoutFileName);
	if (!file.exists())	return;

    if (!file.open(QFile::ReadOnly)) {
        QString msg = tr("Failed to open %1\n%2").arg(layoutFileName,file.errorString());
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), msg);
        return;
    }

	if (!hMainWindowSaveRestore.restore( file ) ||
		!hHSplitterSaveRestore.restore( file ) ||
		!hVSplitterSaveRestore.restore( file ) ||
		!hVSplitter2SaveRestore.restore( file ))
	{
		file.close();
        QString msg = tr("Error reading %1\n%2").arg(layoutFileName,file.errorString());
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), msg);
        return;
    }

	file.close();
}

void MDTRA_MainWindow :: saveSettings( void )
{
	QSettings settings(g_pApp->applicationDirPath().append("/").append(CONFIG_DIRECTORY SETTINGS_FILENAME), QSettings::IniFormat);

	settings.setValue("Preferences/MultisampleAA", m_bMultisampleAA);
	settings.setValue("Preferences/AllowSSE", m_bAllowSSE);
	settings.setValue("Preferences/LowPriority", m_bLowPriority);
	settings.setValue("Preferences/UseCUDA", m_bUseCUDA);
	settings.setValue("Preferences/CUDADevice", m_iCUDADevice);
	settings.setValue("Preferences/Profiling", m_bProfilingEnabled);
	settings.setValue("Preferences/EnableBalloonTips", m_bEnableBalloonTips);
	settings.setValue("Preferences/ThreadCount", m_iThreadCount);
	settings.setValue("Preferences/PlotShowGrid", m_bPlotShowGrid);
	settings.setValue("Preferences/PlotShowLabels", m_bPlotShowLabels);
	settings.setValue("Preferences/PlotShowLegend", m_bPlotShowLegend);
	settings.setValue("Preferences/PlotMouseTrack", m_bPlotMouseTrack);
	settings.setValue("Preferences/PlotDataFilter", m_bPlotDataFilter);
	settings.setValue("Preferences/PlotDataFilterSize", m_iPlotDataFilterSize);
	settings.setValue("Preferences/PlotPolarAngles", m_bPlotPolarAngles);
	settings.setValue("Preferences/XScaleUnits", m_xScaleUnits);
	settings.setValue("Preferences/Viewer", m_iViewer);
	settings.setValue("Preferences/RasMolPath", m_rasMolPath);
	settings.setValue("Preferences/VMDPath", m_vmdPath);
	settings.setValue("Preferences/DefaultFormat", g_PDBFormatManager.getDefaultFormat());
	settings.setValue("Preferences/PlotLanguage", m_PlotLanguage);
	
	float probeRadius;
	int subdivisions;
	bool excludeWater;
	MDTRA_GetSASParms( probeRadius, subdivisions, excludeWater );
	settings.setValue("Preferences/SASProbeRadius", probeRadius);
	settings.setValue("Preferences/SASSubdivisions", subdivisions);
	settings.setValue("Preferences/SASExcludeWater", excludeWater);

	for (int i = 0; i < MDTRA_MIN( MAX_RECENT_FILES, m_RecentFileList.count() ); i++) {
		settings.setValue(QString("RecentFiles/RecentFile%1").arg(i+1), m_RecentFileList[i]);
	}

	m_pColorManager->saveSettings( settings );
	g_PDBFormatManager.saveFormats();
}

void MDTRA_MainWindow :: restoreSettings( void )
{
	g_PDBFormatManager.loadFormats();

	QSettings settings(g_pApp->applicationDirPath().append("/").append(CONFIG_DIRECTORY SETTINGS_FILENAME), QSettings::IniFormat);

	m_bMultisampleAA = settings.value("Preferences/MultisampleAA").toBool();
	m_bAllowSSE = settings.value("Preferences/AllowSSE").toBool();
	m_bLowPriority = settings.value("Preferences/LowPriority").toBool();
	m_bUseCUDA = settings.value("Preferences/UseCUDA").toBool();
	m_iCUDADevice = settings.value("Preferences/CUDADevice").toInt();
	m_bProfilingEnabled = settings.value("Preferences/Profiling").toBool();
	m_bEnableBalloonTips = settings.value("Preferences/EnableBalloonTips").toBool();
	m_iThreadCount = settings.value("Preferences/ThreadCount").toInt();
	m_bPlotShowGrid = settings.value("Preferences/PlotShowGrid").toBool();
	m_bPlotShowLabels = settings.value("Preferences/PlotShowLabels").toBool();
	m_bPlotShowLegend = settings.value("Preferences/PlotShowLegend").toBool();
	m_bPlotMouseTrack = settings.value("Preferences/PlotMouseTrack").toBool();
	m_bPlotDataFilter = settings.value("Preferences/PlotDataFilter").toBool();
	m_iPlotDataFilterSize = settings.value("Preferences/PlotDataFilterSize").toInt();
	m_bPlotPolarAngles = settings.value("Preferences/PlotPolarAngles").toBool();
	m_xScaleUnits = settings.value("Preferences/XScaleUnits").toInt();
	m_iViewer = settings.value("Preferences/Viewer").toInt();
	m_rasMolPath = settings.value("Preferences/RasMolPath").toString();
	m_vmdPath = settings.value("Preferences/VMDPath").toString();
	m_PlotLanguage = (MDTRA_Languages)settings.value("Preferences/PlotLanguage").toInt();
	g_PDBFormatManager.setDefaultFormat( settings.value("Preferences/DefaultFormat").toUInt() );

	float probeRadius = settings.value("Preferences/SASProbeRadius").toFloat();
	if ( probeRadius >= 0.1f ) {
		int subdivisions = settings.value("Preferences/SASSubdivisions").toInt();
		bool excludeWater = settings.value("Preferences/SASExcludeWater").toBool();
		MDTRA_SetSASParms( probeRadius, subdivisions, excludeWater );
	}

	m_RecentFileList.clear();
	for (int i = 0; i < MAX_RECENT_FILES; i++) {
		QString recentFileName = settings.value(QString("RecentFiles/RecentFile%1").arg(i+1)).toString();
		if (!recentFileName.isEmpty())
			m_RecentFileList.append(recentFileName);
	}
	updateRecentFiles();

	m_pColorManager->restoreSettings( settings );	

	if (!g_bSupportsSSE)
		m_bAllowSSE = false;
#if defined(MDTRA_ALLOW_SSE)
	g_bAllowSSE = m_bAllowSSE;
#endif

	setUseCUDA(m_bUseCUDA);
	
	for (int i = 0; i < m_pActionList.size(); ++i) {
		if (m_pActionList.at(i)->objectName() == QString("PlotToggleGrid")) {
			m_pActionList.at(i)->setChecked(m_bPlotShowGrid);
		}
		if (m_pActionList.at(i)->objectName() == QString("PlotToggleLabels")) {
			m_pActionList.at(i)->setChecked(m_bPlotShowLabels);
		}
		if (m_pActionList.at(i)->objectName() == QString("PlotToggleLegend")) {
			m_pActionList.at(i)->setChecked(m_bPlotShowLegend);
		}
		if (m_pActionList.at(i)->objectName() == QString("PlotToggleMouseTrack")) {
			m_pActionList.at(i)->setChecked(m_bPlotMouseTrack);
		}
	}
}

void MDTRA_MainWindow :: showCmdList( void )
{
	// Display a list of registered command shortcuts
	QStringList cmdList;
	int numActions = sizeof(g_MenuItems) / sizeof(g_MenuItems[0]);

	for (int i = 0; i < numActions; i++) {
		MDTRAMenuDesc *pAct = &g_MenuItems[i];
		if (!pAct->m_Shortcut)
			continue;
		QString cmdName = QString("%1: %2").arg(pAct->m_Shortcut).arg(pAct->m_Hint);
		cmdList << cmdName;
	}
	cmdList.sort();

	MDTRA_MessageDialog dialog( tr( "Command List" ), cmdList.join("<br>"), this );
	dialog.exec();
}

void MDTRA_MainWindow :: showReferenceManual( void )
{
	QDesktopServices::openUrl(QUrl(QApplication::applicationDirPath().append("/").append(HELP_FILENAME), QUrl::TolerantMode));
}

void MDTRA_MainWindow :: showAbout( void )
{
	QString progCopyright(  tr("This program is free software; you can redistribute it and/or "
							"modify it under the terms of the GNU General Public License "
							"as published by the Free Software Foundation; either version 2 "
							"of the License, or (at your option) any later version.<br><br>"
							"This program is distributed in the hope that it will be useful, "
							"but WITHOUT ANY WARRANTY; without even the implied warranty of "
							"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
							"GNU General Public License for more details.") );

	QString aboutMsg(tr("<h2>" APPLICATION_TITLE_FULL "</h2><h3>Version: " APPLICATION_VERSION "</h3><br>Author: Alexander V. Popov.<br>Copyright &copy; 2011-2015. All rights reserved.<br><br>").append(progCopyright));
	QMessageBox::about(this, tr("About %1").arg(APPLICATION_TITLE_SMALL), aboutMsg);
}

void MDTRA_MainWindow :: showGPUInfo( void )
{
	MDTRA_GPUInfoDialog dialog( this );
	dialog.exec();
}

void MDTRA_MainWindow :: add_data_stream( void )
{
	MDTRA_StreamDialog dialog( -1, this );
	if (dialog.exec()) updateTitleBar( true );
}

void MDTRA_MainWindow :: edit_data_stream( void )
{
	if (m_pStreamList->selectedItems().count() <= 0)
		return;

	const QListWidgetItem *pItem = m_pStreamList->selectedItems().at(0);
	MDTRA_StreamDialog dialog( pItem->data(Qt::UserRole).toInt(), this );
	if (dialog.exec()) updateTitleBar( true );
}

void MDTRA_MainWindow :: remove_data_stream( void )
{
	if (m_pStreamList->selectedItems().count() <= 0)
		return;

	const QListWidgetItem *pItem = m_pStreamList->selectedItems().at(0);
	int streamIndex = pItem->data(Qt::UserRole).toInt();
	int dsCount = m_pProject->getDataSourceCountByStreamIndex( streamIndex );
	int rdsCount = m_pProject->getResultDataSourceCountByStreamIndex( streamIndex );
	QString dsMessage = "";
	QString dsMessage2 = "";
	if (dsCount > 0)
		dsMessage = tr("\n%1 data source(s) for this stream will also be removed!").arg(dsCount);
	if (rdsCount > 0)
		dsMessage2 = tr("\n%1 result data source(s) using data sources connected with this stream will also be removed!").arg(rdsCount);

	if (QMessageBox::No == QMessageBox::warning( this, tr("Confirm"), tr("Do you want to remove selected stream from the project?%1%2").arg(dsMessage, dsMessage2), 
												 QMessageBox::Yes | QMessageBox::Default,
												 QMessageBox::No | QMessageBox::Escape )) {
		return;
	}

	m_pProject->unregisterStream( streamIndex );
	updateTitleBar( true );
}

void MDTRA_MainWindow :: add_data_source( void )
{
	if (m_pProject->getValidStreamCount() < 1) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot add data source: no valid streams!"));
        return;
	}

	MDTRA_DataSourceDialog dialog( -1, this );
	if (dialog.exec()) updateTitleBar( true );
}

void MDTRA_MainWindow :: add_multiple_data_sources( void )
{
	if (m_pProject->getValidStreamCount() < 1) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot add data sources: no valid streams!"));
        return;
	}

	MDTRA_MultiDataSourceDialog dialog( this );
	if (dialog.exec()) updateTitleBar( true );
}

void MDTRA_MainWindow :: edit_data_source( void )
{
	if (m_pDataSourceList->selectedItems().count() <= 0)
		return;

	const QListWidgetItem *pItem = m_pDataSourceList->selectedItems().at(0);

	MDTRA_DataSource *pDS = m_pProject->fetchDataSourceByIndex( pItem->data(Qt::UserRole).toInt() );
	if (pDS && (!m_pProject->fetchStreamByIndex( pDS->streamIndex ) || !m_pProject->fetchStreamByIndex( pDS->streamIndex )->pdb)) {
		QMessageBox::warning( this, tr(APPLICATION_TITLE_SMALL), tr("Data source is read-only because of missing stream files!"));
		return;
	}

	MDTRA_DataSourceDialog dialog( pItem->data(Qt::UserRole).toInt(), this );
	if (dialog.exec()) updateTitleBar( true );
}

void MDTRA_MainWindow :: remove_data_source( void )
{
	if (m_pDataSourceList->selectedItems().count() <= 0)
		return;

	const QListWidgetItem *pItem = m_pDataSourceList->selectedItems().at(0);
	int dsIndex = pItem->data(Qt::UserRole).toInt();
	int rdsCount = m_pProject->getResultDataSourceCountByDataSourceIndex( dsIndex );
	QString dsMessage = "";
	if (rdsCount > 0)
		dsMessage = tr("\n%1 result data source(s) using this data source will also be removed!").arg(rdsCount);

	if (QMessageBox::No == QMessageBox::warning( this, tr("Confirm"), tr("Do you want to remove selected data source from the project?%1").arg(dsMessage), 
												 QMessageBox::Yes | QMessageBox::Default,
												 QMessageBox::No | QMessageBox::Escape )) {
		return;
	}

	m_pProject->unregisterDataSource( pItem->data(Qt::UserRole).toInt() );
	updateTitleBar( true );
}

void MDTRA_MainWindow :: add_result_collector( void )
{
	if (m_pDataSourceList->count() < 1) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot add result: no data sources registered!"));
        return;
	}
	if (m_pProject->getValidStreamCount() < 1) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot add result: no valid streams!"));
        return;
	}

	MDTRA_ResultDialog dialog( -1, this );
	if (dialog.exec()) {
		updateTitleBar( true );
		m_pResultCollectionList->setCurrentRow( m_pResultCollectionList->count()-1 );
	}
}
void MDTRA_MainWindow :: edit_result_collector( void )
{
	if (m_pResultCollectionList->selectedItems().count() <= 0)
		return;

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);

	MDTRA_Result *pResult = m_pProject->fetchResultByIndex( pItem->data(Qt::UserRole).toInt() );
	if (pResult) {
		for (int i = 0; i < pResult->sourceList.count(); i++) {
			MDTRA_DataSource *pDS = m_pProject->fetchDataSourceByIndex( pResult->sourceList.at(i).dataSourceIndex );
			if (!pDS) continue;
			if (!m_pProject->fetchStreamByIndex( pDS->streamIndex ) || !m_pProject->fetchStreamByIndex( pDS->streamIndex )->pdb) {
				QMessageBox::warning( this, tr(APPLICATION_TITLE_SMALL), tr("Result collector is read-only because of missing stream files!"));
				return;
			}
		}
	}

	MDTRA_ResultDialog dialog( pItem->data(Qt::UserRole).toInt(), this );
	if (dialog.exec()) { 
		updateTitleBar( true );
		select_result_collector();
	}
}
void MDTRA_MainWindow :: invalidate_result_collector( void )
{
	if (m_pResultCollectionList->selectedItems().count() <= 0)
		return;

	if (QMessageBox::No == QMessageBox::warning( this, tr("Confirm"), tr("Do you want to invalidate selected result?"), 
												 QMessageBox::Yes | QMessageBox::Default,
												 QMessageBox::No | QMessageBox::Escape )) {
		return;
	}

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);
	m_pProject->invalidateResult( pItem->data(Qt::UserRole).toInt(), true );
	updateTitleBar( true );
	select_result_collector();
}
void MDTRA_MainWindow :: remove_result_collector( void )
{
	if (m_pResultCollectionList->selectedItems().count() <= 0)
		return;

	if (QMessageBox::No == QMessageBox::warning( this, tr("Confirm"), tr("Do you want to remove selected result from the project?"), 
												 QMessageBox::Yes | QMessageBox::Default,
												 QMessageBox::No | QMessageBox::Escape )) {
		return;
	}

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);
	m_pProject->unregisterResult( pItem->data(Qt::UserRole).toInt() );
	updateTitleBar( true );
	m_pResultCollectionList->setCurrentRow( 0 );
}

void MDTRA_MainWindow :: export_result( void )
{
	if (m_pResultCollectionList->selectedItems().count() <= 0)
		return;

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);

	QString exportFilter = "Text Files (*.txt);;CSV Files (*.csv)";
	QString selectedFilter;
	QMap<QString,QString> extMap;
	extMap["Text Files (*.txt)"] = ".txt";
	extMap["CSV Files (*.csv)"] = ".csv";

	QString fileName = QFileDialog::getSaveFileName( this, tr("Export Result"), m_currentFileDir, exportFilter, &selectedFilter );
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		if ( fi.suffix().isEmpty() ) {
			fileName.append( extMap[selectedFilter] );
			fi.setFile( fileName );
		}
		QFile f(fileName);

		//Open file for writing
		if (!f.open(QFile::WriteOnly | QFile::Truncate)) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open file for writing!\nFile: %1\nReason: %2").arg(fileName,f.errorString()));
			return;
		}

		int exportFilter = 0;
		int currentFilter = plotDataFilterSize();
		if ( plotDataFilter() && (currentFilter > 1)) {
			if (QMessageBox::Yes == QMessageBox::information(this, tr(APPLICATION_TITLE_SMALL), tr("Plot trajectory smoothing (CMA-%1) is enabled.\nDo you wish to export smoothed results?").arg(currentFilter), 
				QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape)) {
				exportFilter = currentFilter;
			}
		}

		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QTextStream stream(&f);
		m_pProject->exportResult( pItem->data(Qt::UserRole).toInt(), fi.suffix(), &stream, exportFilter );
		f.close();
		QApplication::restoreOverrideCursor();
	}
}

void MDTRA_MainWindow :: export_stats( void )
{
	if (m_pResultCollectionList->selectedItems().count() <= 0)
		return;

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);

	QString exportFilter = "Text Files (*.txt);;CSV Files (*.csv)";
	QString selectedFilter;
	QMap<QString,QString> extMap;
	extMap["Text Files (*.txt)"] = ".txt";
	extMap["CSV Files (*.csv)"] = ".csv";

	QString fileName = QFileDialog::getSaveFileName( this, tr("Export Statistics"), m_currentFileDir, exportFilter, &selectedFilter );
	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		if ( fi.suffix().isEmpty() ) {
			fileName.append( extMap[selectedFilter] );
			fi.setFile( fileName );
		}
		QFile f(fileName);

		//Open file for writing
		if (!f.open(QFile::WriteOnly | QFile::Truncate)) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open file for writing!\nFile: %1\nReason: %2").arg(fileName,f.errorString()));
			return;
		}

		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QTextStream stream(&f);
		m_pProject->exportStats( pItem->data(Qt::UserRole).toInt(), fi.suffix(), &stream );
		f.close();
		QApplication::restoreOverrideCursor();
	}
}

void MDTRA_MainWindow :: remove_selection( void )
{
	if (m_pStreamList->hasFocus()) {
		remove_data_stream();
	} else if (m_pDataSourceList->hasFocus()) {
		remove_data_source();
	} else if (m_pResultCollectionList->hasFocus()) {
		remove_result_collector();
	}
}

void MDTRA_MainWindow :: build_result_collectors( void )
{
	if (m_pResultCollectionList->count() < 1) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Nothing to build!"));
        return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	m_pTrayIcon->setToolTip(tr(APPLICATION_TITLE_SMALL " " APPLICATION_VERSION " [Building...]" ));
	statusBar()->showMessage(tr("Building results, please wait..."));

	if (!m_pProject->build( false )) {
		QApplication::restoreOverrideCursor();
		statusBar()->showMessage("");
		m_pTrayIcon->setToolTip(tr(APPLICATION_TITLE_SMALL " " APPLICATION_VERSION " [Idle]" ));
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Nothing to build!"));
        return;
	}

	QApplication::restoreOverrideCursor();
	statusBar()->showMessage(tr("Done"), 2000);
	m_pTrayIcon->setToolTip(tr(APPLICATION_TITLE_SMALL " " APPLICATION_VERSION " [Idle]" ));
	if ( m_bEnableBalloonTips )
		m_pTrayIcon->showMessage(tr(APPLICATION_TITLE_SMALL " " APPLICATION_VERSION), tr("Build process is complete!"));
	updateTitleBar( true );
}

void MDTRA_MainWindow :: rebuild_all_result_collectors( void )
{
	if (m_pResultCollectionList->count() < 1) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Nothing to build!"));
        return;
	}

	if (QMessageBox::No == QMessageBox::warning( this, tr("Confirm"), tr("Rebuild all results?"), 
												 QMessageBox::Yes | QMessageBox::Default,
												 QMessageBox::No | QMessageBox::Escape )) {
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	m_pTrayIcon->setToolTip(tr(APPLICATION_TITLE_SMALL " " APPLICATION_VERSION " [Building...]" ));
	statusBar()->showMessage(tr("Rebilding all results, please wait..."));
	m_pPlot->disable( tr("Rebuilding results, please wait...") );

	if (!m_pProject->build( true )) {
		QApplication::restoreOverrideCursor();
		statusBar()->showMessage("");
		m_pTrayIcon->setToolTip(tr(APPLICATION_TITLE_SMALL " " APPLICATION_VERSION " [Idle]" ));
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Nothing to build!"));
        return;
	}

	QApplication::restoreOverrideCursor();
	statusBar()->showMessage(tr("Done"), 2000);
	m_pTrayIcon->setToolTip(tr(APPLICATION_TITLE_SMALL " " APPLICATION_VERSION " [Idle]" ));
	if ( m_bEnableBalloonTips )
		m_pTrayIcon->showMessage(tr(APPLICATION_TITLE_SMALL " " APPLICATION_VERSION), tr("Build process is complete!"));
	updateTitleBar( true );
}

int MDTRA_MainWindow :: getSelectedResultCollectorIndex( void )
{
	if (m_pResultCollectionList->selectedItems().count() <= 0) 
		return 0;

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);
	MDTRA_Result *pResult = m_pProject->fetchResultByIndex( pItem->data(Qt::UserRole).toInt() );
	if (!pResult || !pResult->status)
		return 0;
	
	return pResult->index;
}

void MDTRA_MainWindow :: select_result_collector( void )
{
	m_pResultTable->clear();
	m_pStatTable->clear();
	if (m_pPlot) m_pPlot->clearPlotData();
	updateStatusBarPlotXY( -1, 0 );

	if (m_pResultCollectionList->selectedItems().count() <= 0) {
		m_pResultTable->setRowCount( 0 );
		m_pResultTable->setColumnCount( 0 );
		m_pStatTable->setRowCount( 0 );
		m_pStatTable->setColumnCount( 0 );
		if (m_pPlot) m_pPlot->disable( tr("No result selected") );
		return;
	}

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);
	MDTRA_Result *pResult = m_pProject->fetchResultByIndex( pItem->data(Qt::UserRole).toInt() );
	if (!pResult || !pResult->status) {
		m_pResultTable->setRowCount( 0 );
		m_pResultTable->setColumnCount( 0 );
		m_pStatTable->setRowCount( 0 );
		m_pStatTable->setColumnCount( 0 );
		if (m_pPlot) m_pPlot->disable( tr("Data were modified, press F7 to update") );
		return;
	}

	if (m_pPDBRenderer) {
		m_pPDBRenderer->beginDataDesc();
		for (int i = 0; i < pResult->sourceList.count(); i++) {
			const MDTRA_DataSource *pDS = m_pProject->fetchDataSourceByIndex( pResult->sourceList.at(i).dataSourceIndex );
			assert( pDS != NULL );
			if ( pDS )
				m_pPDBRenderer->addDataDescStream( pDS->streamIndex );
		}
		m_pPDBRenderer->endDataDesc();
	}

	if (m_pPlot) {
		m_pPlot->setPlotTitle(pResult->name);
		m_pPlot->setDataType(pResult->type);
		m_pPlot->setDataScaleUnits(pResult->units);
		m_pPlot->setLabels(&pResult->labelList);

		QString szYAxisTitle;

		switch (pResult->type) {
		case MDTRA_DT_ANGLE:
		case MDTRA_DT_ANGLE2:
		case MDTRA_DT_TORSION:
		case MDTRA_DT_TORSION_UNSIGNED:
		case MDTRA_DT_DIHEDRAL:
		case MDTRA_DT_DIHEDRAL_ABS:
		case MDTRA_DT_PLANEANGLE:
		case MDTRA_DT_FORCE:
		case MDTRA_DT_RESULTANT_FORCE:
		case MDTRA_DT_DISTANCE:
		case MDTRA_DT_RMSD:
		case MDTRA_DT_RMSD_SEL:
		case MDTRA_DT_RMSF:
		case MDTRA_DT_RMSF_SEL:
		case MDTRA_DT_RADIUS_OF_GYRATION:
		case MDTRA_DT_SAS:
		case MDTRA_DT_SAS_SEL:
		case MDTRA_DT_OCCA:
		case MDTRA_DT_OCCA_SEL:
			szYAxisTitle = QString::fromLocal8Bit(UTIL_GetYAxisTitle(pResult->type, m_PlotLanguage));
			break;
		case MDTRA_DT_USER:
			{
				const MDTRA_DataSource *pDS = m_pProject->fetchDataSourceByIndex( pResult->sourceList.at(0).dataSourceIndex );
				if (pDS) {
					szYAxisTitle = pDS->userdata;
					if (pDS->userFlags & DATASOURCE_USERFLAG_ANGLE_DEG)
						m_pPlot->setDataScaleUnits(MDTRA_YSU_DEGREES);
					else if (pDS->userFlags & DATASOURCE_USERFLAG_ANGLE_RAD)
						m_pPlot->setDataScaleUnits(MDTRA_YSU_RADIANS);
					else
						m_pPlot->setDataScaleUnits(MDTRA_YSU_ANGSTROMS);
				}
			}
			break;
		default:
			szYAxisTitle = QString("???");
			break;
		}

		if ( pResult->type != MDTRA_DT_USER )
			szYAxisTitle.append( QString::fromLocal8Bit(UTIL_GetYAxisUnitTitle(pResult->units,m_PlotLanguage)) );

		m_pPlot->setYAxisTitle(szYAxisTitle);
	}

	QStringList hLabels, vLabels;
	int maxDataSize = 0;
	int maxUserStat = 0;
	bool expandColumnNames = (pResult->sourceList.count() <= 1);

	for (int i = 0; i < pResult->sourceList.count(); i++) {
		int curDataSize = pResult->sourceList.at(i).iDataSize;
		if (curDataSize > maxDataSize)
			maxDataSize = curDataSize;
		dword statFlags = (pResult->sourceList.at(i).flags >> DSREF_STAT_SHIFT) & DSREF_STAT_MASK;
		for (int j = maxUserStat; j < 6; j++) {
			if ( statFlags & (1<<j) ) maxUserStat = j+1;
		}
		const MDTRA_DataSource *pDS = m_pProject->fetchDataSourceByIndex( pResult->sourceList.at(i).dataSourceIndex );
		QString dsTitle = "";
		if (pDS) dsTitle = pDS->name;
			
		hLabels << dsTitle;
	}
	hLabels << "";

	for (int i = 0; i < maxDataSize; i++) {
		if (pResult->layout == MDTRA_LAYOUT_TIME)
			vLabels << QString("%1").arg(i);
		else
			vLabels << QString("%1").arg(i+1);
	}

	if (m_pPlot)
		m_pPlot->setXLayout( pResult->layout );

	m_pResultTable->setRowCount( maxDataSize );
	m_pResultTable->setColumnCount( pResult->sourceList.count() + 1 );

	bool bHasCorrelationData = pResult->sourceList.at(0).pCorrelation;
	int statTableRowCount = MDTRA_SP_MAX_USED;
	statTableRowCount += maxUserStat;
	if (bHasCorrelationData)
		statTableRowCount += pResult->sourceList.count();

	m_pStatTable->setRowCount( statTableRowCount );
	m_pStatTable->setColumnCount( pResult->sourceList.count() + 1 );

	for (int i = 0; i < pResult->sourceList.count()+1; i++) {
		QTableWidgetItem *newItem;
		if (i == pResult->sourceList.count()) {
			newItem = new QTableWidgetItem( "" );
		} else {
			newItem = new QTableWidgetItem( QString("%1 - %2").arg(i+1).arg(hLabels.at(i)) );
			newItem->setTextAlignment( Qt::AlignLeft );
			newItem->setToolTip( hLabels.at(i) );
		}
		m_pResultTable->setHorizontalHeaderItem( i, newItem );
	}

	m_pResultTable->setVerticalHeaderLabels(vLabels);

	for (int i = 0; i < pResult->sourceList.count()+1; i++) {
		QTableWidgetItem *newItem;
		if (i == pResult->sourceList.count()) {
			newItem = new QTableWidgetItem( "" );
		} else {
			newItem = new QTableWidgetItem( QString("%1 - %2").arg(i+1).arg(hLabels.at(i)) );
			newItem->setTextAlignment( Qt::AlignLeft );
			newItem->setToolTip( hLabels.at(i) );
		}
		m_pStatTable->setHorizontalHeaderItem( i, newItem );
	}

	for (int i = 0; i < MDTRA_SP_MAX_USED; i++) {
		QTableWidgetItem *newItem = new QTableWidgetItem( UTIL_GetStatParmLabel(i) );
		newItem->setToolTip( UTIL_GetStatParmName(i) );
		m_pStatTable->setVerticalHeaderItem( i, newItem );
	}
	for (int i = 0; i < maxUserStat; i++) {
		QTableWidgetItem *newItem = new QTableWidgetItem( QString("U%1").arg(i+1) );
		newItem->setToolTip( QString("User Parameter %1").arg(i+1) );
		m_pStatTable->setVerticalHeaderItem( MDTRA_SP_MAX_USED + i, newItem );
	}
	if (bHasCorrelationData) {
		for (int i = 0; i < pResult->sourceList.count(); i++) {
			QTableWidgetItem *newItem = new QTableWidgetItem( tr("r(%1:X)").arg(i+1) );
			newItem->setToolTip( tr("Pearson correlation between \"%1\" and other data source (column X)").arg(hLabels.at(i)));
			m_pStatTable->setVerticalHeaderItem( MDTRA_SP_MAX_USED + maxUserStat + i, newItem );
		}
	}

	for (int i = 0; i < pResult->sourceList.count(); i++) {
		QString dataTitle;
		for (int j = 0; j < pResult->sourceList.at(i).iDataSize; j++) {
			QTableWidgetItem *newItem;
			
			if (expandColumnNames)
				newItem = new QTableWidgetItem(tr(" %1 ").arg( pResult->sourceList.at(i).pData[j], 0, 'f', 4));
			else
				newItem = new QTableWidgetItem(tr("%1").arg( pResult->sourceList.at(i).pData[j], 0, 'f', 4));

			newItem->setTextAlignment( Qt::AlignCenter );
			if (m_iViewer > 0 && pResult->layout == MDTRA_LAYOUT_TIME) newItem->setToolTip( tr( "Double-click to view PDB files (trajectory step %1)").arg(j));
			m_pResultTable->setItem(j, i, newItem);
		}
		for (int j = 0; j < MDTRA_SP_MAX_USED; j++) {
			QTableWidgetItem *newItem = new QTableWidgetItem( (pResult->sourceList.at(i).stat[j] == (-FLT_MAX)) ? tr(" - ") : tr(" %1 ").arg( pResult->sourceList.at(i).stat[j], 0, 'f', 4));
			newItem->setTextAlignment( Qt::AlignCenter );
			m_pStatTable->setItem(j, i, newItem);
		}
		dword statFlags = (pResult->sourceList.at(i).flags >> DSREF_STAT_SHIFT) & DSREF_STAT_MASK;
		for (int j = 0; j < maxUserStat; j++) {
			QTableWidgetItem *newItem = new QTableWidgetItem( (!(statFlags & (1<<j))) ? tr(" - ") : tr(" %1 ").arg( pResult->sourceList.at(i).stat[MDTRA_SP_USER1+j], 0, 'f', 4));
			newItem->setTextAlignment( Qt::AlignCenter );
			m_pStatTable->setItem(j+MDTRA_SP_MAX_USED, i, newItem);
		}
		if (bHasCorrelationData) {
			for (int j = 0; j < pResult->sourceList.count(); j++) {
				QTableWidgetItem *newItem;
				
				if (expandColumnNames)
					newItem = new QTableWidgetItem( (pResult->sourceList.at(i).pCorrelation[j] == (-FLT_MAX)) ? tr(" - ") : tr(" %1 ").arg( pResult->sourceList.at(i).pCorrelation[j], 0, 'f', 4));
				else
					newItem = new QTableWidgetItem( (pResult->sourceList.at(i).pCorrelation[j] == (-FLT_MAX)) ? tr("-") : tr("%1").arg( pResult->sourceList.at(i).pCorrelation[j], 0, 'f', 4));

				newItem->setToolTip( tr("Pearson correlation between \"%1\" and \"%2\"").arg(hLabels.at(i), hLabels.at(j)));
				newItem->setTextAlignment( Qt::AlignCenter );
				m_pStatTable->setItem(j+maxUserStat+MDTRA_SP_MAX_USED, i, newItem);
			}
		}
	
		MDTRA_DataSource *pDS = m_pProject->fetchDataSourceByIndex( pResult->sourceList.at(i).dataSourceIndex );
		if (pDS) dataTitle = pDS->name;

		if (m_pPlot)
			m_pPlot->addPlotData( dataTitle, &pResult->sourceList.at(i) );
	}
	
	if (expandColumnNames) {
		m_pResultTable->horizontalHeader()->resizeSections( QHeaderView::ResizeToContents );
		m_pStatTable->horizontalHeader()->resizeSections( QHeaderView::ResizeToContents );
	} else {
		for (int i = 0; i < pResult->sourceList.count(); i++) {
			m_pResultTable->horizontalHeader()->resizeSection( i, m_pResultTable->horizontalHeader()->defaultSectionSize() );
			m_pStatTable->horizontalHeader()->resizeSection( i, m_pStatTable->horizontalHeader()->defaultSectionSize() );
		}
	}

	m_pResultTable->horizontalHeader()->setStretchLastSection( true );	
	m_pStatTable->horizontalHeader()->setStretchLastSection( true );

	if (m_pPlot)
		m_pPlot->enable();
}

void MDTRA_MainWindow :: editPreferences( void )
{
	MDTRA_PreferencesDialog dialog(this);
	if (dialog.exec()) {
		dialog.savePreferences();
		ThreadSetDefault( m_iThreadCount, m_bLowPriority ? 0 : 1 );
		select_result_collector();
	} else {
		dialog.discardPreferences();
	}
}

void MDTRA_MainWindow :: plotToggleGrid( void )
{
	m_bPlotShowGrid = !m_bPlotShowGrid;
	m_pPlot->updateGL();

	for (int i = 0; i < m_pActionList.size(); ++i) {
		if (m_pActionList.at(i)->objectName() == QString("PlotToggleGrid")) {
			m_pActionList.at(i)->setChecked(m_bPlotShowGrid);
		}
	}
}

void MDTRA_MainWindow :: plotToggleLabels( void )
{
	m_bPlotShowLabels = !m_bPlotShowLabels;
	m_pPlot->updateGL();

	for (int i = 0; i < m_pActionList.size(); ++i) {
		if (m_pActionList.at(i)->objectName() == QString("PlotToggleLabels")) {
			m_pActionList.at(i)->setChecked(m_bPlotShowLabels);
		}
	}
}

void MDTRA_MainWindow :: plotToggleLegend( void )
{
	m_bPlotShowLegend = !m_bPlotShowLegend;
	m_pPlot->updateGL();

	for (int i = 0; i < m_pActionList.size(); ++i) {
		if (m_pActionList.at(i)->objectName() == QString("PlotToggleLegend")) {
			m_pActionList.at(i)->setChecked(m_bPlotShowLegend);
		}
	}
}

void MDTRA_MainWindow :: plotToggleMouseTrack( void )
{
	m_bPlotMouseTrack = !m_bPlotMouseTrack;
	m_pPlot->updateGL();

	for (int i = 0; i < m_pActionList.size(); ++i) {
		if (m_pActionList.at(i)->objectName() == QString("PlotToggleMouseTrack")) {
			m_pActionList.at(i)->setChecked(m_bPlotMouseTrack);
		}
	}
}

bool MDTRA_MainWindow :: plotSaveImage( void )
{
	QString imageFilters;
	QString selectedFilter;
	QMap<QString,QString> extMap;
	QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();

	for ( int i = 0; i < imageFormats.count(); i++ ) {
		QString s1 = QString( "%1 Files (*.%2)" ).arg( imageFormats.at(i).toUpper().constData() ).arg( imageFormats.at(i).toLower().constData() );
		extMap[s1] = QString( ".%1" ).arg( imageFormats.at(i).toLower().constData() );
		if (i > 0) imageFilters.append(";;");
		imageFilters.append( s1 );
	}

#ifdef MDTRA_ALLOW_PRINTER
	if (imageFilters.length() > 0) imageFilters.append(";;");
	imageFilters.append("PDF Files (*.pdf);;PostScript Files (*.ps);;");
	extMap["PDF Files (*.pdf)"] = ".pdf";
	extMap["PostScript Files (*.ps)"] = ".ps";
#endif

	if ( imageFilters.length() <= 0 ) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No image formats supported!"));
		return false;
	}

	QString fileName = QFileDialog::getSaveFileName( this, tr("Save File"), m_currentFileDir, imageFilters, &selectedFilter );
	if (!fileName.isEmpty()) {
		QFileInfo fi( fileName );
		if ( fi.suffix().isEmpty() ) {
			fileName.append( extMap[selectedFilter] );
			fi.setFile( fileName );
		}
		QString fType = fi.suffix().toUpper();
		if (m_pPlot->saveScreenshot( fileName, fType.toAscii() )) {
			return true;
		} else {
			return false;
		}
	}
	return false;
}

static void SavePDF_NextPageFn( void )
{
	if ( g_pMainWindow )
		g_pMainWindow->advanceResultCollector();
}

void MDTRA_MainWindow :: advanceResultCollector( void )
{
	int currentRow = m_pResultCollectionList->currentRow();
	if ( currentRow < m_pResultCollectionList->count()-1 )
		m_pResultCollectionList->setCurrentRow( currentRow + 1 );
}

bool MDTRA_MainWindow :: plotSavePDF( void )
{
	bool bResult = false;

#ifdef MDTRA_ALLOW_PRINTER
	QString pdfFilter = "PDF Files (*.pdf);;";
	QString fileName = QFileDialog::getSaveFileName( this, tr("Save PDF"), m_currentFileDir, pdfFilter );
	
	int currentResult = m_pResultCollectionList->currentRow();
	m_pResultCollectionList->setCurrentRow( 0 );

	if (!fileName.isEmpty()) {
		QFileInfo fi(fileName);
		if ( fi.suffix().isEmpty() )
			fileName.append( ".pdf" );

		bResult = m_pPlot->printAllPages( fileName, m_pResultCollectionList->count(), SavePDF_NextPageFn );
	}

	m_pResultCollectionList->setCurrentRow( currentResult );
	updatePlot();
#endif
	return bResult;
}

void MDTRA_MainWindow :: updatePlot( void )
{
	if (m_pPlot)
		m_pPlot->updateGL();
}

void MDTRA_MainWindow :: plotToggleDataVisibility( void )
{
	if (m_pResultCollectionList->selectedItems().count() <= 0)
		return;

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);
	MDTRA_PlotDataFilterDialog dialog( pItem->data(Qt::UserRole).toInt(), this );
	if (dialog.exec()) {
		m_pPlot->updateGL();
		updateTitleBar( true );
	}
}

void MDTRA_MainWindow :: toolsSelectAtoms( void )
{
	if (m_pProject->getValidStreamCount() < 1) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open Selection Tool: no valid streams!"));
        return;
	}

	MDTRA_SelectionDialog dialog(this);
	dialog.exec();
}

void MDTRA_MainWindow :: toolsPCA( void )
{
	if (m_pProject->getValidStreamCount() < 1) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open PCA Tool: no valid streams!"));
        return;
	}

	MDTRA_PCADialog dialog(this);
	if (dialog.exec()) {
		dialog.hide();
		const MDTRA_PCAInfo *pInfo = dialog.getPCAInfo();
		size_t iOutOfMemSize = 0;
		if (!SetupPCA( this, pInfo, &iOutOfMemSize )) {
			if (!iOutOfMemSize)
				QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Failed to setup PCA!"));
			else
				QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Failed to setup PCA: memory allocation error!\nCouldn't allocate %1 Mb of memory.\n\nPlease modify the selection to reduce number of atoms to analyze.").arg(iOutOfMemSize / (1024.0f*1024.0f),0,'f',2));

			FreePCA();
			return;
		}
		if (PerformPCA()) {
			//Show search results dialog
			MDTRA_PCAResultsDialog resultsDialog(this);
			resultsDialog.exec();
		}
		FreePCA();
	}
}

void MDTRA_MainWindow :: toolsDistanceSearch( void )
{
	if (m_pProject->getValidStreamCount() < 2) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open Distance Search Tool: at least 2 valid streams required!"));
        return;
	}

	MDTRA_DistanceSearchDialog dialog(this);
	if (dialog.exec()) {
		dialog.hide();
		const MDTRA_DistanceSearchInfo *pSearchInfo = dialog.getDistanceSearchInfo();
		if (!SetupDistanceSearch( this, pSearchInfo )) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Failed to setup distance search!"));
			FreeDistanceSearch();
			return;
		}
		if (PerformDistanceSearch()) {
			//Show search results dialog
			MDTRA_DistanceSearchResultsDialog resultsDialog(this);
			resultsDialog.exec();
		}
		FreeDistanceSearch();
	}
}

void MDTRA_MainWindow :: toolsTorsionSearch( void )
{
	if (m_pProject->getValidStreamCount() < 2) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open Torsion Search Tool: at least 2 valid streams required!"));
        return;
	}

	MDTRA_TorsionSearchDialog dialog(this);
	if (dialog.exec()) {
		dialog.hide();
		const MDTRA_TorsionSearchInfo *pSearchInfo = dialog.getTorsionSearchInfo();
		if (!SetupTorsionSearch( this, pSearchInfo )) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Failed to setup torsion search!"));
			FreeTorsionSearch();
			return;
		}
		if (PerformTorsionSearch()) {
			//Show search results dialog
			MDTRA_TorsionSearchResultsDialog resultsDialog(this);
			resultsDialog.exec();
		}
		FreeTorsionSearch();
	}
}

void MDTRA_MainWindow :: toolsForceSearch( void )
{
	if (m_pProject->getValidStreamCount( (1<<PDB_FS_FORCE_X)|(1<<PDB_FS_FORCE_Y)|(1<<PDB_FS_FORCE_Z) ) < 2) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open Force Search Tool: at least 2 valid streams (with PDB format supporting forces) required!"));
        return;
	}

	MDTRA_ForceSearchDialog dialog(this);
	if (dialog.exec()) {
		dialog.hide();
		const MDTRA_ForceSearchInfo *pSearchInfo = dialog.getForceSearchInfo();
		if (!SetupForceSearch( this, pSearchInfo )) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Failed to setup force search!"));
			FreeForceSearch();
			return;
		}
		if (PerformForceSearch()) {
			//Show search results dialog
			MDTRA_ForceSearchResultsDialog resultsDialog(this);
			resultsDialog.exec();
		}
		FreeForceSearch();
	}
}

void MDTRA_MainWindow :: toolsHBSearch( void )
{
	if (m_pProject->getValidStreamCount() < 1) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open H-Bonds Search Tool: no valid streams!"));
        return;
	}

	MDTRA_HBSearchDialog dialog(this);
	if (dialog.exec()) {
		dialog.hide();
		const MDTRA_HBSearchInfo *pSearchInfo = dialog.getHBSearchInfo();

		if (!SetupHBSearch( this, pSearchInfo )) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Failed to setup H-Bonds search!"));
			FreeHBSearch();
			return;
		}
		if (PerformHBSearch()) {
			//Show search results dialog
			MDTRA_HBSearchResultsDialog resultsDialog(this);
			resultsDialog.exec();
		}
		FreeHBSearch();
	}
}

void MDTRA_MainWindow :: tools2DRMSD( void )
{
	if (m_pProject->getValidStreamCount() < 1) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open 2D-RMSD Tool: no valid streams!"));
        return;
	}

	MDTRA_2D_RMSD_Dialog dialog(this);
	dialog.exec();
}

void MDTRA_MainWindow :: toolsHistogram( void )
{
	if (m_pProject->getResultCount() < 1) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open Histogram Tool: no valid results!"));
        return;
	}

	MDTRA_HistogramDialog dialog(this);
	dialog.exec();
}

void MDTRA_MainWindow :: toolsDDM( void )
{
	if (m_pProject->getValidStreamCount() < 1) {
        QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open DNA Data Mining Tool: no valid streams!"));
        return;
	}

	MDTRA_DNADataMiningDialog dialog(this);
	dialog.exec();
}

void MDTRA_MainWindow :: prepWaterShell( void )
{
	if (m_pProject->getValidStreamCount() < 1) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Cannot open WaterShell prep: no valid streams!"));
		return;
	}

	MDTRA_PrepWaterShellDialog dialog(this);
	if (dialog.exec()) {
		dialog.hide();
		//!TODO
	}
}

void MDTRA_MainWindow :: exec_on_cell_dblclicked( int row, int column )
{
	if (m_pResultCollectionList->selectedItems().count() <= 0)
		return;

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);
	int resultIndex = pItem->data(Qt::UserRole).toInt();

	MDTRA_Result *pResult = m_pProject->fetchResultByIndex( resultIndex );
	if (!pResult || pResult->layout != MDTRA_LAYOUT_TIME)
		return;

	viewPDBFiles( row );
}

void MDTRA_MainWindow :: viewPDBFiles( int trajectoryPos )
{
	const int exe_wait_timeout_ms = 100;

	if (m_iViewer <= 0)
		return;

	if (m_pResultCollectionList->selectedItems().count() <= 0)
		return;

	if (m_iViewer == 1) {
		if (!m_pPDBRenderer) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Internal PDB viewer is not yet implemented!"));
		} else {
			m_pPDBRenderer->setDataDescSnapshot( trajectoryPos );
			switch_to_pdb_renderer();
		}
		return;
	}

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);
	int resultIndex = pItem->data(Qt::UserRole).toInt();

	QStringList pdbList;
	QList<int> indexList;

	if (!generateTemporaryPDBFiles( resultIndex, trajectoryPos, pdbList, indexList )) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Failed to generate temporary PDB files!"));
		return;
	}

	//create script file
	QString tempPath = QDir::tempPath();
	QString scriptName = QString("%1/mdtra_temp.txt").arg(tempPath);
	QFile f(scriptName);
	if (f.open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream stream(&f);
		switch (m_iViewer) {
		default:
			f.close();
			break;
		case 2:
			{
				//Write RasMol script
				for (int i = 0; i < pdbList.count(); i++) {
					QColor c = getColorManager()->color( COLOR_PLOT_DATA1 + (indexList.at(i) % NUM_DATA_COLORS) );
					stream << "load pdb \"" << pdbList.at(i) << "\"" << endl;
					stream << "molecule " << (i+1) << endl;
					stream << "colour [" << c.red() << "," << c.green() << "," << c.blue() << "]" << endl;
				}
				stream << "rotate all";
				f.close();
				//Run RasMol
				QStringList runArgs;
				runArgs << "-script" << scriptName;
				if (!runExecutable( m_rasMolPath, runArgs )) {
					QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Failed to run executable file: \nFile: %1\nArgs: %2 %3").arg(m_rasMolPath).arg(runArgs.at(0)).arg(runArgs.at(1)));
				}
#if defined(WIN32)
				Sleep(exe_wait_timeout_ms);
#elif defined(LINUX)
				usleep(exe_wait_timeout_ms*1000);
#endif
			}
			break;
		case 3:
			{
				//Write VMD script
				for (int i = 0; i < pdbList.count(); i++) {
					QColor c = getColorManager()->color( COLOR_PLOT_DATA1 + (indexList.at(i) % NUM_DATA_COLORS) );
					stream << "mol addrep " << i << endl;
					stream << "mol new {" << pdbList.at(i) << "} type {pdb}" << endl;
					stream << "color change rgb " << (i+3) << " " <<  (c.red() / 255.0f) << " " << (c.green() / 255.0f) << " " << (c.blue() / 255.0f) << endl;
					stream << "mol modcolor 0 " << i << " ColorID " << (i+3) << endl;
				}
				stream << "display resetview";
				f.close();
				//Run VMD
				QStringList runArgs;
				runArgs << "-e" << scriptName;
				if (!runExecutable( m_vmdPath, runArgs )) {
					QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Failed to run executable file: \nFile: %1\nArgs: %2 %3").arg(m_vmdPath).arg(runArgs.at(0)).arg(runArgs.at(1)));
				}
#if defined(WIN32)
				Sleep(exe_wait_timeout_ms);
#elif defined(LINUX)
				usleep(exe_wait_timeout_ms*1000);
#endif
			}
			break;
		}
		
	} else {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Failed to generate temporary script file: \n\n%1").arg(scriptName));
	}

	//Cleanup
	m_tempFiles << scriptName;
	for (int i = 0; i < pdbList.count(); i++)
		m_tempFiles << pdbList.at(i);
	m_tempFiles.removeDuplicates();

	pdbList.clear();
	indexList.clear();
}

bool MDTRA_MainWindow :: generateTemporaryPDBFiles( int resultIndex, int trajectoryPos, QStringList &pdbList, QList<int> &indexList )
{
	MDTRA_Result *pResult = m_pProject->fetchResultByIndex( resultIndex );
	if (!pResult)
		return false;

	QString tempPath = QDir::tempPath();
	MDTRA_PDB_File *pFirstPDB = NULL;

	for (int i = 0; i < pResult->sourceList.count(); i++) {
		if (!(pResult->sourceList.at(i).flags & DSREF_FLAG_VISIBLE))
			continue;
		int dsIndex = pResult->sourceList.at(i).dataSourceIndex;
		MDTRA_DataSource *pDS = m_pProject->fetchDataSourceByIndex( dsIndex );
		if (!pDS)
			continue;
		MDTRA_Stream *pStream = m_pProject->fetchStreamByIndex( pDS->streamIndex );
		if (!pStream)
			continue;
		if ( trajectoryPos >= pStream->files.count() )
			continue;

		MDTRA_PDB_File *pdbFile = new MDTRA_PDB_File(0, pStream->format_identifier, pStream->files.at(trajectoryPos).toAscii(), pStream->flags);
		if (!pdbFile)
			continue;
		if (!pdbFile->getAtomCount()) {
			delete pdbFile;
			continue;
		}

		pdbFile->move_to_centroid();

		if (!pFirstPDB)
			pFirstPDB = pdbFile;
		else
			pdbFile->align_kabsch( pFirstPDB );

		QString streamName = pStream->name;
		QString tempName = QString("%1/%2_%3.pdb").arg(tempPath).arg(streamName.replace(" ","_")).arg(trajectoryPos);
		if (pdbFile->save( tempName.toAscii() )) {
			pdbList << tempName;
			indexList << i;
		}
		
		if (pFirstPDB != pdbFile)
			delete pdbFile;
	}

	if (pFirstPDB)
		delete pFirstPDB;

	return (pdbList.count() > 0);
}

bool MDTRA_MainWindow :: runExecutable( const QString &exeName, const QStringList &args )
{
	QProcess *qp = new QProcess();
	qp->start(exeName, args);
	return qp->waitForStarted();
}

void  MDTRA_MainWindow :: addLabel( const MDTRA_Label *pLabel )
{
	if (m_pResultCollectionList->selectedItems().count() <= 0)
		return;

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);
	int resultIndex = pItem->data(Qt::UserRole).toInt();

	if (m_pProject->addResultLabel( resultIndex, pLabel ))
		updateTitleBar( true );
}

void  MDTRA_MainWindow :: editLabel( int labelIndex, const MDTRA_Label *pLabel )
{
	if (m_pResultCollectionList->selectedItems().count() <= 0)
		return;

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);
	int resultIndex = pItem->data(Qt::UserRole).toInt();

	if (m_pProject->editResultLabel( resultIndex, labelIndex, pLabel ))
		updateTitleBar( true );
}

bool MDTRA_MainWindow :: removeLabel( int labelIndex )
{
	if (m_pResultCollectionList->selectedItems().count() <= 0)
		return false;

	const QListWidgetItem *pItem = m_pResultCollectionList->selectedItems().at(0);
	int resultIndex = pItem->data(Qt::UserRole).toInt();

	return m_pProject->removeResultLabel( resultIndex, labelIndex );
}

void MDTRA_MainWindow :: setUseCUDA( bool value ) 
{ 
	m_bUseCUDA = value; 

	if (!g_bSupportsCUDA)
		m_bUseCUDA = false;
#if defined(MDTRA_ALLOW_SSE)
	g_bAllowCUDA = m_bUseCUDA;
#endif

	for (int i = 0; i < m_pActionList.size(); ++i) {
		if (m_pActionList.at(i)->objectName() == QString("HelpCUDA")) {
			m_pActionList.at(i)->setEnabled(g_bSupportsCUDA);
		}
	}
}

void MDTRA_MainWindow :: enablePlotControls( bool bEnable )
{
	for (int i = 0; i < m_pActionList.size(); ++i) {
		if (m_pActionList.at(i)->objectName() == "PlotToggleMouseTrack") {
			m_pActionList.at(i)->setEnabled( bEnable );
		} else if (m_pActionList.at(i)->objectName() == "PlotToggleGrid") {
			m_pActionList.at(i)->setEnabled( bEnable );
		} else if (m_pActionList.at(i)->objectName() == "PlotToggleLabels") {
			m_pActionList.at(i)->setEnabled( bEnable );
		} else if (m_pActionList.at(i)->objectName() == "PlotToggleLegend") {
			m_pActionList.at(i)->setEnabled( bEnable );
		}
	}
}

void MDTRA_MainWindow :: switch_to_plot( void )
{
	if ( !m_pPDBRenderer || !m_pPDBRenderer->isVisible() )
		return;

	m_pPDBRenderer->setActive( false );
	m_pPDBRenderer->setVisible( false );
	m_pPlot->setVisible( true );
	m_pPlotBox->setTitle( tr("Plot") );
	enablePlotControls( true );

	updatePanelVisibility( "ViewSwitchToPDB", false );
	updatePanelVisibility( "ViewSwitchToPlot", true );
}

void MDTRA_MainWindow :: switch_to_pdb_renderer( void )
{
	if ( !m_pPDBRenderer || m_pPDBRenderer->isVisible() )
		return;

	m_pPlot->setVisible( false );
	m_pPDBRenderer->setVisible( true );
	m_pPDBRenderer->setActive( true );
	m_pPlotBox->setTitle( tr("PDB Renderer") );
	enablePlotControls( false );

	updatePanelVisibility( "ViewSwitchToPlot", false );
	updatePanelVisibility( "ViewSwitchToPDB", true );
}

void MDTRA_MainWindow :: toggle_pdb_renderer( void )
{
	if ( m_pPlot->isVisible() )
		switch_to_pdb_renderer();
	else
		switch_to_plot();	
}

