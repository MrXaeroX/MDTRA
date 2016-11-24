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
#include "mdtra_math.h"
#include "mdtra_project.h"
#include "mdtra_pdb.h"
#include "mdtra_pdb_format.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_SAS.h"
#include "mdtra_utils.h"
#include "mdtra_progressDialog.h"
#include "mdtra_prog_state.h"
#include "mdtra_prog_interpreter.h"

#include <QtCore/QTextStream>
#include <QtGui/QListWidget>
#include <QtGui/QMessageBox>

extern QStringList UTIL_MakeRelativeFileNames( const QStringList &list, const QString &basePath );
extern QStringList UTIL_MakeAbsoluteFileNames( const QStringList &list, const QString &basePath );

#define DEFAULT_SNAPSHOT_PS			1.0f

static const char *szLayoutNames[MDTRA_LAYOUT_MAX] = 
{
	"Time-Based ",
	"Residue-Based ",
};

const char *szScaleUnitNames[MDTRA_YSU_MAX] = 
{
	"Angstroms",
	"Nanometers",
	"Degrees",
	"Radians",
	"Kcal/A",
	"Micronewtons",
	"Square Angstroms",
	"Square Nanometers",
};

//////////////////////////////////////////////////////////////////////

MDTRA_Project :: MDTRA_Project( MDTRA_MainWindow *pMainWindow ) 
			   : m_pMainWindow(pMainWindow)
{
	assert( m_pMainWindow != NULL );
}

MDTRA_Project :: ~MDTRA_Project()
{
	clear();
}

void MDTRA_Project :: clear()
{
	for (int i = 0; i < m_StreamList.count(); i++) {
		if (m_StreamList.at(i).pdb) delete m_StreamList.at(i).pdb;
	}
	for (int i = 0; i < m_ResultList.count(); i++) {
		for (int j = 0; j < m_ResultList.at(i).sourceList.count(); j++) {
			if (m_ResultList.at(i).sourceList.at(j).pData) UTIL_AlignedFree( m_ResultList.at(i).sourceList.at(j).pData );
			if (m_ResultList.at(i).sourceList.at(j).pCorrelation) UTIL_AlignedFree( m_ResultList.at(i).sourceList.at(j).pCorrelation );
		}
	}

	m_StreamList.clear();
	m_DataSourceList.clear();
	m_ResultList.clear();

	m_pMainWindow->resetCounters();

	updateStreamList();
	updateDataSourceList();
	updateResultList();
}

bool MDTRA_Project :: loadFile( const QString &projectPath, QDataStream *stream )
{
	quint32 magic;
	*stream >> magic;
	if (magic != MDTRA_PROJECT_FILE_MAGIC) {
		return false;
	}
	
	quint32 version;
	*stream >> version;

	if (version < MDTRA_PROJECT_FILE_VERSION_OLD) {
		return false;
	} else if (version > MDTRA_PROJECT_FILE_VERSION) {
		return false;
	}

	stream->setVersion( QDataStream::Qt_4_0 );

	qint32 c;

	//read streams
	*stream >> c;
	m_pMainWindow->setStreamCounter( c );
	for (int i = 0; i < m_StreamList.count(); i++) {
		if (m_StreamList.at(i).pdb) delete m_StreamList.at(i).pdb;
	}
	m_StreamList.clear();
	qint32 streamCount;
	*stream >> streamCount;
	for (int i = 0; i < streamCount; i++) {
		MDTRA_Stream newstream;
		*stream >> newstream.index;
		*stream >> newstream.name;
		*stream >> newstream.files;
		*stream >> newstream.xscale;
		*stream >> newstream.format_identifier;
		*stream >> newstream.flags;
		stream->readRawData( newstream.reserved, sizeof( newstream.reserved ) );

		if (newstream.flags & STREAM_FLAG_RELATIVE_PATHS)
			newstream.files = UTIL_MakeAbsoluteFileNames( newstream.files, projectPath );
		
		if (newstream.files.count() > 0) {
			newstream.pdb = new MDTRA_PDB_File;
			if (!newstream.pdb->load( 0, newstream.format_identifier, newstream.files.at(0).toAscii(), newstream.flags )) {
				delete newstream.pdb;
				newstream.pdb = NULL;
			} else {
				newstream.pdb->move_to_centroid();
			}
		} else {
			newstream.pdb = NULL;
		}
		m_StreamList << newstream;
	}

	//read data sources
	*stream >> c;
	m_pMainWindow->setDataSourceCounter( c );
	m_DataSourceList.clear();
	qint32 dsCount;
	*stream >> dsCount;
	for (int i = 0; i < dsCount; i++) {
		MDTRA_DataSource newds;
		qint32 temp;
		*stream >> newds.index;
		*stream >> newds.streamIndex;
		*stream >> temp;

		if (version <= MDTRA_PROJECT_FILE_VERSION_OLD) {
			if ( temp >= MDTRA_DT_TORSION ) temp+=2;
		}

		newds.type = (MDTRA_DataType)temp;

		for (int j = 0; j < MAX_DATA_SOURCE_ARGS; j++) {
			*stream >> newds.arg[j].atomIndex;
		}
		*stream >> newds.name;
		*stream >> newds.selection.string;
		*stream >> newds.selection.flags;
		*stream >> newds.selection.size;
		newds.selection.data = NULL;
		if (newds.selection.size > 0) {
			newds.selection.data = new int[newds.selection.size];
			stream->readRawData( (char*)newds.selection.data, sizeof( int )*newds.selection.size );
		}

		*stream >> newds.selection2.string;
		*stream >> newds.selection2.flags;
		*stream >> newds.selection2.size;
		newds.selection2.data = NULL;
		if (newds.selection2.size > 0) {
			newds.selection2.data = new int[newds.selection2.size];
			stream->readRawData( (char*)newds.selection2.data, sizeof( int )*newds.selection2.size );
		}
		*stream >> newds.prog.sourceCode;
		*stream >> newds.prog.byteCodeSize;
		newds.prog.byteCode = NULL;
		if (newds.prog.byteCodeSize > 0) {
			newds.prog.byteCode = new byte[newds.prog.byteCodeSize];
			stream->readRawData( (char*)newds.prog.byteCode, newds.prog.byteCodeSize );
		}
	
		stream->readRawData( newds.reserved, sizeof( newds.reserved ) );
		*stream >> newds.userFlags;
		*stream >> newds.userdata;

		m_DataSourceList << newds;
	}

	//read results
	*stream >> c;
	m_pMainWindow->setResultCounter( c );
	m_ResultList.clear();
	qint32 resultCount;
	*stream >> resultCount;
	for (int i = 0; i < resultCount; i++) {
		MDTRA_Result newresult;
		qint32 temp;
		*stream >> newresult.index;
		*stream >> newresult.status;
		*stream >> temp;
		newresult.type = (MDTRA_DataType)temp;
		*stream >> temp;
		newresult.units = (MDTRA_YScaleUnits)temp;
		*stream >> temp;
		newresult.layout = (MDTRA_Layout)temp;
		*stream >> newresult.name;
		stream->readRawData( newresult.reserved, sizeof( newresult.reserved ) );
		*stream >> temp;
		for (int j = 0; j < temp; j++) {
			MDTRA_DSRef dsref;
			*stream >> dsref.dataSourceIndex;
			*stream >> dsref.flags;
			*stream >> dsref.yscale;
			*stream >> dsref.bias;
			*stream >> dsref.xscale;
			stream->readRawData( dsref.reserved, sizeof( dsref.reserved ) );

			for (int k = 0; k < MDTRA_SP_MAX; k++)
				*stream >> dsref.stat[k];
	
			*stream >> dsref.iDataSize;
			*stream >> dsref.iActualDataSize;
			if (dsref.iDataSize <= 0)
				dsref.pData = NULL;
			else {
				dsref.pData = (float*)UTIL_AlignedMalloc( dsref.iDataSize * sizeof(float) );
				for (int k = 0; k < dsref.iDataSize; k++) {
					*stream >> dsref.pData[k];
				}
			}

			quint32 ctest;
			*stream >> ctest;
			if (ctest) {
				dsref.pCorrelation = (float*)UTIL_AlignedMalloc( temp * sizeof(float) );
				for (int k = 0; k < temp; k++) {
					*stream >> dsref.pCorrelation[k];
				}
			} else {
				dsref.pCorrelation = NULL;
			}

			newresult.sourceList << dsref;
		}
		*stream >> temp;
		for (int j = 0; j < temp; j++) {
			MDTRA_Label lbl;
			*stream >> lbl.text;
			*stream >> lbl.x;
			*stream >> lbl.y;
			*stream >> lbl.wide;
			*stream >> lbl.tall;
			*stream >> lbl.flags;
			*stream >> lbl.sourceNum;
			*stream >> lbl.snapshotNum;
			newresult.labelList << lbl;
		}
		m_ResultList << newresult;
	}
	
	updateStreamList();
	updateDataSourceList();
	updateResultList();
	return true;
}

