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
%start WholeSelection
%{
void yyerror( const char *s );
int yylex( void );
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mdtra_pdb_flags.h"
#include "mdtra_select.h"

#define YYERROR_VERBOSE	1
%}
%union {
	int ival;
	float fval;
	char cval;
	char *szval;
	eSelectionCmp cmpVal;
	eSelectionOp opVal;
	SelectionCondPtr cond;
	SelectionCondListPtr condList;
}

%token <ival>	AND NOT OR WITHIN
%token <ival>	ALL ATOMNO BACKBONE NONE NUMBER SIDECHAIN PROTEIN NUCLEIC DNA WATER 
%token <cval>	CHAIN
%token <fval>	REAL
%token <szval>	ELEMENT STRING

%type <condList> WholeSelection ConditionList
%type <cond>	Condition
%type <cmpVal>	CmpOp
%type <opVal>	ExprOp

%%

WholeSelection:  ConditionList 
	{ 
		g_SelectionParms.condList = $1;
	};

ConditionList: Condition
	{
		SelectionCondListPtr condList;
		if ($1->def != SELECT_DEF_CONDLIST) {
			condList = new SelectionCondList;
			condList->c1 = $1;
			condList->c2 = NULL;
			condList->op = SELECT_OP_NONE;
		} else {
			condList = $1->u.l;
			delete $1;
		}
		$$ = condList;
	};
	| '(' ConditionList ')' 
	{
		$$ = $2;
	};
	| NOT Condition
	{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = $2;
		condList->c2 = NULL;
		condList->op = SELECT_OP_NOT;
		$$ = condList;
	};
	| NOT '(' ConditionList ')'
	{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_CONDLIST;
		condList->c1->u.l = $3;
		condList->c2 = NULL;
		condList->op = SELECT_OP_NOT;
		$$ = condList;
	};
	| ConditionList ExprOp '(' ConditionList ')'
	{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_CONDLIST;
		condList->c1->u.l = $1;
		condList->op = $2;
		condList->c2 = new SelectionCond;
		condList->c2->def = SELECT_DEF_CONDLIST;
		condList->c2->u.l = $4;
		$$ = condList;
	};
	| ConditionList ExprOp Condition
	{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_CONDLIST;
		condList->c1->u.l = $1;
		condList->op = $2;
		condList->c2 = $3;
		$$ = condList;
	};
	| ConditionList ExprOp NOT Condition
	{
		SelectionCondListPtr condList2;
		condList2 = new SelectionCondList;
		condList2->c1 = $4;
		condList2->c2 = NULL;
		condList2->op = SELECT_OP_NOT;

		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_CONDLIST;
		condList->c1->u.l = $1;
		condList->c2 = new SelectionCond;
		condList->c2->def = SELECT_DEF_CONDLIST;
		condList->c2->u.l = condList2;
		condList->op = $2;
		$$ = condList;
	};
	| WITHIN '(' NUMBER ',' ConditionList ')'
	{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_ALL;
		condList->c2 = new SelectionCond;
		condList->c2->def = SELECT_DEF_CONDLIST;
		condList->c2->u.l = $5;
		condList->op = SELECT_OP_WITHIN;
		condList->arg = (float)$3;
		$$ = condList;
	};
	| WITHIN '(' REAL ',' ConditionList ')'
	{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_ALL;
		condList->c2 = new SelectionCond;
		condList->c2->def = SELECT_DEF_CONDLIST;
		condList->c2->u.l = $5;
		condList->op = SELECT_OP_WITHIN;
		condList->arg = $3;
		$$ = condList;
	};
	| ConditionList WITHIN '(' NUMBER ',' ConditionList ')'
	{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_CONDLIST;
		condList->c1->u.l = $1;
		condList->c2 = new SelectionCond;
		condList->c2->def = SELECT_DEF_CONDLIST;
		condList->c2->u.l = $6;
		condList->op = SELECT_OP_WITHIN;
		condList->arg = (float)$4;
		$$ = condList;
	};
	| ConditionList WITHIN '(' REAL ',' ConditionList ')'
	{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_CONDLIST;
		condList->c1->u.l = $1;
		condList->c2 = new SelectionCond;
		condList->c2->def = SELECT_DEF_CONDLIST;
		condList->c2->u.l = $6;
		condList->op = SELECT_OP_WITHIN;
		condList->arg = $4;
		$$ = condList;
	};
	
