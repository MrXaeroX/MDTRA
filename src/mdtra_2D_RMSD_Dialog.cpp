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

// Purpose:
//	Implementation of MDTRA_2D_RMSD_Dialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb.h"
#include "mdtra_compact_pdb.h"
#include "mdtra_utils.h"
#include "mdtra_select.h"
#include "mdtra_inputTextDialog.h"
#include "mdtra_progressDialog.h"
#include "mdtra_2D_RMSD_Dialog.h"
#include "mdtra_2D_RMSD_Plot.h"

#include <QtGui/QFileDialog>
#include <QtGui/QImageWriter>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QKeyEvent>


#define MAX_TEXTURE_SIZE	512

static inline int DataCellIndex( int d1, int d2 )
{
	assert( d1 < d2 );
	return ((d2*(d2-1))>>1) + d1;
}

static inline int DataCellSize( int num )
{
	return (num*(num-1))>>1;
}

MDTRA_2D_RMSD_Dialog :: MDTRA_2D_RMSD_Dialog( QWidget *parent )
					  : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	m_bProfiling = m_pMainWindow->allowProfiling();

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/rmsd2d.png") );

#ifndef QT_NO_WHATSTHIS
	sel_string->setWhatsThis(QString("<b>Selection Quick Hint</b><br><br>atomno=100 <i>select single atom</i><br>atomno&gt;=100 and atomno&lt;=200 <i>select range of atoms</i><br>carbon <i>select all carbons</i><br>:a <i>select chain A</i><br>*.ca <i>select all alpha-carbons</i><br>lys.cg <i>select all gamma-carbons in lysine residues</i><br>:a.cg <i>select all gamma-carbons in chain A</i><br>1-3.ca <i>select alpha-carbons in residues 1, 2 and 3</i><br>*.h? <i>select hydrogens with 2-character name</i><br>*.h[12] <i>select hydrogens named H1 and H2</i><br>*.h[1-3] <i>select hydrogens named H1, H2 and H3</i><br>*.h* <i>select all hydrogens</i><br>lys <i>select all lysines (LYS)</i><br>{lys} <i>select all lysines, including neutral (LYS, LYN)</i><br>gl? <i>select all glutamates and glutamines</i><br>(lys, arg) and :b <i>select lysines and arginines in chain B</i><br>protein <i>select all protein atoms</i><br>dna <i>select all nucleic atoms (may also specify \"nucleic\" keyword)</i><br>water and *.o <i>select all oxygen atoms in water molecules</i><br>within (3.5, lys) <i>select all atoms within 3.5 angstroms of any lysine residue</i><br>not dna within (3.0, dna) <i>select non-nucleic atoms within 3.0 angstroms of nucleic acid</i>"));
#endif // QT_NO_WHATSTHIS

	m_pPDBFiles = NULL;
	m_pDataBuffer = NULL;
	m_pTextureData = NULL;
	m_iTextureSize = 0;
	m_iNumPDBFiles = 0;
	m_iSelectionFlags = 0;
	m_iSelectionSize = 0;
	m_pSelectionData = NULL;
	m_bSelectionError = false;
	m_bBuildStarted = false;
	m_bCachedRGB = false;
	m_flCachedPlotMin = -1.0f;
	sel_string->clear();
	m_cachedSelectionText.clear();
	m_cachedSelectionText2.clear();
	m_cachedTitle.clear();
	m_pSelectionParser = new MDTRA_SelectionParser<MDTRA_PDB_File>;
	sel_parse->setIcon( QIcon(":/png/16x16/csel.png") );

	//fill stream combo
	for (int i = 0; i < m_pMainWindow->getProject()->getStreamCount(); i++) {
		MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStream( i );
		if (pStream && pStream->pdb) {
			sCombo->addItem( QIcon(":/png/16x16/stream.png"), 
							 tr("STREAM %1: %2 (%3 files)").arg(pStream->index).arg(pStream->name).arg(pStream->files.count()),
							 pStream->index );
		}
	}
	sCombo->setCurrentIndex(0);
	m_cachedStreamIndex = 0;
	m_cachedPDBStreamIndex = -1;
	m_cachedPDBMinMax[0] = 0;
	m_cachedPDBMinMax[1] = 0;
	exec_on_stream_change();

	progressBar->setVisible( false );
	progressBarMsg->setText( tr("Data not ready") );

	QGLFormat pixelFormat;
	pixelFormat.setDoubleBuffer(true);
	pixelFormat.setDepth(false);
	pixelFormat.setRgba(true);
	pixelFormat.setSampleBuffers( m_pMainWindow->multisampleAA() );
	QGLFormat::setDefaultFormat(pixelFormat);

	m_pPlot = new MDTRA_2D_RMSD_Plot( pixelFormat, parent );
	m_pPlot->setParent( groupBox_3 );
	m_pPlot->setStatusTip( tr("2D RMSD Plot") );
	m_pPlot->setGeometry(QRect(10, 25, groupBox_3->width() - 20, groupBox_3->height() - 35));
	m_pPlot->setVisible( false );

