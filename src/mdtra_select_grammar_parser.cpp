
/*  A Bison parser, made from d:\documents\96_prog\mdtra_github\mdtra_main\src\mdtra_select_grammar.y
    by GNU Bison v1.28 for Win32  */

#define YYBISON 1  /* Identify Bison output.  */

#define	AND	257
#define	NOT	258
#define	OR	259
#define	WITHIN	260
#define	ALL	261
#define	ATOMNO	262
#define	BACKBONE	263
#define	NONE	264
#define	NUMBER	265
#define	SIDECHAIN	266
#define	PROTEIN	267
#define	NUCLEIC	268
#define	DNA	269
#define	WATER	270
#define	CHAIN	271
#define	REAL	272
#define	ELEMENT	273
#define	STRING	274


void yyerror( const char *s );
int yylex( void );
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mdtra_pdb_flags.h"
#include "mdtra_select.h"

#define YYERROR_VERBOSE	1

typedef union {
	int ival;
	float fval;
	char cval;
	char *szval;
	eSelectionCmp cmpVal;
	eSelectionOp opVal;
	SelectionCondPtr cond;
	SelectionCondListPtr condList;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		85
#define	YYFLAG		-32768
#define	YYNTBASE	29

#define YYTRANSLATE(x) ((unsigned)(x) <= 274 ? yytranslate[x] : 34)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    21,
    22,     2,     2,    23,    24,    25,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    27,
    26,    28,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     8,    11,    16,    22,    26,    31,    38,
    45,    53,    61,    63,    65,    69,    71,    73,    75,    77,
    79,    81,    83,    85,    87,    91,    94,    99,   101,   105,
   108,   113,   117,   123,   128,   135,   139,   143,   148,   150,
   152,   154,   156,   159,   162,   164
};

static const short yyrhs[] = {    30,
     0,    31,     0,    21,    30,    22,     0,     4,    31,     0,
     4,    21,    30,    22,     0,    30,    32,    21,    30,    22,
     0,    30,    32,    31,     0,    30,    32,     4,    31,     0,
     6,    21,    11,    23,    30,    22,     0,     6,    21,    18,
    23,    30,    22,     0,    30,     6,    21,    11,    23,    30,
    22,     0,    30,     6,    21,    18,    23,    30,    22,     0,
    19,     0,     7,     0,     8,    33,    11,     0,    17,     0,
    10,     0,     9,     0,    13,     0,    14,     0,    15,     0,
    16,     0,    12,     0,    20,     0,    20,    24,    11,     0,
    20,    17,     0,    20,    24,    11,    17,     0,    11,     0,
    11,    25,    20,     0,    11,    17,     0,    11,    17,    25,
    20,     0,    11,    24,    11,     0,    11,    24,    11,    25,
    20,     0,    11,    24,    11,    17,     0,    11,    24,    11,
    17,    25,    20,     0,    20,    25,    20,     0,    17,    25,
    20,     0,    20,    17,    25,    20,     0,     5,     0,    23,
     0,     3,     0,    26,     0,    27,    26,     0,    28,    26,
     0,    27,     0,    28,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    58,    63,    77,    81,    90,   101,   114,   125,   144,   157,
   170,   184,   199,   205,   210,   217,   226,   231,   236,   241,
   246,   251,   256,   261,   269,   277,   288,   299,   307,   316,
   328,   341,   349,   358,   369,   381,   389,   400,   412,   413,
   414,   416,   417,   418,   419,   420
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","AND","NOT",
"OR","WITHIN","ALL","ATOMNO","BACKBONE","NONE","NUMBER","SIDECHAIN","PROTEIN",
"NUCLEIC","DNA","WATER","CHAIN","REAL","ELEMENT","STRING","'('","')'","','",
"'-'","'.'","'='","'<'","'>'","WholeSelection","ConditionList","Condition","ExprOp",
"CmpOp", NULL
};
#endif