bool MDTRA_Project :: saveFile( const QString &projectPath, QDataStream *stream )
{
	// Write a header with a "magic number" and a version
	*stream << (quint32)MDTRA_PROJECT_FILE_MAGIC;
	*stream << (qint32)MDTRA_PROJECT_FILE_VERSION;
	stream->setVersion( QDataStream::Qt_4_0 );

	//write streams
	*stream << (qint32)m_pMainWindow->getStreamCounter();
	*stream << (qint32)m_StreamList.count();
	for (int i = 0; i < m_StreamList.count(); i++) {
		*stream << (qint32)m_StreamList.at(i).index;
		*stream << m_StreamList.at(i).name;
		if (m_StreamList.at(i).flags & STREAM_FLAG_RELATIVE_PATHS)
			*stream << UTIL_MakeRelativeFileNames( m_StreamList.at(i).files, projectPath );
		else
			*stream << m_StreamList.at(i).files;
		*stream << m_StreamList.at(i).xscale;
		*stream << m_StreamList.at(i).format_identifier;
		*stream << m_StreamList.at(i).flags;
		stream->writeRawData( m_StreamList.at(i).reserved, sizeof(m_StreamList.at(i).reserved) );
	}

	//write data sources
	*stream << (qint32)m_pMainWindow->getDataSourceCounter();
	*stream << (qint32)m_DataSourceList.count();
	for (int i = 0; i < m_DataSourceList.count(); i++) {
		*stream << (qint32)m_DataSourceList.at(i).index;
		*stream << (qint32)m_DataSourceList.at(i).streamIndex;
		*stream << (qint32)m_DataSourceList.at(i).type;
		for (int j = 0; j < MAX_DATA_SOURCE_ARGS; j++) {
			*stream << (qint32)m_DataSourceList.at(i).arg[j].atomIndex;
		}
		*stream << m_DataSourceList.at(i).name;
		*stream << m_DataSourceList.at(i).selection.string;
		*stream << (qint32)m_DataSourceList.at(i).selection.flags;
		*stream << (qint32)m_DataSourceList.at(i).selection.size;
		if ( m_DataSourceList.at(i).selection.size > 0 ) 
			stream->writeRawData( (const char*)m_DataSourceList.at(i).selection.data, sizeof(int)*m_DataSourceList.at(i).selection.size );
		*stream << m_DataSourceList.at(i).selection2.string;
		*stream << (qint32)m_DataSourceList.at(i).selection2.flags;
		*stream << (qint32)m_DataSourceList.at(i).selection2.size;
		if ( m_DataSourceList.at(i).selection2.size > 0 ) 
			stream->writeRawData( (const char*)m_DataSourceList.at(i).selection2.data, sizeof(int)*m_DataSourceList.at(i).selection2.size );
		*stream << m_DataSourceList.at(i).prog.sourceCode;
		*stream << m_DataSourceList.at(i).prog.byteCodeSize;
		if ( m_DataSourceList.at(i).prog.byteCodeSize > 0 ) 
			stream->writeRawData( (char*)m_DataSourceList.at(i).prog.byteCode, m_DataSourceList.at(i).prog.byteCodeSize );
		stream->writeRawData( (char*)m_DataSourceList.at(i).reserved, sizeof(m_DataSourceList.at(i).reserved) );
		*stream << m_DataSourceList.at(i).userFlags;
		*stream << m_DataSourceList.at(i).userdata;
	}

	//write results
	*stream << (qint32)m_pMainWindow->getResultCounter();
	*stream << (qint32)m_ResultList.count();
	for (int i = 0; i < m_ResultList.count(); i++) {
		*stream << (qint32)m_ResultList.at(i).index;
		*stream << (qint32)m_ResultList.at(i).status;
		*stream << (qint32)m_ResultList.at(i).type;
		*stream << (qint32)m_ResultList.at(i).units;
		*stream << (qint32)m_ResultList.at(i).layout;
		*stream << m_ResultList.at(i).name;
		stream->writeRawData( m_ResultList.at(i).reserved, sizeof(m_ResultList.at(i).reserved) );
		*stream << (qint32)m_ResultList.at(i).sourceList.count();
		for (int j = 0; j < m_ResultList.at(i).sourceList.count(); j++) {
			*stream << (qint32)m_ResultList.at(i).sourceList.at(j).dataSourceIndex;
			*stream << m_ResultList.at(i).sourceList.at(j).flags;
			*stream << m_ResultList.at(i).sourceList.at(j).yscale;
			*stream << m_ResultList.at(i).sourceList.at(j).bias;
			*stream << m_ResultList.at(i).sourceList.at(j).xscale;
			stream->writeRawData( m_ResultList.at(i).sourceList.at(j).reserved, sizeof(m_ResultList.at(i).sourceList.at(j).reserved) );

			for (int k = 0; k < MDTRA_SP_MAX; k++) {
				*stream << m_ResultList.at(i).sourceList.at(j).stat[k];
			}
			*stream << m_ResultList.at(i).sourceList.at(j).iDataSize;
			*stream << m_ResultList.at(i).sourceList.at(j).iActualDataSize;
			for (int k = 0; k < m_ResultList.at(i).sourceList.at(j).iDataSize; k++) {
				*stream << m_ResultList.at(i).sourceList.at(j).pData[k];
			}
			if (m_ResultList.at(i).sourceList.at(j).pCorrelation) {
				*stream << quint32(1);
				for (int k = 0; k < m_ResultList.at(i).sourceList.count(); k++) {
					*stream << m_ResultList.at(i).sourceList.at(j).pCorrelation[k];
				}
			} else {
				*stream << quint32(0);
			}
		}
		*stream << (qint32)m_ResultList.at(i).labelList.count();
		for (int j = 0; j < m_ResultList.at(i).labelList.count(); j++) {
			*stream << m_ResultList.at(i).labelList.at(j).text;
			*stream << m_ResultList.at(i).labelList.at(j).x;
			*stream << m_ResultList.at(i).labelList.at(j).y;
			*stream << m_ResultList.at(i).labelList.at(j).wide;
			*stream << m_ResultList.at(i).labelList.at(j).tall;
			*stream << m_ResultList.at(i).labelList.at(j).flags;
			*stream << m_ResultList.at(i).labelList.at(j).sourceNum;
			*stream << m_ResultList.at(i).labelList.at(j).snapshotNum;
		}
	}

	return true;
}

bool MDTRA_Project :: checkUniqueStreamName( const QString &name )
{
	for (int i = 0; i < m_StreamList.count(); i++) {
		if (m_StreamList.at(i).name == name)
			return false;
	}
	return true;
}

MDTRA_Stream *MDTRA_Project :: fetchStreamByIndex( int streamIndex ) const
{
	for (int i = 0; i < m_StreamList.count(); i++) {
		if (m_StreamList.at(i).index == streamIndex)
			return const_cast<MDTRA_Stream*>(&m_StreamList.at(i));
	}
	return NULL;
}

MDTRA_Stream *MDTRA_Project :: fetchStream( int index ) const
{
	if (index < 0 || index >= m_StreamList.count())
		return NULL;

	return const_cast<MDTRA_Stream*>(&m_StreamList.at(index));
}

int MDTRA_Project :: getValidStreamCount() const
{
	int c = 0;
	for (int i = 0; i < m_StreamList.count(); i++) {
		if (m_StreamList.at(i).pdb)
			c++;
	}
	return c;
}

int MDTRA_Project :: getValidStreamCount( int formatMask ) const
{
	if ( !formatMask )
		return getValidStreamCount();

	int c = 0;
	bool invalid;
	for (int i = 0; i < m_StreamList.count(); i++) {
		if (m_StreamList.at(i).pdb ) {
			invalid = false;
			for ( int j = 0; j < PDB_FS_MAX; j++ ) {
				if ( (formatMask & (1<<j)) && !g_PDBFormatManager.checkFormat(m_StreamList.at(i).format_identifier,(PDBFieldSense_e)j) ) {
					invalid = true;
					break;
				}
			}
			if ( !invalid )
				c++;
		}
	}
	return c;
}

int MDTRA_Project :: getStreamCountByFormatIdentifier( unsigned int format ) const
{
	int c = 0;
	for (int i = 0; i < m_StreamList.count(); i++) {
		if (m_StreamList.at(i).format_identifier == format)
			c++;
	}
	return c;
}

int MDTRA_Project :: registerStream( const QString &name, const QStringList &files, float xscale, unsigned int format_identifier, unsigned int flags )
{
	MDTRA_Stream stream;
	stream.index = m_pMainWindow->incStreamCounter();
	stream.name = name;
	stream.files = files;
	stream.xscale = xscale;
	stream.format_identifier = format_identifier;
	stream.flags = flags;
	if (files.count() > 0) {
		stream.pdb = new MDTRA_PDB_File;
		if (!stream.pdb->load( 0, stream.format_identifier, stream.files.at(0).toAscii(), stream.flags )) {
			delete stream.pdb;
			stream.pdb = NULL;
		} else {
			stream.pdb->move_to_centroid();
		}
	} else {
		stream.pdb = NULL;
	}
	memset( stream.reserved, 0, sizeof(stream.reserved) );
	m_StreamList << stream;
	updateStreamList();
	return stream.index;
}

void MDTRA_Project :: unregisterStream( int index )
{
	for (int i = 0; i < m_DataSourceList.count(); i++) {
		if (m_DataSourceList.at(i).streamIndex == index) {
			for (int j = 0; j < m_ResultList.count(); j++) {
				QList<MDTRA_DSRef> *pRefList = const_cast<QList<MDTRA_DSRef> *>(&m_ResultList.at(j).sourceList);
				for (int k = 0; k < pRefList->count(); k++) {
					if (pRefList->at(k).dataSourceIndex == m_DataSourceList.at(i).index) {
						if (pRefList->at(k).pData) UTIL_AlignedFree( pRefList->at(k).pData );
						if (pRefList->at(k).pCorrelation) UTIL_AlignedFree( pRefList->at(k).pCorrelation );
						pRefList->removeAt(k);
						k--;
					}
				}
				if (!pRefList->count()) {
					m_ResultList.removeAt(j);
					j--;
				}
			}
			m_DataSourceList.removeAt(i);
			i--;
		}
	}

	for (int i = 0; i < m_StreamList.count(); i++) {
		if (m_StreamList.at(i).index == index) {
			if (m_StreamList.at(i).pdb) delete m_StreamList.at(i).pdb;
			m_StreamList.removeAt(i);
			break;
		}
	}

	updateStreamList();
	updateDataSourceList();
	updateResultList();
}

void MDTRA_Project :: modifyStream( int index, const QString &name, const QStringList &files, float xscale, unsigned int format_identifier, unsigned int flags )
{
	MDTRA_Stream *pStream = fetchStreamByIndex( index );
	if (!pStream)
		return;
	pStream->name = name;
	pStream->files.clear();
	pStream->files = files;
	pStream->xscale = xscale;
	pStream->format_identifier = format_identifier;
	pStream->flags = flags;
	if (pStream->pdb) delete pStream->pdb;
	if (files.count() > 0) {
		pStream->pdb = new MDTRA_PDB_File;
		if (!pStream->pdb->load( 0, pStream->format_identifier, files.at(0).toAscii(), pStream->flags )) {
			delete pStream->pdb;
			pStream->pdb = NULL;
		} else {
			pStream->pdb->move_to_centroid();
		}
	} else {
		pStream->pdb = NULL;
	}
	invalidateDataSourceByStreamIndex( index );
	updateStreamList();
	updateResultList();
}

void MDTRA_Project :: updateStreamList()
{
	m_pMainWindow->getStreamListWidget()->clear();
	for (int i = 0; i < m_StreamList.count(); i++) {
		const MDTRA_Stream *pStream = &m_StreamList.at(i);
		QListWidgetItem *pItem = new QListWidgetItem( QObject::tr("STREAM %1: %2 (%3 files)").arg(pStream->index).arg(pStream->name).arg(pStream->files.count()), m_pMainWindow->getStreamListWidget() );
		pItem->setIcon( QIcon(":/png/16x16/stream.png") );
		pItem->setData( Qt::UserRole, pStream->index );
	}
}

bool MDTRA_Project :: checkUniqueDataSourceName( const QString &name )
{
	for (int i = 0; i < m_DataSourceList.count(); i++) {
		if (m_DataSourceList.at(i).name == name)
			return false;
	}
	return true;
}

int MDTRA_Project :: getDataSourceCountByStreamIndex( int index )
{
	int c = 0;
	for (int i = 0; i < m_DataSourceList.count(); i++) {
		if (m_DataSourceList.at(i).streamIndex == index)
			c++;
	}
	return c;
}

MDTRA_DataSource *MDTRA_Project :: fetchDataSourceByIndex( int index )
{
	for (int i = 0; i < m_DataSourceList.count(); i++) {
		if (m_DataSourceList.at(i).index == index)
			return const_cast<MDTRA_DataSource*>(&m_DataSourceList.at(i));
	}
	return NULL;
}

MDTRA_DataSource *MDTRA_Project :: fetchDataSource( int index )
{
	if (index < 0 || index >= m_DataSourceList.count())
		return NULL;

	return const_cast<MDTRA_DataSource*>(&m_DataSourceList.at(index));
}

int MDTRA_Project :: registerDataSource( const QString &name, int streamIndex, MDTRA_DataType dataType, const MDTRA_DataArg *pdataArgs, const MDTRA_Selection &selection1, const MDTRA_Selection &selection2, const MDTRA_Prog &prog, const QString& userdata, int userFlags, bool updateGUI )
{
	MDTRA_DataSource ds;
	ds.index = m_pMainWindow->incDataSourceCounter();
	ds.name = name;
	ds.selection.string = selection1.string;
	ds.selection.flags = selection1.flags;
	ds.selection.size = selection1.size;
	ds.selection.data = NULL;
	ds.selection2.string = selection2.string;
	ds.selection2.flags = selection2.flags;
	ds.selection2.size = selection2.size;
	ds.selection2.data = NULL;
	ds.prog.sourceCode = prog.sourceCode;
	ds.prog.byteCode = prog.byteCode;
	ds.prog.byteCodeSize = prog.byteCodeSize;
	ds.userdata = userdata;
	ds.userFlags = userFlags;
	ds.streamIndex = streamIndex;
	ds.type = dataType;
	memcpy( ds.arg, pdataArgs, sizeof(MDTRA_DataArg)*MAX_DATA_SOURCE_ARGS );
	if (selection1.size > 0) {
		ds.selection.data = new int[selection1.size];
		memcpy( ds.selection.data, selection1.data, sizeof(int)*selection1.size );
	}
	if (selection2.size > 0) {
		ds.selection2.data = new int[selection2.size];
		memcpy( ds.selection2.data, selection2.data, sizeof(int)*selection2.size );
	}
	memset( ds.reserved, 0, sizeof(ds.reserved) );
	m_DataSourceList << ds;
	if (updateGUI)
		updateDataSourceList();
	return ds.index;
}

