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
#ifndef MDTRA_MATH_H
#define MDTRA_MATH_H

#ifndef M_PI
#define M_PI				3.14159265358
#endif
#ifndef M_PI_F
#define M_PI_F				3.14159265358f
#endif

inline float UTIL_deg2rad( float x )
{
	return x * (M_PI_F/180.0f);
}
inline float UTIL_rad2deg( float x )
{
	return x * (180.0f/M_PI_F);
}

inline float UTIL_angleDiff( float a1, float a2 )
{
	float delta = a1 - a2;
	if ( a1 > a2 ) {
		if ( delta >= 180 ) delta -= 360;
	} else {
		if ( delta <= -180 ) delta += 360;
	}
	return delta;
}

//Vector algebra
#define Vec3_Set( dst, x, y, z )		{ dst[0]=x; dst[1]=y; dst[2]=z; }
#define Vec3_Add( dst, src0, src1 )		{ dst[0]=src0[0]+src1[0]; dst[1]=src0[1]+src1[1]; dst[2]=src0[2]+src1[2]; }
#define Vec3_Sub( dst, src0, src1 )		{ dst[0]=src0[0]-src1[0]; dst[1]=src0[1]-src1[1]; dst[2]=src0[2]-src1[2]; }
#define Vec3_Scale( dst, src0, f )		{ dst[0]=src0[0]*f; dst[1]=src0[1]*f; dst[2]=src0[2]*f; }
#define Vec3_MA( dst, src0, src1, f )	{ dst[0]=src0[0]+src1[0]*f; dst[1]=src0[1]+src1[1]*f; dst[2]=src0[2]+src1[2]*f; }
#define Vec3_Nrm( dst, src0 )			{ float lensq = src0[0]*src0[0]+src0[1]*src0[1]+src0[2]*src0[2]; if (lensq == 0) { dst[0] = 0; dst[1] = 1; dst[2] = 0; } else { float invlen = 1 / sqrt(lensq); dst[0]=src0[0]*invlen; dst[1]=src0[1]*invlen; dst[2]=src0[2]*invlen; } }
#define Vec3_Len( dst, src0 )			{ dst = sqrt(src0[0]*src0[0]+src0[1]*src0[1]+src0[2]*src0[2]); }
#define Vec3_LenSq( dst, src0 )			{ dst = src0[0]*src0[0]+src0[1]*src0[1]+src0[2]*src0[2]; }
#define Vec3_Dot( dst, src0, src1 )		{ dst=src0[0]*src1[0]+src0[1]*src1[1]+src0[2]*src1[2]; }
#define Vec3_Cross( dst, src0, src1 )	{ dst[0]=src0[1]*src1[2]-src0[2]*src1[1]; dst[1]=src0[2]*src1[0]-src0[0]*src1[2]; dst[2]=src0[0]*src1[1]-src0[1]*src1[0]; }


#endif //MDTRA_MATH_H