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
#ifndef MDTRA_SELECT_H
#define MDTRA_SELECT_H

#include <QtCore/QList>
#include <QtCore/QString>

typedef enum {
	SELECT_DEF_CONDLIST = 0,
	SELECT_DEF_ALL,
	SELECT_DEF_ATOM,
	SELECT_DEF_ATOMNO,
	SELECT_DEF_BACKBONE,
	SELECT_DEF_HETERO,
	SELECT_DEF_PROTEIN,
	SELECT_DEF_DNA,
	SELECT_DEF_WATER,
	SELECT_DEF_CHAIN,
	SELECT_DEF_ELEMENT,
	SELECT_DEF_NONE,
	SELECT_DEF_RESIDUE,
	SELECT_DEF_RESIDUENO,
	SELECT_DEF_RESIDUENOATOM,
	SELECT_DEF_SIDECHAIN,
} eSelectionDef;

typedef enum {
	SELECT_OP_EQUAL = 0,
	SELECT_OP_LESS,
	SELECT_OP_LEQUAL,
	SELECT_OP_GREATER,
	SELECT_OP_GEQUAL,
} eSelectionCmp;

typedef enum {
	SELECT_OP_NONE = 0,
	SELECT_OP_OR,
	SELECT_OP_AND,
	SELECT_OP_NOT,
	SELECT_OP_WITHIN,
	SELECT_OP_NOT_WITHIN,
} eSelectionOp;

typedef struct {
	int i;
	eSelectionCmp cmp;
} DefAtomno;

typedef struct {
	char *s;
	char *res;
	int chain;
} DefAtom;

typedef struct {
	char *s;
	int num;
	int chain;
} DefResidue;

typedef struct {
	int min_i;
	int max_i;
	int chain;
} DefResidueNo;

typedef struct {
	char *s;
	int min_i;
	int max_i;
	int chain;
} DefResidueNoAtom;

typedef struct stSelectionCond {
	eSelectionDef def;
	union {
		struct stSelectionCondList *l;
		DefAtom atom;
		DefAtomno atomno;
		DefResidue residue;
		DefResidueNo resno;
		DefResidueNoAtom resnoat;
		char *s;
		int chain;
	} u;
} SelectionCond, *SelectionCondPtr;

typedef struct stSelectionCondList {
	struct stSelectionCond *c1;
	struct stSelectionCond *c2;
	eSelectionOp op;
	float arg;
} SelectionCondList, *SelectionCondListPtr;

typedef struct stSelectionError {
	char *s;
	int pos;
} SelectionError;

typedef struct stSelectionParms {
	bool	debug;
	int		current_pos;
	char*	input;
	SelectionCondListPtr condList;
} SelectionParms;

extern char* MDTRA_Select_CopyString( const char *s );
extern void MDTRA_Select_ParseError( const char *s, int pos );

extern bool yyparse_init(char* inputString);
extern int  yyparse( void );

extern SelectionParms g_SelectionParms;
extern QList<SelectionError> g_SelectionParseErrors;

template<typename T>
class MDTRA_SelectionSet
{
public:
	MDTRA_SelectionSet( int size )
	{
		assert(sizeof(long int) == 4);
		m_realsize = size;
		m_size = size >> 5;
		if (size % 32) m_size++;
		m_set = new long int[m_size];
		clear();
	}
	~MDTRA_SelectionSet()
	{
		if (m_set) delete [] m_set;
	}
	void clear( void )
	{
		memset( m_set, 0, m_size * sizeof(long int) );
	}
	void setAll( void )
	{
		memset( m_set, ~0, m_size * sizeof(long int) );
	}
	void setValue( int index )
	{
		assert( index >= 0 && index < (m_size<<5) );
		m_set[index >> 5] |= ((long int)1) << (index % 32);
	}
	int value( int index ) const 
	{ 
		assert( index >= 0 && index < (m_size<<5) );
		return (int)((m_set[index >> 5] & (((long int)1) << (index % 32))) ? 1 : 0); 
	}
	long int block( int index ) const 
	{ 
		assert( index >= 0 && index < m_size );
		return m_set[index]; 
	}
	void invert( void )
	{
		for (int i = 0; i < m_size; i++)
			m_set[i] ^= ~0;
	}
	void join_or( const MDTRA_SelectionSet *pOther )
	{
		int testSize = MDTRA_MIN( m_size, pOther->size() );
		for (int i = 0; i < testSize; i++)
			m_set[i] |= pOther->block(i);
	}
	void join_and( const MDTRA_SelectionSet *pOther )
	{
		int testSize = MDTRA_MIN( m_size, pOther->size() );
		for (int i = 0; i < testSize; i++)
			m_set[i] &= pOther->block(i);
	}
	void within( const MDTRA_SelectionSet *pOther, const T *pdb, float dist )
	{
		int otherSize = pOther->realsize();
		for (int i = 0; i < m_realsize; i++) {
			if (!value(i))
				continue;
			float minDist = 9999999.0f;
			for (int j = 0; j < otherSize; j++) {
			//	if (j == i)
			//		continue;
				if (!pOther->value(j))
					continue;
				float dist = pdb->get_selection_pair_distance( i, j );
				if (dist < minDist)
					minDist = dist;
			}
			if (minDist > dist)
				m_set[i>>5] &= ~(((long int)1)<<(i%32));
		}
	}