#ifdef _DEBUG
	sel_string->setText("*.ca");
	exec_on_selection_editingFinished();
#endif

	connect(plotTitle, SIGNAL(editingFinished()), this, SLOT(exec_on_update_plot_title()));
	connect(optSmooth, SIGNAL(stateChanged(int)), this, SLOT(exec_on_toggle_smooth()));
	connect(optLegend, SIGNAL(stateChanged(int)), this, SLOT(exec_on_toggle_legend()));
	connect(sCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exec_on_stream_change()));
	connect(btnRebuild, SIGNAL(clicked()), this, SLOT(exec_on_rebuild()));
	connect(btnSave, SIGNAL(clicked()), this, SLOT(exec_on_save()));
	connect(btnClose, SIGNAL(clicked()), this, SLOT(reject()));
	connect(sel_string, SIGNAL(editingFinished()), this, SLOT(exec_on_selection_editingFinished()));
	connect(sel_parse, SIGNAL(clicked()), this, SLOT(exec_on_selection_editingFinished()));
	connect(sel_labelResults, SIGNAL(linkActivated(const QString&)), this, SLOT(exec_on_selection_showWholeSelection()));
}

MDTRA_2D_RMSD_Dialog :: ~MDTRA_2D_RMSD_Dialog()
{
	if (m_pSelectionParser) {
		delete m_pSelectionParser;
		m_pSelectionParser = NULL;
	}
	if (m_pSelectionData) {
		delete [] m_pSelectionData;
		m_pSelectionData = NULL;
	}
	if (m_pPDBFiles) {
		for (int i = 0; i < m_iNumPDBFiles; i++) delete m_pPDBFiles[i];
		delete [] m_pPDBFiles;
		m_pPDBFiles = NULL;
	}
	if (m_pDataBuffer) {
		delete [] m_pDataBuffer;
		m_pDataBuffer = NULL;
	}
	if (m_pTextureData) {
		delete [] m_pTextureData;
		m_pTextureData = NULL;
	}
}

bool MDTRA_2D_RMSD_Dialog :: event( QEvent *ev )
{
	if (ev->type() == QEvent::KeyPress) {
		QKeyEvent *kev = static_cast<QKeyEvent*>(ev);
		if (kev->key() == Qt::Key_Return) {
			if (sel_string->hasFocus()) {
				return true;
			}
		}
	}
	return QDialog::event( ev );
}

int MDTRA_2D_RMSD_Dialog :: current_stream_index( void )
{
	return sCombo->itemData( sCombo->currentIndex() ).toInt();
}

void MDTRA_2D_RMSD_Dialog :: exec_on_stream_change( void )
{
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( current_stream_index() );
	eIndex->setMaximum( pStream->files.count() );
	if (!trajectoryRange->isChecked())
		eIndex->setValue( pStream->files.count() );
	m_cachedTitle.clear();
}

void MDTRA_2D_RMSD_Dialog :: exec_on_selection_editingFinished( void )
{
	update_selection();
	display_selection();
}

static MDTRA_2D_RMSD_Dialog *pFuncSrcObject = NULL;

void rmsd2d_print_f( const QString &msg )
{
	if (!pFuncSrcObject) return;
	pFuncSrcObject->sel_labelResults->setText( pFuncSrcObject->sel_labelResults->text().append( msg ) );
}