static const short yyr1[] = {     0,
    29,    30,    30,    30,    30,    30,    30,    30,    30,    30,
    30,    30,    31,    31,    31,    31,    31,    31,    31,    31,
    31,    31,    31,    31,    31,    31,    31,    31,    31,    31,
    31,    31,    31,    31,    31,    31,    31,    31,    32,    32,
    32,    33,    33,    33,    33,    33
};

static const short yyr2[] = {     0,
     1,     1,     3,     2,     4,     5,     3,     4,     6,     6,
     7,     7,     1,     1,     3,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     3,     2,     4,     1,     3,     2,
     4,     3,     5,     4,     6,     3,     3,     4,     1,     1,
     1,     1,     2,     2,     1,     1
};

static const short yydefact[] = {     0,
     0,     0,    14,     0,    18,    17,    28,    23,    19,    20,
    21,    22,    16,    13,    24,     0,     1,     2,     0,     4,
     0,    42,    45,    46,     0,    30,     0,     0,     0,    26,
     0,     0,     0,    41,    39,     0,    40,     0,     0,     0,
     0,    43,    44,    15,     0,    32,    29,    37,     0,    25,
    36,     3,     0,     0,     0,     7,     5,     0,     0,    31,
    34,     0,    38,    27,     0,     0,     8,     0,     0,     0,
     0,    33,     0,     0,     6,     9,    10,    35,     0,     0,
    11,    12,     0,     0,     0
};

static const short yydefgoto[] = {    83,
    17,    18,    38,    25
};

static const short yypact[] = {    74,
   107,     2,-32768,   117,-32768,-32768,    -6,-32768,-32768,-32768,
-32768,-32768,    10,-32768,    52,    74,    56,-32768,    74,-32768,
     9,-32768,     6,    14,    17,    12,    25,    21,    29,    20,
    49,    32,    -1,-32768,-32768,    44,-32768,    92,     3,    43,
    47,-32768,-32768,-32768,    51,   -10,-32768,-32768,    54,    58,
-32768,-32768,    13,   122,    74,-32768,-32768,    74,    74,-32768,
    67,    77,-32768,-32768,    75,    87,-32768,     7,    11,    41,
   105,-32768,    74,    74,-32768,-32768,-32768,-32768,    45,    50,
-32768,-32768,   140,   146,-32768
};

static const short yypgoto[] = {-32768,
   -16,     0,-32768,-32768
};


#define	YYLAST		146


static const short yytable[] = {    33,
    20,    34,    39,    35,    36,    34,    61,    35,    36,    34,
    26,    35,    36,    34,    62,    35,    36,    27,    28,    40,
    52,    37,    21,    65,    57,    37,    41,    44,    75,    37,
    66,    42,    76,    37,    29,    46,    45,    56,    68,    43,
    47,    69,    70,    34,    49,    35,    36,    34,    48,    35,
    36,    51,    34,    67,    35,    36,    79,    80,    34,    50,
    35,    36,    77,    37,    53,    58,    81,    37,    30,    59,
    60,    82,    37,    63,    64,    31,    32,     1,    37,     2,
     3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    71,    14,    15,    16,    54,    72,    73,     3,     4,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    74,
    14,    15,    55,     3,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    78,    14,    15,    19,     3,     4,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    84,
    14,    15,    22,    23,    24,    85
};

