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
//	Utility functions

#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include "mdtra_main.h"
#include "mdtra_project.h"
#include "mdtra_utils.h"
#include "mdtra_mainWindow.h"

void* UTIL_AlignedMalloc( size_t size )
{
	char *ptr, *ptr2, *aligned_ptr;

	ptr = (char *)malloc(size + 16 + sizeof(size_t));
	if (!ptr) return NULL;

	ptr2 = ptr + sizeof(size_t);
	aligned_ptr = ptr2 + (16 - ((size_t)ptr2 & 15));

	ptr2 = aligned_ptr - sizeof(size_t);
	*((size_t *)ptr2) = (size_t)(aligned_ptr - ptr);

	return aligned_ptr;
}

void* UTIL_AlignedRealloc( void *baseptr, size_t size, size_t oldsize )
{
	void *ptr = UTIL_AlignedMalloc( size );
	if (!ptr) return NULL;

	memcpy( ptr, baseptr, oldsize );
	UTIL_AlignedFree( baseptr );

	return ptr;
}

void UTIL_AlignedFree( void *baseptr ) 
{
	char *ptr = (char*)baseptr - *((size_t*)baseptr - 1);
	free(ptr);
}

char *UTIL_Strdup( const char *s )
{
	if ( !s ) return NULL;
	size_t l = strlen( s );
	char *ss = (char*)malloc( l + 1 );
	if ( !ss ) return NULL;
	memcpy( ss, s, l );
	ss[l] = '\0';
	return ss;
}

void UTIL_PlaneFromPoints( const float *v1, const float *v2, const float *v3, float *n, float *d )
{
	float vec1[3];
	float vec2[3];

	for (int i = 0; i < 3; i++) {
		vec1[i] = v2[i] - v1[i];
		vec2[i] = v3[i] - v1[i];
	}

	n[0] = vec2[1]*vec1[2]-vec2[2]*vec1[1];
	n[1] = vec2[2]*vec1[0]-vec2[0]*vec1[2];
	n[2] = vec2[0]*vec1[1]-vec2[1]*vec1[0];

	float len = sqrtf( n[0]*n[0] + n[1]*n[1] + n[2]*n[2] );
	if (len > 0.0f) {
		for (int i = 0; i < 3; i++) n[i] /= len;
	} else {
		n[0] = 0.0f;
		n[1] = 1.0f;
		n[2] = 0.0f;
	}

	if (d) *d = vec1[0]*n[0] + vec1[1]*n[1] + vec1[2]*n[2];
}

typedef struct stMDTRA_DataSourceDataTypeDesc
{
	MDTRA_DataType typeId;
	const char* title;
	const char* shortTitle;
	MDTRA_Layout defaultLayout;
	bool layoutChangeable;
} MDTRA_DataSourceDataTypeDesc;