void MDTRA_Project :: unregisterDataSource( int index )
{
	for (int i = 0; i < m_ResultList.count(); i++) {
		QList<MDTRA_DSRef> *pRefList = const_cast<QList<MDTRA_DSRef> *>(&m_ResultList.at(i).sourceList);
		for (int j = 0; j < pRefList->count(); j++) {
			if (pRefList->at(j).dataSourceIndex == index) {
				if (pRefList->at(j).pData) UTIL_AlignedFree( pRefList->at(j).pData );
				if (pRefList->at(j).pCorrelation) UTIL_AlignedFree( pRefList->at(j).pCorrelation );
				pRefList->removeAt(j);
				j--;
			}
		}
		if (!pRefList->count()) {
			m_ResultList.removeAt(i);
			i--;
		}
	}

	for (int i = 0; i < m_DataSourceList.count(); i++) {
		if (m_DataSourceList.at(i).index == index) {
			if (m_DataSourceList.at(i).selection.data) delete [] m_DataSourceList.at(i).selection.data;
			if (m_DataSourceList.at(i).selection2.data) delete [] m_DataSourceList.at(i).selection2.data;
			if (m_DataSourceList.at(i).prog.byteCode) delete [] m_DataSourceList.at(i).prog.byteCode;
			m_DataSourceList.removeAt(i);
			break;
		}
	}

	updateDataSourceList();
	updateResultList();
}

void MDTRA_Project :: modifyDataSource( int index, const QString &name, int streamIndex, MDTRA_DataType dataType, const MDTRA_DataArg *pdataArgs, const MDTRA_Selection &selection1, const MDTRA_Selection &selection2, const MDTRA_Prog &prog, const QString& userdata, int userFlags )
{
	MDTRA_DataSource *pDS = fetchDataSourceByIndex( index );
	if (!pDS)
		return;
	pDS->name = name;
	bool dataChange = false;

	if (pDS->selection.string != selection1.string ||
		pDS->selection.flags != selection1.flags || 
		pDS->selection.size != selection1.size) {
		pDS->selection.string = selection1.string;
		pDS->selection.flags = selection1.flags;
		if (pDS->selection.data) delete [] pDS->selection.data;
		pDS->selection.data = NULL;
		pDS->selection.size = selection1.size;
		if (selection1.size > 0) {
			pDS->selection.data = new int[selection1.size];
			memcpy( pDS->selection.data, selection1.data, sizeof(int)*selection1.size );
		}
		dataChange = true;
	}
	if (pDS->selection2.string != selection2.string ||
		pDS->selection2.flags != selection2.flags ||
		pDS->selection2.size != selection2.size) {
		pDS->selection2.string = selection2.string;
		pDS->selection2.flags = selection2.flags;
		if (pDS->selection2.data) delete [] pDS->selection2.data;
		pDS->selection2.data = NULL;
		pDS->selection2.size = selection2.size;
		if (selection2.size > 0) {
			pDS->selection2.data = new int[selection2.size];
			memcpy( pDS->selection2.data, selection2.data, sizeof(int)*selection2.size );
		}
		dataChange = true;
	}
	if (pDS->prog.sourceCode != prog.sourceCode) {
		pDS->prog.sourceCode = prog.sourceCode;
		if (pDS->prog.byteCode) delete [] pDS->prog.byteCode;
		pDS->prog.byteCode = prog.byteCode;
		pDS->prog.byteCodeSize = prog.byteCodeSize;
		dataChange = true;
	}

	if ( dataChange ||
		pDS->streamIndex != streamIndex ||
		pDS->type != dataType ||
		memcmp( pDS->arg, pdataArgs, sizeof(MDTRA_DataArg)*MAX_DATA_SOURCE_ARGS )) {
		invalidateDataSourceByIndex( index );
	}

	pDS->userdata = userdata;
	pDS->userFlags = userFlags;
	pDS->streamIndex = streamIndex;
	pDS->type = dataType;
	memcpy( pDS->arg, pdataArgs, sizeof(MDTRA_DataArg)*MAX_DATA_SOURCE_ARGS );
	updateDataSourceList();
	updateResultList();
}

void MDTRA_Project :: invalidateDataSourceByStreamIndex( int index )
{
	for (int i = 0; i < m_DataSourceList.count(); i++) {
		if (m_DataSourceList.at(i).streamIndex == index) {
			invalidateDataSourceByIndex( m_DataSourceList.at(i).index );
		}
	}
}

void MDTRA_Project :: invalidateDataSourceByIndex( int index )
{
	for (int i = 0; i < m_ResultList.count(); i++) {
		for (int j = 0; j < m_ResultList.at(i).sourceList.count(); j++) {
			if (m_ResultList.at(i).sourceList.at(j).dataSourceIndex == index) {
				invalidateResult( m_ResultList.at(i).index, false );
				continue;
			}
		}
	}
}

void MDTRA_Project :: updateDataSourceList()
{
	m_pMainWindow->getDataSourceListWidget()->clear();
	for (int i = 0; i < m_DataSourceList.count(); i++) {
		const MDTRA_DataSource *pDS = &m_DataSourceList.at(i);
		QListWidgetItem *pItem = new QListWidgetItem( QObject::tr("DATA SOURCE %1: %2\nData Type: %3\nStream source: STREAM %4")
														.arg(pDS->index)
														.arg(pDS->name)
														.arg(UTIL_GetDataSourceTypeName(pDS->type))
														.arg(pDS->streamIndex), 
														m_pMainWindow->getDataSourceListWidget() );
		pItem->setIcon( QIcon(":/png/16x16/source.png") );
		pItem->setData( Qt::UserRole, pDS->index );
	}
}

bool MDTRA_Project :: checkUniqueResultName( const QString &name )
{
	for (int i = 0; i < m_ResultList.count(); i++) {
		if (m_ResultList.at(i).name == name)
			return false;
	}
	return true;
}

MDTRA_Result *MDTRA_Project :: fetchResultByIndex( int index )
{
	for (int i = 0; i < m_ResultList.count(); i++) {
		if (m_ResultList.at(i).index == index)
			return const_cast<MDTRA_Result*>(&m_ResultList.at(i));
	}
	return NULL;
}

MDTRA_Result *MDTRA_Project :: fetchResult( int index )
{
	if (index < 0 || index >= m_ResultList.count())
		return NULL;

	return const_cast<MDTRA_Result*>(&m_ResultList.at(index));
}

int MDTRA_Project :: registerResult( const QString &name, MDTRA_DataType type, MDTRA_YScaleUnits scaleUnits, MDTRA_Layout layout, const QList<MDTRA_DSRef> &dsref, bool updateGUI )
{
	MDTRA_Result result;
	result.index = m_pMainWindow->incResultCounter();
	result.name = name;
	result.status = 0;
	result.type = type;
	result.units = scaleUnits;
	result.layout = layout;
	result.sourceList = dsref;
	memset( result.reserved, 0, sizeof(result.reserved) );
	m_ResultList << result;
	if (updateGUI)
		updateResultList();
	return result.index;
}

void MDTRA_Project :: unregisterResult( int index )
{
	for (int i = 0; i < m_ResultList.count(); i++) {
		if (m_ResultList.at(i).index == index) {
			QList<MDTRA_DSRef> *pRefList = const_cast<QList<MDTRA_DSRef> *>(&m_ResultList.at(i).sourceList);
			for (int j = 0; j < pRefList->count(); j++) {
				if (pRefList->at(j).pData) UTIL_AlignedFree( pRefList->at(j).pData );
				if (pRefList->at(j).pCorrelation) UTIL_AlignedFree( pRefList->at(j).pCorrelation );
			}
			QList<MDTRA_Label> *pLabelList = const_cast<QList<MDTRA_Label> *>(&m_ResultList.at(i).labelList);
			pLabelList->clear();
			m_ResultList.removeAt(i);
			break;
		}
	}
	updateResultList();
}

void MDTRA_Project :: modifyResult( int index, const QString &name, MDTRA_DataType type, MDTRA_YScaleUnits scaleUnits, MDTRA_Layout layout, const QList<MDTRA_DSRef> &dsref )
{
	MDTRA_Result *pResult = fetchResultByIndex( index );
	if (!pResult)
		return;

	int clearStatus = 0;
	if (type != pResult->type)
		clearStatus = 1;
	if (scaleUnits != pResult->units)
		clearStatus = 1;
	if (layout != pResult->layout)
		clearStatus = 1;

	if (dsref.count() != pResult->sourceList.count()) {
		clearStatus = 1;
	} else {
		for (int i = 0; i < dsref.count(); i++) {
			if (dsref.at(i).dataSourceIndex != pResult->sourceList.at(i).dataSourceIndex ||
				dsref.at(i).yscale != pResult->sourceList.at(i).yscale ||
				dsref.at(i).bias != pResult->sourceList.at(i).bias) {
				clearStatus = 1;
				break;
			}
		}
	}

	pResult->name = name;
	pResult->type = type;
	pResult->units = scaleUnits;
	pResult->layout = layout;

	if (clearStatus) {
		invalidateResult( index, false );
		pResult->sourceList = dsref;
		for (int i = 0; i < pResult->labelList.count(); i++) {
			if ( pResult->labelList.at(i).sourceNum >= dsref.count() ) {
				pResult->labelList.removeAt(i);
				i--;
			}
		}
		for (int i = 0; i < pResult->sourceList.count(); i++) {
			MDTRA_DSRef* pDSRef = const_cast<MDTRA_DSRef*>(&pResult->sourceList.at(i));
			pDSRef->iDataSize = 0;
			pDSRef->iActualDataSize = 0;
			pDSRef->pData = NULL;
			pDSRef->pCorrelation = NULL;
		}
	} else {
		for (int i = 0; i < pResult->sourceList.count(); i++) {
			MDTRA_DSRef* pDSRef = const_cast<MDTRA_DSRef*>(&pResult->sourceList.at(i));
			pDSRef->flags = dsref.at(i).flags;
		}
	}
	
	updateResultList();
}

void MDTRA_Project :: invalidateResult( int index, bool updateList )
{
	MDTRA_Result *pResult = fetchResultByIndex( index );
	if (!pResult)
		return;

	pResult->status = 0;
	for (int i = 0; i < pResult->sourceList.count(); i++) {
		MDTRA_DSRef* pDSRef = const_cast<MDTRA_DSRef*>(&pResult->sourceList.at(i));
		if (pDSRef->pData) {
			UTIL_AlignedFree( pDSRef->pData );
			pDSRef->pData = NULL;
		}
		if (pDSRef->pCorrelation) {
			UTIL_AlignedFree( pDSRef->pCorrelation );
			pDSRef->pCorrelation = NULL;
		}
		pDSRef->iDataSize = 0;
		pDSRef->iActualDataSize = 0;
	}

	if (updateList)
		updateResultList();
}

bool MDTRA_Project :: addResultLabel( int index, const MDTRA_Label *pLabel )
{
	MDTRA_Result *pResult = fetchResultByIndex( index );
	if (!pResult)
		return false;

	pResult->labelList.append( *pLabel );
	return true;
}

bool MDTRA_Project :: editResultLabel( int index, int labelIndex, const MDTRA_Label *pLabel )
{
	MDTRA_Result *pResult = fetchResultByIndex( index );
	if (!pResult)
		return false;

	if (labelIndex < 0 || labelIndex >= pResult->labelList.count())
		return false;

	MDTRA_Label *lbl = const_cast<MDTRA_Label*>(&pResult->labelList.at( labelIndex ));
	lbl->text = pLabel->text;
	lbl->x = pLabel->x;
	lbl->y = pLabel->y;
	lbl->sourceNum = pLabel->sourceNum;
	lbl->snapshotNum = pLabel->snapshotNum;
	lbl->flags = pLabel->flags;
	lbl->wide = pLabel->wide;
	lbl->tall = pLabel->tall;
	return true;
}