void rmsd2d_select_f( const MDTRA_SelectionSet<MDTRA_PDB_File>* pSet )
{
	if (!pFuncSrcObject) return;
	pFuncSrcObject->m_iSelectionSize = 0;
	for (int i = 0; i < pSet->realsize(); i++)
		if (pSet->value(i)) pFuncSrcObject->m_iSelectionSize++;

	if (!pFuncSrcObject->m_iSelectionSize)
		return;

	pFuncSrcObject->m_pSelectionData = new int[pFuncSrcObject->m_iSelectionSize];
	for (int i = 0, j = 0; i < pSet->realsize(); i++)
		if (pSet->value(i)) pFuncSrcObject->m_pSelectionData[j++] = i;
}

void MDTRA_2D_RMSD_Dialog :: exec_on_selection_showWholeSelection( void )
{
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( current_stream_index() );
	if (!pStream)
		return;

	QString selWholeText;

	for (int i = 0; i < m_iSelectionSize; i++) {

		if (selWholeText.length() > 0)
			selWholeText = selWholeText.append(", ");

		const MDTRA_PDB_Atom *pCurrentAtom = pStream->pdb->fetchAtomByIndex( m_pSelectionData[i] );
		if (!pCurrentAtom)
			selWholeText = selWholeText.append("<bad index>");
		else
			selWholeText = selWholeText.append(QString("%1-%2 %3").arg(pCurrentAtom->trimmed_residue).arg(pCurrentAtom->residuenumber).arg(pCurrentAtom->trimmed_title));
	}

	MDTRA_InputTextDialog dlg( tr("Selection: %1 atom(s)").arg(m_iSelectionSize), tr("<b>Expression:</b> %1").arg(sel_string->text()), selWholeText, true, this );
	dlg.exec();
}

#if defined(WIN32)
#define SELECTION_LABEL_ITEMS	15
#else
#define SELECTION_LABEL_ITEMS	10
#endif

void MDTRA_2D_RMSD_Dialog :: display_selection( void )
{
	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( current_stream_index() );

	sel_atCount->setText( QString("%1").arg( m_iSelectionSize ) );

	if (!m_bSelectionError)
	{
		sel_labelResults->clear();

		//display max. of SELECTION_LABEL_ITEMS items of selection
		for (int i = 0; i < SELECTION_LABEL_ITEMS; i++) {
			if (i >= m_iSelectionSize)
				break;

			if (sel_labelResults->text().length() > 0)
				sel_labelResults->setText( sel_labelResults->text().append(", ") );

			const MDTRA_PDB_Atom *pCurrentAtom = pStream->pdb->fetchAtomByIndex( m_pSelectionData[i] );
			if (!pCurrentAtom)
				sel_labelResults->setText( sel_labelResults->text().append("<bad index>") );
			else
				sel_labelResults->setText( sel_labelResults->text().append(QString("%1-%2 %3").arg(pCurrentAtom->trimmed_residue).arg(pCurrentAtom->residuenumber).arg(pCurrentAtom->trimmed_title)) );
		}

		if (m_iSelectionSize > SELECTION_LABEL_ITEMS)
			sel_labelResults->setText( sel_labelResults->text().append(QString(" (<a href=\"showmore\">%1 more...</a>)").arg(m_iSelectionSize - SELECTION_LABEL_ITEMS)) );
	}
}

void MDTRA_2D_RMSD_Dialog :: update_selection( void )
{
	if ((m_cachedSelectionText == sel_string->text()) && (m_cachedStreamIndex == current_stream_index()))
		return;

	m_cachedSelectionText = sel_string->text();
	m_cachedStreamIndex = current_stream_index();

	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( current_stream_index() );
	assert(pStream->pdb != NULL);

	m_iSelectionFlags = 0;
	m_iSelectionSize = 0;
	m_bSelectionError = false;
	pFuncSrcObject = this;

	if (m_pSelectionData) {
		delete [] m_pSelectionData;
		m_pSelectionData = NULL;
	}

	sel_labelResults->setText(tr("Updating selection, please wait..."));
	QApplication::processEvents();
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	if (!m_pSelectionParser->parse(pStream->pdb, sel_string->text().toAscii(), rmsd2d_select_f)) {
		sel_labelResults->clear();
		m_pSelectionParser->printErrors( rmsd2d_print_f );
		m_bSelectionError = true;
	}

	pFuncSrcObject = NULL;
	QApplication::restoreOverrideCursor();
	return;
}

