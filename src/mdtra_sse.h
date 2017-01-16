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
#ifndef MDTRA_SSE_H
#define MDTRA_SSE_H

#if defined(MDTRA_ALLOW_SSE)
#if defined(WIN32)

#define SSE_PDB_MOVE_TO_CENTROID(pdbc, ox, mx, pf)	\
		__asm	mov		esi, DWORD PTR[this] \
		__asm 	mov		ecx, DWORD PTR[esi].m_iNumAtoms \
		__asm 	mov		edx, DWORD PTR[esi].m_pAtoms \
		__asm 	xor		eax, eax \
		__asm 	movaps	xmm0, xmmword ptr[centroid_origin] \
		__asm add_more: \
		__asm 	test	dword ptr[edx+eax].atomFlags, pf \
		__asm 	jz		add_more_skip \
		__asm 	movaps	xmm1, xmmword ptr[edx+eax].ox \
		__asm 	addps	xmm0, xmm1 \
		__asm add_more_skip: \
		__asm 	add		eax, sizeof_pdb_atom \
		__asm 	loop	add_more \
		__asm 	movaps	xmm1, xmmword ptr[inv_num_atoms_vec] \
		__asm 	mulps	xmm0, xmm1 \
		__asm 	movaps	xmmword ptr[centroid_origin], xmm0 \
		__asm 	mov		ecx, DWORD PTR[esi].m_iNumAtoms \
		__asm 	xor		eax, eax \
		__asm sub_more: \
		__asm 	movaps	xmm1, xmmword ptr[edx+eax].ox \
		__asm 	subps	xmm1, xmm0 \
		__asm 	movaps  xmmword ptr[edx+eax].mx, xmm1 \
		__asm 	add		eax, sizeof_pdb_atom \
		__asm 	loop	sub_more

#define SSE_PDB_CALC_RTR_MATRIX(pdbc, ox, pf)	\
		__asm	mov		esi, DWORD PTR[pOther] \
		__asm	mov		edi, DWORD PTR[esi].m_pAtoms \
		__asm	mov		esi, DWORD PTR[this] \
		__asm	mov		ecx, DWORD PTR[esi].m_iNumAtoms \
		__asm	mov		edx, DWORD PTR[esi].m_pAtoms \
		__asm	xor		eax, eax \
		__asm	xorps	xmm0, xmm0 \
		__asm	movaps	xmm1, xmm0 \
		__asm	movaps	xmm2, xmm0 \
		__asm add_more: \
		__asm	test	dword ptr[edx+eax].atomFlags, pf \
		__asm	jz		add_more_skip \
		__asm	movaps	xmm3, xmmword ptr[edx+eax].ox \
		__asm	movaps	xmm4, xmmword ptr[edi+eax].ox \
		__asm	movaps	xmm5, xmm3 \
		__asm	shufps	xmm5, xmm5, 00000000b \
		__asm	mulps	xmm5, xmm4 \
		__asm	addps	xmm0, xmm5 \
		__asm	movaps	xmm5, xmm3 \
		__asm	shufps	xmm5, xmm5, 01010101b \
		__asm	mulps	xmm5, xmm4 \
		__asm	addps	xmm1, xmm5 \
		__asm	movaps	xmm5, xmm3 \
		__asm	shufps	xmm5, xmm5, 10101010b \
		__asm	mulps	xmm5, xmm4 \
		__asm	addps	xmm2, xmm5 \
		__asm add_more_skip: \
		__asm	add		eax, sizeof_pdb_atom \
		__asm	loop	add_more \
		__asm	movaps  xmmword ptr[rMatrix+0], xmm0 \
		__asm	movaps  xmmword ptr[rMatrix+16], xmm1 \
		__asm	movaps  xmmword ptr[rMatrix+32], xmm2 \
		__asm	movaps	xmm4, xmm0 \
		__asm	shufps	xmm4, xmm4, 00000000b \
		__asm	mulps	xmm4, xmm0 \
		__asm	movaps	xmm3, xmm4 \
		__asm	movaps	xmm4, xmm1 \
		__asm	shufps	xmm4, xmm4, 00000000b \
		__asm	mulps	xmm4, xmm1 \
		__asm	addps	xmm3, xmm4 \
		__asm	movaps	xmm4, xmm2 \
		__asm	shufps	xmm4, xmm4, 00000000b \
		__asm	mulps	xmm4, xmm2 \
		__asm	addps	xmm3, xmm4 \
		__asm	movaps  xmmword ptr[rtrMatrix+0], xmm3 \
		__asm	movaps	xmm4, xmm0 \
		__asm	shufps	xmm4, xmm4, 01010101b \
		__asm	mulps	xmm4, xmm0 \
		__asm	movaps	xmm3, xmm4 \
		__asm	movaps	xmm4, xmm1 \
		__asm	shufps	xmm4, xmm4, 01010101b \
		__asm	mulps	xmm4, xmm1 \
		__asm	addps	xmm3, xmm4 \
		__asm	movaps	xmm4, xmm2 \
		__asm	shufps	xmm4, xmm4, 01010101b \
		__asm	mulps	xmm4, xmm2 \
		__asm	addps	xmm3, xmm4 \
		__asm	movaps  xmmword ptr[rtrMatrix+16], xmm3 \
		__asm	movaps	xmm4, xmm0 \
		__asm	shufps	xmm4, xmm4, 10101010b \
		__asm	mulps	xmm4, xmm0 \
		__asm	movaps	xmm3, xmm4 \
		__asm	movaps	xmm4, xmm1 \
		__asm	shufps	xmm4, xmm4, 10101010b \
		__asm	mulps	xmm4, xmm1 \
		__asm	addps	xmm3, xmm4 \
		__asm	movaps	xmm4, xmm2 \
		__asm	shufps	xmm4, xmm4, 10101010b \
		__asm	mulps	xmm4, xmm2 \
		__asm	addps	xmm3, xmm4 \
		__asm	movaps  xmmword ptr[rtrMatrix+32], xmm3