static MDTRA_DataSourceDataTypeDesc s_DataSourceDataTypeDesc[] = 
{
{ MDTRA_DT_RMSD,				"Root Mean Square Deviation (RMSD)", NULL, MDTRA_LAYOUT_TIME, true },
{ MDTRA_DT_RMSD_SEL,			"Root Mean Square Deviation (RMSD) of Selection", NULL, MDTRA_LAYOUT_TIME, true },
{ MDTRA_DT_RMSF,				"Root Mean Square Fluctuation (RMSF)", NULL, MDTRA_LAYOUT_RESIDUE, false },
{ MDTRA_DT_RMSF_SEL,			"Root Mean Square Fluctuation (RMSF) of Selection", NULL, MDTRA_LAYOUT_RESIDUE, false },
{ MDTRA_DT_RADIUS_OF_GYRATION,	"Radius of Gyration", NULL, MDTRA_LAYOUT_TIME, false },
{ MDTRA_DT_DISTANCE,			"Distance [A-B]", "Distance", MDTRA_LAYOUT_TIME, false },
{ MDTRA_DT_ANGLE,				"Angle [A-B-C]", "Angle", MDTRA_LAYOUT_TIME, false },
{ MDTRA_DT_ANGLE2,				"Angle between Sections [A-B and C-D]", "Angle between Sections", MDTRA_LAYOUT_TIME, false },
{ MDTRA_DT_TORSION,				"Torsion Angle [A-B-C-D]", "Torsion Angle", MDTRA_LAYOUT_TIME, false },
{ MDTRA_DT_TORSION_UNSIGNED,	"Torsion Angle (Unsigned) [A-B-C-D]", "Torsion Angle (Unsigned)", MDTRA_LAYOUT_TIME, false },
{ MDTRA_DT_DIHEDRAL,			"Dihedral Angle [A-B-C-D]", "Dihedral Angle", MDTRA_LAYOUT_TIME, false },
{ MDTRA_DT_DIHEDRAL_ABS,		"Dihedral Angle (Abs) [A-B-C-D]", "Dihedral Angle (Abs)", MDTRA_LAYOUT_TIME, false },
{ MDTRA_DT_PLANEANGLE,			"Angle between Planes [A-B-C and D-E-F]", "Angle between Planes", MDTRA_LAYOUT_TIME, false },
{ MDTRA_DT_FORCE,				"Force Magnitude [A]", "Force Magnitude", MDTRA_LAYOUT_TIME, false },
{ MDTRA_DT_RESULTANT_FORCE,		"Resultant Force [A-B]", "Resultant Force", MDTRA_LAYOUT_TIME, false },
{ MDTRA_DT_SAS,					"Solvent Accessile Surface (SAS) Area", NULL, MDTRA_LAYOUT_TIME, true },
{ MDTRA_DT_SAS_SEL,				"Solvent Accessile Surface (SAS) Area of Selection", NULL, MDTRA_LAYOUT_TIME, true },
{ MDTRA_DT_OCCA,				"Occluded Area", NULL, MDTRA_LAYOUT_TIME, true },
{ MDTRA_DT_OCCA_SEL,			"Occluded Area of Selection", NULL, MDTRA_LAYOUT_TIME, true },
{ MDTRA_DT_USER,				"User-defined Type", NULL, MDTRA_LAYOUT_TIME, true },
};

static const char *szYAxisTitles[MDTRA_DT_MAX][MDTRA_LANG_MAX] = 
{
	{	"RMSD",			"RMSD"		},
	{	"RMSD",			"RMSD"		},
	{	"Distance",		"Расстояние" },
	{	"Angle",		"Угол"		},
	{	"Angle",		"Угол"		},
	{	"Angle",		"Угол"		},
	{	"Angle",		"Угол"		},
	{	"Angle",		"Угол"		},
	{	"Angle",		"Угол"		},
	{	"Angle",		"Угол"		},
	{	"Force",		"Сила"		},
	{	"Force",		"Сила"		},
	{	"SAS Area",		"Площадь SAS" },
	{	"SAS Area",		"Площадь SAS" },
	{	"Occluded Area","Заслонённая площадь" },
	{	"Occluded Area","Заслонённая площадь" },
	{	"",				""			},
	{	"RMSF",			"RMSF"		},
	{	"RMSF",			"RMSF"		},
	{	"Radius",		"Радиус"	},
};
static const char *szYScaleUnitTitles[MDTRA_YSU_MAX][MDTRA_LANG_MAX] = 
{
	{	" [A°]",		" [A°]"		},
	{	" [nm]",		" [нм]"		},
	{	" [deg]",		" [градусы]"},
	{	" [rad]",		" [радианы]"},
	{	" [kcal/A°]",	" [ккал/A°]"},
	{	" [mkN]",		" [мкН]"	},
	{	" [A°^2]",		" [A°^2]"	},
	{	" [nm^2]",		" [нм^2]"	},
};

static const char *szXAxisTitles[MDTRA_XSU_MAX][MDTRA_LANG_MAX] = 
{
	{	"Snapshots",	"Снапшоты"		},
	{	"Time [ps]",	"Время [пс]"	},
	{	"Time [ns]",	"Время [нс]"	},
};

const char *UTIL_GetXAxisTitle( int dataId, int langId ) 
{
	return szXAxisTitles[dataId][langId];
}

const char *UTIL_GetYAxisTitle( int dataId, int langId ) 
{
	return szYAxisTitles[dataId][langId];
}

const char *UTIL_GetYAxisUnitTitle( int dataId, int langId ) 
{
	return szYScaleUnitTitles[dataId][langId];
}

MDTRA_DataType UTIL_GetDataSourceTypeId( int id ) 
{
	int numDSTD = sizeof(s_DataSourceDataTypeDesc) / sizeof(s_DataSourceDataTypeDesc[0]);

	if (id < 0 || id >= numDSTD)
		return MDTRA_DT_RMSD;
	else
		return s_DataSourceDataTypeDesc[id].typeId;
}

