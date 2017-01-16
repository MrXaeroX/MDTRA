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
#ifndef MDTRA_COMPACT_PDB_H
#define MDTRA_COMPACT_PDB_H

typedef struct stMDTRA_Compact_PDB_Atom
{
	XMM_FLOAT		original_xyz[4];
	XMM_FLOAT		modified_xyz[4];
	unsigned int	atomFlags;
	int				serialnumber;
} MDTRA_Compact_PDB_Atom;

template<typename T> class MDTRA_SelectionSet;

class MDTRA_Compact_PDB_File
{
public:
	MDTRA_Compact_PDB_File();
	~MDTRA_Compact_PDB_File();

	bool load( int threadnum, unsigned int format, const char *filename, int streamFlags );
	void unload( void );
	void reset( void );

	int getAtomCount( void ) const { return m_iNumAtoms; }
	const MDTRA_Compact_PDB_Atom* fetchAtomBySerialNumber( int serialnumber ) const;

	void move_to_centroid( void );
	void align_kabsch( const MDTRA_Compact_PDB_File *pOther );
	void set_rmsd_flag( int iSelectionCount, const int *iSelectionData );

	float get_rmsd( const MDTRA_Compact_PDB_File *pOther ) const;
	float get_distance( int atIndex1, int atIndex2 ) const;

protected:
	void set_flags( void );
	bool read_atom( int threadnum, unsigned int format, const char *linebuf, MDTRA_Compact_PDB_Atom *pOut, int &residue_c );
	bool jacobi( float *matrix, float *d, float *v );
	void eigsrt( float *d, float *v );

private:
	int			m_iNumAtoms;
	int			m_iNumBackboneAtoms;
	int			m_iNumAlignedAtoms;
	MDTRA_Compact_PDB_Atom*	m_pAtoms;
};

#endif //MDTRA_COMPACT_PDB_H