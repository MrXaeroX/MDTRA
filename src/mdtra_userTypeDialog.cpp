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
//	Implementation of MDTRA_CodeEditor
//  Implementation of MDTRA_CompilerOutput
//	Implementation of MDTRA_UserTypeDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_utils.h"
#include "mdtra_prog_syntaxHighlight.h"
#include "mdtra_prog_compiler.h"
#include "mdtra_userTypeDialog.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QPainter>

#define MAX_USER_UNIT_TYPES	3

static const char* s_szUserDataTypes[MAX_USER_UNIT_TYPES] = {
"Generic",
"Angle (Degrees)",
"Angle (Radians)"
};

MDTRA_CodeEditor :: MDTRA_CodeEditor( QWidget *parent ) : QPlainTextEdit(parent)
{
	m_lineNumberArea = new MDTRA_LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));

	updateLineNumberAreaWidth(0);
}

int MDTRA_CodeEditor :: lineNumberAreaWidth( void )
{
	int digits = 2;
	int max = qMax( 10, blockCount() );
	while (max >= 100) {
		max /= 10;
		++digits;
	}
	int space = 8 + fontMetrics().width(QLatin1Char('9')) * digits;
	return space;
}

void MDTRA_CodeEditor :: updateLineNumberAreaWidth( int /* newBlockCount */ )
{
	setViewportMargins( lineNumberAreaWidth(), 0, 0, 0 );
}

void MDTRA_CodeEditor :: updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
		m_lineNumberArea->scroll(0, dy);
	else
		m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void MDTRA_CodeEditor :: resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void MDTRA_CodeEditor :: lineNumberAreaPaintEvent(QPaintEvent *event)
 {
	QPainter painter(m_lineNumberArea);
	painter.fillRect(event->rect(), Qt::lightGray);

	QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen( Qt::darkGray );
			painter.drawText(0, top, m_lineNumberArea->width()-4, fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int)blockBoundingRect(block).height();
		++blockNumber;
	}
}

void MDTRA_CodeEditor :: highlightLine( int line, int level )
{
	QList<QTextEdit::ExtraSelection> extraSelections;
	QTextEdit::ExtraSelection selection;
	QColor lineColor = QColor(level?Qt::yellow:Qt::red).lighter(180);

	selection.format.setBackground(lineColor);
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = QTextCursor( document()->findBlockByLineNumber( line - 1 ) );
	selection.cursor.clearSelection();

	extraSelections.append(selection);
	setExtraSelections(extraSelections);

	setTextCursor( selection.cursor );
	ensureCursorVisible();
}

void MDTRA_CodeEditor :: removeHighlights( void )
{
	QList<QTextEdit::ExtraSelection> extraSelections;
	setExtraSelections(extraSelections);
}

//=================================================================

typedef struct stMDTRACodeToolbarDesc
{
	const char *m_Title;
	const char *m_Image;
	const char *m_SlotName;
} MDTRACodeToolbarDesc;

static MDTRACodeToolbarDesc g_ToolbarItems[] = 
{
{ "Load Source Code from a File", ":/png/16x16/open.png", SLOT(exec_on_open_sourcefile()) },
{ "Save Source Code to a File", ":/png/16x16/save.png", SLOT(exec_on_save_sourcefile()) },
{ NULL, NULL },
{ "Compile Source Code", ":/png/24x24/build2.png", SLOT(exec_on_compile()) },
};

//Default program simply returns zero
#define DEFAULT_SOURCE_CODE		\
"function main()\n"	\
"\treturn 0.0;\n"	\
"end;"

MDTRA_UserTypeDialog :: MDTRA_UserTypeDialog( QWidget *mainwindow, QWidget *parent )
					   : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(mainwindow);
	assert(m_pMainWindow != NULL);

	m_pByteCode = NULL;
	m_ByteCodeSize = 0;
	m_ByteCodeMaxSize = 0;

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/source.png") );
	setWindowTitle( tr("Edit User Type") );

	cUnitType->clear();
	for ( int i = 0; i < MAX_USER_UNIT_TYPES; i++ )
		cUnitType->addItem( s_szUserDataTypes[i] );

	codeEdit = new MDTRA_CodeEditor( codeEditHolder );
	codeEdit->setGeometry( 0, 0, codeEditHolder->width(), codeEditHolder->height() );
	codeEdit->setFont( codeEditHolder->font() );
	codeEdit->setTabStopWidth( 32 );

	createToolBar();
	verticalLayout->addWidget(codeToolbar);
	verticalLayout->addWidget(codeEditHolder);

	QWidget* pWidget = compilerOutput->viewport();
	pWidget->installEventFilter(this);

	m_Highlighter = new MDTRA_ProgramSyntaxHighlighter( mainwindow, codeEdit->document() );
	m_Compiler = new MDTRA_Program_Compiler;
	m_bNeedRecompile = true;
	m_bSourceChanged = false;
	m_currentFileDir = "./scripts/";

	connect(sUnitTitle, SIGNAL(textChanged(const QString&)), this, SLOT(exec_on_check_input()));
	connect(codeEdit, SIGNAL(textChanged()), this, SLOT(exec_on_source_changed()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_on_accept()));
}