static MDTRA_Compact_PDB_File** pFileList = NULL;
extern MDTRA_ProgressBarWrapper gProgressBarWrapper;
static int s_selectionSize = 0;
static int* s_selectionData = NULL;
static bool s_bCancelBuild = false;

static void InitProgressBar( QProgressBar *pBar, int max_progress, bool singleThreaded )
{
	pBar->setValue( 0 );
	pBar->setMaximum( 100 );
	gProgressBarWrapper.setProgressBar( pBar );
	gProgressBarWrapper.setMaximumValue( max_progress );
	gProgressBarWrapper.resetValue( 0 );
	gProgressBarWrapper.setSingleThreaded( singleThreaded );
	QApplication::processEvents();
}

static void AdvanceProgressBar( int num )
{
	gProgressBarWrapper.setValue( num );
	if (s_bCancelBuild)
		InterruptThreads();
}

static void fn_PostLoadPDBFiles( int threadnum, int num )
{
	pFileList[num]->set_rmsd_flag( s_selectionSize, s_selectionData );
	pFileList[num]->move_to_centroid();
	AdvanceProgressBar( num + 1 );
}

bool MDTRA_2D_RMSD_Dialog :: load_pdb_files( int trMin, int trMax, bool bSelectionChange )
{
	if (m_cachedPDBStreamIndex == m_cachedStreamIndex && 
		m_cachedPDBMinMax[0] == trMin && 
		m_cachedPDBMinMax[1] == trMax) {

		if (!bSelectionChange)
			return false;

		progressBarMsg->setText( tr("Updating PDB Files...") );

		pFileList = m_pPDBFiles;
		s_selectionSize = m_iSelectionSize;
		s_selectionData = m_pSelectionData;

		InitProgressBar( progressBar, trMax-trMin+1, false );
		RunThreadsOnIndividual( m_iNumPDBFiles, fn_PostLoadPDBFiles );
		return true;
	}

	m_cachedPDBStreamIndex = m_cachedStreamIndex;
	m_cachedPDBMinMax[0] = trMin;
	m_cachedPDBMinMax[1] = trMax;

	progressBarMsg->setText( tr("Loading PDB Files...") );
	InitProgressBar( progressBar, trMax-trMin+1, true );

	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( current_stream_index() );

	for (int i = 0; i < m_iNumPDBFiles; i++) delete m_pPDBFiles[i];
	delete [] m_pPDBFiles;
	delete [] m_pDataBuffer;
	delete [] m_pTextureData;
	m_pPDBFiles = NULL;
	m_pDataBuffer = NULL;
	m_pTextureData = NULL;
	m_iNumPDBFiles = trMax-trMin+1;
	m_iTextureSize = UTIL_BestPowerOf2( m_iNumPDBFiles, MAX_TEXTURE_SIZE );

	m_pPDBFiles = new MDTRA_Compact_PDB_File*[m_iNumPDBFiles];
	memset( m_pPDBFiles, 0, sizeof(MDTRA_Compact_PDB_File*)*m_iNumPDBFiles);

	m_pDataBuffer = new float[DataCellSize(m_iNumPDBFiles)];
	m_pTextureData = new byte[m_iTextureSize*m_iTextureSize*4];

	for (int i = 0; i < m_iNumPDBFiles; i++) {
		m_pPDBFiles[i] = new MDTRA_Compact_PDB_File();
		m_pPDBFiles[i]->load( 0, pStream->format_identifier, pStream->files.at(trMin+i-1).toAscii(), pStream->flags );
		AdvanceProgressBar( i + 1 );
		if (s_bCancelBuild) break;
	}

	if (s_bCancelBuild) {
		m_cachedPDBMinMax[0] = -1;
		m_cachedPDBMinMax[1] = -1;
		return true;
	}

	progressBarMsg->setText( tr("Updating PDB Files...") );

	pFileList = m_pPDBFiles;
	s_selectionSize = m_iSelectionSize;
	s_selectionData = m_pSelectionData;
	InitProgressBar( progressBar, trMax-trMin+1, false );
	RunThreadsOnIndividual( m_iNumPDBFiles, fn_PostLoadPDBFiles );

	if (s_bCancelBuild) {
		m_cachedPDBMinMax[0] = -1;
		m_cachedPDBMinMax[1] = -1;
		return true;
	}

	return true;
}

