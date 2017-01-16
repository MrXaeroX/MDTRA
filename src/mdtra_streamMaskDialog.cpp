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
//	Implementation of MDTRA_StreamMaskDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_streamMaskDialog.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>

MDTRA_StreamMaskDialog :: MDTRA_StreamMaskDialog( QWidget *mainwindow, QWidget *parent )
						: QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(mainwindow);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/stream.png") );

	m_currentFileDir = m_pMainWindow->getCurrentFileDir();

	connect(buttonBrowse, SIGNAL(clicked()), this, SLOT(set_mask()));
}

void MDTRA_StreamMaskDialog :: set_mask( void )
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Browse File Containing Mask Index"), m_currentFileDir, "PDB Files (*.pdb);;All Files (*.*)" );
	int fileNameLen = fileName.length();
	if (!fileNameLen)
		return;

	QString fileName2;
	int indexStart = 0, indexSize = 0;
	bool foundExt = false;
	bool leadingZero = false;
	for (int i = fileNameLen-1; i >= 0; i--) {
		QChar c = fileName.at(i);
		if (!foundExt) {
			if (c == '.') foundExt = true;
			continue;
		}
		if (c.isDigit()) {
			indexSize++;
		} else if (indexSize > 0) {
			indexStart = i + 1;
			leadingZero = (fileName.at(indexStart).toAscii() == '0');
			break;
		}
	}

	if (indexStart > 0)
		fileName2.append( fileName.left( indexStart ) );

	if (leadingZero)
		fileName2.append( QString("%").append(QString("0%1i").arg(indexSize)) );
	else
		fileName2.append( QString("%i") );

	if (indexStart + indexSize < fileNameLen)
		fileName2.append( fileName.right( fileNameLen - indexStart - indexSize ) );
	
	lbStreamMask->setText( fileName2 );
}

QStringList MDTRA_StreamMaskDialog :: getFilesByMask( void )
{
	QStringList fileList;

	if (lbStreamMask->text().length() > 0) {
		bool bIndexRange = valueRangeBox->isChecked();
		quint32 iMaxIndex = eIndex->value();

		quint32 currentIndex = bIndexRange ? sIndex->value() : 0;
		char szFileName[260];
		memset( szFileName, 0, sizeof(szFileName) );
		FILE *fp = NULL;

		while (1) {
			if (bIndexRange && (currentIndex > iMaxIndex))
				break;

			sprintf_s( szFileName, sizeof(szFileName), lbStreamMask->text().toAscii(), currentIndex );

			if (fopen_s( &fp, szFileName, "rb" )) {
				if (!bIndexRange && currentIndex)
					break;
				currentIndex++;
				continue;
			}

			fclose( fp );
			fileList << QString(szFileName);
			currentIndex++;
		}
	}

	return fileList;
}