const char *UTIL_GetDataSourceTypeName( int id ) 
{
	int numDSTD = sizeof(s_DataSourceDataTypeDesc) / sizeof(s_DataSourceDataTypeDesc[0]);

	if (id < 0 || id >= numDSTD)
		return "???";
	else
		return s_DataSourceDataTypeDesc[id].title;
}

const char *UTIL_GetDataSourceShortTypeName( int id ) 
{
	int numDSTD = sizeof(s_DataSourceDataTypeDesc) / sizeof(s_DataSourceDataTypeDesc[0]);

	if (id < 0 || id >= numDSTD)
		return "???";
	else if (s_DataSourceDataTypeDesc[id].shortTitle)
		return s_DataSourceDataTypeDesc[id].shortTitle;
	else
		return s_DataSourceDataTypeDesc[id].title;
}

const char *UTIL_GetDataSourceTypeName( MDTRA_DataType dt ) 
{
	int numDSTD = sizeof(s_DataSourceDataTypeDesc) / sizeof(s_DataSourceDataTypeDesc[0]);
	for ( int i = 0; i < numDSTD; i++ ) {
		if ( s_DataSourceDataTypeDesc[i].typeId == dt )
			return s_DataSourceDataTypeDesc[i].title;
	}
	return "???";
}

bool UTIL_IsDataSourceLayoutChangeable( MDTRA_DataType dt ) 
{
	int numDSTD = sizeof(s_DataSourceDataTypeDesc) / sizeof(s_DataSourceDataTypeDesc[0]);
	for ( int i = 0; i < numDSTD; i++ ) {
		if ( s_DataSourceDataTypeDesc[i].typeId == dt )
			return s_DataSourceDataTypeDesc[i].layoutChangeable;
	}
	return false;
}

MDTRA_Layout UTIL_GetDataSourceDefaultLayout( MDTRA_DataType dt ) 
{
	int numDSTD = sizeof(s_DataSourceDataTypeDesc) / sizeof(s_DataSourceDataTypeDesc[0]);
	for ( int i = 0; i < numDSTD; i++ ) {
		if ( s_DataSourceDataTypeDesc[i].typeId == dt )
			return s_DataSourceDataTypeDesc[i].defaultLayout;
	}
	return MDTRA_LAYOUT_TIME;
}

bool UTIL_IsSharedPlotType( int type1, int type2 ) 
{
	if (type1 == type2)
		return true;
	if (type1 == MDTRA_DT_RMSD && type2 == MDTRA_DT_RMSD_SEL)
		return true;
	if (type2 == MDTRA_DT_RMSD && type1 == MDTRA_DT_RMSD_SEL)
		return true;
	if (type1 == MDTRA_DT_RMSF && type2 == MDTRA_DT_RMSF_SEL)
		return true;
	if (type2 == MDTRA_DT_RMSF && type1 == MDTRA_DT_RMSF_SEL)
		return true;
	if (type1 == MDTRA_DT_SAS && type2 == MDTRA_DT_SAS_SEL)
		return true;
	if (type2 == MDTRA_DT_SAS && type1 == MDTRA_DT_SAS_SEL)
		return true;
	if (type1 == MDTRA_DT_OCCA && type2 == MDTRA_DT_OCCA_SEL)
		return true;
	if (type2 == MDTRA_DT_OCCA && type1 == MDTRA_DT_OCCA_SEL)
		return true;

	return false;
}

static const char *s_szStatParmNames[MDTRA_SP_MAX] = 
{
	"Arithmetic Mean",
	"Geometric Mean",
	"Harmonic Mean",
	"Quadratic Mean (Root Mean Square)",
	"Minimum Value",
	"Maximum Value",
	"Range",
	"Midrange",
	"Median",
	"Sample Variance",
	"Sample Standard Deviation",
	"Sample Standard Error"
};

static const char *s_szStatParmShortNames[MDTRA_SP_MAX] = 
{
	"Arithm. Mean",
	"Geom. Mean",
	"Harm. Mean",
	"Quad. Mean ",
	"MinValue",
	"MaxValue",
	"Range",
	"Midrange",
	"Median",
	"Variance",
	"Std. Dev.",
	"Std. Error"
};