#define SSE_PDB_CALC_EVEC2()	\
		__asm	movaps	xmm0, xmmword ptr[eVec+0]	\
		__asm	movaps	xmm1, xmmword ptr[eVec+16]	\
		__asm	movaps	xmm2, xmm0	\
		__asm	movaps	xmm3, xmm1	\
		__asm	shufps	xmm0, xmm0, 11001001b	\
		__asm	shufps	xmm1, xmm1, 11010010b	\
		__asm	mulps	xmm0, xmm1	\
		__asm	shufps	xmm2, xmm2, 11010010b	\
		__asm	shufps	xmm3, xmm3, 11001001b	\
		__asm	mulps	xmm2, xmm3	\
		__asm	subps	xmm0, xmm2	\
		__asm	movaps	xmmword ptr[eVec+32], xmm0

#define SSE_PDB_CALC_RMSD(pdbc, ox, pf)	\
		__asm	mov		esi, DWORD PTR[pOther]	\
		__asm	mov		edi, DWORD PTR[esi].m_pAtoms	\
		__asm	mov		esi, DWORD PTR[this]	\
		__asm	mov		ecx, DWORD PTR[esi].m_iNumAtoms	\
		__asm	mov		edx, DWORD PTR[esi].m_pAtoms	\
		__asm	xor		eax, eax	\
		__asm	xorps	xmm0, xmm0	\
		__asm add_more:	\
		__asm	test	dword ptr[edx+eax].atomFlags, pf	\
		__asm	jz		add_more_skip	\
		__asm	movaps	xmm1, xmmword ptr[edx+eax].ox	\
		__asm	movaps	xmm2, xmmword ptr[edi+eax].ox	\
		__asm	subps	xmm2, xmm1	\
		__asm	mulps	xmm2, xmm2	\
		__asm	movaps	xmm1, xmm2	\
		__asm	shufps	xmm1, xmm1, 11100101b	\
		__asm	addss	xmm2, xmm1	\
		__asm	shufps	xmm1, xmm1, 11100110b	\
		__asm	addss	xmm2, xmm1	\
		__asm	addps	xmm0, xmm2	\
		__asm add_more_skip:	\
		__asm	add		eax, sizeof_pdb_atom	\
		__asm	loop	add_more	\
		__asm	movss	xmm1, xmmword ptr[inv_num_atoms]	\
		__asm	mulss	xmm0, xmm1	\
		__asm	sqrtss	xmm0, xmm0	\
		__asm	movss   xmmword ptr[flRMSD], xmm0