bool MDTRA_Project :: removeResultLabel( int index, int labelIndex )
{
	MDTRA_Result *pResult = fetchResultByIndex( index );
	if (!pResult)
		return false;

	if (labelIndex < 0 || labelIndex >= pResult->labelList.count())
		return false;

	pResult->labelList.removeAt( labelIndex );
	return true;
}

void MDTRA_Project :: updateResultList()
{
	int oldIndex = m_pMainWindow->getResultListWidget()->currentRow();

	m_pMainWindow->getResultListWidget()->clear();
	for (int i = 0; i < m_ResultList.count(); i++) {
		const MDTRA_Result *pResult = &m_ResultList.at(i);
		QString itemText = QObject::tr("RESULT COLLECTOR %1: %2\nType: %3%4 [%5 data source(s)]\nScale Units: %6\n")
										.arg(pResult->index)
										.arg(pResult->name)
										.arg(UTIL_IsDataSourceLayoutChangeable(pResult->type) ? szLayoutNames[pResult->layout] : "")
										.arg(UTIL_GetDataSourceTypeName(pResult->type))
										.arg(pResult->sourceList.count())
										.arg(szScaleUnitNames[pResult->units]);


		if (pResult->status > 0) {
			itemText.append( QObject::tr("Status: Actual") );
		} else {
			itemText.append( QObject::tr("Status: Modified") );
		}

		QListWidgetItem *pItem = new QListWidgetItem( itemText, m_pMainWindow->getResultListWidget() );
		if (pResult->status > 0) {
			pItem->setIcon( QIcon(":/png/16x16/result.png") );
		} else {
			pItem->setIcon( QIcon(":/png/16x16/result2.png") );
		}
		pItem->setData( Qt::UserRole, pResult->index );
	}

	if (oldIndex >=0 && oldIndex < m_pMainWindow->getResultListWidget()->count())
		m_pMainWindow->getResultListWidget()->setCurrentRow(oldIndex);
}

int MDTRA_Project :: getResultCountByType( MDTRA_DataType type ) const
{
	int c = 0;
	for (int i = 0; i < m_ResultList.count(); i++) {
		if (m_ResultList.at(i).type == type) c++;
	}
	return c;
}

int MDTRA_Project :: getResultDataSourceCountByStreamIndex( int index )
{
	int c = 0;
	for (int i = 0; i < m_DataSourceList.count(); i++) {
		if (m_DataSourceList.at(i).streamIndex == index) {
			for (int j = 0; j < m_ResultList.count(); j++) {
				QList<MDTRA_DSRef> *pRefList = const_cast<QList<MDTRA_DSRef> *>(&m_ResultList.at(j).sourceList);
				for (int k = 0; k < pRefList->count(); k++) {
					if (pRefList->at(k).dataSourceIndex == m_DataSourceList.at(i).index) {
						c++;
					}
				}
			}
		}
	}
	return c;
}

int MDTRA_Project :: getResultDataSourceCountByDataSourceIndex( int index )
{
	int c = 0;
	for (int i = 0; i < m_ResultList.count(); i++) {
		QList<MDTRA_DSRef> *pRefList = const_cast<QList<MDTRA_DSRef> *>(&m_ResultList.at(i).sourceList);
		for (int j = 0; j < pRefList->count(); j++) {
			if (pRefList->at(j).dataSourceIndex == index)
				c++;
		}
	}
	return c;
}

float MDTRA_Project :: sampleData( const float *pDataPtr, int iSample, int iMaxSample, int iFilterSize, MDTRA_YScaleUnits ysu )
{
	if ( iFilterSize <= 1 )
		return pDataPtr[iSample];

	// Apply CMA low-pass filter
	int sampleMin = iSample - (iFilterSize >> 1);
	int sampleMax = iSample + (((iFilterSize-1) >> 1) + 1);

	if ( sampleMin < 0 )
		sampleMin = 0;
	if ( sampleMax > iMaxSample )
		sampleMax = iMaxSample;

	float fSample = 0.0f;

	switch ( ysu ) {
	case MDTRA_YSU_DEGREES:
		{
			const float sampleRef = pDataPtr[iSample];
			for ( int i = sampleMin; i < sampleMax; i++ ) {
				float sampleCurrent = pDataPtr[i];
				float sampleDelta = fabsf( sampleCurrent - sampleRef );
				if ( fabsf( sampleDelta - 360 ) < sampleDelta ) {
					if ( sampleCurrent > sampleRef )
						sampleCurrent -= 360;
					else
						sampleCurrent += 360;
				}
				fSample += sampleCurrent;
			}
		}
		break;
	case MDTRA_YSU_RADIANS:
		{
			const float sampleRef = pDataPtr[iSample];
			for ( int i = sampleMin; i < sampleMax; i++ ) {
				float sampleCurrent = pDataPtr[i];
				float sampleDelta = fabsf( sampleCurrent - sampleRef );
				if ( fabsf( sampleDelta - M_PI_F*2.0f ) < sampleDelta ) {
					if ( sampleCurrent > sampleRef )
						sampleCurrent -= M_PI_F*2.0f;
					else
						sampleCurrent += M_PI_F*2.0f;
				}
				fSample += sampleCurrent;
			}
		}
		break;
	default:
		{
			for ( int i = sampleMin; i < sampleMax; i++ )
				fSample += pDataPtr[i];
		}
		break;
	}

	return ( fSample / (float)(sampleMax - sampleMin) );
}

void MDTRA_Project :: exportResultToTXT( const MDTRA_Result *pResult, QTextStream *stream, int dataFilter )
{
	int iNumRows = 0;
	int iNumCols = pResult->sourceList.count();

	for (int i = 0; i < iNumCols; i++) {
		if (pResult->sourceList.at(i).iActualDataSize > iNumRows) 
			iNumRows = pResult->sourceList.at(i).iActualDataSize;

		const MDTRA_DataSource *pDS = fetchDataSourceByIndex( pResult->sourceList.at(i).dataSourceIndex );
		QString headerName = QString("\t\"%1\"").arg( pDS ? pDS->name : "???" );
		*stream << headerName;
	}
	*stream << endl;

	for (int i = 0; i < iNumRows; i++) {
		*stream << (QString("%1").arg( i + 1 ));
		for (int j = 0; j < iNumCols; j++) {
			const MDTRA_DSRef *dataRef = &pResult->sourceList.at(j);
			if ( i < dataRef->iActualDataSize ) {
				float value = sampleData( dataRef->pData, i, dataRef->iActualDataSize, dataFilter, pResult->units );
				*stream << (QString("\t%1").arg( value, 0, 'f', 5));
			} else {
				*stream << "\t";
			}
		}
		*stream << endl;
	}
}

void MDTRA_Project :: exportResultToCSV( const MDTRA_Result *pResult, QTextStream *stream, int dataFilter )
{
	int iNumRows = 0;
	int iNumCols = pResult->sourceList.count();

	*stream << QString("\"\"");
	for (int i = 0; i < iNumCols; i++) {
		if (pResult->sourceList.at(i).iActualDataSize > iNumRows) 
			iNumRows = pResult->sourceList.at(i).iActualDataSize;

		const MDTRA_DataSource *pDS = fetchDataSourceByIndex( pResult->sourceList.at(i).dataSourceIndex );
		QString headerName = QString(";\"%1\"").arg( pDS ? pDS->name : "???" );
		*stream << headerName;
	}
	*stream << endl;

	for (int i = 0; i < iNumRows; i++) {
		*stream << (QString("%1").arg( i + 1 ));
		for (int j = 0; j < iNumCols; j++) {
			const MDTRA_DSRef *dataRef = &pResult->sourceList.at(j);
			if ( i < dataRef->iActualDataSize ) {
				float value = sampleData( dataRef->pData, i, dataRef->iActualDataSize, dataFilter, pResult->units );
				*stream << (QString(";%1").arg( value, 0, 'f', 5));
			} else {
				*stream << ";";
			}
		}
		*stream << endl;
	}
}

void MDTRA_Project :: exportResult( int index, const QString &fileSuffix, QTextStream *stream, int dataFilter )
{
	const MDTRA_Result *pResult = fetchResultByIndex( index );
	if (!pResult)
		return;

	if (fileSuffix.toLower() == "txt") {
		exportResultToTXT( pResult, stream, dataFilter );
	} else if (fileSuffix.toLower() == "csv") {
		exportResultToCSV( pResult, stream, dataFilter );
	}
}

void MDTRA_Project :: exportStatsToTXT( const MDTRA_Result *pResult, QTextStream *stream )
{
	int iNumCols = pResult->sourceList.count();
	int maxUserStat = 0; 
	bool bHasCorrelationData = pResult->sourceList.at(0).pCorrelation;

	for (int i = 0; i < iNumCols; i++) {
		dword statFlags = (pResult->sourceList.at(i).flags >> DSREF_STAT_SHIFT) & DSREF_STAT_MASK;
		for (int j = maxUserStat; j < 6; j++) {
			if ( statFlags & (1<<j) ) maxUserStat = j+1;
		}
		const MDTRA_DataSource *pDS = fetchDataSourceByIndex( pResult->sourceList.at(i).dataSourceIndex );
		QString headerName = QString("\t\"%1\"").arg( pDS ? pDS->name : "???" );
		*stream << headerName;
	}
	*stream << endl;

	for (int i = 0; i < MDTRA_SP_MAX_USED; i++) {
		*stream << (QString("\"%1\"").arg( UTIL_GetStatParmName(i) ));
		for (int j = 0; j < iNumCols; j++) {
			*stream << ((pResult->sourceList.at(j).stat[i] == (-FLT_MAX)) ? QString("\tN/A") : QString("\t%1").arg( pResult->sourceList.at(j).stat[i], 0, 'f', 5));
		}
		*stream << endl;
	}
	for (int i = 0; i < maxUserStat; i++) {
		*stream << (QString("\"User Parameter %1\"").arg( i+1 ));
		for (int j = 0; j < iNumCols; j++) {
			dword statFlags = (pResult->sourceList.at(i).flags >> DSREF_STAT_SHIFT) & DSREF_STAT_MASK;
			*stream << ((!(statFlags & (1 << j))) ? QString("\tN/A") : QString("\t%1").arg( pResult->sourceList.at(j).stat[MDTRA_SP_USER1+i], 0, 'f', 5));
		}
		*stream << endl;
	}

	if (bHasCorrelationData) {
		for (int i = 0; i < iNumCols; i++) {
			*stream << (QString("\"r(%1:X)\"").arg( i+1 ));
			for (int j = 0; j < iNumCols; j++) {
				*stream << ((pResult->sourceList.at(i).pCorrelation[j] == (-FLT_MAX)) ? QString("\tN/A") : QString("\t%1").arg( pResult->sourceList.at(i).pCorrelation[j], 0, 'f', 5));
			}
			*stream << endl;
		}
	}
}

void MDTRA_Project :: exportStatsToCSV( const MDTRA_Result *pResult, QTextStream *stream )
{
	int iNumCols = pResult->sourceList.count();
	int maxUserStat = 0; 
	bool bHasCorrelationData = pResult->sourceList.at(0).pCorrelation;

	*stream << QString("\"\"");
	for (int i = 0; i < iNumCols; i++) {
		dword statFlags = (pResult->sourceList.at(i).flags >> DSREF_STAT_SHIFT) & DSREF_STAT_MASK;
		for (int j = maxUserStat; j < 6; j++) {
			if ( statFlags & (1<<j) ) maxUserStat = j+1;
		}
		const MDTRA_DataSource *pDS = fetchDataSourceByIndex( pResult->sourceList.at(i).dataSourceIndex );
		QString headerName = QString(";\"%1\"").arg( pDS ? pDS->name : "???" );
		*stream << headerName;
	}
	*stream << endl;

	for (int i = 0; i < MDTRA_SP_MAX_USED; i++) {
		*stream << (QString("\"%1\"").arg( UTIL_GetStatParmName(i) ));
		for (int j = 0; j < iNumCols; j++) {
			*stream << ((pResult->sourceList.at(j).stat[i] == (-FLT_MAX)) ? QString(";\"N/A\"") : QString(";%1").arg( pResult->sourceList.at(j).stat[i], 0, 'f', 5));
		}
		*stream << endl;
	}
	for (int i = 0; i < maxUserStat; i++) {
		*stream << (QString("\"User Parameter %1\"").arg( i+1 ));
		for (int j = 0; j < iNumCols; j++) {
			dword statFlags = (pResult->sourceList.at(i).flags >> DSREF_STAT_SHIFT) & DSREF_STAT_MASK;
			*stream << ((!(statFlags & (1 << j))) ? QString(";\"N/A\"") : QString(";%1").arg( pResult->sourceList.at(j).stat[MDTRA_SP_USER1+i], 0, 'f', 5));
		}
		*stream << endl;
	}

	if (bHasCorrelationData) {
		for (int i = 0; i < iNumCols; i++) {
			*stream << (QString("\"r(%1:X)\"").arg( i+1 ));
			for (int j = 0; j < iNumCols; j++) {
				*stream << ((pResult->sourceList.at(i).pCorrelation[j] == (-FLT_MAX)) ? QString(";\"N/A\"") : QString(";\"%1\"").arg( pResult->sourceList.at(i).pCorrelation[j], 0, 'f', 5));
			}
			*stream << endl;
		}
	}
}