static const char *s_szStatParmLabels[MDTRA_SP_MAX] = 
{
	"M",
	"Mg",
	"Mh",
	"RMS",
	"Min",
	"Max",
	"R",
	"Mr",
	"Med",
	"S2",
	"S",
	"SE"
};

const char *UTIL_GetStatParmName( int index ) 
{
	if (index < 0 || index >= MDTRA_SP_MAX)
		return "???";
	else
		return s_szStatParmNames[index];
}

const char *UTIL_GetStatParmShortName( int index ) 
{
	if (index < 0 || index >= MDTRA_SP_MAX)
		return "???";
	else
		return s_szStatParmShortNames[index];
}

const char *UTIL_GetStatParmLabel( int index ) 
{
	if (index < 0 || index >= MDTRA_SP_MAX)
		return "???";
	else
		return s_szStatParmLabels[index];
}

static struct {
	const char *symbol;
	const char *title;
} element_map[] = {
	//two-digit
	{ "??", "<unknown>" },	//MUST be the first in map
	{ "FE", "iron" },
	{ "ZN", "zinc" },
	{ "MG", "magnesium" },
	{ "SE", "selenium" },
	{ "CL", "chlorine" },
	{ "BR", "bromine" },
	//one-digit
	{ "H", "hydrogen" },
	{ "C", "carbon" },
	{ "N", "nitrogen" },
	{ "O", "oxygen" },
	{ "S", "sulfur" },
	{ "P", "phosphorus" },
	{ "F", "fluorine" },
	{ "I", "iodine" },
};

const char *UTIL_SymbolForAtomTitle( const char *trimmed_title )
{
	int mapSize = sizeof(element_map) / sizeof(element_map[0]);
	for (int i = 0; i < mapSize; i++) {
		if (!_strnicmp(trimmed_title, element_map[i].symbol, strlen(element_map[i].symbol)))
			return element_map[i].symbol;
	}
	return element_map[0].symbol;
}

bool UTIL_IsElementTitle( const char *element_title )
{
	int mapSize = sizeof(element_map) / sizeof(element_map[0]);
	for (int i = 1; i < mapSize; i++) {
		if (!_stricmp(element_title, element_map[i].title))
			return true;
	}
	return false;
}

bool UTIL_SymbolMatch( const char *symbol, const char *element_title )
{
	int mapSize = sizeof(element_map) / sizeof(element_map[0]);
	for (int i = 1; i < mapSize; i++) {
		if (!_stricmp(symbol, element_map[i].symbol) && !_stricmp(element_title, element_map[i].title))
			return true;
	}
	return false;
}

static const char *string_contains(const char *str1, const char *str2, int casesensitive) 
{
	int len, i, j;

	len = strlen(str1) - strlen(str2);
	for (i = 0; i <= len; i++, str1++) {
		for (j = 0; str2[j]; j++) {
			if (casesensitive) {
				if (str1[j] != str2[j]) {
					break;
				}
			}
			else {
				if (toupper(str1[j]) != toupper(str2[j])) {
					break;
				}
			}
		}
		if (!str2[j]) {
			return str1;
		}
	}
	return NULL;
}

bool UTIL_strfilter(const char *filter, const char *name, int casesensitive)
{
	char buf[1024];
	const char *ptr;
	int i, found;

	while (*filter) {
		if (*filter == '*') {
			filter++;
			if (!*filter) return true;
			for (i = 0; *filter; i++) {
				if (*filter == '*' || *filter == '?') break;
				buf[i] = *filter;
				filter++;
			}
			buf[i] = '\0';
			if (strlen(buf)) {
				ptr = string_contains(name, buf, casesensitive);
				if (!ptr) return false;
				name = ptr + strlen(buf);
			}
		}
		else if (*filter == '?') {
			if (!*name) return false;
			filter++;
			name++;
		}
		else if (*filter == '[' && *(filter+1) == '[') {
			filter++;
		}
		else if (*filter == '[') {
			filter++;
			found = false;
			while(*filter && !found) {
				if (*filter == ']' && *(filter+1) != ']') break;
				if (*(filter+1) == '-' && *(filter+2) && (*(filter+2) != ']' || *(filter+3) == ']')) {
					if (casesensitive) {
						if (*name >= *filter && *name <= *(filter+2)) found = true;
					}
					else {
						if (toupper(*name) >= toupper(*filter) &&
							toupper(*name) <= toupper(*(filter+2))) found = true;
					}
					filter += 3;
				}
				else {
					if (casesensitive) {
						if (*filter == *name) found = true;
					}
					else {
						if (toupper(*filter) == toupper(*name)) found = true;
					}
					filter++;
				}
			}
			if (!found) return false;
			while(*filter) {
				if (*filter == ']' && *(filter+1) != ']') break;
				filter++;
			}
			filter++;
			name++;
		}
		else {
			if (casesensitive) {
				if (*filter != *name) return false;
			}
			else {
				if (toupper(*filter) != toupper(*name)) return false;
			}
			filter++;
			name++;
		}
	}
	return (*name == 0);
}