Condition: ELEMENT
	{ 
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_ELEMENT;
		$$->u.s = $1;
	};
	| ALL
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_ALL;
	};
	| ATOMNO CmpOp NUMBER
	{ 
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_ATOMNO;
		$$->u.atomno.cmp = $2;
		$$->u.atomno.i = $3;
	};
	| CHAIN
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_CHAIN;
		if ($1 >= 'A' && $1 <= 'Z')
			$$->u.chain = $1 - 'A';
		else
			$$->u.chain = $1 - 'a';
	};
	| NONE
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_NONE;
	};
	| BACKBONE
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_BACKBONE;
	};
	| PROTEIN
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_PROTEIN;
	};
	| NUCLEIC
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_DNA;
	};
	| DNA
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_DNA;
	};
	| WATER
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_WATER;
	};
	| SIDECHAIN
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_SIDECHAIN;
	};
	| STRING
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_RESIDUE;
		$$->u.residue.s = $1;
		$$->u.residue.num = 0;
		$$->u.residue.chain = -1;
	};
	| STRING '-' NUMBER
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_RESIDUE;
		$$->u.residue.s = $1;
		$$->u.residue.num = $3;
		$$->u.residue.chain = -1;
	};
	| STRING CHAIN
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_RESIDUE;
		$$->u.residue.s = $1;
		$$->u.residue.num = 0;
		if ($2 >= 'A' && $2 <= 'Z')
			$$->u.residue.chain = $2 - 'A';
		else
			$$->u.residue.chain = $2 - 'a';
	};
	| STRING '-' NUMBER CHAIN
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_RESIDUE;
		$$->u.residue.s = $1;
		$$->u.residue.num = $3;
		if ($4 >= 'A' && $4 <= 'Z')
			$$->u.residue.chain = $4 - 'A';
		else
			$$->u.residue.chain = $4 - 'a';
	};
	| NUMBER
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_RESIDUENO;
		$$->u.resno.min_i = $1;
		$$->u.resno.max_i = $1;
		$$->u.resno.chain = -1;
	};
	| NUMBER '.' STRING
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_RESIDUENOATOM;
		$$->u.resnoat.min_i = $1;
		$$->u.resnoat.max_i = $1;
		$$->u.resnoat.chain = -1;
		$$->u.resnoat.s = $3;
	};
	| NUMBER CHAIN
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_RESIDUENO;
		$$->u.resno.min_i = $1;
		$$->u.resno.max_i = $1;
		$$->u.resno.chain = -1;
		if ($2 >= 'A' && $2 <= 'Z')
			$$->u.resno.chain = $2 - 'A';
		else
			$$->u.resno.chain = $2 - 'a';
	};
	| NUMBER CHAIN '.' STRING
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_RESIDUENOATOM;
		$$->u.resnoat.min_i = $1;
		$$->u.resnoat.max_i = $1;
		$$->u.resnoat.chain = -1;
		if ($2 >= 'A' && $2 <= 'Z')
			$$->u.resnoat.chain = $2 - 'A';
		else
			$$->u.resnoat.chain = $2 - 'a';
		$$->u.resnoat.s = $4;
	};
	| NUMBER '-' NUMBER
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_RESIDUENO;
		$$->u.resno.min_i = $1;
		$$->u.resno.max_i = $3;
		$$->u.resno.chain = -1;
	};
	| NUMBER '-' NUMBER '.' STRING
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_RESIDUENOATOM;
		$$->u.resnoat.min_i = $1;
		$$->u.resnoat.max_i = $3;
		$$->u.resnoat.s = $5;
		$$->u.resnoat.chain = -1;
	};
	| NUMBER '-' NUMBER CHAIN
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_RESIDUENO;
		$$->u.resno.min_i = $1;
		$$->u.resno.max_i = $3;
		if ($4 >= 'A' && $4 <= 'Z')
			$$->u.resno.chain = $4 - 'A';
		else
			$$->u.resno.chain = $4 - 'a';
	};
	| NUMBER '-' NUMBER CHAIN '.' STRING
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_RESIDUENOATOM;
		$$->u.resnoat.min_i = $1;
		$$->u.resnoat.max_i = $3;
		if ($4 >= 'A' && $4 <= 'Z')
			$$->u.resnoat.chain = $4 - 'A';
		else
			$$->u.resnoat.chain = $4 - 'a';
		$$->u.resnoat.s = $6;
	};
	| STRING '.' STRING
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_ATOM;
		$$->u.atom.s = $3;
		$$->u.atom.res = $1;
		$$->u.atom.chain = -1;
	};
	| CHAIN '.' STRING
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_ATOM;
		$$->u.atom.s = $3;
		$$->u.atom.res = NULL;
		if ($1 >= 'A' && $1 <= 'Z')
			$$->u.atom.chain = $1 - 'A';
		else
			$$->u.atom.chain = $1 - 'a';
	};
	| STRING CHAIN '.' STRING
	{
		$$ = new SelectionCond;
		$$->def = SELECT_DEF_ATOM;
		$$->u.atom.s = $4;
		$$->u.atom.res = $1;
		if ($2 >= 'A' && $2 <= 'Z')
			$$->u.atom.chain = $2 - 'A';
		else
			$$->u.atom.chain = $2 - 'a';
	};

ExprOp:	  OR				{ $$ = SELECT_OP_OR; }
		| ','				{ $$ = SELECT_OP_OR; }
		| AND				{ $$ = SELECT_OP_AND; }

CmpOp:	  '='				{ $$ = SELECT_OP_EQUAL; }
		| '<''='			{ $$ = SELECT_OP_LEQUAL; }
		| '>''='			{ $$ = SELECT_OP_GEQUAL; }
		| '<'				{ $$ = SELECT_OP_LESS; }
		| '>'				{ $$ = SELECT_OP_GREATER; }

%% 

void yyerror( const char *s )
{
	if (g_SelectionParms.debug)
		MDTRA_Select_ParseError(s, g_SelectionParms.current_pos);
	else
		MDTRA_Select_ParseError("syntax error", g_SelectionParms.current_pos);
}