void MDTRA_2D_RMSD_Dialog :: profileStart( void )
{
#if defined(WIN32)
	m_profStart = timeGetTime();
#elif defined(LINUX)
	struct timeval tp;
	gettimeofday(&tp, NULL);
	m_profStart = tp.tv_sec*1000 + tp.tv_usec/1000;
#endif
}

void MDTRA_2D_RMSD_Dialog :: profileEnd( void )
{
	dword totalTime;
#if defined(WIN32)
	totalTime = timeGetTime() - m_profStart;
#elif defined(LINUX)
	struct timeval tp;
	gettimeofday(&tp, NULL);
	totalTime = tp.tv_sec*1000 + tp.tv_usec/1000 - m_profStart;
#endif
	QMessageBox::information( this, tr("Profiler"), QString("2D-RMSD calculation time: %1 ms").arg(totalTime) );
}

void MDTRA_2D_RMSD_Dialog :: calc_rmsd( int trMin, int trMax )
{
	int dataSize = trMax-trMin+1;
	memset(m_pDataBuffer, 0, sizeof(float)*DataCellSize(dataSize));

	progressBarMsg->setText( tr("Calculating RMSD...") );
	InitProgressBar( progressBar, DataCellSize(dataSize), true );

	m_dataMax = 0;

	if (m_bProfiling)
		profileStart();

	for (int i = 1; i < dataSize; i++) {
		for (int j = 0; j < i; j++) {
			int dataPos = DataCellIndex( j, i );
			float *pCell = m_pDataBuffer + dataPos;
			m_pPDBFiles[i]->align_kabsch( m_pPDBFiles[j] );
			*pCell = m_pPDBFiles[i]->get_rmsd( m_pPDBFiles[j] );
			if (*pCell > m_dataMax) m_dataMax = *pCell;
			AdvanceProgressBar( dataPos + 1 );
			if (s_bCancelBuild) break;
		}
		if (s_bCancelBuild) break;
	}

	if (s_bCancelBuild)
		return;
	
	if (m_bProfiling)
		profileEnd();
}