static struct {
	const char *groupname;
	const char *resname;
} residue_group_map[] = {
	{ "{ALA}", "ALA" },
	{ "{GLY}", "GLY" },
	{ "{SER}", "SER" },
	{ "{THR}", "THR" },
	{ "{LEU}", "LEU" },
	{ "{ILE}", "ILE" },
	{ "{VAL}", "VAL" },
	{ "{ASN}", "ASN" },
	{ "{GLN}", "GLN" },
	{ "{ARG}", "ARG" },
	{ "{HIS}", "HID" },
	{ "{HIS}", "HIE" },
	{ "{HIS}", "HIP" },
	{ "{HIS}", "HIS" },
	{ "{HIS}", "HSD" },
	{ "{HIS}", "HSE" },
	{ "{HIS}", "HSP" },
	{ "{TRP}", "TRP" },
	{ "{PHE}", "PHE" },
	{ "{TYR}", "TYR" },
	{ "{GLU}", "GLU" },
	{ "{GLU}", "GLH" },
	{ "{ASP}", "ASP" },
	{ "{ASP}", "ASH" },
	{ "{LYS}", "LYS" },
	{ "{LYS}", "LYN" },
	{ "{PRO}", "PRO" },
	{ "{PRO}", "PRH" },
	{ "{CYS}", "CYS" },
	{ "{CYS}", "CYM" },
	{ "{CYS}", "CYX" },
	{ "{MET}", "MET" },
	{ "{A}", "DA" },
	{ "{A}", "DA3" },
	{ "{A}", "DA5" },
	{ "{A}", "A" },
	{ "{A}", "A3" },
	{ "{A}", "A5" },
	{ "{T}", "DT" },
	{ "{T}", "DT3" },
	{ "{T}", "DT5" },
	{ "{T}", "T" },
	{ "{T}", "T3" },
	{ "{T}", "T5" },
	{ "{C}", "DC" },
	{ "{C}", "DC3" },
	{ "{C}", "DC5" },
	{ "{C}", "C" },
	{ "{C}", "C3" },
	{ "{C}", "C5" },
	{ "{G}", "DG" },
	{ "{G}", "DG3" },
	{ "{G}", "DG5" },
	{ "{G}", "G" },
	{ "{G}", "G3" },
	{ "{G}", "G5" },
	{ "{U}", "DU" },
	{ "{U}", "DU3" },
	{ "{U}", "DU5" },
	{ "{U}", "U" },
	{ "{U}", "U3" },
	{ "{U}", "U5" },
};

bool UTIL_ResidueGroupMatch( const char *check, const char *ref )
{
	if (!check || *check != '{')
		return false;
	int checklen = strlen(check);
	if (check[checklen-1] != '}')
		return false;

	int mapSize = sizeof(residue_group_map) / sizeof(residue_group_map[0]);
	for (int i = 0; i < mapSize; i++) {
		if (!_stricmp(ref, residue_group_map[i].resname) && UTIL_strfilter(check, residue_group_map[i].groupname, 0))
			return true;
	}

	return false;
}

int UTIL_BestPowerOf2( int x, int limit )
{
	dword v = 1;
	dword limit2 = (MDTRA_MIN( (x << 1), limit )) >> 1;

	while (v <= limit2)
		v <<= 1;

	return v;
}

