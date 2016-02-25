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
//	Implementation of MDTRA_PDBFormatManager

#include "mdtra_main.h"
#include "mdtra_utils.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_pdb_format.h"

#include <QtCore/QDataStream>
#include <QtCore/QFileInfo>
#include <QtGui/QApplication>
#include <QtGui/QComboBox>
#include <QtGui/QListWidget>

PDBFormat_t g_DefaultFormat_Generic = 
{
	"Generic Format", PDB_GENERIC_FORMAT, false,
	{
		{ PDB_FT_INTEGER, 6 , 5 },		//PDB_FS_SERIALNUMBER
		{ PDB_FT_INTEGER, 22 , 4 },		//PDB_FS_RESIDUENUMBER
		{ PDB_FT_STRING, 12 , 4 },		//PDB_FS_ATOMTITLE
		{ PDB_FT_STRING, 17 , 3 },		//PDB_FS_RESIDUETITLE
		{ PDB_FT_CHARACTER, 21 , 1 },	//PDB_FS_CHAIN
		{ PDB_FT_FLOAT, 30 , 8 },		//PDB_FS_COORD_X
		{ PDB_FT_FLOAT, 38 , 8 },		//PDB_FS_COORD_Y
		{ PDB_FT_FLOAT, 46 , 8 },		//PDB_FS_COORD_Z
		{ PDB_FT_FLOAT, 0 , 0 },		//PDB_FS_FORCE_X
		{ PDB_FT_FLOAT, 0 , 0 },		//PDB_FS_FORCE_Y
		{ PDB_FT_FLOAT, 0 , 0 },		//PDB_FS_FORCE_Z
	}
};

PDBFormat_t g_DefaultFormat_BioPASED = 
{
	"BioPASED Format", PDB_BIOPASED_FORMAT, false,
	{
		{ PDB_FT_INTEGER, 6 , 5 },		//PDB_FS_SERIALNUMBER
		{ PDB_FT_INTEGER, 22 , 4 },		//PDB_FS_RESIDUENUMBER
		{ PDB_FT_STRING, 12 , 5 },		//PDB_FS_ATOMTITLE
		{ PDB_FT_STRING, 17 , 4 },		//PDB_FS_RESIDUETITLE
		{ PDB_FT_CHARACTER, 21 , 1 },	//PDB_FS_CHAIN
		{ PDB_FT_FLOAT, 30 , 8 },		//PDB_FS_COORD_X
		{ PDB_FT_FLOAT, 38 , 8 },		//PDB_FS_COORD_Y
		{ PDB_FT_FLOAT, 46 , 8 },		//PDB_FS_COORD_Z
		{ PDB_FT_FLOAT, 55 , 8 },		//PDB_FS_FORCE_X
		{ PDB_FT_FLOAT, 63 , 8 },		//PDB_FS_FORCE_Y
		{ PDB_FT_FLOAT, 71 , 8 },		//PDB_FS_FORCE_Z
	}
};

MDTRA_PDBFormatManager g_PDBFormatManager;

MDTRA_PDBFormatManager :: MDTRA_PDBFormatManager()
{
	m_pFormatList = NULL;
	m_iLastFreeIdentifier = PDB_USER_FORMAT;
	memset( m_iCachedFormatIdentifier, 0xFF, sizeof(m_iCachedFormatIdentifier) );
	memset( m_pCachedFormat, 0, sizeof(m_pCachedFormat) );
	m_iNumUserFormats = 0;
}

MDTRA_PDBFormatManager :: ~MDTRA_PDBFormatManager()
{
	PDBFormat_t *pFormat = m_pFormatList;
	PDBFormat_t *pTemp;
	while (pFormat) {
		pTemp = pFormat->next;
		delete pFormat;
		pFormat = pTemp;
	}
}

