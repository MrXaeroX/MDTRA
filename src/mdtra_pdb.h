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
#ifndef MDTRA_PDB_H
#define MDTRA_PDB_H

typedef struct stMDTRA_SRFAtom
{
	const char*	atomTitle;
	float		origin[3];
} MDTRA_SRFAtom;

typedef struct stMDTRA_SRFDef
{
	const char*	residueTitle;
	int			numAtoms;
	MDTRA_SRFAtom* atomList;
} MDTRA_SRFDef;

typedef struct stMDTRA_PDB_Atom
{
	XMM_FLOAT	original_xyz[4];
	XMM_FLOAT	xyz[4];
	XMM_FLOAT	xyz2[4];
	float		force[3];
	float		vdwRadius;
	float		sasRadius;
	int			atomFlags;
	int			serialnumber;
	int			residuenumber;
	int			residueserial;
	short		chain;
	short		chainIndex;
	char		title[6];
	char		trimmed_title[6];
	char		symbol[3];
	char		residue[5];
	char		trimmed_residue[5];
} MDTRA_PDB_Atom;

template<typename T> class MDTRA_SelectionSet;

class MDTRA_PDB_File
{
public:
	MDTRA_PDB_File();
	MDTRA_PDB_File( int threadnum, unsigned int format, const char *filename, int streamFlags );
	~MDTRA_PDB_File();

	bool load( int threadnum, unsigned int format, const char *filename, int streamFlags );
	bool load( const MDTRA_PDB_File* pOther );
	bool save( const char *filename );
	void unload( void );
	void reset( void );
	void alloc_floats( int count );
	void free_floats( void );
	float *get_floats( void ) { return m_pTempFloats; }

	int getAtomCount( void ) const { return m_iNumAtoms; }
	int getResidueCount( void ) const { return m_iNumResidues; }
	int getFirstResidue( void ) const { return m_iFirstResidue; }
	const MDTRA_PDB_Atom* fetchAtomByIndex( int index ) const;
	const MDTRA_PDB_Atom* fetchAtomBySerialNumber( int serialnumber ) const;
	const MDTRA_PDB_Atom* fetchAtomByResidueSerial( int residueserial ) const;
	const MDTRA_PDB_Atom* fetchAtomByDesc( int chain, int resnum, const char *atname ) const;
	int fetchAtomIndexBySerialNumber( int serialnumber ) const;
	int fetchAtomIndexByDesc( int chain, int resnum, const char *atname ) const;

	void clear_coords( void );
	void average_coords( const MDTRA_PDB_File *pOther, float scale );
	void finalize_coords( void );
	void move_to_centroid( void );
	void move_to_centroid2( void );
	void align_kabsch( const MDTRA_PDB_File *pOther );
	void align_kabsch2( const MDTRA_PDB_File *pOther );
	void set_flag( int iSelectionCount, const int *iSelectionData, int flag );

	float get_rmsd( const MDTRA_PDB_File *pOther ) const;
	float get_rmsd2( const MDTRA_PDB_File *pOther ) const;
	float get_radius_of_gyration( void ) const;
	float get_distance( int atIndex1, int atIndex2 ) const;
	float get_angle( int atIndex1, int atIndex2, int atIndex3 ) const;
	float get_angle_between_sections( int atIndex1, int atIndex2, int atIndex3, int atIndex4 ) const;
	float get_torsion( int atIndex1, int atIndex2, int atIndex3, int atIndex4, bool bSigned ) const;
	float get_dihedral( int atIndex1, int atIndex2, int atIndex3, int atIndex4, bool absValue ) const;
	float get_angle_between_planes( int atIndex1, int atIndex2, int atIndex3, int atIndex4, int atIndex5, int atIndex6 ) const;
	float get_force( int atIndex1 ) const;
	float get_resultant_force( int atIndex1, int atIndex2 ) const;
	void  get_force_vector( int atIndex1, float *vecOut ) const;
	float get_sas( int threadnum ) const;
	float get_occa( int threadnum ) const;
	void  get_rmsd_of_residues( const MDTRA_PDB_File *pOther, float *pOutData ) const;
	void  get_rmsd2_of_residues( const MDTRA_PDB_File *pOther, float *pOutData ) const;
	void  get_rmsf_of_atoms( const MDTRA_PDB_File *pOther, float *pOutData ) const;
	void  get_rmsf2_of_atoms( const MDTRA_PDB_File *pOther, float *pOutData ) const;
	void  get_sas_of_residues( int threadnum, float *pOutData ) const;
	void  get_occa_of_residues( int threadnum, float *pOutData ) const;
	bool  get_residue_transform( int atIndex, float* pOutRotation, float* pOutTranslation ) const;
	float get_hbenergy( int atIndex1, int atIndex2 ) const;

	float get_selection_pair_distance( int atIndex1, int atIndex2 ) const;
	bool  is_selection_pair_of_same_residue( int atIndex1, int atIndex2 ) const;

	void selectElement( const char *s, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const;
	void selectAtomno( int atomno, int cmp, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const;
	void selectChain( int chain, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const;
	void selectAtom( const char *s, const char *res, int chain, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const;
	void selectResidue( const char *s, int num, int chain, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const;
	void selectResidueno( int min_i, int max_i, int chain, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const;
	void selectResiduenoAtom( const char *s, int min_i, int max_i, int chain, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const;
	void selectByFlags( int flags, MDTRA_SelectionSet<MDTRA_PDB_File> *pSet ) const;

protected:
	bool ensure_atom_buffer_size( void );
	bool read_atom( int threadnum, unsigned int format, const char *linebuf, MDTRA_PDB_Atom *pOut );
	void set_atom_flags( int residueFlags, MDTRA_PDB_Atom *pOut );
	void set_flags( void );
	bool jacobi3( float *matrix, float *d, float *v ) const;
	bool jacobi4( float *matrix, float *d, float *v ) const;
	void eigsrt3( float *d, float *v ) const;
	void eigsrt4( float *d, float *v ) const;
	const MDTRA_SRFDef* get_residue_SRFDef( const char *residueTitle ) const;

private:
	int					m_iNumAtoms;
	int					m_iNumBackboneAtoms;
	int					m_iNumLastFlaggedAtoms;
	int					m_iMaxAtoms;
	MDTRA_PDB_Atom*		m_pAtoms;
	float				m_vecCentroidOrigin[3];
	int					m_iNumResidues;
	int					m_iMaxResidues;
	int*				m_pResidues;
	float*				m_pTempFloats;
	int					m_iLastChainIndex;
	bool				m_bChainTerminator;
	int					m_iFirstResidue;
};

#endif //MDTRA_PDB_H