static const short yycheck[] = {    16,
     1,     3,    19,     5,     6,     3,    17,     5,     6,     3,
    17,     5,     6,     3,    25,     5,     6,    24,    25,    11,
    22,    23,    21,    11,    22,    23,    18,    11,    22,    23,
    18,    26,    22,    23,    25,    11,    25,    38,    55,    26,
    20,    58,    59,     3,    25,     5,     6,     3,    20,     5,
     6,    20,     3,    54,     5,     6,    73,    74,     3,    11,
     5,     6,    22,    23,    21,    23,    22,    23,    17,    23,
    20,    22,    23,    20,    17,    24,    25,     4,    23,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    25,    19,    20,    21,     4,    20,    23,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    23,
    19,    20,    21,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    20,    19,    20,    21,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,     0,
    19,    20,    26,    27,    28,     0
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */

/* This file comes from bison-@bison_version@.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
{ 
		g_SelectionParms.condList = yyvsp[0].condList;
	;
    break;}
case 2:
{
		SelectionCondListPtr condList;
		if (yyvsp[0].cond->def != SELECT_DEF_CONDLIST) {
			condList = new SelectionCondList;
			condList->c1 = yyvsp[0].cond;
			condList->c2 = NULL;
			condList->op = SELECT_OP_NONE;
		} else {
			condList = yyvsp[0].cond->u.l;
			delete yyvsp[0].cond;
		}
		yyval.condList = condList;
	;
    break;}
case 3:
{
		yyval.condList = yyvsp[-1].condList;
	;
    break;}
case 4:
{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = yyvsp[0].cond;
		condList->c2 = NULL;
		condList->op = SELECT_OP_NOT;
		yyval.condList = condList;
	;
    break;}
case 5:
{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_CONDLIST;
		condList->c1->u.l = yyvsp[-1].condList;
		condList->c2 = NULL;
		condList->op = SELECT_OP_NOT;
		yyval.condList = condList;
	;
    break;}
case 6:
{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_CONDLIST;
		condList->c1->u.l = yyvsp[-4].condList;
		condList->op = yyvsp[-3].opVal;
		condList->c2 = new SelectionCond;
		condList->c2->def = SELECT_DEF_CONDLIST;
		condList->c2->u.l = yyvsp[-1].condList;
		yyval.condList = condList;
	;
    break;}
case 7:
{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_CONDLIST;
		condList->c1->u.l = yyvsp[-2].condList;
		condList->op = yyvsp[-1].opVal;
		condList->c2 = yyvsp[0].cond;
		yyval.condList = condList;
	;
    break;}
case 8:
{
		SelectionCondListPtr condList2;
		condList2 = new SelectionCondList;
		condList2->c1 = yyvsp[0].cond;
		condList2->c2 = NULL;
		condList2->op = SELECT_OP_NOT;

		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_CONDLIST;
		condList->c1->u.l = yyvsp[-3].condList;
		condList->c2 = new SelectionCond;
		condList->c2->def = SELECT_DEF_CONDLIST;
		condList->c2->u.l = condList2;
		condList->op = yyvsp[-2].opVal;
		yyval.condList = condList;
	;
    break;}
case 9:
{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_ALL;
		condList->c2 = new SelectionCond;
		condList->c2->def = SELECT_DEF_CONDLIST;
		condList->c2->u.l = yyvsp[-1].condList;
		condList->op = SELECT_OP_WITHIN;
		condList->arg = (float)yyvsp[-3].ival;
		yyval.condList = condList;
	;
    break;}
case 10:
{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_ALL;
		condList->c2 = new SelectionCond;
		condList->c2->def = SELECT_DEF_CONDLIST;
		condList->c2->u.l = yyvsp[-1].condList;
		condList->op = SELECT_OP_WITHIN;
		condList->arg = yyvsp[-3].fval;
		yyval.condList = condList;
	;
    break;}
case 11:
{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_CONDLIST;
		condList->c1->u.l = yyvsp[-6].condList;
		condList->c2 = new SelectionCond;
		condList->c2->def = SELECT_DEF_CONDLIST;
		condList->c2->u.l = yyvsp[-1].condList;
		condList->op = SELECT_OP_WITHIN;
		condList->arg = (float)yyvsp[-3].ival;
		yyval.condList = condList;
	;
    break;}
case 12:
{
		SelectionCondListPtr condList;
		condList = new SelectionCondList;
		condList->c1 = new SelectionCond;
		condList->c1->def = SELECT_DEF_CONDLIST;
		condList->c1->u.l = yyvsp[-6].condList;
		condList->c2 = new SelectionCond;
		condList->c2->def = SELECT_DEF_CONDLIST;
		condList->c2->u.l = yyvsp[-1].condList;
		condList->op = SELECT_OP_WITHIN;
		condList->arg = yyvsp[-3].fval;
		yyval.condList = condList;
	;
    break;}
case 13:
{ 
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_ELEMENT;
		yyval.cond->u.s = yyvsp[0].szval;
	;
    break;}
case 14:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_ALL;
	;
    break;}
case 15:
{ 
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_ATOMNO;
		yyval.cond->u.atomno.cmp = yyvsp[-1].cmpVal;
		yyval.cond->u.atomno.i = yyvsp[0].ival;
	;
    break;}
case 16:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_CHAIN;
		if (yyvsp[0].cval >= 'A' && yyvsp[0].cval <= 'Z')
			yyval.cond->u.chain = yyvsp[0].cval - 'A';
		else
			yyval.cond->u.chain = yyvsp[0].cval - 'a';
	;
    break;}
case 17:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_NONE;
	;
    break;}
case 18:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_BACKBONE;
	;
    break;}
case 19:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_PROTEIN;
	;
    break;}
case 20:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_DNA;
	;
    break;}
case 21:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_DNA;
	;
    break;}
case 22:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_WATER;
	;
    break;}
case 23:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_SIDECHAIN;
	;
    break;}
case 24:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_RESIDUE;
		yyval.cond->u.residue.s = yyvsp[0].szval;
		yyval.cond->u.residue.num = 0;
		yyval.cond->u.residue.chain = -1;
	;
    break;}
case 25:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_RESIDUE;
		yyval.cond->u.residue.s = yyvsp[-2].szval;
		yyval.cond->u.residue.num = yyvsp[0].ival;
		yyval.cond->u.residue.chain = -1;
	;
    break;}
case 26:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_RESIDUE;
		yyval.cond->u.residue.s = yyvsp[-1].szval;
		yyval.cond->u.residue.num = 0;
		if (yyvsp[0].cval >= 'A' && yyvsp[0].cval <= 'Z')
			yyval.cond->u.residue.chain = yyvsp[0].cval - 'A';
		else
			yyval.cond->u.residue.chain = yyvsp[0].cval - 'a';
	;
    break;}
case 27:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_RESIDUE;
		yyval.cond->u.residue.s = yyvsp[-3].szval;
		yyval.cond->u.residue.num = yyvsp[-1].ival;
		if (yyvsp[0].cval >= 'A' && yyvsp[0].cval <= 'Z')
			yyval.cond->u.residue.chain = yyvsp[0].cval - 'A';
		else
			yyval.cond->u.residue.chain = yyvsp[0].cval - 'a';
	;
    break;}
case 28:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_RESIDUENO;
		yyval.cond->u.resno.min_i = yyvsp[0].ival;
		yyval.cond->u.resno.max_i = yyvsp[0].ival;
		yyval.cond->u.resno.chain = -1;
	;
    break;}
case 29:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_RESIDUENOATOM;
		yyval.cond->u.resnoat.min_i = yyvsp[-2].ival;
		yyval.cond->u.resnoat.max_i = yyvsp[-2].ival;
		yyval.cond->u.resnoat.chain = -1;
		yyval.cond->u.resnoat.s = yyvsp[0].szval;
	;
    break;}
case 30:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_RESIDUENO;
		yyval.cond->u.resno.min_i = yyvsp[-1].ival;
		yyval.cond->u.resno.max_i = yyvsp[-1].ival;
		yyval.cond->u.resno.chain = -1;
		if (yyvsp[0].cval >= 'A' && yyvsp[0].cval <= 'Z')
			yyval.cond->u.resno.chain = yyvsp[0].cval - 'A';
		else
			yyval.cond->u.resno.chain = yyvsp[0].cval - 'a';
	;
    break;}
case 31:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_RESIDUENOATOM;
		yyval.cond->u.resnoat.min_i = yyvsp[-3].ival;
		yyval.cond->u.resnoat.max_i = yyvsp[-3].ival;
		yyval.cond->u.resnoat.chain = -1;
		if (yyvsp[-2].cval >= 'A' && yyvsp[-2].cval <= 'Z')
			yyval.cond->u.resnoat.chain = yyvsp[-2].cval - 'A';
		else
			yyval.cond->u.resnoat.chain = yyvsp[-2].cval - 'a';
		yyval.cond->u.resnoat.s = yyvsp[0].szval;
	;
    break;}
case 32:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_RESIDUENO;
		yyval.cond->u.resno.min_i = yyvsp[-2].ival;
		yyval.cond->u.resno.max_i = yyvsp[0].ival;
		yyval.cond->u.resno.chain = -1;
	;
    break;}
case 33:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_RESIDUENOATOM;
		yyval.cond->u.resnoat.min_i = yyvsp[-4].ival;
		yyval.cond->u.resnoat.max_i = yyvsp[-2].ival;
		yyval.cond->u.resnoat.s = yyvsp[0].szval;
		yyval.cond->u.resnoat.chain = -1;
	;
    break;}
case 34:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_RESIDUENO;
		yyval.cond->u.resno.min_i = yyvsp[-3].ival;
		yyval.cond->u.resno.max_i = yyvsp[-1].ival;
		if (yyvsp[0].cval >= 'A' && yyvsp[0].cval <= 'Z')
			yyval.cond->u.resno.chain = yyvsp[0].cval - 'A';
		else
			yyval.cond->u.resno.chain = yyvsp[0].cval - 'a';
	;
    break;}
case 35:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_RESIDUENOATOM;
		yyval.cond->u.resnoat.min_i = yyvsp[-5].ival;
		yyval.cond->u.resnoat.max_i = yyvsp[-3].ival;
		if (yyvsp[-2].cval >= 'A' && yyvsp[-2].cval <= 'Z')
			yyval.cond->u.resnoat.chain = yyvsp[-2].cval - 'A';
		else
			yyval.cond->u.resnoat.chain = yyvsp[-2].cval - 'a';
		yyval.cond->u.resnoat.s = yyvsp[0].szval;
	;
    break;}
case 36:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_ATOM;
		yyval.cond->u.atom.s = yyvsp[0].szval;
		yyval.cond->u.atom.res = yyvsp[-2].szval;
		yyval.cond->u.atom.chain = -1;
	;
    break;}
case 37:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_ATOM;
		yyval.cond->u.atom.s = yyvsp[0].szval;
		yyval.cond->u.atom.res = NULL;
		if (yyvsp[-2].cval >= 'A' && yyvsp[-2].cval <= 'Z')
			yyval.cond->u.atom.chain = yyvsp[-2].cval - 'A';
		else
			yyval.cond->u.atom.chain = yyvsp[-2].cval - 'a';
	;
    break;}
case 38:
{
		yyval.cond = new SelectionCond;
		yyval.cond->def = SELECT_DEF_ATOM;
		yyval.cond->u.atom.s = yyvsp[0].szval;
		yyval.cond->u.atom.res = yyvsp[-3].szval;
		if (yyvsp[-2].cval >= 'A' && yyvsp[-2].cval <= 'Z')
			yyval.cond->u.atom.chain = yyvsp[-2].cval - 'A';
		else
			yyval.cond->u.atom.chain = yyvsp[-2].cval - 'a';
	;
    break;}
case 39:
{ yyval.opVal = SELECT_OP_OR; ;
    break;}
case 40:
{ yyval.opVal = SELECT_OP_OR; ;
    break;}
case 41:
{ yyval.opVal = SELECT_OP_AND; ;
    break;}
case 42:
{ yyval.cmpVal = SELECT_OP_EQUAL; ;
    break;}
case 43:
{ yyval.cmpVal = SELECT_OP_LEQUAL; ;
    break;}
case 44:
{ yyval.cmpVal = SELECT_OP_GEQUAL; ;
    break;}
case 45:
{ yyval.cmpVal = SELECT_OP_LESS; ;
    break;}
case 46:
{ yyval.cmpVal = SELECT_OP_GREATER; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */


  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy_s(msg, size + 15, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat_s(msg, size + 15, count == 0 ? ", expecting `" : " or `");
			strcat_s(msg, size + 15, yytname[x]);
			strcat_s(msg, size + 15, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
 

void yyerror( const char *s )
{
	if (g_SelectionParms.debug)
		MDTRA_Select_ParseError(s, g_SelectionParms.current_pos);
	else
		MDTRA_Select_ParseError("syntax error", g_SelectionParms.current_pos);
}