unsigned int MDTRA_PDBFormatManager :: registerFormat( const PDBFormat_t* pFormat )
{
	PDBFormat_t *pNewFormat = new PDBFormat_t;
	if (!pNewFormat) return 0xFFFFFFFF;

	memcpy( pNewFormat, pFormat, sizeof(PDBFormat_t) );
	if (pNewFormat->identifier == 0xFFFFFFFF) {
		m_iNumUserFormats++;
		pNewFormat->identifier = m_iLastFreeIdentifier;
		m_iLastFreeIdentifier++;
	}
	pNewFormat->next = NULL;

	if (!m_pFormatList) {
		m_pFormatList = pNewFormat;
	} else {
		PDBFormat_t *pLast = m_pFormatList;
		while (pLast->next) 	pLast = pLast->next;
		pLast->next = pNewFormat;
	}
	return pNewFormat->identifier;
}

bool MDTRA_PDBFormatManager :: modifyFormat( const PDBFormat_t* pFormat, const char *title, const PDBField_t *pFields )
{
	//sanity check: don't modify default formats
	if (pFormat->identifier < PDB_USER_FORMAT)
		return false;

	PDBFormat_t *pCurFormat = m_pFormatList; 
	while (pCurFormat) {
		if (pCurFormat == pFormat) {
			strcpy_s( pCurFormat->title, title );
			memcpy( pCurFormat->fields, pFields, sizeof(pCurFormat->fields) );
			return true;
		}
		pCurFormat = pCurFormat->next;
	}

	return false;
}

void MDTRA_PDBFormatManager :: unregisterFormat( const PDBFormat_t* pFormat )
{
	//sanity check: don't unregister default formats
	if (pFormat->identifier < PDB_USER_FORMAT)
		return;

	PDBFormat_t *pCurFormat = m_pFormatList; 
	PDBFormat_t *pPrevFormat = NULL;

	while (pCurFormat) {
		if (pCurFormat == pFormat) {
			if (pPrevFormat) pPrevFormat->next = pCurFormat->next;
			else m_pFormatList = pCurFormat->next;
			delete pCurFormat;
			m_iNumUserFormats--;
			break;
		}
		pPrevFormat = pCurFormat;
		pCurFormat = pCurFormat->next;
	}


	for (int i = 0; i < MDTRA_MAX_THREADS; i++) {
		if (m_pCachedFormat[i] == pFormat) {
			m_iCachedFormatIdentifier[i] = 0xFFFFFFFF;
			m_pCachedFormat[i] = NULL;
		}
	}
}

void MDTRA_PDBFormatManager :: pushFormats( void )
{
	if (m_pStoredFormatList)
		return;

	m_iStoredDefaultFormatIdentifier = m_iDefaultFormatIdentifier;
	m_iStoredNumUserFormats = m_iNumUserFormats;
	m_pStoredFormatList = m_pFormatList;
	m_pFormatList = NULL;

	PDBFormat_t *pFormat = m_pStoredFormatList;
	while (pFormat) {
		registerFormat( pFormat );
		pFormat = pFormat->next;
	}

	memset( m_iCachedFormatIdentifier, 0xFF, sizeof(m_iCachedFormatIdentifier) );
	memset( m_pCachedFormat, 0, sizeof(m_pCachedFormat) );
}

void MDTRA_PDBFormatManager :: popFormats( bool restore )
{
	if (!m_pStoredFormatList)
		return;

	if (restore) {
		PDBFormat_t *pFormat = m_pFormatList;
		PDBFormat_t *pTemp;
		while (pFormat) {
			pTemp = pFormat->next;
			delete pFormat;
			pFormat = pTemp;
		}
		m_pFormatList = m_pStoredFormatList;
		m_iDefaultFormatIdentifier = m_iStoredDefaultFormatIdentifier;
		m_iNumUserFormats = m_iStoredNumUserFormats;
	} else {
		PDBFormat_t *pFormat = m_pStoredFormatList;
		PDBFormat_t *pTemp;
		while (pFormat) {
			pTemp = pFormat->next;
			delete pFormat;
			pFormat = pTemp;
		}
	}
	m_pStoredFormatList = NULL;
}