#elif defined(LINUX)

#define SSE_PDB_MOVE_TO_CENTROID(pdbc, ox, mx, pf)	\
		__asm__ __volatile__( \
			"movl		%1, %%ecx\n\t" \
			"movl		%4, %%edx\n\t" \
			"xorl		%%eax, %%eax\n\t" \
			"movaps		%2, %%xmm0\n\t" \
			"1:\n\t" \
			"movl		%3, %%esi\n\t" \
			"movl		(%%esi,%%eax), %%esi\n\t" \
			"testl		%6, %%esi\n\t" \
			"jz			2f\n\t" \
			"movaps		(%%edx,%%eax), %%xmm1\n\t" \
			"addps		%%xmm1, %%xmm0\n\t" \
			"2:\n\t" \
			"addl		%7, %%eax\n\t" \
			"loop		1b\n\t" \
			"movaps		%8, %%xmm1\n\t" \
			"mulps		%%xmm1, %%xmm0\n\t" \
			"movaps		%%xmm0, %0\n\t" \
			"movl		%1, %%ecx\n\t" \
			"xorl		%%eax, %%eax\n\t" \
			"3:\n\t" \
			"movl		%5, %%esi\n\t" \
			"movaps		(%%edx,%%eax), %%xmm1\n\t" \
			"subps		%%xmm0, %%xmm1\n\t" \
			"movaps		%%xmm1, (%%esi,%%eax)\n\t" \
			"addl		%7, %%eax\n\t" \
			"loop		3b\n\t" \
			: "=m"(centroid_origin) \
			: "m"(m_iNumAtoms), \
			  "m"(*centroid_origin), \
			  "g"((char*)m_pAtoms+offsetof(pdbc,atomFlags)), \
			  "g"((char*)m_pAtoms+offsetof(pdbc,ox)), \
			  "g"((char*)m_pAtoms+offsetof(pdbc,mx)), \
			  "i"(pf), \
			  "m"(sizeof_pdb_atom), \
			  "m"(*inv_num_atoms_vec) \
			: "%eax", "%ecx", "%edx", "%esi", "memory" \
		)