byte UTIL_FloatToColor( int channel, float v )
{
	byte outValue = 0;

	switch (channel) {
	case 0: //red
		{
			if (v <= 0.5f) {
			} else if (v <= 0.7f) {
				outValue = 255 * (5.0f * v - 2.5f);
			} else if (v <= 0.9f) {
				outValue = 255;
			} else if (v <= 1.0f) {
				outValue = 128 + 127 * (10.0f - 10.0f * v);
			}
		}
		break;
	case 1: //green
		{
			if (v <= 0.1f) {
			} else if (v <= 0.3f) {
				outValue = 255 * (5.0f * v - 0.5f);
			} else if (v <= 0.7f) {
				outValue = 255;
			} else if (v <= 0.9f) {
				outValue = 255 * (4.5f - 5.0f * v);
			}
		}
		break;
	case 2: //blue
		{
			if (v <= 0.1f) {
				outValue = 128 + 127 * (v * 10.0f);
			} else if (v <= 0.3f) {
				outValue = 255;
			} else if (v <= 0.5f) {
				outValue = 255 * (2.5f - 5.0f * v);
			}
		}
		break;
	default:
		break;
	}

	return outValue;
}

int UTIL_Atoi( const char *str )
{
	int val, sign, c;

	if( !str ) 
		return 0;

	// check for empty charachters in string
    while( *str && (byte)(*str) <= 32 ) str++;
	
	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else
		sign = 1;
		
	val = 0;

	// check for hex
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X') )
	{
		str += 2;
		while (1)
		{
			c = *str++;
			if (c >= '0' && c <= '9')
				val = (val<<4) + c - '0';
			else if (c >= 'a' && c <= 'f')
				val = (val<<4) + c - 'a' + 10;
			else if (c >= 'A' && c <= 'F')
				val = (val<<4) + c - 'A' + 10;
			else
				return val*sign;
		}
	}
	
	// check for character
	if (str[0] == '\'')
	{
		return sign * str[1];
	}
	
	// assume decimal
	while (1)
	{
		c = *str++;
		if (c < '0' || c > '9')
			return val*sign;
		val = val*10 + c - '0';
	}
	
	return 0;
}

float UTIL_Atof( const char *str )
{
	double val;
	int sign, c, decimal, total;

	if( !str ) 
		return 0;

	// check for empty charachters in string
    while( *str && (byte)(*str) <= 32 ) str++;
	
	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else
		sign = 1;
		
	val = 0;

	// check for hex
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X') )
	{
		str += 2;
		while (1)
		{
			c = *str++;
			if (c >= '0' && c <= '9')
				val = (val*16) + c - '0';
			else if (c >= 'a' && c <= 'f')
				val = (val*16) + c - 'a' + 10;
			else if (c >= 'A' && c <= 'F')
				val = (val*16) + c - 'A' + 10;
			else
				return (float)val*sign;
		}
	}
	
	// check for character
	if (str[0] == '\'')
	{
		return (float)(sign * str[1]);
	}
	
	// assume decimal
	decimal = -1;
	total = 0;
	while (1)
	{
		c = *str++;
		if (c == '.')
		{
			decimal = total;
			continue;
		}
		if (c <'0' || c > '9')
			break;
		val = val*10 + c - '0';
		total++;
	}

	if (decimal == -1)
		return (float)val*sign;
	while (total > decimal)
	{
		val /= 10;
		total--;
	}
	
	return (float)val*sign;
}

QString UTIL_MakeRelativeFileName( const QString &name, const QStringList &baseDirectories )
{
	//check if it is already relative
#if defined(WIN32)
	if (name.at(1).toAscii() != ':')
		return name;
#elif defined(LINUX)
	if (name.at(0).toAscii() != '/')
		return name;
#endif

	QString correctPath = name;
	correctPath = correctPath.replace('\\', '/');
	QStringList curDirectories = correctPath.split('/');

	//get the shortest of the two paths
	int length = MDTRA_MIN(curDirectories.size(), baseDirectories.size());
 
	//find common root
	int lastCommonRoot = -1;
	for ( int index = 0; index < length; index++ ) {
		if (baseDirectories[index] == curDirectories[index])
			lastCommonRoot = index;
		else
			break;
	}

	//if we didn't find a common prefix then return absolute path
    if (lastCommonRoot == -1)
		return name;

	//build the relative path
	QString relative;

	//add on the ..
	for (int index = lastCommonRoot + 1; index < baseDirectories.size(); index++) {
		if (baseDirectories[index].length() > 0)
			relative.append("../");
	}

	//add the folders
	for (int index = lastCommonRoot + 1; index < curDirectories.size() - 1; index++) {
		relative.append(curDirectories[index] + "/");
	}
	relative.append(curDirectories[curDirectories.size() - 1]);

	return relative;
}