MDTRA_UserTypeDialog :: ~MDTRA_UserTypeDialog()
{
	m_Tags.clear();
	if ( m_Highlighter ) {
		delete m_Highlighter;
		m_Highlighter = NULL;
	}
	if ( m_Compiler ) {
		delete m_Compiler;
		m_Compiler = NULL;
	}
}

void MDTRA_UserTypeDialog :: createToolBar( void )
{
	const MDTRACodeToolbarDesc *pCurrentToolbarDesc = NULL;
	const MDTRACodeToolbarDesc *pToolbarDesc = g_ToolbarItems;
	int numActions = sizeof(g_ToolbarItems)/sizeof(g_ToolbarItems[0]);

	codeToolbar = new QToolBar();
	codeToolbar->setMovable( false );
	codeToolbar->setFloatable( false );
	codeToolbar->setIconSize( QSize(16, 16) );
	//codeToolbar->setStyleSheet("QToolBar { border: 0px }");

	for (int i = 0; i < numActions; i++) {
		pCurrentToolbarDesc = &pToolbarDesc[i];
		if (!pCurrentToolbarDesc->m_Title) {
			codeToolbar->addSeparator();
		} else {
			QAction *pToolBarAction = new QAction(this);
			pToolBarAction->setStatusTip(pCurrentToolbarDesc->m_Title);
			pToolBarAction->setIcon(QIcon(pCurrentToolbarDesc->m_Image));
			connect(pToolBarAction, SIGNAL(triggered()), this, pCurrentToolbarDesc->m_SlotName);
			codeToolbar->addAction(pToolBarAction);
		}
	}
}

bool MDTRA_UserTypeDialog :: eventFilter( QObject *obj, QEvent *event )
{
	if (event->type() == QEvent::MouseButtonDblClick) {
		if (obj == compilerOutput->viewport()) {
			int tagnum = compilerOutput->textCursor().blockNumber();
			unsigned int tag = m_Tags.at(tagnum);
			int line = tag & 0xFFFF;
			int level = tag >> 16;
			if ( line > 0 )
				codeEdit->highlightLine( line, level );
			return true;
		}
	}
 	return QDialog::eventFilter(obj, event);
}

void MDTRA_UserTypeDialog :: exec_on_check_input( void )
{
	bool bOkEnabled = (sUnitTitle->text().length() > 0);
	buttonBox->button( QDialogButtonBox::Ok )->setEnabled( bOkEnabled );
}

void MDTRA_UserTypeDialog :: exec_on_source_changed( void )
{
	codeEdit->removeHighlights();
	m_bNeedRecompile = true;
	m_bSourceChanged = true;
}

void MDTRA_UserTypeDialog :: exec_on_open_sourcefile( void )
{
	if ( m_bSourceChanged ) {
		int r = QMessageBox::warning( this, tr("Confirm"), tr("Do you want to save changes in current source code?\nIf you select \"No\", all changes will be lost."), 
									  QMessageBox::Yes | QMessageBox::Default,
									  QMessageBox::No,
									  QMessageBox::Cancel | QMessageBox::Escape );
		if (r == QMessageBox::Yes) {
			exec_on_save_sourcefile();
		} else if (r == QMessageBox::Cancel) {
			return;
		}
	}

	QString fileName = QFileDialog::getOpenFileName( this, tr("Open Script File"), m_currentFileDir, "Lua Scripts (*.lua)" );
	if (!fileName.isEmpty()) {
		// load script
		QFileInfo fi(fileName);
		m_currentFileDir = fi.path();

		QFile f(fileName);
		f.open(QFile::ReadOnly);
		codeEdit->setPlainText(f.readAll());
		f.close();
	}
}

void MDTRA_UserTypeDialog :: exec_on_save_sourcefile( void )
{
	QString fileName = QFileDialog::getSaveFileName( this, tr("Save Script File"), m_currentFileDir, "Lua Scripts (*.lua)" );
	if (!fileName.isEmpty()) {
		// save script
		QFileInfo fi(fileName);
		m_currentFileDir = fi.path();
		if ( fi.suffix().isEmpty() )
			fileName.append( ".lua" );

		QFile f(fileName);
		f.open(QFile::WriteOnly);
		f.write(codeEdit->toPlainText().toAscii());
		f.close();
	}
}

