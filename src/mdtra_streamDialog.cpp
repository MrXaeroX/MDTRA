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
//	Implementation of MDTRA_StreamDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pdb_format.h"
#include "mdtra_streamDialog.h"
#include "mdtra_streamMaskDialog.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>

MDTRA_StreamDialog :: MDTRA_StreamDialog( int index, QWidget *parent )
				    : QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/stream.png") );

	if (index < 0) {
		setWindowTitle( tr("Add Stream") );
	} else {
		setWindowTitle( tr("Edit Stream") );
	}

	m_currentFileDir = m_pMainWindow->getCurrentFileDir();
	m_iStreamIndex = index;

	sList->setSortingEnabled( true );

	int currentFormat = g_PDBFormatManager.getDefaultFormat();

	if (index < 0) {
		QString streamTitle = tr("Stream %1").arg(m_pMainWindow->getStreamCounter());
		lineEdit->setText(streamTitle);
		timestep->setValue( 1.0f );
	} else {
		MDTRA_Stream *pStream = m_pMainWindow->getProject()->fetchStreamByIndex( index );
		if (pStream) {
			lineEdit->setText( pStream->name );
			sList->addItems( pStream->files );
			timestep->setValue( pStream->xscale );
			relativeCb->setChecked( (pStream->flags & STREAM_FLAG_RELATIVE_PATHS) != 0 );
			hetatmCb->setChecked( (pStream->flags & STREAM_FLAG_IGNORE_HETATM) != 0 );
			solventCb->setChecked( (pStream->flags & STREAM_FLAG_IGNORE_SOLVENT) != 0 );
			currentFormat = pStream->format_identifier;
		}
	}

	g_PDBFormatManager.fillFormatCombo( formatCombo, currentFormat );

	connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(exec_on_check_streamInput()));
	connect(sbAdd, SIGNAL(clicked()), this, SLOT(add_stream_files()));
	connect(sbAddMask, SIGNAL(clicked()), this, SLOT(add_stream_files_by_mask()));
	connect(sbRemove, SIGNAL(clicked()), this, SLOT(remove_stream_files()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_on_accept()));

	exec_on_check_streamInput();
}

void MDTRA_StreamDialog :: exec_on_check_streamInput( void )
{
	buttonBox->button( QDialogButtonBox::Ok )->setEnabled( 
		(lineEdit->text().length() > 0) &&
		(sList->count() > 0)	);
}

void MDTRA_StreamDialog :: exec_on_accept( void )
{
	if (m_iStreamIndex < 0) {
		if (!m_pMainWindow->getProject()->checkUniqueStreamName( lineEdit->text() )) {
			QMessageBox::warning(this, tr(APPLICATION_TITLE_SMALL), tr("Stream \"%1\" already registered.\nPlease enter another stream title.").arg(lineEdit->text()));
			return;
		}
	}

	QStringList fileList;
	for (int i = 0; i < sList->count(); i++)
		fileList << sList->item(i)->text();

	unsigned int format = formatCombo->itemData( formatCombo->currentIndex(), Qt::UserRole ).toUInt();
	unsigned int flags = 0;

	if (relativeCb->isChecked())
		flags |= STREAM_FLAG_RELATIVE_PATHS;

	if (hetatmCb->isChecked())
		flags |= STREAM_FLAG_IGNORE_HETATM;
	if (solventCb->isChecked())
		flags |= STREAM_FLAG_IGNORE_SOLVENT;

	if (m_iStreamIndex < 0) {
		m_pMainWindow->getProject()->registerStream( lineEdit->text(), fileList, timestep->value(), format, flags );
	} else {
		m_pMainWindow->getProject()->modifyStream( m_iStreamIndex, lineEdit->text(), fileList, timestep->value(), format, flags );
	}

	accept();
}

void MDTRA_StreamDialog :: add_stream_files( void )
{

	QStringList fileList = QFileDialog::getOpenFileNames( this, tr("Add Files to Stream"), 
							m_currentFileDir, "PDB Files (*.pdb);;All Files (*.*)" );
	if (fileList.isEmpty()) {
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QFileInfo fi(fileList.at(0));
	m_currentFileDir = fi.path();

	for (int i = 0; i < fileList.count(); i++) {
		if (!sList->findItems(fileList.at(i), Qt::MatchFixedString).count())
			sList->addItem(fileList.at(i));
	}

	exec_on_check_streamInput();
	QApplication::restoreOverrideCursor();
}

void MDTRA_StreamDialog :: add_stream_files_by_mask( void )
{
	MDTRA_StreamMaskDialog dialog( m_pMainWindow, this );
	if (dialog.exec()) {
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QStringList fileList = dialog.getFilesByMask();
		if (fileList.isEmpty()) {
			QApplication::restoreOverrideCursor();
			return;
		}

		QFileInfo fi(fileList.at(0));
		m_currentFileDir = fi.path();

		for (int i = 0; i < fileList.count(); i++) {
			if (!sList->findItems(fileList.at(i), Qt::MatchFixedString).count())
				sList->addItem(fileList.at(i));
		}

		exec_on_check_streamInput();
		QApplication::restoreOverrideCursor();
	}
}

void MDTRA_StreamDialog :: remove_stream_files( void )
{
	QList<QListWidgetItem*> selection = sList->selectedItems();
	if (!selection.count())
		return;

	if (QMessageBox::No == QMessageBox::warning( this, tr("Confirm"), tr("Do you want to remove %1 file(s) from the stream?").arg(selection.count()), 
												 QMessageBox::Yes | QMessageBox::Default,
												 QMessageBox::No | QMessageBox::Escape )) {
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	for (int i = 0; i < selection.count(); i++) {
		QListWidgetItem *pItem = sList->takeItem( sList->row( selection.at(i) ) );
		delete pItem;
	}

	exec_on_check_streamInput();
	QApplication::restoreOverrideCursor();
}