QString UTIL_MakeAbsoluteFileName( const QString &name, const QString &basePath )
{
	//check if it is already absolute
#if defined(WIN32)
	if (name.at(1).toAscii() == ':')
		return name;
#elif defined(LINUX)
	if (name.at(0).toAscii() == '/')
		return name;
#endif

	QString absname = basePath;
	absname.append( name );
	QFileInfo fi( absname );

	if (!fi.exists())
		return absname;

	return fi.canonicalFilePath();
}

QStringList UTIL_MakeRelativeFileNames( const QStringList &list, const QString &basePath )
{
	QString correctBasePath = basePath;
	correctBasePath = correctBasePath.replace('\\', '/');
	QStringList baseDirectories = correctBasePath.split('/');

	QStringList outList;
	for (int i = 0; i < list.count(); i++) {
		outList << UTIL_MakeRelativeFileName( list.at(i), baseDirectories );
	}
	return outList;
}

QStringList UTIL_MakeAbsoluteFileNames( const QStringList &list, const QString &basePath )
{
	QString correctBasePath = basePath;
	correctBasePath = correctBasePath.replace('\\', '/');
	if (correctBasePath.at(correctBasePath.size()-1) != '/')
		correctBasePath.append('/');

	QStringList outList;
	for (int i = 0; i < list.count(); i++) {
		outList << UTIL_MakeAbsoluteFileName( list.at(i), correctBasePath );
	}
	return outList;
}

int UTIL_ExtractFilePath( char *dest, size_t size, const char *src )
{
	size_t slen = strlen( src );
	if ( !slen ) {
		*dest = '\0';
		return 0;
	}

	// Returns the path up to, but not including the last slash
	const char *s = src + slen - 1;

	while ( s != src && *s != '/' && *s != '\\' )
		--s;

	int maxsize = MDTRA_MIN( (int)(s-src), (int)size-1 );

	if ( maxsize > 0 )
		strncpy_s( dest, size, src, maxsize );

	dest[maxsize] = 0;
	return maxsize;
}

void Prof_Log( const char *func, qword cycles )
{
	FILE* fp = NULL;
	if ( fopen_s( &fp, "profiler.log", "a" ) )
		return;
	
	dword hi = cycles >> 32;
	dword lo = cycles & ~dword(0);

	if ( hi )
		fprintf( fp, "%s: %u%u\n", func, hi, lo );
	else
		fprintf( fp, "%s: %u\n", func, lo );

	fclose( fp );
}

typedef struct stMDTRA_AtomColorInfo {
	const char* symbol;
	float color[4];
} MDTRA_AtomColorInfo;