void MDTRA_PDBFormatManager :: loadFormats( void )
{
	//create default formats
	registerFormat( &g_DefaultFormat_Generic );
	registerFormat( &g_DefaultFormat_BioPASED );

	//load last free identifier from file
	//load user formats from file

	QFileInfo fi(g_pApp->applicationDirPath().append("/").append(CONFIG_DIRECTORY FORMATS_FILENAME));
	QFile f(g_pApp->applicationDirPath().append("/").append(CONFIG_DIRECTORY FORMATS_FILENAME));

	//Open file for reading
	if (!f.open(QFile::ReadOnly))
         return;

	QDataStream stream(&f);

	stream >> m_iLastFreeIdentifier;
	stream >> m_iNumUserFormats;

	for (int i = 0; i < m_iNumUserFormats; i++) {
		PDBFormat_t fmt;
		memset( &fmt, 0, sizeof(fmt) );
		fmt.editable = true;

		stream >> fmt.identifier;
		stream.readRawData( fmt.title, sizeof(fmt.title) );
		stream.readRawData( (char*)&fmt.fields[0], sizeof(fmt.fields) );
		registerFormat( &fmt );
	}
}

void MDTRA_PDBFormatManager :: saveFormats( void )
{
	//save last free identifier to file
	//save user formats to file

	QFileInfo fi(g_pApp->applicationDirPath().append("/").append(CONFIG_DIRECTORY FORMATS_FILENAME));
	QFile f(g_pApp->applicationDirPath().append("/").append(CONFIG_DIRECTORY FORMATS_FILENAME));

	//Open file for writing
	if (!f.open(QFile::WriteOnly | QFile::Truncate))
         return;

	QDataStream stream(&f);
	
	stream << m_iLastFreeIdentifier;
	stream << m_iNumUserFormats;

	PDBFormat_t *pFormat = m_pFormatList;
	while (pFormat) {
		if (pFormat->identifier >= PDB_USER_FORMAT) {
			stream << pFormat->identifier;
			stream.writeRawData( pFormat->title, sizeof(pFormat->title) );
			stream.writeRawData( (char*)&pFormat->fields[0], sizeof(pFormat->fields) );
		}
		pFormat = pFormat->next;
	}

	f.close();
}

const PDBFormat_t *MDTRA_PDBFormatManager :: fetchFormat( unsigned int identifier )
{
	PDBFormat_t *pFormat = m_pFormatList;
	while (pFormat) {
		if (pFormat->identifier == identifier)
			return pFormat;
		pFormat = pFormat->next;
	}
	return NULL;
}

bool MDTRA_PDBFormatManager :: checkFormat( unsigned int identifier, PDBFieldSense_e fieldSense )
{
	const PDBFormat_t *fmt = fetchFormat( identifier );
	if ( !fmt || !fmt->fields[fieldSense].size )
		return false;
	return true;
}

void MDTRA_PDBFormatManager :: setDefaultFormat( unsigned int format )
{
	m_iDefaultFormatIdentifier = format;
	if (!fetchFormat( format )) m_iDefaultFormatIdentifier = PDB_GENERIC_FORMAT;
}

int MDTRA_PDBFormatManager :: parseInteger( const char *buffer, const PDBField_t *pField ) 
{
	if (pField->size <= 0)	//not defined
		return 0;

	char localbuffer[81];
	memset(localbuffer, 0, sizeof(localbuffer));
	strncpy_s( localbuffer, buffer + pField->start, pField->size );

	switch (pField->type) {
	default:
	case PDB_FT_STRING:
	case PDB_FT_INTEGER:
		return UTIL_Atoi(localbuffer);
	case PDB_FT_CHARACTER:
		return localbuffer[0];
	case PDB_FT_FLOAT:
		return (int)UTIL_Atof(localbuffer);
	}
}

float MDTRA_PDBFormatManager :: parseFloat( const char *buffer, const PDBField_t *pField ) 
{
	if (pField->size <= 0)	//not defined
		return 0.0f;

	char localbuffer[81];
	memset(localbuffer, 0, sizeof(localbuffer));
	strncpy_s( localbuffer, buffer + pField->start, pField->size );

	switch (pField->type) {
	default:
	case PDB_FT_STRING:
	case PDB_FT_FLOAT:
		return (float)UTIL_Atof(localbuffer);
	case PDB_FT_CHARACTER:
		return (float)localbuffer[0];
	case PDB_FT_INTEGER:
		return (float)UTIL_Atoi(localbuffer);
	}
}