#define SSE_PDB_CALC_RTR_MATRIX(pdbc, ox, pf)	\
		__asm__ __volatile__( \
			"movl		%3, %%ecx\n\t" \
			"xorl		%%eax, %%eax\n\t" \
			"xorps		%%xmm0, %%xmm0\n\t" \
			"movaps		%%xmm0, %%xmm1\n\t" \
			"movaps		%%xmm0, %%xmm2\n\t" \
			"1:\n\t" \
			"movl		%4, %%esi\n\t" \
			"movl		(%%esi,%%eax), %%esi\n\t" \
			"testl		%7, %%esi\n\t" \
			"jz			2f\n\t" \
			"movl		%5, %%esi\n\t" \
			"movaps		(%%esi,%%eax), %%xmm3\n\t" \
			"movl		%6, %%esi\n\t" \
			"movaps		(%%esi,%%eax), %%xmm4\n\t" \
			"movaps		%%xmm3, %%xmm5\n\t" \
			"shufps		$0x0, %%xmm5, %%xmm5\n\t" \
			"mulps		%%xmm4, %%xmm5\n\t" \
			"addps		%%xmm5, %%xmm0\n\t" \
			"movaps		%%xmm3, %%xmm5\n\t" \
			"shufps		$0x55, %%xmm5, %%xmm5\n\t" \
			"mulps		%%xmm4, %%xmm5\n\t" \
			"addps		%%xmm5, %%xmm1\n\t" \
			"movaps		%%xmm3, %%xmm5\n\t" \
			"shufps		$0xAA, %%xmm5, %%xmm5\n\t" \
			"mulps		%%xmm4, %%xmm5\n\t" \
			"addps		%%xmm5, %%xmm2\n\t" \
			"2:\n\t" \
			"addl		%8, %%eax\n\t" \
			"loop		1b\n\t" \
			"movaps		%%xmm0, %0\n\t" \
			"movaps		%%xmm1, %1\n\t" \
			"movaps		%%xmm2, %2\n\t" \
			: "=m"(rMatrix[0]), \
			  "=m"(rMatrix[4]), \
			  "=m"(rMatrix[8]) \
			: "m"(m_iNumAtoms), \
			  "g"((char*)m_pAtoms+offsetof(pdbc,atomFlags)), \
			  "g"((char*)m_pAtoms+offsetof(pdbc,ox)), \
			  "g"((char*)pOther->m_pAtoms+offsetof(pdbc,ox)), \
			  "i"(pf), \
			  "m"(sizeof_pdb_atom) \
			: "%eax", "%ecx", "%esi", "memory" \
		); \
		__asm__ __volatile__( \
			"movaps		%3, %%xmm0\n\t" \
			"movaps		%4, %%xmm1\n\t" \
			"movaps		%5, %%xmm2\n\t" \
			"movaps		%%xmm0, %%xmm4\n\t" \
			"shufps		$0x0, %%xmm4, %%xmm4\n\t" \
			"mulps		%%xmm0, %%xmm4\n\t" \
			"movaps		%%xmm4, %%xmm3\n\t" \
			"movaps		%%xmm1, %%xmm4\n\t" \
			"shufps		$0x0, %%xmm4, %%xmm4\n\t" \
			"mulps		%%xmm1, %%xmm4\n\t" \
			"addps		%%xmm4, %%xmm3\n\t" \
			"movaps		%%xmm2, %%xmm4\n\t" \
			"shufps		$0x0, %%xmm4, %%xmm4\n\t" \
			"mulps		%%xmm2, %%xmm4\n\t" \
			"addps		%%xmm4, %%xmm3\n\t" \
			"movaps		%%xmm3, %0\n\t" \
			"movaps		%%xmm0, %%xmm4\n\t" \
			"shufps		$0x55, %%xmm4, %%xmm4\n\t" \
			"mulps		%%xmm0, %%xmm4\n\t" \
			"movaps		%%xmm4, %%xmm3\n\t" \
			"movaps		%%xmm1, %%xmm4\n\t" \
			"shufps		$0x55, %%xmm4, %%xmm4\n\t" \
			"mulps		%%xmm1, %%xmm4\n\t" \
			"addps		%%xmm4, %%xmm3\n\t" \
			"movaps		%%xmm2, %%xmm4\n\t" \
			"shufps		$0x55, %%xmm4, %%xmm4\n\t" \
			"mulps		%%xmm2, %%xmm4\n\t" \
			"addps		%%xmm4, %%xmm3\n\t" \
			"movaps		%%xmm3, %1\n\t" \
			"movaps		%%xmm0, %%xmm4\n\t" \
			"shufps		$0xAA, %%xmm4, %%xmm4\n\t" \
			"mulps		%%xmm0, %%xmm4\n\t" \
			"movaps		%%xmm4, %%xmm3\n\t" \
			"movaps		%%xmm1, %%xmm4\n\t" \
			"shufps		$0xAA, %%xmm4, %%xmm4\n\t" \
			"mulps		%%xmm1, %%xmm4\n\t" \
			"addps		%%xmm4, %%xmm3\n\t" \
			"movaps		%%xmm2, %%xmm4\n\t" \
			"shufps		$0xAA, %%xmm4, %%xmm4\n\t" \
			"mulps		%%xmm2, %%xmm4\n\t" \
			"addps		%%xmm4, %%xmm3\n\t" \
			"movaps		%%xmm3, %2\n\t" \
			: "=m"(rtrMatrix[0]), \
			  "=m"(rtrMatrix[4]), \
			  "=m"(rtrMatrix[8]) \
			: "m"(rMatrix[0]), \
			  "m"(rMatrix[4]), \
			  "m"(rMatrix[8]) \
			: "memory" \
		)