	int size( void ) const { return m_size; }
	int realsize( void ) const { return m_realsize; }

private:
	long int *m_set;
	int m_size;
	int m_realsize;
};

template<typename T>
class MDTRA_SelectionParser
{
	typedef void (*print_func)(const QString&);
	typedef void (*select_func)(const MDTRA_SelectionSet<T>*);
public:
	MDTRA_SelectionParser() {}
	~MDTRA_SelectionParser()
	{
		g_SelectionParseErrors.clear();
	}

	bool parse( const T *pdb, const char *tokens, select_func func )
	{
		g_SelectionParseErrors.clear();
		if (!tokens || !strlen(tokens)) {
			MDTRA_Select_ParseError("empty string", 0);
			return false;
		}

#if defined(_DEBUG) || defined(_SELECT_DEBUG)
		g_SelectionParms.debug = 1;
#else
		g_SelectionParms.debug = 0;
#endif

		char *instring = MDTRA_Select_CopyString(tokens);

		if (yyparse_init( instring ))
			yyparse();
		delete [] instring;

		if (g_SelectionParms.condList) {
			SelectionCondList *pCurrentList = g_SelectionParms.condList;
			execCondList( pCurrentList, pdb, func );
			freeCondList( pCurrentList );
		}

		return (g_SelectionParseErrors.count() == 0);
	}
	void printErrors( print_func func )
	{
		if (!func || g_SelectionParseErrors.count() == 0)
			return;

		for (int i = 0; i < g_SelectionParseErrors.count(); i++) {
			func( QString("<b>Error at %1:</b> %2\n").arg(g_SelectionParseErrors.at(i).pos).arg(g_SelectionParseErrors.at(i).s));
		}

		//If not call this, it will sometimes produce an error at 0 next time
		char emptyString[] = "";
		yyparse_init(emptyString);
		yyparse();
	}

protected:

	void execCondList_i( SelectionCondListPtr condList, MDTRA_SelectionSet<T> *pSet, const T *pdb )
	{
		switch (condList->op) {
		case SELECT_OP_NONE:
			execCond_i( condList->c1, pSet, pdb );
			break;
		case SELECT_OP_NOT:
			execCond_i( condList->c1, pSet, pdb );
			pSet->invert();
			break;
		case SELECT_OP_OR:
			{
				MDTRA_SelectionSet<T> auxSet( pdb->getAtomCount() );
				execCond_i( condList->c1, pSet, pdb );
				execCond_i( condList->c2, &auxSet, pdb );
				pSet->join_or(&auxSet);
			}
			break;
		case SELECT_OP_AND:
			{
				MDTRA_SelectionSet<T> auxSet( pdb->getAtomCount() );
				execCond_i( condList->c1, pSet, pdb );
				execCond_i( condList->c2, &auxSet, pdb );
				pSet->join_and(&auxSet);
			}
			break;
		case SELECT_OP_WITHIN:
			{
				MDTRA_SelectionSet<T> auxSet( pdb->getAtomCount() );
				execCond_i( condList->c1, pSet, pdb );
				execCond_i( condList->c2, &auxSet, pdb );
				pSet->within(&auxSet, pdb, condList->arg);
			}
			break;
		case SELECT_OP_NOT_WITHIN:
			{
				MDTRA_SelectionSet<T> auxSet( pdb->getAtomCount() );
				execCond_i( condList->c1, pSet, pdb );
				execCond_i( condList->c2, &auxSet, pdb );
				pSet->within(&auxSet, pdb, condList->arg);
				pSet->invert();
			}
			break;
		default:
			break;
		}
	}
	void execCond_i( SelectionCondPtr cond, MDTRA_SelectionSet<T> *pSet, const T *pdb )
	{
		switch (cond->def)
		{
		case SELECT_DEF_CONDLIST:
			execCondList_i( cond->u.l, pSet, pdb );
			break;
		case SELECT_DEF_ALL:
			pSet->setAll();
			break;
		case SELECT_DEF_NONE:
			pSet->clear();
			break;
		case SELECT_DEF_ELEMENT:
			pSet->clear();
			pdb->selectElement( cond->u.s, pSet );
			break;
		case SELECT_DEF_ATOM:
			pSet->clear();
			pdb->selectAtom( cond->u.atom.s, cond->u.atom.res, cond->u.atom.chain, pSet );
			break;
		case SELECT_DEF_ATOMNO:
			pSet->clear();
			pdb->selectAtomno( cond->u.atomno.i, cond->u.atomno.cmp, pSet );
			break;
		case SELECT_DEF_CHAIN:
			pSet->clear();
			pdb->selectChain( cond->u.chain, pSet );
			break;
		case SELECT_DEF_RESIDUE:
			pSet->clear();
			pdb->selectResidue( cond->u.residue.s, cond->u.residue.num, cond->u.residue.chain, pSet );
			break;
		case SELECT_DEF_RESIDUENO:
			pSet->clear();
			pdb->selectResidueno( cond->u.resno.min_i, cond->u.resno.max_i, cond->u.resno.chain, pSet );
			break;
		case SELECT_DEF_RESIDUENOATOM:
			pSet->clear();
			pdb->selectResiduenoAtom( cond->u.resnoat.s, cond->u.resnoat.min_i, cond->u.resnoat.max_i, cond->u.resnoat.chain, pSet );
			break;
		case SELECT_DEF_BACKBONE:
			pSet->clear();
			pdb->selectByFlags( PDB_FLAG_BACKBONE, pSet );
			break;
		case SELECT_DEF_PROTEIN:
			pSet->clear();
			pdb->selectByFlags( PDB_FLAG_PROTEIN, pSet );
			break;
		case SELECT_DEF_DNA:
			pSet->clear();
			pdb->selectByFlags( PDB_FLAG_NUCLEIC, pSet );
			break;
		case SELECT_DEF_WATER:
			pSet->clear();
			pdb->selectByFlags( PDB_FLAG_WATER, pSet );
			break;
		case SELECT_DEF_SIDECHAIN:
			pSet->clear();
			pdb->selectByFlags( PDB_FLAG_BACKBONE, pSet );
			pSet->invert();
			break;
		default:
			break;
		}
	}
	void execCondList( SelectionCondListPtr condList, const T *pdb, select_func func )
	{
		//Allocate base set
		MDTRA_SelectionSet<T> *selSet;
	
		selSet = new MDTRA_SelectionSet<T>( pdb->getAtomCount() );

		//Execute condList
		execCondList_i( condList, selSet, pdb );

		//Mark all atoms from set
		if (func) func( selSet );
	
		delete selSet;
	}
	void freeCond( SelectionCondPtr cond )
	{
		switch (cond->def)
		{
		case SELECT_DEF_CONDLIST:
			freeCondList( cond->u.l ); 
			break;
		case SELECT_DEF_ELEMENT:
			delete[] cond->u.s;
			break;
		case SELECT_DEF_RESIDUE:
			delete[] cond->u.residue.s;
			break;
		case SELECT_DEF_ATOM:
			if (cond->u.atom.res)
				delete[] cond->u.atom.res;
			delete[] cond->u.atom.s;
			break;
		default:
			break;
		}
		delete cond;
	}
	void freeCondList( SelectionCondListPtr condList )
	{
		if (condList->c1) {
			freeCond(condList->c1);
		}
		if (condList->c2) {
			freeCond(condList->c2);
		}
		delete condList;
	}
};

#endif //MDTRA_SELECT_H