void MDTRA_Project :: exportStats( int index, const QString &fileSuffix, QTextStream *stream )
{
	const MDTRA_Result *pResult = fetchResultByIndex( index );
	if (!pResult)
		return;

	if (fileSuffix.toLower() == "txt") {
		exportStatsToTXT( pResult, stream );
	} else if (fileSuffix.toLower() == "csv") {
		exportStatsToCSV( pResult, stream );
	}
}

static MDTRA_StreamWork *pLocalStreamWork = NULL;
extern MDTRA_ProgressDialog *pProgressDialog;

static void fn_BuildStreamData_ResidueBased( MDTRA_StreamWorkResult *pResult, MDTRA_PDB_File *pPdbFile, bool &bAligned, int threadnum, int num, int numfiles )
{
	int iNumFloats;
	float *pCurrentFloat;
	bool bInitialFrame = false;

	if ( pResult->pDataSource->type == MDTRA_DT_RMSF || pResult->pDataSource->type == MDTRA_DT_RMSF_SEL )
		iNumFloats = pPdbFile->getAtomCount();
	else
		iNumFloats = pPdbFile->getResidueCount();

	float *pFloats = pPdbFile->get_floats();
	assert( pFloats != NULL );

	pCurrentFloat = pFloats;
	for (int i = 0; i < iNumFloats; i++, pCurrentFloat++) {
		*pCurrentFloat = -1.0f;
	}

	//fill values with raw, unmapped data
	//get raw, unmapped data
	switch (pResult->pDataSource->type) {
	case MDTRA_DT_RMSD:
		if (num == 0) bInitialFrame = true;
		if (!bAligned) {
			//align PDB file with first file in stream
			pPdbFile->move_to_centroid();
			pPdbFile->align_kabsch( pLocalStreamWork->pStream->pdb );
			bAligned = true;
		}
		pPdbFile->get_rmsd_of_residues( pLocalStreamWork->pStream->pdb, pFloats );
		break;
	case MDTRA_DT_RMSD_SEL:
		if (pLocalStreamWork->pStream->pdb == pPdbFile || !pResult->pRefPDB) {
			memset( pFloats, 0, iNumFloats*sizeof(float) );
			bInitialFrame = true;
		} else {
			//align both current PDB file and first file in stream
			pPdbFile->set_flag( pResult->pDataSource->selection.size, pResult->pDataSource->selection.data, PDB_FLAG_RMSD );
			pPdbFile->move_to_centroid2();
			pPdbFile->align_kabsch2( pResult->pRefPDB );
			pPdbFile->get_rmsd2_of_residues( pResult->pRefPDB, pFloats );
		}
		break;
	case MDTRA_DT_RMSF:
		if (num == 0) bInitialFrame = true;
		if (!bAligned) {
			//align PDB file with first file in stream
			pPdbFile->move_to_centroid();
			pPdbFile->align_kabsch( pLocalStreamWork->pStream->pdb );
			bAligned = true;
		}
		pPdbFile->get_rmsf_of_atoms( pLocalStreamWork->averagePDB, pFloats );
		break;
	case MDTRA_DT_RMSF_SEL:
		if (pLocalStreamWork->pStream->pdb == pPdbFile || !pResult->pRefPDB) {
			memset( pFloats, 0, iNumFloats*sizeof(float) );
			bInitialFrame = true;
		} else {
			//align both current PDB file and first file in stream
			pPdbFile->set_flag( pResult->pDataSource->selection.size, pResult->pDataSource->selection.data, PDB_FLAG_RMSF );
			pPdbFile->move_to_centroid2();
			pPdbFile->align_kabsch2( pResult->pRefPDB );
			pPdbFile->get_rmsf2_of_atoms( pLocalStreamWork->averagePDB, pFloats );
		}
		break;
	case MDTRA_DT_SAS:
		pPdbFile->set_flag( 0, NULL, PDB_FLAG_SAS );
		pPdbFile->get_sas_of_residues( threadnum, pFloats );
		break;
	case MDTRA_DT_SAS_SEL:
		pPdbFile->set_flag( pResult->pDataSource->selection.size, pResult->pDataSource->selection.data, PDB_FLAG_SAS );
		pPdbFile->get_sas_of_residues( threadnum, pFloats );
		break;
	case MDTRA_DT_OCCA:
		pPdbFile->set_flag( 0, NULL, PDB_FLAG_SAS );
		pPdbFile->set_flag( pResult->pDataSource->selection2.size, pResult->pDataSource->selection2.data, PDB_FLAG_OCCLUDER );
		pPdbFile->get_occa_of_residues( threadnum, pFloats );
		break;
	case MDTRA_DT_OCCA_SEL:
		pPdbFile->set_flag( pResult->pDataSource->selection.size, pResult->pDataSource->selection.data, PDB_FLAG_SAS );
		pPdbFile->set_flag( pResult->pDataSource->selection2.size, pResult->pDataSource->selection2.data, PDB_FLAG_OCCLUDER );
		pPdbFile->get_occa_of_residues( threadnum, pFloats );
		break;
	case MDTRA_DT_USER:
		//clear
		{
			memset( pFloats, 0, sizeof(*pFloats) * iNumFloats );
			MDTRA_Program_Interpreter* pInterpreter = (MDTRA_Program_Interpreter*)pResult->pProgInterpreter;
			if (pInterpreter && pInterpreter->IsValid()) {
				pInterpreter->SetResidueBuffer( threadnum, pFloats, iNumFloats );
				pInterpreter->Main( threadnum, num, pPdbFile, NULL );
			}
		}
		break;
	default:
		break;
	}

	pCurrentFloat = pFloats;
	for (int i = 0; i < iNumFloats; i++, pCurrentFloat++) {
		float flCurrentFloat = (*pCurrentFloat);
		if (flCurrentFloat >= 0) {
			//apply unit scale
			switch (pResult->pResult->units) {
			case MDTRA_YSU_ANGSTROMS:
				//distance is in angstroms by default
				break;
			case MDTRA_YSU_NANOMETERS:
				flCurrentFloat *= 0.1f;
				break;
			case MDTRA_YSU_RADIANS:
				//angle is in radians by default
				break;
			case MDTRA_YSU_DEGREES:
				flCurrentFloat = UTIL_rad2deg( flCurrentFloat );
				break;
			case MDTRA_YSU_KCALOVERA:
				//force is in kcal/A by default
				break;
			case MDTRA_YSU_MKNEWTON:
				flCurrentFloat *= 0.4184f;
				break;
			case MDTRA_YSU_SQANGSTROMS:
				//area is in sq. A by default
				break;
			case MDTRA_YSU_SQNANOMETERS:
				flCurrentFloat *= 0.01f;
				break;
			default:
				break;
			}

			//apply reference scale and bias
			flCurrentFloat = flCurrentFloat * pResult->pDSRef->yscale + pResult->pDSRef->bias;

			//scale by number of snapshots
			flCurrentFloat /= (float)numfiles;

			//increment some statistic parameters
			pResult->threadStat[MDTRA_TSP_ARITHMETIC_MEAN][threadnum] += flCurrentFloat;

			//write residue-based output
			pResult->pDSRef->pData[iNumFloats*threadnum+i] += flCurrentFloat;
			if (!bInitialFrame && (i >= pResult->pDSRef->iActualDataSize)) {
				pResult->pDSRef->iActualDataSize = i+1;
			}
		}
	}
}

