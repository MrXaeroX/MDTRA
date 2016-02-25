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


extern YYSTYPE yylval;