void MDTRA_2D_RMSD_Dialog :: build_texture( int trMin, int trMax )
{
	float c_minScaleRMSD = optAutoPlotMin->isChecked() ? 1.0f : spinPlotMin->value();

	int c = trMax-trMin+1;
	int ts = UTIL_BestPowerOf2( c, MAX_TEXTURE_SIZE );

	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( current_stream_index() );

	float scale = 1.0f;
	if (m_dataMax > 0.0f)
		scale = 1.0f / MDTRA_MAX( c_minScaleRMSD, m_dataMax );

	progressBarMsg->setText( tr("Building Texture...") );
	InitProgressBar( progressBar, (ts*ts)>>1, true );

	memset(m_pTextureData, 0, ts*ts*4);

	//OutputDebugString( QString( "Start building texture...%1 x %1\n").arg(ts).toAscii());

	for (int i = 0; i < ts; i++) {
		for (int j = 0; j < ts; j++) {
			float td = 0.0f;
			if (i > j)
				continue;
			if (i == j) {
				if (optRGB->isChecked()) {
					m_pTextureData[(i*ts+j)*4+0] = UTIL_FloatToColor(0, 0);
					m_pTextureData[(i*ts+j)*4+1] = UTIL_FloatToColor(1, 0);
					m_pTextureData[(i*ts+j)*4+2] = UTIL_FloatToColor(2, 0);
				}
				continue;
			}
			if (ts >= c) {
				int data_i = (i * c) / ts;
				int data_j = (j * c) / ts;
				//float td2 = -1.0f;
				if (data_i != data_j) {
					td = m_pDataBuffer[DataCellIndex(data_i, data_j)] * scale;
					//td2 = m_pDataBuffer[DataCellIndex(data_i, data_j)];
				}
				if ( td < 0.0f ) td = 0.0f;
				if ( td > 1.0f ) td = 1.0f;
				//OutputDebugString( QString( "Pixel (%1, %2): data (%3, %4) = %5 [%6]\n").arg(i).arg(j).arg(data_i).arg(data_j).arg(td).arg(td2).toAscii());
			} else {
				int data_i_min = (i * c) / ts;
				int data_j_min = (j * c) / ts;
				int data_i_max = ((i+1) * c) / ts;
				int data_j_max = ((j+1) * c) / ts;
				for (int ii = data_i_min; ii < data_i_max; ii++) {
					for (int jj = data_j_min; jj < data_j_max; jj++) {
						if (ii != jj) td += m_pDataBuffer[DataCellIndex(ii, jj)] * scale;
					}
				}
				td /= (float)((data_i_max - data_i_min)*(data_j_max - data_j_min));
				if ( td < 0.0f ) td = 0.0f;
				if ( td > 1.0f ) td = 1.0f;
			}

			assert( td >= 0.0f );
			assert( td <= 1.0f );

			if (!optRGB->isChecked()) {
				m_pTextureData[(i*ts+j)*4+0] = td * 255;
				m_pTextureData[(i*ts+j)*4+1] = td * 255;
				m_pTextureData[(i*ts+j)*4+2] = td * 255;
			} else {
				m_pTextureData[(i*ts+j)*4+0] = UTIL_FloatToColor(0, td);
				m_pTextureData[(i*ts+j)*4+1] = UTIL_FloatToColor(1, td);
				m_pTextureData[(i*ts+j)*4+2] = UTIL_FloatToColor(2, td);
			}
			m_pTextureData[(i*ts+j)*4+3] = 255;
			memcpy(&m_pTextureData[(j*ts+i)*4], &m_pTextureData[(i*ts+j)*4], 4);
			AdvanceProgressBar( (i*ts+j+1)>>1 );
			if (s_bCancelBuild) break;
		}
		if (s_bCancelBuild) break;
	}

	if (s_bCancelBuild)
		return;

	QString strPlotTitle = m_cachedTitle;
	if (strPlotTitle.indexOf("%s") != -1)
		strPlotTitle = strPlotTitle.replace("%s", "%1").arg(pStream->name);

	m_pPlot->setTitle( strPlotTitle );
	m_pPlot->setDimension( c, pStream->xscale );
	m_pPlot->loadTexture( ts, ts, m_pTextureData );
	m_pPlot->setSmoothTexture( optSmooth->isChecked() );

	m_pPlot->updateLegend( MDTRA_MAX( c_minScaleRMSD, m_dataMax ), optRGB->isChecked() );
	m_pPlot->setShowLegend( optLegend->isChecked() );
}

void MDTRA_2D_RMSD_Dialog :: exec_on_toggle_smooth( void )
{
	if (m_pPlot->isVisible()) {
		m_pPlot->setSmoothTexture( optSmooth->isChecked() );
		m_pPlot->updateGL();
	}
}

void MDTRA_2D_RMSD_Dialog :: exec_on_toggle_legend( void )
{
	if (m_pPlot->isVisible()) {
		m_pPlot->setShowLegend( optLegend->isChecked() );
		m_pPlot->updateGL();
	}
}

void MDTRA_2D_RMSD_Dialog :: exec_on_update_plot_title( void )
{
	if (m_pPlot->isVisible()) {
		if (m_cachedTitle != plotTitle->text()) {
			m_cachedTitle = plotTitle->text();
			QString strPlotTitle = m_cachedTitle;
			if (strPlotTitle.indexOf("%s") != -1) {
				const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( current_stream_index() );
				strPlotTitle = strPlotTitle.replace("%s", "%1").arg(pStream->name);
			}
			m_pPlot->setTitle( strPlotTitle );
			m_pPlot->updateGL();
		}
	}
}

void MDTRA_2D_RMSD_Dialog :: cancel_build( void )
{
	m_bBuildStarted = false;
	s_bCancelBuild = false;
	btnRebuild->setText(tr("&Rebuild"));
	btnClose->setEnabled( true );
}

