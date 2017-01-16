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
#ifndef MDTRA_PROG_SYNTAXHIGHLIGHT_H
#define MDTRA_PROG_SYNTAXHIGHLIGHT_H

#include <QtGui/QSyntaxHighlighter>

class MDTRA_MainWindow;

class MDTRA_ProgramSyntaxHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

	enum ProgramConstruct {
		PC_KEYWORD = 0,
		PC_FUNCTION,
		PC_ARGUMENT,
		PC_STRING,
		PC_COMMENT,
		PC_LAST,
	};
	enum ProgramBlockState {
		PBS_NORMAL = -1,
		PBS_INSIDE_COMMENT,
		PBS_INSIDE_IDENTIFIER,
		PBS_INSIDE_SQSTRING,
		PBS_INSIDE_DQSTRING,
	};

public:
	MDTRA_ProgramSyntaxHighlighter( QWidget *mainwindow, QTextDocument *document );
	void setFormatFor( ProgramConstruct construct, const QTextCharFormat &format ) { m_formats[construct] = format; }
	QTextCharFormat formatFor( ProgramConstruct construct ) const { return m_formats[construct]; }
    
protected:
	virtual void highlightBlock( const QString &text );

private:
	MDTRA_MainWindow* m_pMainWindow;
	QTextCharFormat m_formats[PC_LAST];
};

#endif //MDTRA_PROG_SYNTAXHIGHLIGHT_H