void MDTRA_UserTypeDialog :: exec_on_compile( void )
{
	compile();
}

void MDTRA_UserTypeDialog :: exec_on_accept( void )
{
	if ( m_bNeedRecompile ) {
		if ( !compile() ) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Failed to compile program source code!\n\nData source with invalid code cannot be saved.\nPlease correct all the errors. You may want to refer to program manual."));
			return;
		}
	}
	accept();
}

void MDTRA_UserTypeDialog :: setUserTypeInfo( const QString* pUnitTitle, const int* unitFlags, const QString* pSrcCode )
{
	sUnitTitle->setText( *pUnitTitle );
	if ( !sUnitTitle->text().length() )
		sUnitTitle->setText( "User Type" );

	if ( *unitFlags & DATASOURCE_USERFLAG_ANGLE_DEG )
		cUnitType->setCurrentIndex( 1 );
	else if ( *unitFlags & DATASOURCE_USERFLAG_ANGLE_RAD )
		cUnitType->setCurrentIndex( 2 );
	else
		cUnitType->setCurrentIndex( 0 );

	if ( *pSrcCode == "" ) {
		codeEdit->setPlainText( DEFAULT_SOURCE_CODE );
	} else {
		codeEdit->setPlainText( *pSrcCode );
	}
	m_bSourceChanged = false;
}

void MDTRA_UserTypeDialog :: getUserTypeInfo( QString* pUnitTitle, int* unitFlags, QString* pSrcCode, int* byteCodeSize, byte** ppbyteCode )
{
	*pUnitTitle = sUnitTitle->text();
	*pSrcCode = codeEdit->toPlainText();
	*byteCodeSize = m_ByteCodeSize;
	*ppbyteCode = m_pByteCode;

	*unitFlags &= ~(DATASOURCE_USERFLAG_ANGLE_DEG|DATASOURCE_USERFLAG_ANGLE_RAD);

	if ( cUnitType->currentIndex() == 1 )
		*unitFlags |= DATASOURCE_USERFLAG_ANGLE_DEG;
	else if ( cUnitType->currentIndex() == 2 )
		*unitFlags |= DATASOURCE_USERFLAG_ANGLE_RAD;
}

void MDTRA_UserTypeDialog :: output( const QString& msg, unsigned int tag )
{
	if (compilerOutput->toPlainText().isEmpty())
		compilerOutput->setHtml( msg );
	else
		compilerOutput->setHtml( compilerOutput->toHtml() + msg );

	QTextCursor c = compilerOutput->textCursor();
	c.movePosition(QTextCursor::End);
	compilerOutput->setTextCursor(c);

	m_Tags << tag;
}

bool MDTRA_UserTypeDialog :: compile( void )
{
	int numMessages;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	compilerOutput->clear();
	m_Tags.clear();
	codeEdit->removeHighlights();
	output( m_Compiler->Logo(), 0 );
	output( QString("Compile started: %1 bytes of source code").arg(codeEdit->toPlainText().size()), 0 );

	// compile the code
	if ( m_Compiler->Compile( codeEdit->toPlainText() ) ) {
		// succeeded
		numMessages = m_Compiler->GetMessageCount();
		for ( int i = 0; i < numMessages; i++ ) {
			output( QString("&gt; ").append(m_Compiler->GetMessage( i )), m_Compiler->GetMessageTag( i ) );
		}

		if ( m_Compiler->GetByteCodeSize() > m_ByteCodeMaxSize ) {
			// need to realloc buffer
			delete [] m_pByteCode;
			m_pByteCode = NULL;
		}
		m_ByteCodeSize = m_Compiler->GetByteCodeSize();
		if ( !m_pByteCode ) {
			m_ByteCodeMaxSize = m_ByteCodeSize;
			m_pByteCode = new byte[m_ByteCodeMaxSize];
		}
		memcpy( m_pByteCode, m_Compiler->GetByteCode(), m_ByteCodeSize );

		output( QString("Succeeded: %1 bytes of bytecode generated").arg( m_ByteCodeSize ), 0 );
		m_bNeedRecompile = false;
		QApplication::restoreOverrideCursor();
		return true;
	}

	numMessages = m_Compiler->GetMessageCount();
	for ( int i = 0; i < numMessages; i++ ) {
		output( QString("&gt; ").append(m_Compiler->GetMessage( i )), m_Compiler->GetMessageTag( i ) );
	}

	output( QString("Failed: no bytecode generated"), 0 );
	m_bNeedRecompile = true;
	QApplication::restoreOverrideCursor();
	return false;
}