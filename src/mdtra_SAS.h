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
#ifndef MDTRA_SAS_H
#define MDTRA_SAS_H

extern void MDTRA_InitSAS( void );
extern void MDTRA_ShutdownSAS( void );
extern void MDTRA_SetSASParms( float probeRadius, int subdivisions, bool excludeWater );
extern void MDTRA_GetSASParms( float& probeRadius, int& subdivisions, bool& excludeWater );
extern float MDTRA_GetVdWRadius( const char* symbol );
extern float MDTRA_GetSASRadius( float vdwr );
extern float MDTRA_CalculateSAS( int threadnum, const struct stMDTRA_PDB_Atom *pAtoms, int numAtoms );
extern float MDTRA_CalculateOcclusion( int threadnum, const struct stMDTRA_PDB_Atom *pAtoms, int numAtoms );
extern void MDTRA_CalculateSASPerResidue( int threadnum, const struct stMDTRA_PDB_Atom *pAtoms, int numAtoms, float* pResidueSAS );
extern void MDTRA_CalculateOcclusionPerResidue( int threadnum, const struct stMDTRA_PDB_Atom *pAtoms, int numAtoms, float* pResidueOCC );

#endif //MDTRA_SAS_H