static void fn_BuildStreamData_TimeBased( MDTRA_StreamWorkResult *pResult, MDTRA_PDB_File *pPdbFile, bool &bAligned, int threadnum, int num )
{
	float flResultData = 0.0f;
		
	//get raw, unmapped data
	switch (pResult->pDataSource->type) {
	case MDTRA_DT_RMSD:
		if (!bAligned) {
			//align PDB file with first file in stream
			pPdbFile->move_to_centroid();
			pPdbFile->align_kabsch( pLocalStreamWork->pStream->pdb );
			bAligned = true;
		}
		flResultData = pPdbFile->get_rmsd( pLocalStreamWork->pStream->pdb );
		break;
	case MDTRA_DT_RMSD_SEL:
		if (pLocalStreamWork->pStream->pdb == pPdbFile || !pResult->pRefPDB) {
			flResultData = 0.0f;
		} else {
			//align both current PDB file and first file in stream
			pPdbFile->set_flag( pResult->pDataSource->selection.size, pResult->pDataSource->selection.data, PDB_FLAG_RMSD );
			pPdbFile->move_to_centroid2();
			pPdbFile->align_kabsch2( pResult->pRefPDB );
			flResultData = pPdbFile->get_rmsd2( pResult->pRefPDB );
		}
		break;
	case MDTRA_DT_RADIUS_OF_GYRATION:
		pPdbFile->move_to_centroid();
		flResultData = pPdbFile->get_radius_of_gyration();
		break;
	case MDTRA_DT_DISTANCE:
		flResultData = pPdbFile->get_distance( pResult->pDataSource->arg[0].atomIndex, pResult->pDataSource->arg[1].atomIndex );
		break;
	case MDTRA_DT_ANGLE:
		flResultData = pPdbFile->get_angle( pResult->pDataSource->arg[0].atomIndex, pResult->pDataSource->arg[1].atomIndex, pResult->pDataSource->arg[2].atomIndex );
		break;
	case MDTRA_DT_ANGLE2:
		flResultData = pPdbFile->get_angle_between_sections( pResult->pDataSource->arg[0].atomIndex, pResult->pDataSource->arg[1].atomIndex, pResult->pDataSource->arg[2].atomIndex, pResult->pDataSource->arg[3].atomIndex );
		break;
	case MDTRA_DT_TORSION:
		flResultData = pPdbFile->get_torsion( pResult->pDataSource->arg[0].atomIndex, pResult->pDataSource->arg[1].atomIndex, pResult->pDataSource->arg[2].atomIndex, pResult->pDataSource->arg[3].atomIndex, true );
		break;
	case MDTRA_DT_TORSION_UNSIGNED:
		flResultData = pPdbFile->get_torsion( pResult->pDataSource->arg[0].atomIndex, pResult->pDataSource->arg[1].atomIndex, pResult->pDataSource->arg[2].atomIndex, pResult->pDataSource->arg[3].atomIndex, false );
		break;
	case MDTRA_DT_DIHEDRAL:
		flResultData = pPdbFile->get_dihedral( pResult->pDataSource->arg[0].atomIndex, pResult->pDataSource->arg[1].atomIndex, pResult->pDataSource->arg[2].atomIndex, pResult->pDataSource->arg[3].atomIndex, false );
		break;
	case MDTRA_DT_DIHEDRAL_ABS:
		flResultData = pPdbFile->get_dihedral( pResult->pDataSource->arg[0].atomIndex, pResult->pDataSource->arg[1].atomIndex, pResult->pDataSource->arg[2].atomIndex, pResult->pDataSource->arg[3].atomIndex, true );
		break;
	case MDTRA_DT_PLANEANGLE:
		flResultData = pPdbFile->get_angle_between_planes( pResult->pDataSource->arg[0].atomIndex, pResult->pDataSource->arg[1].atomIndex, pResult->pDataSource->arg[2].atomIndex, pResult->pDataSource->arg[3].atomIndex, pResult->pDataSource->arg[4].atomIndex, pResult->pDataSource->arg[5].atomIndex );
		break;
	case MDTRA_DT_FORCE:
		flResultData = pPdbFile->get_force( pResult->pDataSource->arg[0].atomIndex );
		break;
	case MDTRA_DT_RESULTANT_FORCE:
		flResultData = pPdbFile->get_resultant_force( pResult->pDataSource->arg[0].atomIndex, pResult->pDataSource->arg[1].atomIndex );
		break;
	case MDTRA_DT_SAS:
		pPdbFile->set_flag( 0, NULL, PDB_FLAG_SAS );
		flResultData = pPdbFile->get_sas( threadnum );
		break;
	case MDTRA_DT_SAS_SEL:
		pPdbFile->set_flag( pResult->pDataSource->selection.size, pResult->pDataSource->selection.data, PDB_FLAG_SAS );
		flResultData = pPdbFile->get_sas( threadnum );
		break;
	case MDTRA_DT_OCCA:
		pPdbFile->set_flag( 0, NULL, PDB_FLAG_SAS );
		pPdbFile->set_flag( pResult->pDataSource->selection2.size, pResult->pDataSource->selection2.data, PDB_FLAG_OCCLUDER );
		flResultData = pPdbFile->get_occa( threadnum );
		break;
	case MDTRA_DT_OCCA_SEL:
		pPdbFile->set_flag( pResult->pDataSource->selection.size, pResult->pDataSource->selection.data, PDB_FLAG_SAS );
		pPdbFile->set_flag( pResult->pDataSource->selection2.size, pResult->pDataSource->selection2.data, PDB_FLAG_OCCLUDER );
		flResultData = pPdbFile->get_occa( threadnum );
		break;
	case MDTRA_DT_USER:
		{
			MDTRA_Program_Interpreter* pInterpreter = (MDTRA_Program_Interpreter*)pResult->pProgInterpreter;
			if (pInterpreter && pInterpreter->IsValid()) {
				pInterpreter->Main( threadnum, num, pPdbFile, &flResultData );
			} else 
				flResultData = 0.0f;
		}
		break;
	default:
		break;
	}

	//apply unit scale
	switch (pResult->pResult->units) {
	case MDTRA_YSU_ANGSTROMS:
		//distance is in angstroms by default
		break;
	case MDTRA_YSU_NANOMETERS:
		flResultData *= 0.1f;
		break;
	case MDTRA_YSU_RADIANS:
		//angle is in radians by default
		break;
	case MDTRA_YSU_DEGREES:
		flResultData = UTIL_rad2deg( flResultData );
		break;
	case MDTRA_YSU_KCALOVERA:
		//force is in kcal/A by default
		break;
	case MDTRA_YSU_MKNEWTON:
		flResultData *= 0.4184f;
		break;
	case MDTRA_YSU_SQANGSTROMS:
		//area is in sq. A by default
		break;
	case MDTRA_YSU_SQNANOMETERS:
		flResultData *= 0.01f;
		break;
	default:
		break;
	}

	//apply reference scale and bias
	flResultData = flResultData * pResult->pDSRef->yscale + pResult->pDSRef->bias;

	//check if we have negative
	if (flResultData <= 0.0f)
		pResult->threadAllPositive[threadnum] = false;
	if (flResultData == 0.0f)
		pResult->threadHasZero[threadnum] = true;

	//increment some statistic parameters
	pResult->threadStat[MDTRA_TSP_ARITHMETIC_MEAN][threadnum] += flResultData;
	pResult->threadStat[MDTRA_TSP_HARMONIC_MEAN][threadnum] += (1.0f / flResultData);
	pResult->threadStat[MDTRA_TSP_QUADRATIC_MEAN][threadnum] += (flResultData*flResultData);
	
	if (pResult->threadStatInit[threadnum]) {
		pResult->threadStat[MDTRA_TSP_GEOMETRIC_MEAN][threadnum] *= flResultData;
		if (flResultData > pResult->threadStat[MDTRA_SP_MAX_VALUE][threadnum]) 
			pResult->threadStat[MDTRA_TSP_MAX_VALUE][threadnum] = flResultData;
		if (flResultData < pResult->threadStat[MDTRA_SP_MIN_VALUE][threadnum]) 
			pResult->threadStat[MDTRA_TSP_MIN_VALUE][threadnum] = flResultData;
	} else {
		pResult->threadStat[MDTRA_TSP_GEOMETRIC_MEAN][threadnum] = flResultData;
		pResult->threadStat[MDTRA_TSP_MAX_VALUE][threadnum] = flResultData;
		pResult->threadStat[MDTRA_TSP_MIN_VALUE][threadnum] = flResultData;
		pResult->threadStatInit[threadnum] = true;
	}

	//write time-based output
	pResult->pDSRef->pData[num] = flResultData;
	if (num >= pResult->pDSRef->iActualDataSize)
		pResult->pDSRef->iActualDataSize = num + 1;
}

static void fn_BuildStreamData( int threadnum, int num )
{
	//this function handles a single stream file
	//num = stream file index in stream's file list
	//this function MUST be thread-safe

	//Load PDB file
	MDTRA_PDB_File *pPdbFile;
	bool bAligned;
	bool bLoadFailed = false;
	if (num == 0) {
		pPdbFile = pLocalStreamWork->pStream->pdb;
		bAligned = true;
	} else {
		pPdbFile = pLocalStreamWork->tempPDB[threadnum];
		if (!pPdbFile->load( threadnum, pLocalStreamWork->pStream->format_identifier, pLocalStreamWork->pStream->files.at(num).toAscii(), pLocalStreamWork->pStream->flags) )
			bLoadFailed = true;
		bAligned = false;
	}

#ifdef _DEBUG
	OutputDebugString( QString("Using: %1\n").arg(pLocalStreamWork->pStream->files.at(num)).toAscii() );
#endif

	//Get all results
	int iNumResults = pLocalStreamWork->pResults.count();
	for (int i = 0; i < iNumResults; i++) {
		MDTRA_StreamWorkResult *pResult = const_cast<MDTRA_StreamWorkResult*>(&pLocalStreamWork->pResults.at(i));

		switch (pResult->pResult->layout) {
		default:
		case MDTRA_LAYOUT_TIME:
			if (bLoadFailed)
				pResult->pDSRef->pData[num] = 0.0f;
			else
				fn_BuildStreamData_TimeBased( pResult, pPdbFile, bAligned, threadnum, num );
			break;
		case MDTRA_LAYOUT_RESIDUE:
			if (!bLoadFailed)
				fn_BuildStreamData_ResidueBased( pResult, pPdbFile, bAligned, threadnum, num, pLocalStreamWork->pStream->files.count() );
			break;
		}
	}

	if (pProgressDialog) {
		pProgressDialog->advanceCurrentFile( pLocalStreamWork->workBase+num+1 );
		if (pProgressDialog->checkInterrupt()) {
			InterruptThreads();
		}
	}
}

static void fn_BuildStreamAverage( int threadnum, int num )
{
	//this function handles a single stream file
	//num = stream file index in stream's file list
	//this function MUST be thread-safe

	//Load PDB file
	MDTRA_PDB_File *pPdbFile;
	bool bAligned;
	bool bLoadFailed = false;
	if (num == 0) {
		pPdbFile = pLocalStreamWork->pStream->pdb;
		bAligned = true;
	} else {
		pPdbFile = pLocalStreamWork->tempPDB[threadnum];
		if (!pPdbFile->load( threadnum, pLocalStreamWork->pStream->format_identifier, pLocalStreamWork->pStream->files.at(num).toAscii(), pLocalStreamWork->pStream->flags ) )
			bLoadFailed = true;
		bAligned = false;
	}

#ifdef _DEBUG
	OutputDebugString( QString("Averaging: %1\n").arg(pLocalStreamWork->pStream->files.at(num)).toAscii() );
#endif

	if (!bAligned) {
		//align PDB file with first file in stream
		pPdbFile->move_to_centroid();
		pPdbFile->align_kabsch( pLocalStreamWork->pStream->pdb );
		bAligned = true;
	}

	//make average
	ThreadLock();
	pLocalStreamWork->averagePDB->average_coords( pPdbFile, 1.0f / (float)pLocalStreamWork->workCount );
	ThreadUnlock();

	if (pProgressDialog) {
		pProgressDialog->advanceCurrentFile( pLocalStreamWork->workBase+num+1 );
		if (pProgressDialog->checkInterrupt()) {
			InterruptThreads();
		}
	}
}

static void fn_FinalizeRMSF( MDTRA_StreamWork *pStreamWork, MDTRA_StreamWorkResult *pWorkResult, int atomFlags )
{
	int iRSN = -1;
	int iLastValidRSN = 0;
	int iNumAtoms = 0;
	float *pflInput = pWorkResult->pDSRef->pData;

	for ( int i = 0; i < pWorkResult->pDSRef->iActualDataSize; i++, pflInput++ ) {
		const MDTRA_PDB_Atom *pAt = pStreamWork->averagePDB->fetchAtomByIndex( i );
		assert( pAt != NULL );

		// check if changing residue
		if ( pAt->residueserial != iRSN ) {
			if ( iRSN > 0 ) {
				for ( int j = iRSN+1; j < pAt->residueserial; j++ )
					pWorkResult->pDSRef->pData[j-1] = 0;
				if ( iNumAtoms > 0 ) {
					pWorkResult->pDSRef->pData[iRSN-1] *= 1.0f / (float)iNumAtoms;
					iLastValidRSN = iRSN;
				} else {
					pWorkResult->pDSRef->pData[iRSN-1] = 0.0f;
				}
			}
			iRSN = pAt->residueserial;
			if ( i != iRSN-1 )
				pWorkResult->pDSRef->pData[iRSN-1] = 0;
			iNumAtoms = 0;
		}

		if ( pAt->atomFlags & atomFlags ) {
			if ( i != iRSN-1 )
				pWorkResult->pDSRef->pData[iRSN-1] += sqrtf( *pflInput );
			else
				pWorkResult->pDSRef->pData[iRSN-1] = sqrtf( pWorkResult->pDSRef->pData[iRSN-1] );
			iNumAtoms++;
		}
	}

	if ( iRSN > 0 ) {
		if ( iNumAtoms > 0 ) {
			pWorkResult->pDSRef->pData[iRSN-1] *= 1.0f / (float)iNumAtoms;
			iLastValidRSN = iRSN;
		} else {
			pWorkResult->pDSRef->pData[iRSN-1] = 0.0f;
		}

		for ( int i = iRSN; i < pStreamWork->averagePDB->getResidueCount(); i++ ) {
			pWorkResult->pDSRef->pData[i] = 0;
		}
	}

	pWorkResult->pDSRef->iDataSize = pStreamWork->averagePDB->getResidueCount();
	pWorkResult->pDSRef->iActualDataSize = iLastValidRSN;
}


static const QList<MDTRA_Result> *pLocalResultList = NULL;

