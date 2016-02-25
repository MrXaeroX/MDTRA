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
#ifndef MDTRA_PDB_FORMAT_H
#define MDTRA_PDB_FORMAT_H

#define PDB_GENERIC_FORMAT		0
#define PDB_BIOPASED_FORMAT		1
#define PDB_USER_FORMAT			100

typedef enum
{
	PDB_FT_STRING = 0,
	PDB_FT_CHARACTER,
	PDB_FT_INTEGER,
	PDB_FT_FLOAT,
	PDB_FT_MAX,
} PDBFieldType_e;

typedef enum
{
	PDB_FS_SERIALNUMBER = 0,
	PDB_FS_RESIDUENUMBER,
	PDB_FS_ATOMTITLE,
	PDB_FS_RESIDUETITLE,
	PDB_FS_CHAIN,
	PDB_FS_COORD_X,
	PDB_FS_COORD_Y,
	PDB_FS_COORD_Z,
	PDB_FS_FORCE_X,
	PDB_FS_FORCE_Y,
	PDB_FS_FORCE_Z,
	PDB_FS_MAX,
} PDBFieldSense_e;


typedef struct PDBField_s
{
	PDBFieldType_e	type;
	int				start;
	int				size;
} PDBField_t;

typedef struct PDBFormat_s
{
	char			title[64];
	unsigned int	identifier;
	bool			editable;
	PDBField_t		fields[PDB_FS_MAX];
	struct PDBFormat_s *next;
} PDBFormat_t;

class MDTRA_PDBFormatManager
{
public:
	MDTRA_PDBFormatManager();
	~MDTRA_PDBFormatManager();

	void fillFormatCombo( void *pComboBox, unsigned int selectedFormat );
	void fillFormatList( void *pListBox, unsigned int selectedFormat, unsigned int defaultFormat );
	unsigned int registerFormat( const PDBFormat_t* pFormat );
	bool modifyFormat( const PDBFormat_t* pFormat, const char *title, const PDBField_t *pFields );
	void unregisterFormat( const PDBFormat_t* pFormat );
	void loadFormats( void );
	void saveFormats( void );

	unsigned int getFreeIdentifier( void ) { return m_iLastFreeIdentifier; }
	unsigned int getDefaultFormat( void ) { return m_iDefaultFormatIdentifier; }
	void setDefaultFormat( unsigned int format );
	const PDBFormat_t *fetchFormat( unsigned int identifier );
	bool parse( int threadnum, unsigned int format, const char *buffer, int *out_serialnumber, int *out_residuenumber, char *out_atomtitle, char *out_residuetitle, short *out_chain, float *out_coords, float *out_force );
	bool checkFormat( unsigned int identifier, PDBFieldSense_e fieldSense );

	void pushFormats( void );
	void popFormats( bool restore );

private:
	int parseInteger( const char *buffer, const PDBField_t *pField );
	float parseFloat( const char *buffer, const PDBField_t *pField );
	void parseString( const char *buffer, const PDBField_t *pField, char *out_string );

private:
	PDBFormat_t *m_pFormatList;
	const PDBFormat_t *m_pCachedFormat[MDTRA_MAX_THREADS];
	int m_iNumUserFormats;
	unsigned int m_iLastFreeIdentifier;
	unsigned int m_iCachedFormatIdentifier[MDTRA_MAX_THREADS];
	unsigned int m_iDefaultFormatIdentifier;

	PDBFormat_t *m_pStoredFormatList;
	unsigned int m_iStoredDefaultFormatIdentifier;
	int m_iStoredNumUserFormats;
};

extern MDTRA_PDBFormatManager g_PDBFormatManager;

#endif //MDTRA_PDB_FORMAT_H