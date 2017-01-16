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
#ifndef MDTRA_PDB_FLAGS_H
#define MDTRA_PDB_FLAGS_H

#define PDB_FLAG_PROTEIN			(1<<0)
#define PDB_FLAG_NUCLEIC			(1<<1)
#define PDB_FLAG_BACKBONE			(1<<2)
#define PDB_FLAG_WATER				(1<<3)
#define PDB_FLAG_HYDROGEN			(1<<4)
#define PDB_FLAG_SELECTED			(1<<5)
#define PDB_FLAG_RMSD				(1<<6)
#define PDB_FLAG_RMSF				(1<<7)
#define PDB_FLAG_SAS				(1<<8)
#define PDB_FLAG_OCCLUDER			(1<<9)
#define PDB_FLAG_PCA				(1<<10)
#define PDB_FLAG_ION				(1<<11)


#endif //MDTRA_PDB_FLAGS_H