static void fn_BuildCorrelationTable( int threadnum, int num )
{
	MDTRA_Result *pResult = const_cast<MDTRA_Result*>(&pLocalResultList->at(num));
	if (!pResult->buildCorrelation || !pResult->sourceList.count())
		return;
	if (!pResult->sourceList.at(0).pCorrelation)
		return;

	int iNumSrc = pResult->sourceList.count();
	for (int i = 0; i < iNumSrc; i++) {
		MDTRA_DSRef *pRef = const_cast<MDTRA_DSRef*>(&pResult->sourceList.at(i));
		float flDataSize = pRef->iActualDataSize;
		float flSigma_i = (pRef->stat[MDTRA_SP_QUADRATIC_MEAN]*pRef->stat[MDTRA_SP_QUADRATIC_MEAN] - pRef->stat[MDTRA_SP_ARITHMETIC_MEAN]*pRef->stat[MDTRA_SP_ARITHMETIC_MEAN]);

		for (int j = 0; j < iNumSrc; j++) {
			MDTRA_DSRef *pRef2 = const_cast<MDTRA_DSRef*>(&pResult->sourceList.at(j));
			if (pRef->iActualDataSize != pRef2->iActualDataSize) {
				continue;
			}
			if (i == j) {
				continue;
			} else if (i > j) {
				pRef->pCorrelation[j] = pRef2->pCorrelation[i];
				continue;
			}

			//calculate covariation
			float flCov = 0.0f;
			for (int k = 0; k < pRef->iActualDataSize; k++) {
				flCov += pRef->pData[k]*pRef2->pData[k];
			}
			flCov /= flDataSize;
			flCov -= pRef->stat[MDTRA_SP_ARITHMETIC_MEAN]*pRef2->stat[MDTRA_SP_ARITHMETIC_MEAN];

			//calculate dispersion
			float flSigma_j = (pRef2->stat[MDTRA_SP_QUADRATIC_MEAN]*pRef2->stat[MDTRA_SP_QUADRATIC_MEAN] - pRef2->stat[MDTRA_SP_ARITHMETIC_MEAN]*pRef2->stat[MDTRA_SP_ARITHMETIC_MEAN]);
			
			//calculate correlation
			pRef->pCorrelation[j] = flCov / sqrtf(flSigma_i * flSigma_j);
		}
	}

	if (pProgressDialog) {
		if (pProgressDialog->checkInterrupt()) {
			InterruptThreads();
		}
	}
}

void MDTRA_Project :: profileStart()
{
#if defined(WIN32)
	m_profStart = timeGetTime();
#elif defined(LINUX)
	struct timeval tp;
	gettimeofday(&tp, NULL);
	m_profStart = tp.tv_sec*1000 + tp.tv_usec/1000;
#endif
}

void MDTRA_Project :: profileEnd()
{
	dword totalTime;
#if defined(WIN32)
	totalTime = timeGetTime() - m_profStart;
#elif defined(LINUX)
	struct timeval tp;
	gettimeofday(&tp, NULL);
	totalTime = tp.tv_sec*1000 + tp.tv_usec/1000 - m_profStart;
#endif
	QMessageBox::information( pProgressDialog, QObject::tr("Profiler"), QString("Stream build time: %1 ms").arg(totalTime) );
}

