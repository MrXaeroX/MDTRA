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
#ifndef MDTRA_TYPES_H
#define MDTRA_TYPES_H

#if !defined(byte)
typedef unsigned char byte;
#endif
#if !defined(word)
typedef unsigned short word;
#endif
#if !defined(dword)
typedef unsigned int dword;
#endif
#if !defined(qword)
#if defined(WIN32)
typedef unsigned __int64 qword;
#else
typedef unsigned long long qword;
#endif
#endif

#define XMM_ALIGN_SIZE		16
#if defined(_MSC_VER)
#define XMM_ALIGN			_declspec(align(XMM_ALIGN_SIZE))
#elif defined(__GNUC__)
#define XMM_ALIGN			__attribute__((aligned(XMM_ALIGN_SIZE)))
#else
#define XMM_ALIGN
#endif
#define XMM_FLOAT			XMM_ALIGN float

typedef enum eMDTRA_StatParm
{
	MDTRA_SP_ARITHMETIC_MEAN = 0,
	MDTRA_SP_GEOMETRIC_MEAN,
	MDTRA_SP_HARMONIC_MEAN,
	MDTRA_SP_QUADRATIC_MEAN,
	MDTRA_SP_MIN_VALUE,
	MDTRA_SP_MAX_VALUE,
	MDTRA_SP_RANGE,
	MDTRA_SP_MIDRANGE,
	MDTRA_SP_MEDIAN,
	MDTRA_SP_VARIANCE,
	MDTRA_SP_STDDEV,
	MDTRA_SP_STDERR,
	MDTRA_SP_MAX_USED,
	MDTRA_SP_USER1,
	MDTRA_SP_USER2,
	MDTRA_SP_USER3,
	MDTRA_SP_USER4,
	MDTRA_SP_USER5,
	MDTRA_SP_USER6,
	MDTRA_SP_MAX,
} MDTRA_StatParm;

typedef enum eMDTRA_ThreadStatParm
{
	MDTRA_TSP_ARITHMETIC_MEAN = 0,
	MDTRA_TSP_GEOMETRIC_MEAN,
	MDTRA_TSP_HARMONIC_MEAN,
	MDTRA_TSP_QUADRATIC_MEAN,
	MDTRA_TSP_MIN_VALUE,
	MDTRA_TSP_MAX_VALUE,
	MDTRA_TSP_MAX,
} MDTRA_ThreadStatParm;

typedef enum eMDTRA_DataType
{
	MDTRA_DT_RMSD = 0,
	MDTRA_DT_RMSD_SEL,
	MDTRA_DT_DISTANCE,
	MDTRA_DT_ANGLE,
	MDTRA_DT_ANGLE2,
	MDTRA_DT_TORSION,
	MDTRA_DT_TORSION_UNSIGNED,
	MDTRA_DT_DIHEDRAL,
	MDTRA_DT_DIHEDRAL_ABS,
	MDTRA_DT_PLANEANGLE,
	MDTRA_DT_FORCE,
	MDTRA_DT_RESULTANT_FORCE,
	MDTRA_DT_SAS,
	MDTRA_DT_SAS_SEL,
	MDTRA_DT_OCCA,
	MDTRA_DT_OCCA_SEL,
	MDTRA_DT_USER,
	MDTRA_DT_RMSF,
	MDTRA_DT_RMSF_SEL,
	MDTRA_DT_RADIUS_OF_GYRATION,
	MDTRA_DT_MAX
} MDTRA_DataType;

typedef enum eMDTRA_XScaleUnits
{
	MDTRA_XSU_SNAPSHOTS = 0,
	MDTRA_XSU_PS,
	MDTRA_XSU_NS,
	MDTRA_XSU_MAX
} MDTRA_XScaleUnits;

typedef enum eMDTRA_YScaleUnits
{
	MDTRA_YSU_ANGSTROMS = 0,
	MDTRA_YSU_NANOMETERS,
	MDTRA_YSU_DEGREES,
	MDTRA_YSU_RADIANS,
	MDTRA_YSU_KCALOVERA,
	MDTRA_YSU_MKNEWTON,
	MDTRA_YSU_SQANGSTROMS,
	MDTRA_YSU_SQNANOMETERS,
	MDTRA_YSU_MAX
} MDTRA_YScaleUnits;

typedef enum eMDTRA_Layout
{
	MDTRA_LAYOUT_TIME = 0,
	MDTRA_LAYOUT_RESIDUE,
	MDTRA_LAYOUT_MAX
} MDTRA_Layout;

#define DSREF_PE_SHIFT			0
#define DSREF_PE_MASK			0x3F
#define DSREF_IE_SHIFT			6
#define DSREF_IE_MASK			0x3F
#define DSREF_IS_SHIFT			12
#define DSREF_IS_MASK			0x3F
#define DSREF_STAT_SHIFT		18
#define DSREF_STAT_MASK			0x3F
#define DSREF_FLAG_VISIBLE		( 1 << 31 )

typedef struct stMDTRA_DSRef
{
	int					dataSourceIndex;
	dword				flags;
	float				xscale;
	float				yscale;
	float				bias;
	float				stat[MDTRA_SP_MAX];
	int					iDataSize;
	int					iActualDataSize;
	float*				pData;
	float*				pCorrelation;
	char				reserved[32];
} MDTRA_DSRef;

#define MAX_DATA_SOURCE_ARGS	6

#define STREAM_FLAG_RELATIVE_PATHS		( 1 << 0 )
#define STREAM_FLAG_IGNORE_HETATM		( 1 << 1 )
#define STREAM_FLAG_IGNORE_SOLVENT		( 1 << 2 )

#endif //MDTRA_TYPES_H