#define SSE_PDB_CALC_EVEC2()	\
		__asm__ __volatile__( \
			"movaps	%1, %%xmm0\n\t" \
			"movaps	%2, %%xmm1\n\t" \
			"movaps	%%xmm0, %%xmm2\n\t" \
			"movaps	%%xmm1, %%xmm3\n\t" \
			"shufps	$0xC9, %%xmm0, %%xmm0\n\t" \
			"shufps	$0xD2, %%xmm1, %%xmm1\n\t" \
			"mulps	%%xmm1, %%xmm0\n\t" \
			"shufps	$0xD2, %%xmm2, %%xmm2\n\t" \
			"shufps	$0xC9, %%xmm3, %%xmm3\n\t" \
			"mulps	%%xmm3, %%xmm2\n\t" \
			"subps	%%xmm2, %%xmm0\n\t" \
			"movaps	%%xmm0, %0\n\t" \
			: "=m"(eVec[8]) \
			: "m"(eVec[0]), "m"(eVec[4]) \
			: "memory" \
		)

#define SSE_PDB_CALC_RMSD(pdbc, ox, pf)	\
		__asm__ __volatile__( \
			"movl		%1, %%ecx\n\t" \
			"xorl		%%eax, %%eax\n\t" \
			"xorps		%%xmm0, %%xmm0\n\t" \
			"1:\n\t" \
			"movl		%2, %%esi\n\t" \
			"movl		(%%esi,%%eax), %%esi\n\t" \
			"testl		%5, %%esi\n\t" \
			"jz			2f\n\t" \
			"movl		%3, %%esi\n\t" \
			"movaps		(%%esi,%%eax), %%xmm1\n\t" \
			"movl		%4, %%esi\n\t" \
			"movaps		(%%esi,%%eax), %%xmm2\n\t" \
			"subps		%%xmm1, %%xmm2\n\t" \
			"mulps		%%xmm2, %%xmm2\n\t" \
			"movaps		%%xmm2, %%xmm1\n\t" \
			"shufps		$0xE5, %%xmm1, %%xmm1\n\t" \
			"addss		%%xmm1, %%xmm2\n\t" \
			"shufps		$0xE6, %%xmm1, %%xmm1\n\t" \
			"addss		%%xmm1, %%xmm2\n\t" \
			"addps		%%xmm2, %%xmm0\n\t" \
			"2:\n\t" \
			"addl		%6, %%eax\n\t" \
			"loop		1b\n\t" \
			"movss		%7, %%xmm1\n\t" \
			"mulss		%%xmm1, %%xmm0\n\t" \
			"sqrtss		%%xmm0, %%xmm0\n\t" \
			"movaps		%%xmm0, %0\n\t" \
			: "=m"(flRMSD) \
			: "m"(m_iNumAtoms), \
			  "g"((char*)m_pAtoms+offsetof(pdbc,atomFlags)), \
			  "g"((char*)m_pAtoms+offsetof(pdbc,ox)), \
			  "g"((char*)pOther->m_pAtoms+offsetof(pdbc,ox)), \
			  "i"(pf), \
			  "m"(sizeof_pdb_atom), \
			  "m"(inv_num_atoms) \
			: "%eax", "%ecx", "%esi", "memory" \
		)

#endif
#endif //MDTRA_ALLOW_SSE

#endif //MDTRA_SSE_H