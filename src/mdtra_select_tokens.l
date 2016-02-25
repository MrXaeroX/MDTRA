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
/* definitions */
digit    [0-9]
char     [a-zA-Z]
alphanum [0-9a-zA-Z_\*\?\'\{\}]
brackets \[[a-zA-Z0-9-]+\]

%option nounput

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mdtra_pdb_flags.h"
#include "mdtra_select.h"
#include "mdtra_types.h"
#include "mdtra_utils.h"
#include "mdtra_select_grammar_parser.h"

#define YY_INPUT(buf,result,max_size) \
{ \
	int c = *(g_SelectionParms.input++); \
	g_SelectionParms.current_pos++; \
	result = (c == 0) ? YY_NULL : (buf[0] = c, 1); \
}

#define YY_ALWAYS_INTERACTIVE 1
%}

%%

\(			return('(');
\)			return(')');
=			return('=');
>			return('>');
\<			return('<');
-			return('-');
\.			return('.');
,			return(',');
or			return(OR);
and			return(AND);
not			return(NOT);
within		return(WITHIN);
OR			return(OR);
AND			return(AND);
NOT			return(NOT);
WITHIN		return(WITHIN);

all			return(ALL);
ALL			return(ALL);
atomno		return(ATOMNO);
ATOMNO		return(ATOMNO);
none		return(NONE);
NONE		return(NONE);
backbone	return(BACKBONE);
BACKBONE	return(BACKBONE);
sidechain	return(SIDECHAIN);
SIDECHAIN	return(SIDECHAIN);
nucleic		return(NUCLEIC);
NUCLEIC		return(NUCLEIC);
protein		return(PROTEIN);
PROTEIN		return(PROTEIN);
dna			return(DNA);
DNA			return(DNA);
water		return(WATER);
WATER		return(WATER);

[ \t]+		{}

[0-9]*\.[0-9]+ {
				yylval.fval = (float)atof(yytext);
				return REAL;
			}

{digit}+	{
				yylval.ival = atoi(yytext);
				return NUMBER;
			}

\:{char}	{
				yylval.cval = yytext[1];
				return CHAIN;
			}

{alphanum}+	 {
				if (UTIL_IsElementTitle(yytext)) {
					yylval.szval = MDTRA_Select_CopyString( yytext );
					return ELEMENT;
				} else {
					yylval.szval = MDTRA_Select_CopyString( yytext );
					return STRING;
				}
			}	
{alphanum}*{brackets}+{alphanum}* {
				yylval.szval = MDTRA_Select_CopyString( yytext );
				return STRING;
			}	

%%

bool yyparse_init( char* inputString )
{
	g_SelectionParms.input = inputString;
	g_SelectionParms.current_pos = 0;
	g_SelectionParms.condList = NULL;
	return true;
}

#ifndef yywrap
int yywrap( void )
{
	return 1;
}
#endif