void MDTRA_PDBFormatManager :: parseString( const char *buffer, const PDBField_t *pField, char *out_string ) 
{
	if (pField->size <= 0)	//not defined
		out_string[0] = 0;
	else
		strncpy_s( out_string, pField->size+1, buffer + pField->start, pField->size );
}

bool MDTRA_PDBFormatManager :: parse( int threadnum, unsigned int format, const char *buffer, 
									  int *out_serialnumber, int *out_residuenumber, 
									  char *out_atomtitle, char *out_residuetitle, 
									  short *out_chain, float *out_coords, float *out_force )
{
	//parse line and extract data based on format def
	if (m_iCachedFormatIdentifier[threadnum] != format) {
		m_iCachedFormatIdentifier[threadnum] = format;
		m_pCachedFormat[threadnum] = fetchFormat( format );
	}

	if (!m_pCachedFormat[threadnum])
		return false;

	if (out_serialnumber) *out_serialnumber = parseInteger( buffer, &m_pCachedFormat[threadnum]->fields[PDB_FS_SERIALNUMBER] );
	if (out_residuenumber) *out_residuenumber = parseInteger( buffer, &m_pCachedFormat[threadnum]->fields[PDB_FS_RESIDUENUMBER] );
	if (out_chain) *out_chain = (short)parseInteger( buffer, &m_pCachedFormat[threadnum]->fields[PDB_FS_CHAIN] );
	if (out_atomtitle) parseString( buffer, &m_pCachedFormat[threadnum]->fields[PDB_FS_ATOMTITLE], out_atomtitle );
	if (out_residuetitle) parseString( buffer, &m_pCachedFormat[threadnum]->fields[PDB_FS_RESIDUETITLE], out_residuetitle );
	if (out_coords) {
		out_coords[0] = parseFloat( buffer, &m_pCachedFormat[threadnum]->fields[PDB_FS_COORD_X] );
		out_coords[1] = parseFloat( buffer, &m_pCachedFormat[threadnum]->fields[PDB_FS_COORD_Y] );
		out_coords[2] = parseFloat( buffer, &m_pCachedFormat[threadnum]->fields[PDB_FS_COORD_Z] );
	}
	if (out_force) {
		out_force[0] = parseFloat( buffer, &m_pCachedFormat[threadnum]->fields[PDB_FS_FORCE_X] );
		out_force[1] = parseFloat( buffer, &m_pCachedFormat[threadnum]->fields[PDB_FS_FORCE_Y] );
		out_force[2] = parseFloat( buffer, &m_pCachedFormat[threadnum]->fields[PDB_FS_FORCE_Z] );
	}

	return true;
}

void MDTRA_PDBFormatManager :: fillFormatCombo( void *pComboBox, unsigned int selectedFormat )
{
	QComboBox *pQtComboBox = (QComboBox*)pComboBox;
	pQtComboBox->clear();

	PDBFormat_t *pFormat = m_pFormatList;
	while (pFormat) {
		pQtComboBox->addItem( pFormat->title, pFormat->identifier );
		if ( pFormat->identifier == selectedFormat )
			pQtComboBox->setCurrentIndex( pQtComboBox->count() - 1 );
		pFormat = pFormat->next;
	}
}

void MDTRA_PDBFormatManager :: fillFormatList( void *pListBox, unsigned int selectedFormat, unsigned int defaultFormat )
{
	QListWidget *pQtListBox = (QListWidget*)pListBox;
	pQtListBox->clear();

	PDBFormat_t *pFormat = m_pFormatList;
	while (pFormat) {
		QListWidgetItem *pItem;

		if ( pFormat->identifier == defaultFormat )
			pItem = new QListWidgetItem( QString("%1 [DEFAULT]").arg(pFormat->title), pQtListBox );
		else
			pItem = new QListWidgetItem( QString("%1").arg(pFormat->title), pQtListBox );
			
		pItem->setData( Qt::UserRole, pFormat->identifier );

		if ( pFormat->identifier == selectedFormat )
			pQtListBox->setCurrentItem( pQtListBox->item( pQtListBox->count() - 1 ) );

		pFormat = pFormat->next;
	}
}