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
//	Implementation of MDTRA_PCAResultsDialog

#include "mdtra_main.h"
#include "mdtra_mainWindow.h"
#include "mdtra_project.h"
#include "mdtra_pca.h"
#include "mdtra_pcaResultsDialog.h"

#include <QtCore/QTextStream>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

extern float *g_pEigenVectors;
extern float *g_pEigenValues;
extern int g_iNumEigens;
extern int g_iNumDisplayEigens;

MDTRA_PCAResultsDialog :: MDTRA_PCAResultsDialog( QWidget *parent )
						: QDialog( parent )
{
	m_pMainWindow = qobject_cast<MDTRA_MainWindow*>(parent);
	assert(m_pMainWindow != NULL);

	setupUi( this );
	setFixedSize( width(), height() );
	setWindowIcon( QIcon(":/png/16x16/pca.png") );

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QApplication::processEvents();	

	//fill text info
	QString textInfo;
	textInfo = textInfo + tr("<b>Eigen vectors</b>: %1/%2 displayed<br />").arg(g_iNumDisplayEigens).arg(g_iNumEigens);
	const float *pEV = g_pEigenVectors;
	for (int i = 0; i < g_iNumDisplayEigens; i++) {
		textInfo = textInfo + "( ";
		for (int j = 0; j < g_iNumEigens; j++, pEV++) {
			if (j > 0) textInfo = textInfo + ", ";
			textInfo = textInfo + QString::number( *pEV, 'f' );
		}
		textInfo = textInfo + ")<br />";
	}

	textInfo = textInfo + tr("<br /><b>Eigen values</b>: %1/%2 displayed<br />").arg(g_iNumDisplayEigens).arg(g_iNumEigens);
	pEV = g_pEigenValues;
	for (int i = 0; i < g_iNumDisplayEigens; i++, pEV++) {
		textInfo = textInfo + QString::number( *pEV, 'f' ) + "<br />"; 
	}

	textEdit->setHtml(textInfo);

	QApplication::restoreOverrideCursor();

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(exec_save()));
}

void MDTRA_PCAResultsDialog :: exportResults( const QString &fileSuffix, QTextStream *stream )
{
	bool bCSV = false;
	if (fileSuffix.toLower() == "csv") {
		bCSV = true;
	}

	if (bCSV) {
		*stream << QString("\"Eigenvectors:\";%1").arg(g_iNumDisplayEigens);
		*stream << endl;
		*stream << QString("\"\";\"X\";\"Y\";\"Z\"");
	} else {
		*stream << QString("\"Eigenvectors:\"\t%1").arg(g_iNumDisplayEigens);
		*stream << endl;
		*stream << QString("\t\t\"X\"\t\"Y\"\t\"Z\"");
	}
	*stream << endl;

	const float *pEV = g_pEigenVectors;
	for (int i = 0; i < g_iNumDisplayEigens; i++) {
		*stream << QString("%1").arg(i+1);
		for (int j = 0; j < g_iNumEigens; j++, pEV++) {
			if (bCSV)
				*stream << QString(";%1").arg(*pEV, 0, 'f', 6);
			else
				*stream << QString("\t%1").arg(*pEV, 0, 'f', 6);
		}
		*stream << endl;
	}
	*stream << endl;

	if (bCSV) {
		*stream << QString("\"Eigenvalues:\";%1").arg(g_iNumDisplayEigens);
		*stream << endl;
		*stream << QString("\"\";\"Value\"");
	} else {
		*stream << QString("\"Eigenvalues:\"\t%1").arg(g_iNumDisplayEigens);
		*stream << endl;
		*stream << QString("\t\t\"Value\"");
	}
	*stream << endl;

	pEV = g_pEigenValues;
	for (int i = 0; i < g_iNumDisplayEigens; i++, pEV++) {
		if (bCSV)
			*stream << QString("%1;%2").arg(i+1).arg(*pEV, 0, 'f', 6);
		else
			*stream << QString("%1\t%2").arg(i+1).arg(*pEV, 0, 'f', 6);
		*stream << endl;
	}
}

void MDTRA_PCAResultsDialog :: exec_save( void )
{
	QString exportFilter = "Text Files (*.txt);;CSV Files (*.csv)";
	QString selectedFilter;
	QMap<QString,QString> extMap;
	extMap["Text Files (*.txt)"] = ".txt";
	extMap["CSV Files (*.csv)"] = ".csv";

	QString fileName = QFileDialog::getSaveFileName( this, tr("Save PCA Analysis Results"), m_pMainWindow->getCurrentFileDir(), exportFilter, &selectedFilter );
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
		exportResults( fi.suffix(), &stream );
		f.close();
		QApplication::restoreOverrideCursor();
	}
}