static MDTRA_AtomColorInfo s_AtomColorInfo[] = {
	{ "??", { 1.0f, 0.0f, 0.5f, 1.0f } },
	{ "H",  { 1.0f, 1.0f, 1.0f, 1.0f } },
	{ "C",  { 0.25f, 0.25f, 0.25f, 1.0f } },
	{ "N",  { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ "O",  { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ "F",  { 0.69f, 1.0f, 0.0f, 1.0f } },
	{ "P",  { 1.0f, 0.5f, 0.0f, 1.0f } },
	{ "S",  { 1.0f, 1.0f, 0.0f, 1.0f } },
	{ "CL", { 0.5f, 1.0f, 0.0f, 1.0f } },
	{ "SE", { 1.0f, 0.78f, 0.0f, 1.0f } },
	{ "BR", { 0.78f, 0.39f, 0.0f, 1.0f } },
	{ "I",  { 0.5f, 0.0f, 1.0f, 1.0f } },
	{ "ZN", { 0.49f, 0.5f, 0.69f, 1.0f } },
	{ "MG", { 0.49f, 0.5f, 0.69f, 1.0f } },
	{ "FE", { 0.49f, 0.5f, 0.69f, 1.0f } },
};

const float *UTIL_Color4Sym( const char* symbol )
{
	if ( symbol && strlen(symbol) ) {
		int n = sizeof(s_AtomColorInfo)/sizeof(s_AtomColorInfo[0]);
		for ( int i = 1; i < n; i++ ) {
			if ( !_stricmp( s_AtomColorInfo[i].symbol, symbol ) )
				return s_AtomColorInfo[i].color;
		}
	}
	return s_AtomColorInfo[0].color;
}

int UTIL_GetMainDirectory( char *out, size_t outSize )
{
	// return main program directory
	char buffer[8192];
	memset( buffer, 0, sizeof(buffer) );
#if defined(_WIN32)
	GetModuleFileName( (HMODULE)0, buffer, (DWORD)sizeof(buffer) );
#else
	ssize_t result = readlink( "/proc/self/exe", out, outSize );
	if ( result == -1 ) {
		fprintf( stderr, "UTIL_GetMainDirectory(): %s\n", strerror( errno ) );
		return 0;
	}
#endif
	UTIL_ExtractFilePath( out, outSize, buffer );
	return 1;
}

#if defined(_WIN32)

char **UTIL_ListFiles( const char *directory, const char *extension, int *numfiles )
{
	struct _finddata_t findinfo;
	char *list[MAX_FOUND_FILES];
	char **listCopy;
	char search[8192];
	int flag = 1;
	int nfiles = 0;

	if ( !extension )
		extension = "";

	if ( extension[0] == '/' && extension[1] == 0 ) {
		extension = "";
		flag = 0;
	} else {
		flag = _A_SUBDIR;
	}

	sprintf_s( search, sizeof(search), "%s/*%s", directory, extension );

	intptr_t findhandle = _findfirst( search, &findinfo );
	if ( findhandle == -1 ) {
		*numfiles = 0;
		return NULL;
	}

	do {
		if ( flag ^ ( findinfo.attrib & _A_SUBDIR ) ) {
			if ( nfiles == MAX_FOUND_FILES - 1 )
				break;
			size_t sizFilename = strlen( findinfo.name ) + 1;
			list[ nfiles ] = (char*)malloc( sizFilename );
			memcpy( list[ nfiles ], findinfo.name, sizFilename );
			++nfiles;
		}
	} while ( _findnext( findhandle, &findinfo ) != -1 );
	_findclose( findhandle );

	list[ nfiles ] = NULL;

	// return a copy of the list
	*numfiles = nfiles;
	if ( !nfiles )
		return NULL;

	listCopy = (char**)malloc( (nfiles + 1)*sizeof(char*) );
	for ( int i = 0; i < nfiles; ++i )
		listCopy[ i ] = list[ i ];
	listCopy[ nfiles ] = NULL;

	return listCopy;
}

#else

char **UTIL_ListFiles( const char *directory, const char *extension, int *numfiles )
{
	DIR *dirhandle;
	struct dirent *d;
	struct stat st;
	char *list[MAX_FOUND_FILES];
	char **listCopy;
	char search[8192];
	int flag = 1;
	int nfiles = 0;

	if ( !extension )
		extension = "";

	if ( extension[0] == '/' && extension[1] == 0 ) {
		extension = "";
		flag = 0;
	} 

	if ( ( dirhandle = opendir( directory ) ) == NULL ) {
		*numfiles = 0;
		return NULL;
	}

	while ( ( d = readdir( dirhandle ) ) != NULL ) {
		sprintf_s( search, sizeof(search), "%s/%s", directory, d->d_name );
		if ( stat( search, &st ) == -1 )
			continue;

		if ( !( flag ^ ( S_ISDIR(st.st_mode) ) ) )
			continue;

		if ( *extension ) {
			if ( strlen( d->d_name ) < strlen( extension ) || _stricmp( d->d_name + strlen( d->d_name ) - strlen( extension ), extension ) )
				continue;
		}
		if ( nfiles == MAX_FOUND_FILES - 1 )
			break;

		size_t sizFilename = strlen( d->d_name ) + 1;
		list[ nfiles ] = (char*)malloc( sizFilename );
		memcpy( list[ nfiles ], d->d_name, sizFilename );
		++nfiles;
	}
	closedir( dirhandle );

	list[ nfiles ] = 0;

	// return a copy of the list
	*numfiles = nfiles;
	if ( !nfiles )
		return NULL;

	listCopy = (char**)malloc( (nfiles + 1)*sizeof(char*) );
	for ( int i = 0; i < nfiles; ++i )
		listCopy[ i ] = list[ i ];
	listCopy[ nfiles ] = NULL;

	return listCopy;
}

#endif

void UTIL_FreeFileList( char **list )
{
	if ( !list ) 
		return;
	for ( int i = 0; list[i]; ++i )
		free( list[i] );
	free( list );
}