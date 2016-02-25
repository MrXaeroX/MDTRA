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
#ifndef MDTRA_USERTYPEDIALOG_H
#define MDTRA_USERTYPEDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>

#include "ui_userTypeDialog.h"
#include "mdtra_types.h"

class MDTRA_MainWindow;
class MDTRA_ProgramSyntaxHighlighter;
class MDTRA_Program_Compiler;

class MDTRA_CodeEditor : public QPlainTextEdit
{
	Q_OBJECT
public:
	MDTRA_CodeEditor( QWidget *parent = 0 );

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();
	void highlightLine( int line, int level );
	void removeHighlights( void );

protected:
     void resizeEvent(QResizeEvent *event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect &, int);

private:
	QWidget* m_lineNumberArea;
};

class MDTRA_LineNumberArea : public QWidget
{
public:
	MDTRA_LineNumberArea( MDTRA_CodeEditor *editor ) : QWidget(editor) { m_codeEditor = editor; }
	QSize sizeHint() const { return QSize( m_codeEditor->lineNumberAreaWidth(), 0 ); }
protected:
	void paintEvent(QPaintEvent *event) { m_codeEditor->lineNumberAreaPaintEvent(event); }
private:
	MDTRA_CodeEditor* m_codeEditor;
};

class MDTRA_UserTypeDialog : public QDialog, public Ui_userTypeDialog
{
	Q_OBJECT

public:
    MDTRA_UserTypeDialog( QWidget *mainwindow, QWidget *parent = 0 );
	virtual ~MDTRA_UserTypeDialog();
	void setUserTypeInfo( const QString* pUnitTitle, const int* unitFlags, const QString* pSrcCode );
	void getUserTypeInfo( QString* pUnitTitle, int* unitFlags, QString* pSrcCode, int* byteCodeSize, byte** ppbyteCode );
	bool eventFilter( QObject *obj, QEvent *event );

private slots:
	void exec_on_check_input( void );
	void exec_on_source_changed( void );
	void exec_on_accept( void );
	void exec_on_open_sourcefile( void );
	void exec_on_save_sourcefile( void );
	void exec_on_compile( void );

private:
	void createToolBar( void );
	void output( const QString& msg, unsigned int tag );
	bool compile( void );

private:
	MDTRA_MainWindow* m_pMainWindow;
	MDTRA_ProgramSyntaxHighlighter* m_Highlighter;
	MDTRA_Program_Compiler* m_Compiler;
	MDTRA_CodeEditor* codeEdit;
	QToolBar* codeToolbar;
	QString	m_currentFileDir;
	QVector<unsigned int> m_Tags;
	bool m_bSourceChanged;
	bool m_bNeedRecompile;
	byte* m_pByteCode;
	int m_ByteCodeSize;
	int m_ByteCodeMaxSize;
};

#endif //MDTRA_USERTYPEDIALOG_H