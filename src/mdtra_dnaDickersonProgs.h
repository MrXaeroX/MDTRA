/***************************************************************************
* Copyright (C) 2011-2012 Alexander V. Popov.
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
#ifndef MDTRA_DNADICKERSONPROGS_H
#define MDTRA_DNADICKERSONPROGS_H

const char DNA_PHASE_ANGLE_PROGRAM[] = 
"function main()\n"
"\tlocal sine_const = 3.0776835;\n"
"\tlocal nu0 = torsion(arg4, arg5, arg1, arg2);\n"
"\tlocal nu1 = torsion(arg5, arg1, arg2, arg3);\n"
"\tlocal nu2 = torsion(arg1, arg2, arg3, arg4);\n"
"\tlocal nu3 = torsion(arg2, arg3, arg4, arg5);\n"
"\tlocal nu4 = torsion(arg3, arg4, arg5, arg1);\n"
"\tlocal x = (nu4+nu1)-(nu3+nu0);\n"
"\tlocal y = nu2*sine_const;\n"
"\tlocal ang = math.deg(math.atan2(x,y));\n"
"\tif ( ang < 0 ) then ang = ang + 360.0; end;\n"
"\treturn ang;\n"
"end;";

const char DNA_SHEAR_PROGRAM[] = 
"function main()\n"
"\tr1, o1 = get_residue_transform( arg1 );\n"
"\tr2, o2 = get_residue_transform( arg2 );\n"
"\tr2[2] = -r2[2];\n"
"\tr2[3] = -r2[3];\n"
"\tr2[5] = -r2[5];\n"
"\tr2[6] = -r2[6];\n"
"\tr2[8] = -r2[8];\n"
"\tr2[9] = -r2[9];\n"
"\tz1 = vec3( r1[3], r1[6], r1[9] );\n"
"\tz2 = vec3( r2[3], r2[6], r2[9] );\n"
"\thinge = normalize( cross( z1, z2 ) );\n"
"\troll_tilt = math.acosn( dot( z1, z2 ) );\n"
"\tgrm1 = build_rotation_matrix( hinge, roll_tilt*0.5 );\n"
"\tgrm2 = build_rotation_matrix( hinge, roll_tilt*-0.5 );\n"
"\tr1a = grm1 * r1;\n"
"\tr2a = grm2 * r2;\n"
"\tr_average = vec3();\n"
"\tr_average[1] = r1a[1] + r2a[1];\n"
"\tr_average[2] = r1a[4] + r2a[4];\n"
"\tr_average[3] = r1a[7] + r2a[7];\n"
"\tr_average = normalize( r_average );\n"
"\todiff = o2 - o1;\n"
"\treturn dot( odiff, r_average );\n"
"end;";

const char DNA_STRETCH_PROGRAM[] = 
"function main()\n"
"\tr1, o1 = get_residue_transform( arg1 );\n"
"\tr2, o2 = get_residue_transform( arg2 );\n"
"\tr2[2] = -r2[2];\n"
"\tr2[3] = -r2[3];\n"
"\tr2[5] = -r2[5];\n"
"\tr2[6] = -r2[6];\n"
"\tr2[8] = -r2[8];\n"
"\tr2[9] = -r2[9];\n"
"\tz1 = vec3( r1[3], r1[6], r1[9] );\n"
"\tz2 = vec3( r2[3], r2[6], r2[9] );\n"
"\thinge = normalize( cross( z1, z2 ) );\n"
"\troll_tilt = math.acosn( dot( z1, z2 ) );\n"
"\tgrm1 = build_rotation_matrix( hinge, roll_tilt*0.5 );\n"
"\tgrm2 = build_rotation_matrix( hinge, roll_tilt*-0.5 );\n"
"\tr1a = grm1 * r1;\n"
"\tr2a = grm2 * r2;\n"
"\tr_average = vec3();\n"
"\tr_average[1] = r1a[2] + r2a[2];\n"
"\tr_average[2] = r1a[5] + r2a[5];\n"
"\tr_average[3] = r1a[8] + r2a[8];\n"
"\tr_average = normalize( r_average );\n"
"\todiff = o2 - o1;\n"
"\treturn dot( odiff, r_average );\n"
"end;";

const char DNA_STAGGER_PROGRAM[] = 
"function main()\n"
"\tr1, o1 = get_residue_transform( arg1 );\n"
"\tr2, o2 = get_residue_transform( arg2 );\n"
"\tr2[2] = -r2[2];\n"
"\tr2[3] = -r2[3];\n"
"\tr2[5] = -r2[5];\n"
"\tr2[6] = -r2[6];\n"
"\tr2[8] = -r2[8];\n"
"\tr2[9] = -r2[9];\n"
"\tz1 = vec3( r1[3], r1[6], r1[9] );\n"
"\tz2 = vec3( r2[3], r2[6], r2[9] );\n"
"\thinge = normalize( cross( z1, z2 ) );\n"
"\troll_tilt = math.acosn( dot( z1, z2 ) );\n"
"\tgrm1 = build_rotation_matrix( hinge, roll_tilt*0.5 );\n"
"\tgrm2 = build_rotation_matrix( hinge, roll_tilt*-0.5 );\n"
"\tr1a = grm1 * r1;\n"
"\tr2a = grm2 * r2;\n"
"\tr_average = vec3();\n"
"\tr_average[1] = r1a[3] + r2a[3];\n"
"\tr_average[2] = r1a[6] + r2a[6];\n"
"\tr_average[3] = r1a[9] + r2a[9];\n"
"\tr_average = normalize( r_average );\n"
"\todiff = o2 - o1;\n"
"\treturn dot( odiff, r_average );\n"
"end;";

const char DNA_BUCKLE_PROGRAM[] = 
"function main()\n"
"\tr1, o1 = get_residue_transform( arg1 );\n"
"\tr2, o2 = get_residue_transform( arg2 );\n"
"\tr2[2] = -r2[2];\n"
"\tr2[3] = -r2[3];\n"
"\tr2[5] = -r2[5];\n"
"\tr2[6] = -r2[6];\n"
"\tr2[8] = -r2[8];\n"
"\tr2[9] = -r2[9];\n"
"\tz1 = vec3( r1[3], r1[6], r1[9] );\n"
"\tz2 = vec3( r2[3], r2[6], r2[9] );\n"
"\thinge = normalize( cross( z1, z2 ) );\n"
"\troll_tilt = math.acosn( dot( z1, z2 ) );\n"
"\tgrm1 = build_rotation_matrix( hinge, roll_tilt*0.5 );\n"
"\tgrm2 = build_rotation_matrix( hinge, roll_tilt*-0.5 );\n"
"\tr1a = grm1 * r1;\n"
"\tr2a = grm2 * r2;\n"
"\tr_average = vec3();\n"
"\tr_average[1] = r1a[2] + r2a[2];\n"
"\tr_average[2] = r1a[5] + r2a[5];\n"
"\tr_average[3] = r1a[8] + r2a[8];\n"
"\tr_average = normalize( r_average );\n"
"\tr_average2 = vec3();\n"
"\tr_average2[1] = r1a[3] + r2a[3];\n"
"\tr_average2[2] = r1a[6] + r2a[6];\n"
"\tr_average2[3] = r1a[9] + r2a[9];\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tphase = math.acosn( dot( hinge, r_average ) );\n"
"\tv1 = vec3( r1a[2], r1a[5], r1a[8] );\n"
"\tv2 = vec3( r2a[2], r2a[5], r2a[8] );\n"
"\tsigntest = dot( cross( v1, v2 ), r_average2 );\n"
"\tif ( signtest < 0 ) then phase = -phase; end;\n"
"\tparm = roll_tilt * math.sin( phase );\n"
"\treturn math.deg(parm);\n"
"end;";

const char DNA_PROPELLER_PROGRAM[] = 
"function main()\n"
"\tr1, o1 = get_residue_transform( arg1 );\n"
"\tr2, o2 = get_residue_transform( arg2 );\n"
"\tr2[2] = -r2[2];\n"
"\tr2[3] = -r2[3];\n"
"\tr2[5] = -r2[5];\n"
"\tr2[6] = -r2[6];\n"
"\tr2[8] = -r2[8];\n"
"\tr2[9] = -r2[9];\n"
"\tz1 = vec3( r1[3], r1[6], r1[9] );\n"
"\tz2 = vec3( r2[3], r2[6], r2[9] );\n"
"\thinge = normalize( cross( z1, z2 ) );\n"
"\troll_tilt = math.acosn( dot( z1, z2 ) );\n"
"\tgrm1 = build_rotation_matrix( hinge, roll_tilt*0.5 );\n"
"\tgrm2 = build_rotation_matrix( hinge, roll_tilt*-0.5 );\n"
"\tr1a = grm1 * r1;\n"
"\tr2a = grm2 * r2;\n"
"\tr_average = vec3();\n"
"\tr_average[1] = r1a[2] + r2a[2];\n"
"\tr_average[2] = r1a[5] + r2a[5];\n"
"\tr_average[3] = r1a[8] + r2a[8];\n"
"\tr_average = normalize( r_average );\n"
"\tr_average2 = vec3();\n"
"\tr_average2[1] = r1a[3] + r2a[3];\n"
"\tr_average2[2] = r1a[6] + r2a[6];\n"
"\tr_average2[3] = r1a[9] + r2a[9];\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tphase = math.acosn( dot( hinge, r_average ) );\n"
"\tv1 = vec3( r1a[2], r1a[5], r1a[8] );\n"
"\tv2 = vec3( r2a[2], r2a[5], r2a[8] );\n"
"\tsigntest = dot( cross( v1, v2 ), r_average2 );\n"
"\tif ( signtest < 0 ) then phase = -phase; end;\n"
"\tparm = roll_tilt * math.cos( phase );\n"
"\treturn math.deg(parm);\n"
"end;";

const char DNA_OPENING_PROGRAM[] = 
"function main()\n"
"\tr1, o1 = get_residue_transform( arg1 );\n"
"\tr2, o2 = get_residue_transform( arg2 );\n"
"\tr2[2] = -r2[2];\n"
"\tr2[3] = -r2[3];\n"
"\tr2[5] = -r2[5];\n"
"\tr2[6] = -r2[6];\n"
"\tr2[8] = -r2[8];\n"
"\tr2[9] = -r2[9];\n"
"\tz1 = vec3( r1[3], r1[6], r1[9] );\n"
"\tz2 = vec3( r2[3], r2[6], r2[9] );\n"
"\thinge = normalize( cross( z1, z2 ) );\n"
"\troll_tilt = math.acosn( dot( z1, z2 ) );\n"
"\tgrm1 = build_rotation_matrix( hinge, roll_tilt*0.5 );\n"
"\tgrm2 = build_rotation_matrix( hinge, roll_tilt*-0.5 );\n"
"\tr1a = grm1 * r1;\n"
"\tr2a = grm2 * r2;\n"
"\tr_average2 = vec3();\n"
"\tr_average2[1] = r1a[3] + r2a[3];\n"
"\tr_average2[2] = r1a[6] + r2a[6];\n"
"\tr_average2[3] = r1a[9] + r2a[9];\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tv1 = vec3( r1a[1], r1a[4], r1a[7] );\n"
"\tv2 = vec3( r2a[1], r2a[4], r2a[7] );\n"
"\topening = math.acosn( dot( v1, v2 ) );\n"
"\tv1 = vec3( r1a[2], r1a[5], r1a[8] );\n"
"\tv2 = vec3( r2a[2], r2a[5], r2a[8] );\n"
"\tsigntest = dot( cross( v1, v2 ), r_average2 );\n"
"\tif ( signtest < 0 ) then opening = -opening; end;\n"
"\treturn math.deg(opening);\n"
"end;";

const char DNA_SHIFT_PROGRAM[] = 
"function main()\n"
"\tr11, o11 = get_residue_transform( arg1 );\n"
"\tr21, o21 = get_residue_transform( arg2 );\n"
"\tr12, o12 = get_residue_transform( arg3 );\n"
"\tr22, o22 = get_residue_transform( arg4 );\n"
"\tr21[2] = -r21[2];\n"
"\tr21[3] = -r21[3];\n"
"\tr21[5] = -r21[5];\n"
"\tr21[6] = -r21[6];\n"
"\tr21[8] = -r21[8];\n"
"\tr21[9] = -r21[9];\n"
"\tr22[2] = -r22[2];\n"
"\tr22[3] = -r22[3];\n"
"\tr22[5] = -r22[5];\n"
"\tr22[6] = -r22[6];\n"
"\tr22[8] = -r22[8];\n"
"\tr22[9] = -r22[9];\n"
"\tz11 = vec3( r11[3], r11[6], r11[9] );\n"
"\tz21 = vec3( r21[3], r21[6], r21[9] );\n"
"\tz12 = vec3( r12[3], r12[6], r12[9] );\n"
"\tz22 = vec3( r22[3], r22[6], r22[9] );\n"
"\thinge1 = normalize( cross( z11, z21 ) );\n"
"\thinge2 = normalize( cross( z12, z22 ) );\n"
"\troll_tilt1 = math.acosn( dot( z11, z21 ) );\n"
"\troll_tilt2 = math.acosn( dot( z12, z22 ) );\n"
"\tgrm11 = build_rotation_matrix( hinge1, roll_tilt1 * 0.5 );\n"
"\tgrm21 = build_rotation_matrix( hinge1, roll_tilt1 * -0.5 );\n"
"\tgrm12 = build_rotation_matrix( hinge2, roll_tilt2 * 0.5 );\n"
"\tgrm22 = build_rotation_matrix( hinge2, roll_tilt2 * -0.5 );\n"
"\tr11a = grm11 * r11;\n"
"\tr21a = grm21 * r21;\n"
"\tr12a = grm12 * r12;\n"
"\tr22a = grm22 * r22;\n"
"\tom1 = (o11 + o21) * 0.5;\n"
"\tom2 = (o12 + o22) * 0.5;\n"
"\tr_average1 = vec3( r11a[1] + r21a[1], r11a[4] + r21a[4], r11a[7] + r21a[7] );\n"
"\tr_average1 = normalize( r_average1 );\n"
"\tr_average2 = vec3( r11a[2] + r21a[2], r11a[5] + r21a[5], r11a[8] + r21a[8] );\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tr_average3 = vec3( r11a[3] + r21a[3], r11a[6] + r21a[6], r11a[9] + r21a[9] );\n"
"\tr_average3 = normalize( r_average3 );\n"
"\trm1 = mat3( r_average1[1], r_average2[1], r_average3[1], r_average1[2], r_average2[2], r_average3[2], r_average1[3], r_average2[3], r_average3[3] );\n"
"\tr_average1 = vec3( r12a[1] + r22a[1], r12a[4] + r22a[4], r12a[7] + r22a[7] );\n"
"\tr_average1 = normalize( r_average1 );\n"
"\tr_average2 = vec3( r12a[2] + r22a[2], r12a[5] + r22a[5], r12a[8] + r22a[8] );\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tr_average3 = vec3( r12a[3] + r22a[3], r12a[6] + r22a[6], r12a[9] + r22a[9] );\n"
"\tr_average3 = normalize( r_average3 );\n"
"\trm2 = mat3( r_average1[1], r_average2[1], r_average3[1], r_average1[2], r_average2[2], r_average3[2], r_average1[3], r_average2[3], r_average3[3] );\n"
"\tz1 = vec3( rm1[3], rm1[6], rm1[9] );\n"
"\tz2 = vec3( rm2[3], rm2[6], rm2[9] );\n"
"\thinge = normalize( cross( z1, z2 ) );\n"
"\troll_tilt = math.acosn( dot( z1, z2 ) );\n"
"\tgrm1 = build_rotation_matrix( hinge, roll_tilt * 0.5 );\n"
"\tgrm2 = build_rotation_matrix( hinge, roll_tilt * -0.5 );\n"
"\trm1a = grm1 * rm1;\n"
"\trm2a = grm2 * rm2;\n"
"\tr_average = vec3();\n"
"\tr_average[1] = rm1a[1] + rm2a[1];\n"
"\tr_average[2] = rm1a[4] + rm2a[4];\n"
"\tr_average[3] = rm1a[7] + rm2a[7];\n"
"\tr_average = normalize( r_average );\n"
"\todiff = om2 - om1;\n"
"\tparm = dot( odiff, r_average );\n"
"\treturn parm;\n"
"end;";

const char DNA_SLIDE_PROGRAM[] = 
"function main()\n"
"\tr11, o11 = get_residue_transform( arg1 );\n"
"\tr21, o21 = get_residue_transform( arg2 );\n"
"\tr12, o12 = get_residue_transform( arg3 );\n"
"\tr22, o22 = get_residue_transform( arg4 );\n"
"\tr21[2] = -r21[2];\n"
"\tr21[3] = -r21[3];\n"
"\tr21[5] = -r21[5];\n"
"\tr21[6] = -r21[6];\n"
"\tr21[8] = -r21[8];\n"
"\tr21[9] = -r21[9];\n"
"\tr22[2] = -r22[2];\n"
"\tr22[3] = -r22[3];\n"
"\tr22[5] = -r22[5];\n"
"\tr22[6] = -r22[6];\n"
"\tr22[8] = -r22[8];\n"
"\tr22[9] = -r22[9];\n"
"\tz11 = vec3( r11[3], r11[6], r11[9] );\n"
"\tz21 = vec3( r21[3], r21[6], r21[9] );\n"
"\tz12 = vec3( r12[3], r12[6], r12[9] );\n"
"\tz22 = vec3( r22[3], r22[6], r22[9] );\n"
"\thinge1 = normalize( cross( z11, z21 ) );\n"
"\thinge2 = normalize( cross( z12, z22 ) );\n"
"\troll_tilt1 = math.acosn( dot( z11, z21 ) );\n"
"\troll_tilt2 = math.acosn( dot( z12, z22 ) );\n"
"\tgrm11 = build_rotation_matrix( hinge1, roll_tilt1 * 0.5 );\n"
"\tgrm21 = build_rotation_matrix( hinge1, roll_tilt1 * -0.5 );\n"
"\tgrm12 = build_rotation_matrix( hinge2, roll_tilt2 * 0.5 );\n"
"\tgrm22 = build_rotation_matrix( hinge2, roll_tilt2 * -0.5 );\n"
"\tr11a = grm11 * r11;\n"
"\tr21a = grm21 * r21;\n"
"\tr12a = grm12 * r12;\n"
"\tr22a = grm22 * r22;\n"
"\tom1 = (o11 + o21) * 0.5;\n"
"\tom2 = (o12 + o22) * 0.5;\n"
"\tr_average1 = vec3( r11a[1] + r21a[1], r11a[4] + r21a[4], r11a[7] + r21a[7] );\n"
"\tr_average1 = normalize( r_average1 );\n"
"\tr_average2 = vec3( r11a[2] + r21a[2], r11a[5] + r21a[5], r11a[8] + r21a[8] );\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tr_average3 = vec3( r11a[3] + r21a[3], r11a[6] + r21a[6], r11a[9] + r21a[9] );\n"
"\tr_average3 = normalize( r_average3 );\n"
"\trm1 = mat3( r_average1[1], r_average2[1], r_average3[1], r_average1[2], r_average2[2], r_average3[2], r_average1[3], r_average2[3], r_average3[3] );\n"
"\tr_average1 = vec3( r12a[1] + r22a[1], r12a[4] + r22a[4], r12a[7] + r22a[7] );\n"
"\tr_average1 = normalize( r_average1 );\n"
"\tr_average2 = vec3( r12a[2] + r22a[2], r12a[5] + r22a[5], r12a[8] + r22a[8] );\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tr_average3 = vec3( r12a[3] + r22a[3], r12a[6] + r22a[6], r12a[9] + r22a[9] );\n"
"\tr_average3 = normalize( r_average3 );\n"
"\trm2 = mat3( r_average1[1], r_average2[1], r_average3[1], r_average1[2], r_average2[2], r_average3[2], r_average1[3], r_average2[3], r_average3[3] );\n"
"\tz1 = vec3( rm1[3], rm1[6], rm1[9] );\n"
"\tz2 = vec3( rm2[3], rm2[6], rm2[9] );\n"
"\thinge = normalize( cross( z1, z2 ) );\n"
"\troll_tilt = math.acosn( dot( z1, z2 ) );\n"
"\tgrm1 = build_rotation_matrix( hinge, roll_tilt * 0.5 );\n"
"\tgrm2 = build_rotation_matrix( hinge, roll_tilt * -0.5 );\n"
"\trm1a = grm1 * rm1;\n"
"\trm2a = grm2 * rm2;\n"
"\tr_average = vec3();\n"
"\tr_average[1] = rm1a[2] + rm2a[2];\n"
"\tr_average[2] = rm1a[5] + rm2a[5];\n"
"\tr_average[3] = rm1a[8] + rm2a[8];\n"
"\tr_average = normalize( r_average );\n"
"\todiff = om2 - om1;\n"
"\tparm = dot( odiff, r_average );\n"
"\treturn parm;\n"
"end;";

const char DNA_RISE_PROGRAM[] = 
"function main()\n"
"\tr11, o11 = get_residue_transform( arg1 );\n"
"\tr21, o21 = get_residue_transform( arg2 );\n"
"\tr12, o12 = get_residue_transform( arg3 );\n"
"\tr22, o22 = get_residue_transform( arg4 );\n"
"\tr21[2] = -r21[2];\n"
"\tr21[3] = -r21[3];\n"
"\tr21[5] = -r21[5];\n"
"\tr21[6] = -r21[6];\n"
"\tr21[8] = -r21[8];\n"
"\tr21[9] = -r21[9];\n"
"\tr22[2] = -r22[2];\n"
"\tr22[3] = -r22[3];\n"
"\tr22[5] = -r22[5];\n"
"\tr22[6] = -r22[6];\n"
"\tr22[8] = -r22[8];\n"
"\tr22[9] = -r22[9];\n"
"\tz11 = vec3( r11[3], r11[6], r11[9] );\n"
"\tz21 = vec3( r21[3], r21[6], r21[9] );\n"
"\tz12 = vec3( r12[3], r12[6], r12[9] );\n"
"\tz22 = vec3( r22[3], r22[6], r22[9] );\n"
"\thinge1 = normalize( cross( z11, z21 ) );\n"
"\thinge2 = normalize( cross( z12, z22 ) );\n"
"\troll_tilt1 = math.acosn( dot( z11, z21 ) );\n"
"\troll_tilt2 = math.acosn( dot( z12, z22 ) );\n"
"\tgrm11 = build_rotation_matrix( hinge1, roll_tilt1 * 0.5 );\n"
"\tgrm21 = build_rotation_matrix( hinge1, roll_tilt1 * -0.5 );\n"
"\tgrm12 = build_rotation_matrix( hinge2, roll_tilt2 * 0.5 );\n"
"\tgrm22 = build_rotation_matrix( hinge2, roll_tilt2 * -0.5 );\n"
"\tr11a = grm11 * r11;\n"
"\tr21a = grm21 * r21;\n"
"\tr12a = grm12 * r12;\n"
"\tr22a = grm22 * r22;\n"
"\tom1 = (o11 + o21) * 0.5;\n"
"\tom2 = (o12 + o22) * 0.5;\n"
"\tr_average1 = vec3( r11a[1] + r21a[1], r11a[4] + r21a[4], r11a[7] + r21a[7] );\n"
"\tr_average1 = normalize( r_average1 );\n"
"\tr_average2 = vec3( r11a[2] + r21a[2], r11a[5] + r21a[5], r11a[8] + r21a[8] );\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tr_average3 = vec3( r11a[3] + r21a[3], r11a[6] + r21a[6], r11a[9] + r21a[9] );\n"
"\tr_average3 = normalize( r_average3 );\n"
"\trm1 = mat3( r_average1[1], r_average2[1], r_average3[1], r_average1[2], r_average2[2], r_average3[2], r_average1[3], r_average2[3], r_average3[3] );\n"
"\tr_average1 = vec3( r12a[1] + r22a[1], r12a[4] + r22a[4], r12a[7] + r22a[7] );\n"
"\tr_average1 = normalize( r_average1 );\n"
"\tr_average2 = vec3( r12a[2] + r22a[2], r12a[5] + r22a[5], r12a[8] + r22a[8] );\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tr_average3 = vec3( r12a[3] + r22a[3], r12a[6] + r22a[6], r12a[9] + r22a[9] );\n"
"\tr_average3 = normalize( r_average3 );\n"
"\trm2 = mat3( r_average1[1], r_average2[1], r_average3[1], r_average1[2], r_average2[2], r_average3[2], r_average1[3], r_average2[3], r_average3[3] );\n"
"\tz1 = vec3( rm1[3], rm1[6], rm1[9] );\n"
"\tz2 = vec3( rm2[3], rm2[6], rm2[9] );\n"
"\thinge = normalize( cross( z1, z2 ) );\n"
"\troll_tilt = math.acosn( dot( z1, z2 ) );\n"
"\tgrm1 = build_rotation_matrix( hinge, roll_tilt * 0.5 );\n"
"\tgrm2 = build_rotation_matrix( hinge, roll_tilt * -0.5 );\n"
"\trm1a = grm1 * rm1;\n"
"\trm2a = grm2 * rm2;\n"
"\tr_average = vec3();\n"
"\tr_average[1] = rm1a[3] + rm2a[3];\n"
"\tr_average[2] = rm1a[6] + rm2a[6];\n"
"\tr_average[3] = rm1a[9] + rm2a[9];\n"
"\tr_average = normalize( r_average );\n"
"\todiff = om2 - om1;\n"
"\tparm = dot( odiff, r_average );\n"
"\treturn parm;\n"
"end;";

const char DNA_TILT_PROGRAM[] = 
"function main()\n"
"\tr11, o11 = get_residue_transform( arg1 );\n"
"\tr21, o21 = get_residue_transform( arg2 );\n"
"\tr12, o12 = get_residue_transform( arg3 );\n"
"\tr22, o22 = get_residue_transform( arg4 );\n"
"\tr21[2] = -r21[2];\n"
"\tr21[3] = -r21[3];\n"
"\tr21[5] = -r21[5];\n"
"\tr21[6] = -r21[6];\n"
"\tr21[8] = -r21[8];\n"
"\tr21[9] = -r21[9];\n"
"\tr22[2] = -r22[2];\n"
"\tr22[3] = -r22[3];\n"
"\tr22[5] = -r22[5];\n"
"\tr22[6] = -r22[6];\n"
"\tr22[8] = -r22[8];\n"
"\tr22[9] = -r22[9];\n"
"\tz11 = vec3( r11[3], r11[6], r11[9] );\n"
"\tz21 = vec3( r21[3], r21[6], r21[9] );\n"
"\tz12 = vec3( r12[3], r12[6], r12[9] );\n"
"\tz22 = vec3( r22[3], r22[6], r22[9] );\n"
"\thinge1 = normalize( cross( z11, z21 ) );\n"
"\thinge2 = normalize( cross( z12, z22 ) );\n"
"\troll_tilt1 = math.acosn( dot( z11, z21 ) );\n"
"\troll_tilt2 = math.acosn( dot( z12, z22 ) );\n"
"\tgrm11 = build_rotation_matrix( hinge1, roll_tilt1 * 0.5 );\n"
"\tgrm21 = build_rotation_matrix( hinge1, roll_tilt1 * -0.5 );\n"
"\tgrm12 = build_rotation_matrix( hinge2, roll_tilt2 * 0.5 );\n"
"\tgrm22 = build_rotation_matrix( hinge2, roll_tilt2 * -0.5 );\n"
"\tr11a = grm11 * r11;\n"
"\tr21a = grm21 * r21;\n"
"\tr12a = grm12 * r12;\n"
"\tr22a = grm22 * r22;\n"
"\tom1 = (o11 + o21) * 0.5;\n"
"\tom2 = (o12 + o22) * 0.5;\n"
"\tr_average1 = vec3( r11a[1] + r21a[1], r11a[4] + r21a[4], r11a[7] + r21a[7] );\n"
"\tr_average1 = normalize( r_average1 );\n"
"\tr_average2 = vec3( r11a[2] + r21a[2], r11a[5] + r21a[5], r11a[8] + r21a[8] );\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tr_average3 = vec3( r11a[3] + r21a[3], r11a[6] + r21a[6], r11a[9] + r21a[9] );\n"
"\tr_average3 = normalize( r_average3 );\n"
"\trm1 = mat3( r_average1[1], r_average2[1], r_average3[1], r_average1[2], r_average2[2], r_average3[2], r_average1[3], r_average2[3], r_average3[3] );\n"
"\tr_average1 = vec3( r12a[1] + r22a[1], r12a[4] + r22a[4], r12a[7] + r22a[7] );\n"
"\tr_average1 = normalize( r_average1 );\n"
"\tr_average2 = vec3( r12a[2] + r22a[2], r12a[5] + r22a[5], r12a[8] + r22a[8] );\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tr_average3 = vec3( r12a[3] + r22a[3], r12a[6] + r22a[6], r12a[9] + r22a[9] );\n"
"\tr_average3 = normalize( r_average3 );\n"
"\trm2 = mat3( r_average1[1], r_average2[1], r_average3[1], r_average1[2], r_average2[2], r_average3[2], r_average1[3], r_average2[3], r_average3[3] );\n"
"\tz1 = vec3( rm1[3], rm1[6], rm1[9] );\n"
"\tz2 = vec3( rm2[3], rm2[6], rm2[9] );\n"
"\thinge = normalize( cross( z1, z2 ) );\n"
"\troll_tilt = math.acosn( dot( z1, z2 ) );\n"
"\tgrm1 = build_rotation_matrix( hinge, roll_tilt * 0.5 );\n"
"\tgrm2 = build_rotation_matrix( hinge, roll_tilt * -0.5 );\n"
"\trm1a = grm1 * rm1;\n"
"\trm2a = grm2 * rm2;\n"
"\tr_average = vec3();\n"
"\tr_average[1] = rm1a[2] + rm2a[2];\n"
"\tr_average[2] = rm1a[5] + rm2a[5];\n"
"\tr_average[3] = rm1a[8] + rm2a[8];\n"
"\tr_average = normalize( r_average );\n"
"\tr_average2 = vec3();\n"
"\tr_average2[1] = rm1a[3] + rm2a[3];\n"
"\tr_average2[2] = rm1a[6] + rm2a[6];\n"
"\tr_average2[3] = rm1a[9] + rm2a[9];\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tphase = math.acosn( dot( hinge, r_average ) );\n"
"\tv1 = vec3( rm1a[2], rm1a[5], rm1a[8] );\n"
"\tv2 = vec3( rm2a[2], rm2a[5], rm2a[8] );\n"
"\tsigntest = dot( cross( v1, v2 ), r_average2 );\n"
"\tif ( signtest < 0 ) then phase = -phase; end;\n"
"\tparm = roll_tilt * math.sin( phase );\n"
"\treturn math.deg(parm);\n"
"end;";

const char DNA_ROLL_PROGRAM[] = 
"function main()\n"
"\tr11, o11 = get_residue_transform( arg1 );\n"
"\tr21, o21 = get_residue_transform( arg2 );\n"
"\tr12, o12 = get_residue_transform( arg3 );\n"
"\tr22, o22 = get_residue_transform( arg4 );\n"
"\tr21[2] = -r21[2];\n"
"\tr21[3] = -r21[3];\n"
"\tr21[5] = -r21[5];\n"
"\tr21[6] = -r21[6];\n"
"\tr21[8] = -r21[8];\n"
"\tr21[9] = -r21[9];\n"
"\tr22[2] = -r22[2];\n"
"\tr22[3] = -r22[3];\n"
"\tr22[5] = -r22[5];\n"
"\tr22[6] = -r22[6];\n"
"\tr22[8] = -r22[8];\n"
"\tr22[9] = -r22[9];\n"
"\tz11 = vec3( r11[3], r11[6], r11[9] );\n"
"\tz21 = vec3( r21[3], r21[6], r21[9] );\n"
"\tz12 = vec3( r12[3], r12[6], r12[9] );\n"
"\tz22 = vec3( r22[3], r22[6], r22[9] );\n"
"\thinge1 = normalize( cross( z11, z21 ) );\n"
"\thinge2 = normalize( cross( z12, z22 ) );\n"
"\troll_tilt1 = math.acosn( dot( z11, z21 ) );\n"
"\troll_tilt2 = math.acosn( dot( z12, z22 ) );\n"
"\tgrm11 = build_rotation_matrix( hinge1, roll_tilt1 * 0.5 );\n"
"\tgrm21 = build_rotation_matrix( hinge1, roll_tilt1 * -0.5 );\n"
"\tgrm12 = build_rotation_matrix( hinge2, roll_tilt2 * 0.5 );\n"
"\tgrm22 = build_rotation_matrix( hinge2, roll_tilt2 * -0.5 );\n"
"\tr11a = grm11 * r11;\n"
"\tr21a = grm21 * r21;\n"
"\tr12a = grm12 * r12;\n"
"\tr22a = grm22 * r22;\n"
"\tom1 = (o11 + o21) * 0.5;\n"
"\tom2 = (o12 + o22) * 0.5;\n"
"\tr_average1 = vec3( r11a[1] + r21a[1], r11a[4] + r21a[4], r11a[7] + r21a[7] );\n"
"\tr_average1 = normalize( r_average1 );\n"
"\tr_average2 = vec3( r11a[2] + r21a[2], r11a[5] + r21a[5], r11a[8] + r21a[8] );\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tr_average3 = vec3( r11a[3] + r21a[3], r11a[6] + r21a[6], r11a[9] + r21a[9] );\n"
"\tr_average3 = normalize( r_average3 );\n"
"\trm1 = mat3( r_average1[1], r_average2[1], r_average3[1], r_average1[2], r_average2[2], r_average3[2], r_average1[3], r_average2[3], r_average3[3] );\n"
"\tr_average1 = vec3( r12a[1] + r22a[1], r12a[4] + r22a[4], r12a[7] + r22a[7] );\n"
"\tr_average1 = normalize( r_average1 );\n"
"\tr_average2 = vec3( r12a[2] + r22a[2], r12a[5] + r22a[5], r12a[8] + r22a[8] );\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tr_average3 = vec3( r12a[3] + r22a[3], r12a[6] + r22a[6], r12a[9] + r22a[9] );\n"
"\tr_average3 = normalize( r_average3 );\n"
"\trm2 = mat3( r_average1[1], r_average2[1], r_average3[1], r_average1[2], r_average2[2], r_average3[2], r_average1[3], r_average2[3], r_average3[3] );\n"
"\tz1 = vec3( rm1[3], rm1[6], rm1[9] );\n"
"\tz2 = vec3( rm2[3], rm2[6], rm2[9] );\n"
"\thinge = normalize( cross( z1, z2 ) );\n"
"\troll_tilt = math.acosn( dot( z1, z2 ) );\n"
"\tgrm1 = build_rotation_matrix( hinge, roll_tilt * 0.5 );\n"
"\tgrm2 = build_rotation_matrix( hinge, roll_tilt * -0.5 );\n"
"\trm1a = grm1 * rm1;\n"
"\trm2a = grm2 * rm2;\n"
"\tr_average = vec3();\n"
"\tr_average[1] = rm1a[2] + rm2a[2];\n"
"\tr_average[2] = rm1a[5] + rm2a[5];\n"
"\tr_average[3] = rm1a[8] + rm2a[8];\n"
"\tr_average = normalize( r_average );\n"
"\tr_average2 = vec3();\n"
"\tr_average2[1] = rm1a[3] + rm2a[3];\n"
"\tr_average2[2] = rm1a[6] + rm2a[6];\n"
"\tr_average2[3] = rm1a[9] + rm2a[9];\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tphase = math.acosn( dot( hinge, r_average ) );\n"
"\tv1 = vec3( rm1a[2], rm1a[5], rm1a[8] );\n"
"\tv2 = vec3( rm2a[2], rm2a[5], rm2a[8] );\n"
"\tsigntest = dot( cross( v1, v2 ), r_average2 );\n"
"\tif ( signtest < 0 ) then phase = -phase; end;\n"
"\tparm = roll_tilt * math.cos( phase );\n"
"\treturn math.deg(parm);\n"
"end;";

const char DNA_TWIST_PROGRAM[] = 
"function main()\n"
"\tr11, o11 = get_residue_transform( arg1 );\n"
"\tr21, o21 = get_residue_transform( arg2 );\n"
"\tr12, o12 = get_residue_transform( arg3 );\n"
"\tr22, o22 = get_residue_transform( arg4 );\n"
"\tr21[2] = -r21[2];\n"
"\tr21[3] = -r21[3];\n"
"\tr21[5] = -r21[5];\n"
"\tr21[6] = -r21[6];\n"
"\tr21[8] = -r21[8];\n"
"\tr21[9] = -r21[9];\n"
"\tr22[2] = -r22[2];\n"
"\tr22[3] = -r22[3];\n"
"\tr22[5] = -r22[5];\n"
"\tr22[6] = -r22[6];\n"
"\tr22[8] = -r22[8];\n"
"\tr22[9] = -r22[9];\n"
"\tz11 = vec3( r11[3], r11[6], r11[9] );\n"
"\tz21 = vec3( r21[3], r21[6], r21[9] );\n"
"\tz12 = vec3( r12[3], r12[6], r12[9] );\n"
"\tz22 = vec3( r22[3], r22[6], r22[9] );\n"
"\thinge1 = normalize( cross( z11, z21 ) );\n"
"\thinge2 = normalize( cross( z12, z22 ) );\n"
"\troll_tilt1 = math.acosn( dot( z11, z21 ) );\n"
"\troll_tilt2 = math.acosn( dot( z12, z22 ) );\n"
"\tgrm11 = build_rotation_matrix( hinge1, roll_tilt1 * 0.5 );\n"
"\tgrm21 = build_rotation_matrix( hinge1, roll_tilt1 * -0.5 );\n"
"\tgrm12 = build_rotation_matrix( hinge2, roll_tilt2 * 0.5 );\n"
"\tgrm22 = build_rotation_matrix( hinge2, roll_tilt2 * -0.5 );\n"
"\tr11a = grm11 * r11;\n"
"\tr21a = grm21 * r21;\n"
"\tr12a = grm12 * r12;\n"
"\tr22a = grm22 * r22;\n"
"\tom1 = (o11 + o21) * 0.5;\n"
"\tom2 = (o12 + o22) * 0.5;\n"
"\tr_average1 = vec3( r11a[1] + r21a[1], r11a[4] + r21a[4], r11a[7] + r21a[7] );\n"
"\tr_average1 = normalize( r_average1 );\n"
"\tr_average2 = vec3( r11a[2] + r21a[2], r11a[5] + r21a[5], r11a[8] + r21a[8] );\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tr_average3 = vec3( r11a[3] + r21a[3], r11a[6] + r21a[6], r11a[9] + r21a[9] );\n"
"\tr_average3 = normalize( r_average3 );\n"
"\trm1 = mat3( r_average1[1], r_average2[1], r_average3[1], r_average1[2], r_average2[2], r_average3[2], r_average1[3], r_average2[3], r_average3[3] );\n"
"\tr_average1 = vec3( r12a[1] + r22a[1], r12a[4] + r22a[4], r12a[7] + r22a[7] );\n"
"\tr_average1 = normalize( r_average1 );\n"
"\tr_average2 = vec3( r12a[2] + r22a[2], r12a[5] + r22a[5], r12a[8] + r22a[8] );\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tr_average3 = vec3( r12a[3] + r22a[3], r12a[6] + r22a[6], r12a[9] + r22a[9] );\n"
"\tr_average3 = normalize( r_average3 );\n"
"\trm2 = mat3( r_average1[1], r_average2[1], r_average3[1], r_average1[2], r_average2[2], r_average3[2], r_average1[3], r_average2[3], r_average3[3] );\n"
"\tz1 = vec3( rm1[3], rm1[6], rm1[9] );\n"
"\tz2 = vec3( rm2[3], rm2[6], rm2[9] );\n"
"\thinge = normalize( cross( z1, z2 ) );\n"
"\troll_tilt = math.acosn( dot( z1, z2 ) );\n"
"\tgrm1 = build_rotation_matrix( hinge, roll_tilt * 0.5 );\n"
"\tgrm2 = build_rotation_matrix( hinge, roll_tilt * -0.5 );\n"
"\trm1a = grm1 * rm1;\n"
"\trm2a = grm2 * rm2;\n"
"\tr_average2 = vec3();\n"
"\tr_average2[1] = rm1a[3] + rm2a[3];\n"
"\tr_average2[2] = rm1a[6] + rm2a[6];\n"
"\tr_average2[3] = rm1a[9] + rm2a[9];\n"
"\tr_average2 = normalize( r_average2 );\n"
"\tv1 = vec3( rm1a[1], rm1a[4], rm1a[7] );\n"
"\tv2 = vec3( rm2a[1], rm2a[4], rm2a[7] );\n"
"\ttwist = math.acosn( dot( v1, v2 ) );\n"
"\tv1 = vec3( rm1a[2], rm1a[5], rm1a[8] );\n"
"\tv2 = vec3( rm2a[2], rm2a[5], rm2a[8] );\n"
"\tsigntest = dot( cross( v1, v2 ), r_average2 );\n"
"\tif ( signtest < 0 ) then twist = -twist; end;\n"
"\treturn math.deg(twist);\n"
"end;";

#endif //MDTRA_DNADICKERSONPROGS_H