void MDTRA_2D_RMSD_Dialog :: exec_on_rebuild( void )
{
	if (m_bBuildStarted) {
		s_bCancelBuild = true;
		return;
	}

	const MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( current_stream_index() );

	if (!pStream || !pStream->pdb) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Bad stream(s) selected!"));
		return;
	}

	if (sel_string->text().isEmpty()) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Selection expression is empty!"));
		return;
	}

	update_selection();
	display_selection();
	if (m_bSelectionError) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Error in selection expression:\n\n%1").arg(sel_string->text()));
		return;
	} else if (m_iSelectionSize <= 0) {
		QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("No atoms were selected using selection expression:\n\n%1").arg(sel_string->text()));
		return;
	}

	bool bSelectionChange = true;
	if ((m_cachedSelectionText2 == m_cachedSelectionText) && (m_cachedStreamIndex == m_cachedPDBStreamIndex))
		bSelectionChange = false;
	m_cachedSelectionText2 = m_cachedSelectionText;

	m_bBuildStarted = true;
	s_bCancelBuild = false;
	btnRebuild->setText(tr("S&top"));
	btnClose->setEnabled( false );
	btnSave->setEnabled( false );

	//Define trajectory fragment to analyze
	int trajectoryMin = 1;
	int trajectoryMax = pStream->files.count();
	if (trajectoryRange->isChecked()) {
		trajectoryMin = MDTRA_MAX( trajectoryMin, sIndex->value() );
		trajectoryMax = MDTRA_MIN( trajectoryMax, eIndex->value() );
	}

	//Show progress info and hide plot
	m_pPlot->setVisible( false );
	groupBox_3->repaint();
	progressBarMsg->setVisible( true );
	progressBar->setVisible( true );

	//Load compact PDBs
	bool bFileChange = load_pdb_files( trajectoryMin, trajectoryMax, bSelectionChange );

	if (s_bCancelBuild) {
		progressBarMsg->setText( tr("Data not ready") );
		m_cachedSelectionText2.clear();
		progressBar->setVisible( false );
		cancel_build();
		return;
	}

	bool bTextureChange = bFileChange;
	if (m_bCachedRGB != optRGB->isChecked() || 
		m_cachedTitle != plotTitle->text()) {
		m_bCachedRGB = optRGB->isChecked();
		m_cachedTitle = plotTitle->text();
		bTextureChange = true;
	}

	float plotMin = optAutoPlotMin->isChecked() ? 1.0f : spinPlotMin->value();
	if (m_flCachedPlotMin != plotMin) {
		m_flCachedPlotMin = plotMin;
		bTextureChange = true;
	}

	if (bFileChange) {
		//Calculate RMSD buffer
		calc_rmsd( trajectoryMin, trajectoryMax );

		if (s_bCancelBuild) {
			progressBarMsg->setText( tr("Data not ready") );
			m_cachedSelectionText2.clear();
			progressBar->setVisible( false );
			cancel_build();
			return;
		}
	}
	if (bTextureChange) {
		//Prepare texture buffer
		build_texture( trajectoryMin, trajectoryMax );

		if (s_bCancelBuild) {
			progressBarMsg->setText( tr("Data not ready") );
			m_cachedSelectionText2.clear();
			progressBar->setVisible( false );
			cancel_build();
			return;
		}
	}

	//Hide progress info and show plot
	progressBarMsg->setVisible( false );
	progressBar->setVisible( false );
	m_pPlot->setVisible( true );
	m_pPlot->repaint();
	cancel_build();
	btnSave->setEnabled( true );
	btnClose->setEnabled( true );
}

void MDTRA_2D_RMSD_Dialog :: exec_on_save( void )
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
		return;
	}

	QString fileName = QFileDialog::getSaveFileName( this, tr("Save 2D-RMSD Plot"), m_pMainWindow->getCurrentFileDir(), imageFilters, &selectedFilter );
	if (!fileName.isEmpty()) {
		QFileInfo fi( fileName );
		if ( fi.suffix().isEmpty() ) {
			fileName.append( extMap[selectedFilter] );
			fi.setFile( fileName );
		}
		QString fType = fi.suffix().toUpper();
		m_pPlot->saveScreenshot( fileName, fType.toAscii() );
	}
}

void MDTRA_2D_RMSD_Dialog :: reject( void )
{
	if (m_bBuildStarted)
		return;

	QDialog::reject();
}
