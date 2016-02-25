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
#ifndef MDTRA_INPUTTEXTDIALOG_H
#define MDTRA_INPUTTEXTDIALOG_H

#include <QtGui/QDialog>

#include "ui_inputTextDialog.h"

class MDTRA_InputTextDialog : public QDialog, public Ui_MDTRA_InputTextDialog
{
	Q_OBJECT

public:
    MDTRA_InputTextDialog( const QString &title, const QString &subtitle, const QString &msg, bool readOnly, QWidget *parent = 0 );
};

#endif //MDTRA_INPUTTEXTDIALOG_H