bool MDTRA_Project :: build( bool rebuildAll )
{
	QList<MDTRA_StreamWork> streamWorkList;
	QList<float> dataList;
	MDTRA_StreamWork streamWork;
	MDTRA_StreamWorkResult streamWorkResult;
	int worksize = 0;
	int calcSAS = 0;
	bool profiling = m_pMainWindow->allowProfiling();

	//Collect work per stream
	for (int i = 0; i < m_StreamList.count(); i++) {
		streamWork.pStream = &m_StreamList.at(i);
		if (!streamWork.pStream->pdb)
			continue;

		streamWork.workCount = streamWork.pStream->files.count();
		streamWork.pResults.clear();
		streamWork.averagePDB = NULL;
		for (int j = 0; j < CountThreads(); j++) {
			streamWork.tempPDB[j] = new MDTRA_PDB_File;
		}

		int iAllocateFloats = 0;
		int calcRMSF = 0;

		for (int j = 0; j < m_ResultList.count(); j++) {
			MDTRA_Result* pCurrentResult = const_cast<MDTRA_Result*>(&m_ResultList.at(j));
			if (!rebuildAll && pCurrentResult->status) {
				pCurrentResult->buildCorrelation = false;
				continue;
			}
			int localDataSize;
			int localDataCount;
			switch (pCurrentResult->layout) {
			default:
			case MDTRA_LAYOUT_TIME: 
				localDataCount = streamWork.workCount;
				localDataSize = streamWork.workCount;
				break;
			case MDTRA_LAYOUT_RESIDUE:
				if ( pCurrentResult->type == MDTRA_DT_RMSF || pCurrentResult->type == MDTRA_DT_RMSF_SEL ) {
					localDataCount = streamWork.pStream->pdb->getAtomCount();
					localDataSize = localDataCount * CountThreads();
				} else {
					localDataCount = streamWork.pStream->pdb->getResidueCount();
					localDataSize = localDataCount * CountThreads();
				}
				if (localDataCount > iAllocateFloats) iAllocateFloats = localDataCount;
				break;
			}

			pCurrentResult->buildCorrelation = true;
			for (int k = 0; k < pCurrentResult->sourceList.count(); k++) {
				MDTRA_DSRef* pRef = const_cast<MDTRA_DSRef*>(&pCurrentResult->sourceList.at(k));
				const MDTRA_DataSource *pDS = fetchDataSourceByIndex( pRef->dataSourceIndex );
				if (pDS->streamIndex == streamWork.pStream->index) {
					//this is our work
					memset( pRef->stat, 0, sizeof(pRef->stat) );
					if (pRef->pData) {
						UTIL_AlignedFree( pRef->pData );
						pRef->pData = NULL;
					}

					MDTRA_Program_Interpreter *pInterpreter = NULL;
					if (pDS->type == MDTRA_DT_USER) {
						pInterpreter = new MDTRA_Program_Interpreter( pDS );
						if ( !pInterpreter->Init( streamWork.pStream->pdb, localDataCount ) ) {
							delete pInterpreter;
							continue;
						}
					}

					if (pDS->type == MDTRA_DT_SAS || 
						pDS->type == MDTRA_DT_SAS_SEL || 
						pDS->type == MDTRA_DT_OCCA ||
						pDS->type == MDTRA_DT_OCCA_SEL ||
						pDS->type == MDTRA_DT_USER)
						calcSAS = 1;

					if (pDS->type == MDTRA_DT_RMSF || 
						pDS->type == MDTRA_DT_RMSF_SEL)
						calcRMSF = 1;
					
					pRef->xscale = streamWork.pStream->xscale;
					pRef->iDataSize = localDataCount;
					pRef->iActualDataSize = 0;
					worksize += localDataSize;
					pRef->pData = (float*)UTIL_AlignedMalloc( localDataSize * sizeof(float) );
					memset( pRef->pData, 0, sizeof(float)*localDataSize );
	
					streamWorkResult.pDataSource = pDS;
					streamWorkResult.pResult = const_cast<MDTRA_Result*>(&m_ResultList.at(j));
					streamWorkResult.pDSRef = pRef;
					streamWorkResult.pRefPDB = NULL;
					streamWorkResult.pProgInterpreter = pInterpreter;

					if (pDS->type == MDTRA_DT_RMSD_SEL) {
						streamWorkResult.pRefPDB = new MDTRA_PDB_File;
						if (!streamWorkResult.pRefPDB->load( 0, m_StreamList.at(i).format_identifier, m_StreamList.at(i).files.at(0).toAscii(), m_StreamList.at(i).flags )) {
							delete streamWorkResult.pRefPDB;
							streamWorkResult.pRefPDB = NULL;
						} else {
							streamWorkResult.pRefPDB->set_flag(pDS->selection.size, pDS->selection.data, PDB_FLAG_RMSD);
							streamWorkResult.pRefPDB->move_to_centroid2();
						}
					}
					else if (pDS->type == MDTRA_DT_RMSF_SEL) {
						streamWorkResult.pRefPDB = new MDTRA_PDB_File;
						if (!streamWorkResult.pRefPDB->load( 0, m_StreamList.at(i).format_identifier, m_StreamList.at(i).files.at(0).toAscii(), m_StreamList.at(i).flags )) {
							delete streamWorkResult.pRefPDB;
							streamWorkResult.pRefPDB = NULL;
						} else {
							streamWorkResult.pRefPDB->set_flag(pDS->selection.size, pDS->selection.data, PDB_FLAG_RMSF);
							streamWorkResult.pRefPDB->move_to_centroid2();
						}
					}

					memset( streamWorkResult.threadStatInit, 0, sizeof(streamWorkResult.threadStatInit) );
					memset( streamWorkResult.threadStat, 0, sizeof(streamWorkResult.threadStat) );
					memset( streamWorkResult.threadHasZero, 0, sizeof(streamWorkResult.threadHasZero) );
					for (int c = 0; c < MDTRA_MAX_THREADS; c++)
						streamWorkResult.threadAllPositive[c] = true;
					streamWork.pResults << streamWorkResult;
				}
			}
		}

		if (iAllocateFloats > 0) {
			streamWork.pStream->pdb->alloc_floats( iAllocateFloats );
			for (int j = 0; j < CountThreads(); j++) {
				streamWork.tempPDB[j]->alloc_floats( iAllocateFloats );
			}
		}

		if ( calcRMSF ) {
			streamWork.averagePDB = new MDTRA_PDB_File;
			streamWork.averagePDB->load( streamWork.pStream->pdb );
			streamWork.averagePDB->clear_coords();
#ifdef _DEBUG
			OutputDebugString( QString("Stream %1 requires averaging pass\n").arg(streamWork.pStream->index).toAscii() );
#endif
		}

		if (streamWork.pResults.count() > 0)
			streamWorkList << streamWork;
	}

	if (!worksize)
		return false;

	worksize *= sizeof(float);
#ifdef _DEBUG
	OutputDebugString( QString("Stream work size: %1 kb\n").arg(worksize / 1024.0f).toAscii() );
#endif

	MDTRA_ProgressDialog dlgProgress( m_pMainWindow );
	dlgProgress.setStreamCount( streamWorkList.count() );
	dlgProgress.show();

	pProgressDialog = &dlgProgress;

	if ( calcSAS )
		MDTRA_InitSAS();

	//build each stream
	for (int i = 0; i < streamWorkList.count(); i++) {
		pLocalStreamWork = const_cast< MDTRA_StreamWork*>(&streamWorkList.at(i));
#ifdef _DEBUG
		OutputDebugString( QString("pLocalStreamWork: %1 results (stream %2)\n").arg(pLocalStreamWork->pResults.count()).arg(pLocalStreamWork->pStream->index).toAscii() );
#endif
		if ( pLocalStreamWork->averagePDB ) {
			dlgProgress.setFileCount( pLocalStreamWork->workCount * 2 );
		} else {
			dlgProgress.setFileCount( pLocalStreamWork->workCount );
		}
		dlgProgress.setCurrentStream( i );
		dlgProgress.setCurrentFile( 0 );

		if (profiling)
			profileStart();

		pLocalStreamWork->workBase = 0;

		if ( pLocalStreamWork->averagePDB ) {
			RunThreadsOnIndividual( pLocalStreamWork->workCount, fn_BuildStreamAverage );
			if (dlgProgress.checkInterrupt())
				break;
			pLocalStreamWork->workBase = pLocalStreamWork->workCount;
			pLocalStreamWork->averagePDB->finalize_coords();
		} 

		RunThreadsOnIndividual( pLocalStreamWork->workCount, fn_BuildStreamData );

		if (profiling)
			profileEnd();

		for (int j = 0; j < pLocalStreamWork->pResults.count(); j++) {
			MDTRA_StreamWorkResult *pWorkResult = const_cast<MDTRA_StreamWorkResult*>(&pLocalStreamWork->pResults.at(j));

			bool geomMeanValid = true;
			bool harmMeanValid = true;

			if (pWorkResult->pResult->layout == MDTRA_LAYOUT_RESIDUE) {
				//collapse thread results for residue-based layout
				for (int k = 1; k < CountThreads(); k++) {
					for (int l = 0; l < pWorkResult->pDSRef->iActualDataSize; l++) {
						pWorkResult->pDSRef->pData[l] += pWorkResult->pDSRef->pData[pWorkResult->pDSRef->iDataSize*k+l];
					}
				}

				if (pWorkResult->pResult->type == MDTRA_DT_RMSF || pWorkResult->pResult->type == MDTRA_DT_RMSF_SEL) {
					//perform RMSF finalization
					int finalFlags;
					if ( pWorkResult->pResult->type == MDTRA_DT_RMSF ) {
						finalFlags = PDB_FLAG_BACKBONE;
					} else {
						finalFlags = PDB_FLAG_RMSF;
						pLocalStreamWork->averagePDB->set_flag( pWorkResult->pDataSource->selection.size, pWorkResult->pDataSource->selection.data, finalFlags );
					}
					fn_FinalizeRMSF( pLocalStreamWork, pWorkResult, finalFlags );
					pWorkResult->pDSRef->stat[MDTRA_SP_ARITHMETIC_MEAN] = 0.0f;
				} else {
					//finalize statistic parameters
					for (int k = 0; k < CountThreads(); k++) {
						pWorkResult->pDSRef->stat[MDTRA_SP_ARITHMETIC_MEAN] += pWorkResult->threadStat[MDTRA_TSP_ARITHMETIC_MEAN][k];
					}
				}

				//build statistic parameters
				for (int k = 0; k < pWorkResult->pDSRef->iActualDataSize; k++) {
					if (pWorkResult->pResult->type == MDTRA_DT_RMSF || pWorkResult->pResult->type == MDTRA_DT_RMSF_SEL) {
						pWorkResult->pDSRef->stat[MDTRA_SP_ARITHMETIC_MEAN] += pWorkResult->pDSRef->pData[k];
					}
					pWorkResult->pDSRef->stat[MDTRA_SP_HARMONIC_MEAN] += 1.0f / pWorkResult->pDSRef->pData[k];
					pWorkResult->pDSRef->stat[MDTRA_SP_QUADRATIC_MEAN] += pWorkResult->pDSRef->pData[k]*pWorkResult->pDSRef->pData[k];
					if (k == 0) {
						pWorkResult->pDSRef->stat[MDTRA_SP_GEOMETRIC_MEAN] = pWorkResult->pDSRef->pData[k];
						pWorkResult->pDSRef->stat[MDTRA_SP_MIN_VALUE] = pWorkResult->pDSRef->pData[k];
						pWorkResult->pDSRef->stat[MDTRA_SP_MAX_VALUE] = pWorkResult->pDSRef->pData[k];
					} else {
						pWorkResult->pDSRef->stat[MDTRA_SP_GEOMETRIC_MEAN] *= pWorkResult->pDSRef->pData[k];
						if (pWorkResult->pDSRef->pData[k] < pWorkResult->pDSRef->stat[MDTRA_SP_MIN_VALUE] ) pWorkResult->pDSRef->stat[MDTRA_SP_MIN_VALUE] = pWorkResult->pDSRef->pData[k];
						if (pWorkResult->pDSRef->pData[k] > pWorkResult->pDSRef->stat[MDTRA_SP_MAX_VALUE] ) pWorkResult->pDSRef->stat[MDTRA_SP_MAX_VALUE] = pWorkResult->pDSRef->pData[k];
					}
					if (pWorkResult->pDSRef->pData[k] <= 0.0f) geomMeanValid = false;
					if (pWorkResult->pDSRef->pData[k] == 0.0f) harmMeanValid = false;
				}
			} else {
				//finalize statistic parameters
				for (int k = 0; k < CountThreads(); k++) {
					pWorkResult->pDSRef->stat[MDTRA_SP_ARITHMETIC_MEAN] += pWorkResult->threadStat[MDTRA_TSP_ARITHMETIC_MEAN][k];
					pWorkResult->pDSRef->stat[MDTRA_SP_HARMONIC_MEAN] += pWorkResult->threadStat[MDTRA_TSP_HARMONIC_MEAN][k];
					pWorkResult->pDSRef->stat[MDTRA_SP_QUADRATIC_MEAN] += pWorkResult->threadStat[MDTRA_TSP_QUADRATIC_MEAN][k];
					if (k == 0) {
						pWorkResult->pDSRef->stat[MDTRA_SP_GEOMETRIC_MEAN] = pWorkResult->threadStat[MDTRA_TSP_GEOMETRIC_MEAN][k];
						pWorkResult->pDSRef->stat[MDTRA_SP_MIN_VALUE] = pWorkResult->threadStat[MDTRA_TSP_MIN_VALUE][k];
						pWorkResult->pDSRef->stat[MDTRA_SP_MAX_VALUE] = pWorkResult->threadStat[MDTRA_TSP_MAX_VALUE][k];
					} else {
						pWorkResult->pDSRef->stat[MDTRA_SP_GEOMETRIC_MEAN] *= pWorkResult->threadStat[MDTRA_TSP_GEOMETRIC_MEAN][k];
						if (pWorkResult->threadStat[MDTRA_TSP_MIN_VALUE][k] < pWorkResult->pDSRef->stat[MDTRA_SP_MIN_VALUE] ) pWorkResult->pDSRef->stat[MDTRA_SP_MIN_VALUE] = pWorkResult->threadStat[MDTRA_TSP_MIN_VALUE][k];
						if (pWorkResult->threadStat[MDTRA_TSP_MAX_VALUE][k] > pWorkResult->pDSRef->stat[MDTRA_SP_MAX_VALUE] ) pWorkResult->pDSRef->stat[MDTRA_SP_MAX_VALUE] = pWorkResult->threadStat[MDTRA_TSP_MAX_VALUE][k];
					}
					if (!pWorkResult->threadAllPositive[k])
						geomMeanValid = false;
					if (pWorkResult->threadHasZero[k])
						harmMeanValid = false;
				}
			}

			float flDataSize = pWorkResult->pDSRef->iActualDataSize;
	
			pWorkResult->pDSRef->stat[MDTRA_SP_ARITHMETIC_MEAN] /= flDataSize;
			pWorkResult->pDSRef->stat[MDTRA_SP_VARIANCE] = (flDataSize / (flDataSize-1.0f)) * ((pWorkResult->pDSRef->stat[MDTRA_SP_QUADRATIC_MEAN] / flDataSize) - (pWorkResult->pDSRef->stat[MDTRA_SP_ARITHMETIC_MEAN]*pWorkResult->pDSRef->stat[MDTRA_SP_ARITHMETIC_MEAN]));
			pWorkResult->pDSRef->stat[MDTRA_SP_STDDEV] = sqrtf(pWorkResult->pDSRef->stat[MDTRA_SP_VARIANCE]);
			pWorkResult->pDSRef->stat[MDTRA_SP_STDERR] = pWorkResult->pDSRef->stat[MDTRA_SP_STDDEV] / sqrtf(flDataSize);
			pWorkResult->pDSRef->stat[MDTRA_SP_GEOMETRIC_MEAN] = (geomMeanValid ? pow( pWorkResult->pDSRef->stat[MDTRA_SP_GEOMETRIC_MEAN], 1.0f / flDataSize ) : (-FLT_MAX));
			pWorkResult->pDSRef->stat[MDTRA_SP_HARMONIC_MEAN] = (harmMeanValid ? (flDataSize / pWorkResult->pDSRef->stat[MDTRA_SP_HARMONIC_MEAN]) : (-FLT_MAX));
			pWorkResult->pDSRef->stat[MDTRA_SP_QUADRATIC_MEAN] = sqrtf( pWorkResult->pDSRef->stat[MDTRA_SP_QUADRATIC_MEAN] / flDataSize );
			pWorkResult->pDSRef->stat[MDTRA_SP_RANGE] = pWorkResult->pDSRef->stat[MDTRA_SP_MAX_VALUE] - pWorkResult->pDSRef->stat[MDTRA_SP_MIN_VALUE];
			pWorkResult->pDSRef->stat[MDTRA_SP_MIDRANGE] = 0.5f * pWorkResult->pDSRef->stat[MDTRA_SP_RANGE];

			if (pWorkResult->pDSRef->iActualDataSize > 1) {
				//sort data to get median
				for (int k = 0; k < pWorkResult->pDSRef->iActualDataSize; k++) {
					dataList.append( pWorkResult->pDSRef->pData[k] );
				}
				qSort(dataList);
				if (pWorkResult->pDSRef->iActualDataSize % 2) {
					//odd
					int midValue = (pWorkResult->pDSRef->iActualDataSize - 1) >> 1;
					pWorkResult->pDSRef->stat[MDTRA_SP_MEDIAN] = dataList.at(midValue);
				} else {
					//even
					int midValue = pWorkResult->pDSRef->iActualDataSize >> 1;
					pWorkResult->pDSRef->stat[MDTRA_SP_MEDIAN] = (dataList.at(midValue) + dataList.at(midValue-1)) * 0.5f;
				}
				dataList.clear();
			} else {
				pWorkResult->pDSRef->stat[MDTRA_SP_MEDIAN] = pWorkResult->pDSRef->pData[0];
			}

			//reduce user-defined parameters
			pWorkResult->pDSRef->flags &= ~(DSREF_STAT_MASK << DSREF_STAT_SHIFT);
			MDTRA_Program_Interpreter* pInterpreter = (MDTRA_Program_Interpreter*)pWorkResult->pProgInterpreter;
			if (pInterpreter && pInterpreter->IsValid() && pInterpreter->ShouldReduce()) {
				dword localmask = 0;
				if (pInterpreter->Reduce( pWorkResult->pDSRef->pData, &localmask, &pWorkResult->pDSRef->stat[MDTRA_SP_USER1] )) {
					pWorkResult->pDSRef->flags |= (localmask << DSREF_STAT_SHIFT);
					if (pInterpreter->IsDataModified())
						pWorkResult->pResult->buildCorrelation = false;
				}
			}

			//mark result as actual
			pWorkResult->pResult->status = 1;
		}

		if (dlgProgress.checkInterrupt())
			break;
	}

	pProgressDialog = NULL;

	dlgProgress.setProgressAtMax();

	//build correlation table
	int iNumSrc = m_ResultList.count();
	for (int i = 0; i < iNumSrc; i++) {
		const MDTRA_Result *pResult = &m_ResultList.at(i);
		if (!pResult->buildCorrelation)
			continue;

		int iNumSrc2 = pResult->sourceList.count();
		for (int j = 0; j < iNumSrc2; j++) {
			MDTRA_DSRef *pRef= const_cast<MDTRA_DSRef*>(&pResult->sourceList.at(j));
			if (pRef->pCorrelation) {
				UTIL_AlignedFree( pRef->pCorrelation );
				pRef->pCorrelation = NULL;
			}
			if (iNumSrc2 > 1) {
				pRef->pCorrelation = (float*)UTIL_AlignedMalloc( iNumSrc2 * sizeof(float) );
				for (int c = 0; c < iNumSrc2; c++)
					pRef->pCorrelation[c] = (-FLT_MAX);
			}
		}
	}
	pLocalResultList = &m_ResultList;
	RunThreadsOnIndividual( pLocalResultList->count(), fn_BuildCorrelationTable );
	pLocalResultList = NULL;

	//mark all streams built as actual and clear
	for (int i = 0; i < streamWorkList.count(); i++) {
		MDTRA_StreamWork *pWork = const_cast<MDTRA_StreamWork*>(&streamWorkList.at(i));
		pWork->pStream->pdb->free_floats();
		for (int j = 0; j < pWork->pResults.count(); j++) {
			if (pWork->pResults.at(j).pRefPDB)
				delete pWork->pResults.at(j).pRefPDB;
			if (pWork->pResults.at(j).pProgInterpreter) {
				MDTRA_Program_Interpreter* pInterpreter = (MDTRA_Program_Interpreter*)(pWork->pResults.at(j).pProgInterpreter);
				delete pInterpreter;
			}
		}
		pWork->pResults.clear();
		for (int j = 0; j < CountThreads(); j++)
			delete pWork->tempPDB[j];
		if (pWork->averagePDB)
			delete pWork->averagePDB;
	}
	streamWorkList.clear();

	updateResultList();

	dlgProgress.close();
	dataList.clear();
	return true;
}
