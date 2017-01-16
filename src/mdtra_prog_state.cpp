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
//	Program state management
//	Lua state management
//	Binding MDTRA functions to Lua

#include "mdtra_main.h"
#include "mdtra_pdb.h"
#include "mdtra_pdb_flags.h"
#include "mdtra_prog_state.h"

//------------------------------------------------------------
// Some helper macros
// I know they're tools of satan, but cannot resist temptation
//------------------------------------------------------------
#define LUASTATE_TO_PROGSTATE(x)			(reinterpret_cast<MDTRA_ProgState*>(lua_getuserdata( x )))
#define PROGSTATE_TO_PDBFILE(x)				(reinterpret_cast<MDTRA_PDB_File*>(x->pdbFile))
#define PROGSTATE_TO_PDBREF(x)				(reinterpret_cast<MDTRA_PDB_File*>(x->pdbRef))
#define CHECK_ARGC(x, y)					{ int ac; if ((ac = (lua_gettop(x)))!=y) return luaL_error( L, "'%s' : function called with %d argumens, %d expected",__func_name__,ac,y); }
#define IMPLEMENT_CREATE(x)					MAKETYPEDEF(x)* MDTRA_Prog_Create_##x(lua_State* L) { MAKETYPEDEF(x)* v = (MAKETYPEDEF(x)*)lua_newuserdata(L, sizeof(MAKETYPEDEF(x))); luaL_getmetatable(L, #x); lua_setmetatable(L, -2); return v; }
#define CALL_CREATE(x,y)					MDTRA_Prog_Create_##y(x)
#define MAKETYPEDEF(name)					struct stMDTRA_ProgTypedef_##name
#define GETTYPEDEF(x,y,n,z)					void *__ud##n = luaL_checkudata(x, n, #y); luaL_argcheck(x, __ud##n != NULL, n, "`" #y "' expected"); MAKETYPEDEF(y)* z = (MAKETYPEDEF(y)*)__ud##n;
#define LUAFUNC_BEGIN(struc,name,argc)		static int MDTRA_Prog_##name##_##struc(lua_State *L) { static const char __func_name__[] = #name; CHECK_ARGC(L,argc);
#define LUAFUNC_BEGIN_OVERLOAD(struc,name)	static int MDTRA_Prog_##name##_##struc(lua_State *L) { static const char __func_name__[] = #name;
#define LUAFUNC_END()						return 1; }
#define LUAFUNC_END2()						return 2; }
#define LUAREG_BEGIN_FUNCS(x)				static const luaL_Reg MDTRA_ProgRegFuncs_##x[] = {
#define LUAREG_NAME_FUNCS(x)				MDTRA_ProgRegFuncs_##x
#define LUAREG_BEGIN_META(x)				static const luaL_Reg MDTRA_ProgRegMeta_##x[] = {
#define LUAREG_NAME_META(x)					MDTRA_ProgRegMeta_##x
#define LUAREG_END()						};
#define LUAFUNC_DEF(struc,name)				{ #name, MDTRA_Prog_##name##_##struc }
#define LUAFUNC_NONE()						{ NULL, NULL }

static void *MDTRA_ProgTypedef_Check( lua_State *L, int ud, const char *tname ) 
{
	void *p = lua_touserdata( L, ud );
	if ( p != NULL ) {
		if ( lua_getmetatable( L, ud )) {
			lua_getfield( L, LUA_REGISTRYINDEX, tname );
			if (lua_rawequal( L, -1, -2 )) {
				lua_pop( L, 2 );
				return p;
			}
		}
	}
	return NULL;
}

static const char* MDTRA_ProgTypedef_Typename( lua_State* L, int argn )
{
	int t = lua_type( L, argn );
	if ( t == LUA_TUSERDATA ) {
		void *ud;
		ud = MDTRA_ProgTypedef_Check( L, argn, "vec2" ); if ( ud ) return "vec2";
		ud = MDTRA_ProgTypedef_Check( L, argn, "vec3" ); if ( ud ) return "vec3";
		ud = MDTRA_ProgTypedef_Check( L, argn, "mat3" ); if ( ud ) return "mat3";
	}
	return lua_typename( L, t );
}

static bool MDTRA_ProgTypedef_AcceptableType( lua_State* L, int argn )
{
	int t = lua_type( L, argn );
	if ( t == LUA_TUSERDATA ) {
		void *ud;
		ud = MDTRA_ProgTypedef_Check( L, argn, "vec2" ); if ( ud ) return true;
		ud = MDTRA_ProgTypedef_Check( L, argn, "vec3" ); if ( ud ) return true;
		ud = MDTRA_ProgTypedef_Check( L, argn, "mat3" ); if ( ud ) return true;
		return false;
	}
	return true;
}

/*------------------------------------------------------------*/
/*------------------ START MDTRA -> LUA API ------------------*/
/*------------------------------------------------------------*/
/*------------------------------------------------------------*/
/*						2D Vector Type						  */
/*------------------------------------------------------------*/
struct stMDTRA_ProgTypedef_vec2 {
	double v[2];
};

IMPLEMENT_CREATE(vec2)

//vec2(): three overloads depending on argument count (0, 1, 2)
LUAFUNC_BEGIN_OVERLOAD(vec2, __call)
{
	int argc = lua_gettop( L );
	if (argc > 3) return luaL_error( L, "'%s' : called with %d argumens, maximum %d expected",__func_name__,argc,2);
	MAKETYPEDEF(vec2)* v = CALL_CREATE( L, vec2 );
	switch (argc) {
	default:
	case 1: v->v[0] = v->v[1] = 0.0; break;
	case 2: v->v[0] = v->v[1] = luaL_checknumber( L, 2 ); break;
	case 3: v->v[0] = luaL_checknumber( L, 2 ); v->v[1] = luaL_checknumber( L, 3 ); break;
	}
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec2, __index, 2)
{
	GETTYPEDEF( L, vec2, 1, v );
	int index = luaL_checkint(L, 2);
    luaL_argcheck(L, 1 <= index && index <= 2, 2, "index out of range");
	lua_pushnumber( L, v->v[index-1] );
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec2, __newindex, 3)
{
	GETTYPEDEF( L, vec2, 1, v );
	int index = luaL_checkint(L, 2);
    luaL_argcheck(L, 1 <= index && index <= 2, 2, "index out of range");
	double value = luaL_checknumber(L, 3);
	v->v[index-1] = value;
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec2, __len, 2)
{
	lua_pushinteger( L, 2 );
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec2, __unm, 2)
{
	GETTYPEDEF( L, vec2, 1, v );
	MAKETYPEDEF(vec2)* result = CALL_CREATE( L, vec2 );
	result->v[0] = -v->v[0];
	result->v[1] = -v->v[1];
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec2, __add, 2)
{
	GETTYPEDEF( L, vec2, 1, v );
	GETTYPEDEF( L, vec2, 2, v2 );
	MAKETYPEDEF(vec2)* result = CALL_CREATE( L, vec2 );
	result->v[0] = v->v[0] + v2->v[0];
	result->v[1] = v->v[1] + v2->v[1];
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec2, __sub, 2)
{
	GETTYPEDEF( L, vec2, 1, v );
	GETTYPEDEF( L, vec2, 2, v2 );
	MAKETYPEDEF(vec2)* result = CALL_CREATE( L, vec2 );
	result->v[0] = v->v[0] - v2->v[0];
	result->v[1] = v->v[1] - v2->v[1];
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec2, __mul, 2)
{
	if ( lua_isnumber( L, 1 ) ) {
		double d = luaL_checknumber(L, 1);
		GETTYPEDEF( L, vec2, 2, v );
		MAKETYPEDEF(vec2)* result = CALL_CREATE( L, vec2 );
		result->v[0] = v->v[0] * d;
		result->v[1] = v->v[1] * d;
	} else if ( lua_isnumber( L, 2 ) ) {
		double d = luaL_checknumber(L, 2);
		GETTYPEDEF( L, vec2, 1, v );
		MAKETYPEDEF(vec2)* result = CALL_CREATE( L, vec2 );
		result->v[0] = v->v[0] * d;
		result->v[1] = v->v[1] * d;
	} else if ( MDTRA_ProgTypedef_Check( L, 2, "vec2" ) ) {
		GETTYPEDEF( L, vec2, 1, v );
		GETTYPEDEF( L, vec2, 2, v2 );
		MAKETYPEDEF(vec2)* result = CALL_CREATE( L, vec2 );
		result->v[0] = v->v[0] * v2->v[0];
		result->v[1] = v->v[1] * v2->v[1];
	} else {
		return luaL_error( L, "'%s' : cannot multiply %s and %s",__func_name__,MDTRA_ProgTypedef_Typename(L,1),MDTRA_ProgTypedef_Typename(L,2));
	}
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec2, __div, 2)
{
	if ( lua_isnumber( L, 2 ) ) {
		double d = luaL_checknumber(L, 2);
		GETTYPEDEF( L, vec2, 1, v );
		MAKETYPEDEF(vec2)* result = CALL_CREATE( L, vec2 );
		result->v[0] = v->v[0] / d;
		result->v[1] = v->v[1] / d;
	} else if ( MDTRA_ProgTypedef_Check( L, 2, "vec2" ) ) {
		GETTYPEDEF( L, vec2, 1, v );
		GETTYPEDEF( L, vec2, 2, v2 );
		MAKETYPEDEF(vec2)* result = CALL_CREATE( L, vec2 );
		result->v[0] = v->v[0] / v2->v[0];
		result->v[1] = v->v[1] / v2->v[1];
	} else {
		return luaL_error( L, "'%s' : cannot divide %s by %s",__func_name__,MDTRA_ProgTypedef_Typename(L,1),MDTRA_ProgTypedef_Typename(L,2));
	}
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec2, __eq, 2)
{
	GETTYPEDEF( L, vec2, 1, v );
	GETTYPEDEF( L, vec2, 2, v2 );
	if ( (v->v[0] == v2->v[0]) && (v->v[1] == v2->v[1]) )
		lua_pushboolean( L, 1 );
	else
		lua_pushboolean( L, 0 );	
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec2, __lt, 2)
{
	GETTYPEDEF( L, vec2, 1, v );
	GETTYPEDEF( L, vec2, 2, v2 );
	if ( (v->v[0] < v2->v[0]) && (v->v[1] < v2->v[1]) )
		lua_pushboolean( L, 1 );
	else
		lua_pushboolean( L, 0 );	
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec2, __le, 2)
{
	GETTYPEDEF( L, vec2, 1, v );
	GETTYPEDEF( L, vec2, 2, v2 );
	if ( (v->v[0] <= v2->v[0]) && (v->v[1] <= v2->v[1]) )
		lua_pushboolean( L, 1 );
	else
		lua_pushboolean( L, 0 );	
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec2, __tostring, 1)
{
	GETTYPEDEF( L, vec2, 1, v );
	lua_pushfstring( L, "(%f, %f)", v->v[0], v->v[1] );
}
LUAFUNC_END()

/*------------------------------------------------------------*/
/*						3D Vector Type						  */
/*------------------------------------------------------------*/
struct stMDTRA_ProgTypedef_vec3 {
	double v[3];
};

IMPLEMENT_CREATE(vec3)

//vec3(): three overloads depending on argument count (0, 1, 3)
LUAFUNC_BEGIN_OVERLOAD(vec3, __call)
{
	int argc = lua_gettop( L );
	if (argc == 3 || argc > 4) return luaL_error( L, "'%s' : called with %d argumens, maximum %d expected",__func_name__,argc,2);
	MAKETYPEDEF(vec3)* v = CALL_CREATE( L, vec3 );
	switch (argc) {
	default:
	case 1: v->v[0] = v->v[1] = v->v[2] = 0.0; break;
	case 2: v->v[0] = v->v[1] = v->v[2] = luaL_checknumber( L, 2 ); break;
	case 4: v->v[0] = luaL_checknumber( L, 2 ); v->v[1] = luaL_checknumber( L, 3 ); v->v[2] = luaL_checknumber( L, 4 ); break;
	}
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec3, __index, 2)
{
	GETTYPEDEF( L, vec3, 1, v );
	int index = luaL_checkint(L, 2);
    luaL_argcheck(L, 1 <= index && index <= 3, 2, "index out of range");
	lua_pushnumber( L, v->v[index-1] );
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec3, __newindex, 3)
{
	GETTYPEDEF( L, vec3, 1, v );
	int index = luaL_checkint(L, 2);
    luaL_argcheck(L, 1 <= index && index <= 3, 2, "index out of range");
	double value = luaL_checknumber(L, 3);
	v->v[index-1] = value;
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec3, __len, 2)
{
	lua_pushinteger( L, 3 );
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec3, __unm, 2)
{
	GETTYPEDEF( L, vec3, 1, v );
	MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );
	result->v[0] = -v->v[0];
	result->v[1] = -v->v[1];
	result->v[2] = -v->v[2];
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec3, __add, 2)
{
	GETTYPEDEF( L, vec3, 1, v );
	GETTYPEDEF( L, vec3, 2, v2 );
	MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );
	result->v[0] = v->v[0] + v2->v[0];
	result->v[1] = v->v[1] + v2->v[1];
	result->v[2] = v->v[2] + v2->v[2];
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec3, __sub, 2)
{
	GETTYPEDEF( L, vec3, 1, v );
	GETTYPEDEF( L, vec3, 2, v2 );
	MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );
	result->v[0] = v->v[0] - v2->v[0];
	result->v[1] = v->v[1] - v2->v[1];
	result->v[2] = v->v[2] - v2->v[2];
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec3, __mul, 2)
{
	if ( lua_isnumber( L, 1 ) ) {
		double d = luaL_checknumber(L, 1);
		GETTYPEDEF( L, vec3, 2, v );
		MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );
		result->v[0] = v->v[0] * d;
		result->v[1] = v->v[1] * d;
		result->v[2] = v->v[2] * d;
	} else if ( lua_isnumber( L, 2 ) ) {
		double d = luaL_checknumber(L, 2);
		GETTYPEDEF( L, vec3, 1, v );
		MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );
		result->v[0] = v->v[0] * d;
		result->v[1] = v->v[1] * d;
		result->v[2] = v->v[2] * d;
	} else if ( MDTRA_ProgTypedef_Check( L, 2, "vec3" ) ) {
		GETTYPEDEF( L, vec3, 1, v );
		GETTYPEDEF( L, vec3, 2, v2 );
		MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );
		result->v[0] = v->v[0] * v2->v[0];
		result->v[1] = v->v[1] * v2->v[1];
		result->v[2] = v->v[2] * v2->v[2];
	} else {
		return luaL_error( L, "'%s' : cannot multiply %s and %s",__func_name__,MDTRA_ProgTypedef_Typename(L,1),MDTRA_ProgTypedef_Typename(L,2));
	}
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec3, __div, 2)
{
	if ( lua_isnumber( L, 2 ) ) {
		double d = luaL_checknumber(L, 2);
		GETTYPEDEF( L, vec3, 1, v );
		MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );
		result->v[0] = v->v[0] / d;
		result->v[1] = v->v[1] / d;
		result->v[2] = v->v[2] / d;
	} else if ( MDTRA_ProgTypedef_Check( L, 2, "vec3" ) ) {
		GETTYPEDEF( L, vec3, 1, v );
		GETTYPEDEF( L, vec3, 2, v2 );
		MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );
		result->v[0] = v->v[0] / v2->v[0];
		result->v[1] = v->v[1] / v2->v[1];
		result->v[2] = v->v[2] / v2->v[2];
	} else {
		return luaL_error( L, "'%s' : cannot divide %s by %s",__func_name__,MDTRA_ProgTypedef_Typename(L,1),MDTRA_ProgTypedef_Typename(L,2));
	}
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec3, __eq, 2)
{
	GETTYPEDEF( L, vec3, 1, v );
	GETTYPEDEF( L, vec3, 2, v2 );
	if ( (v->v[0] == v2->v[0]) && (v->v[1] == v2->v[1]) && (v->v[2] == v2->v[2]) )
		lua_pushboolean( L, 1 );
	else
		lua_pushboolean( L, 0 );	
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec3, __lt, 2)
{
	GETTYPEDEF( L, vec3, 1, v );
	GETTYPEDEF( L, vec3, 2, v2 );
	if ( (v->v[0] < v2->v[0]) && (v->v[1] < v2->v[1]) && (v->v[2] < v2->v[2]) )
		lua_pushboolean( L, 1 );
	else
		lua_pushboolean( L, 0 );	
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec3, __le, 2)
{
	GETTYPEDEF( L, vec3, 1, v );
	GETTYPEDEF( L, vec3, 2, v2 );
	if ( (v->v[0] <= v2->v[0]) && (v->v[1] <= v2->v[1]) && (v->v[2] <= v2->v[2]) )
		lua_pushboolean( L, 1 );
	else
		lua_pushboolean( L, 0 );	
}
LUAFUNC_END()
LUAFUNC_BEGIN(vec3, __tostring, 1)
{
	GETTYPEDEF( L, vec3, 1, v );
	lua_pushfstring( L, "(%f, %f, %f)", v->v[0], v->v[1], v->v[2] );
}
LUAFUNC_END()


/*------------------------------------------------------------*/
/*						3x3 Matrix Type						  */
/*------------------------------------------------------------*/
struct stMDTRA_ProgTypedef_mat3 {
	double v[9];
};

IMPLEMENT_CREATE(mat3)

//mat3(): three overloads depending on argument count (0, 1, .., 9)
LUAFUNC_BEGIN_OVERLOAD(mat3, __call)
{
	int argc = lua_gettop( L );
	if ( argc != 10 && argc > 2) return luaL_error( L, "'%s' : called with %d argumens, 0, 1 or 9 expected",__func_name__,argc);
	MAKETYPEDEF(mat3)* v = CALL_CREATE( L, mat3 );
	switch (argc) {
	default:
	case 1: v->v[0] = v->v[1] = v->v[2] = v->v[3] = v->v[4] = v->v[5] = v->v[6] = v->v[7] = v->v[8] = 0.0; break;
	case 2: v->v[0] = v->v[1] = v->v[2] = v->v[3] = v->v[4] = v->v[5] = v->v[6] = v->v[7] = v->v[8] = luaL_checknumber( L, 2 ); break;
	case 10: v->v[0] = luaL_checknumber( L, 2 ); 
			 v->v[1] = luaL_checknumber( L, 3 ); 
			 v->v[2] = luaL_checknumber( L, 4 ); 
			 v->v[3] = luaL_checknumber( L, 5 ); 
			 v->v[4] = luaL_checknumber( L, 6 ); 
			 v->v[5] = luaL_checknumber( L, 7 ); 
			 v->v[6] = luaL_checknumber( L, 8 ); 
			 v->v[7] = luaL_checknumber( L, 9 ); 
			 v->v[8] = luaL_checknumber( L, 10 ); 
			break;
	}
}
LUAFUNC_END()
LUAFUNC_BEGIN(mat3, __index, 2)
{
	GETTYPEDEF( L, mat3, 1, v );
	int index = luaL_checkint(L, 2);
    luaL_argcheck(L, 1 <= index && index <= 9, 2, "index out of range");
	lua_pushnumber( L, v->v[index-1] );
}
LUAFUNC_END()
LUAFUNC_BEGIN(mat3, __newindex, 3)
{
	GETTYPEDEF( L, mat3, 1, v );
	int index = luaL_checkint(L, 2);
    luaL_argcheck(L, 1 <= index && index <= 9, 2, "index out of range");
	double value = luaL_checknumber(L, 3);
	v->v[index-1] = value;
}
LUAFUNC_END()
LUAFUNC_BEGIN(mat3, __len, 2)
{
	lua_pushinteger( L, 9 );
}
LUAFUNC_END()
LUAFUNC_BEGIN(mat3, __unm, 2)
{
	GETTYPEDEF( L, mat3, 1, v );
	MAKETYPEDEF(mat3)* result = CALL_CREATE( L, mat3 );
	for ( int counter = 0; counter < 9; counter++ )
		result->v[counter] = -v->v[counter];
}
LUAFUNC_END()
LUAFUNC_BEGIN(mat3, __add, 2)
{
	GETTYPEDEF( L, mat3, 1, v );
	GETTYPEDEF( L, mat3, 2, v2 );
	MAKETYPEDEF(mat3)* result = CALL_CREATE( L, mat3 );
	for ( int counter = 0; counter < 9; counter++ )
		result->v[counter] = v->v[counter] + v2->v[counter];
}
LUAFUNC_END()
LUAFUNC_BEGIN(mat3, __sub, 2)
{
	GETTYPEDEF( L, mat3, 1, v );
	GETTYPEDEF( L, mat3, 2, v2 );
	MAKETYPEDEF(mat3)* result = CALL_CREATE( L, mat3 );
	for ( int counter = 0; counter < 9; counter++ )
		result->v[counter] = v->v[counter] - v2->v[counter];
}
LUAFUNC_END()

LUAFUNC_BEGIN(mat3, __mul, 2)
{
	if ( MDTRA_ProgTypedef_Check( L, 2, "vec3" ) ) {
		GETTYPEDEF( L, mat3, 1, v );
		GETTYPEDEF( L, vec3, 2, v2 );
		MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );

		result->v[0] = v->v[0] * v2->v[0] + v->v[1] * v2->v[1] + v->v[2] * v2->v[2];
		result->v[1] = v->v[3] * v2->v[0] + v->v[4] * v2->v[1] + v->v[5] * v2->v[2];
		result->v[2] = v->v[6] * v2->v[0] + v->v[7] * v2->v[1] + v->v[8] * v2->v[2];
	} else if ( MDTRA_ProgTypedef_Check( L, 2, "mat3" ) ) {
		GETTYPEDEF( L, mat3, 1, v );
		GETTYPEDEF( L, mat3, 2, v2 );
		MAKETYPEDEF(mat3)* result = CALL_CREATE( L, mat3 );
		result->v[0] = v->v[0] * v2->v[0] + v->v[1] * v2->v[3] + v->v[2] * v2->v[6];
		result->v[1] = v->v[0] * v2->v[1] + v->v[1] * v2->v[4] + v->v[2] * v2->v[7];
		result->v[2] = v->v[0] * v2->v[2] + v->v[1] * v2->v[5] + v->v[2] * v2->v[8];
		result->v[3] = v->v[3] * v2->v[0] + v->v[4] * v2->v[3] + v->v[5] * v2->v[6];
		result->v[4] = v->v[3] * v2->v[1] + v->v[4] * v2->v[4] + v->v[5] * v2->v[7];
		result->v[5] = v->v[3] * v2->v[2] + v->v[4] * v2->v[5] + v->v[5] * v2->v[8];
		result->v[6] = v->v[6] * v2->v[0] + v->v[7] * v2->v[3] + v->v[8] * v2->v[6];
		result->v[7] = v->v[6] * v2->v[1] + v->v[7] * v2->v[4] + v->v[8] * v2->v[7];
		result->v[8] = v->v[6] * v2->v[2] + v->v[7] * v2->v[5] + v->v[8] * v2->v[8];
	} else {
		return luaL_error( L, "'%s' : cannot multiply %s and %s",__func_name__,MDTRA_ProgTypedef_Typename(L,1),MDTRA_ProgTypedef_Typename(L,2));
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(mat3, __tostring, 1)
{
	GETTYPEDEF( L, mat3, 1, v );
	lua_pushfstring( L, "((%f, %f, %f),(%f, %f, %f),(%f, %f, %f))", 
						v->v[0], v->v[1], v->v[2],
						v->v[3], v->v[4], v->v[5],
						v->v[6], v->v[7], v->v[8]);
}
LUAFUNC_END()

/*------------------------------------------------------------*/
/*						Global Functions					  */
/*------------------------------------------------------------*/
LUAFUNC_BEGIN(global, datapos, 0)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	lua_pushinteger( L, S->dataPos );
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, datasize, 0)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	lua_pushinteger( L, S->dataSize );
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, dataread, 1)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (!(S->inputFlags & MDTRA_PSIF_REDUCE)) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may be called only from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		int datapos = luaL_checkinteger( L, 1 );
		luaL_argcheck(L, 1 <= datapos && datapos <= S->dataSize, 1, "index out of range");
		lua_pushnumber( L, S->dataPtr ? S->dataPtr[datapos-1] : 0.0 );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, datawrite, 2)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (!(S->inputFlags & MDTRA_PSIF_REDUCE)) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may be called only from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		int datapos = luaL_checkinteger( L, 1 );
		luaL_argcheck(L, 1 <= datapos && datapos <= S->dataSize, 1, "index out of range");
		double datavalue = luaL_checknumber( L, 2 );

		if (S->dataPtr) {
			S->outputFlags |= MDTRA_PSOF_DATAMODIFIED;
			S->dataPtr[datapos-1] = (float)datavalue;
			lua_pushnumber( L, datavalue );
		} else {
			lua_pushnumber( L, 0.0 );
		}
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, rboutput, 2)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		int resnumber = luaL_checkinteger( L, 1 );
		luaL_argcheck(L, 1 <= resnumber && resnumber <= S->resSize, 1, "index out of range");
		double resvalue = luaL_checknumber( L, 2 );

		if (S->resPtr) {
			S->resPtr[resnumber-1] = (float)resvalue;
			lua_pushnumber( L, resvalue );
		} else {
			lua_pushnumber( L, 0.0 );
		}
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, numatoms, 0)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushinteger( L, 0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );	
	
		int ivalue = 0;
		if ( P ) {
			ivalue = P->getAtomCount();
		}
		lua_pushinteger( L, ivalue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, numresidues, 0)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushinteger( L, 0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );	
	
		int ivalue = 0;
		if ( P ) {
			ivalue = P->getResidueCount();
		}
		lua_pushinteger( L, ivalue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, rmsd, 0)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );	
		MDTRA_PDB_File* P2 = PROGSTATE_TO_PDBREF(S);
		if (!P2 && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : reference PDB file is NULL", __func_name__ );	
	
		float fvalue = 0.0f;
		if ( P && P2 ) {
			if (!( S->outputFlags & MDTRA_PSOF_ALIGNED )) {
				P->move_to_centroid();
				P->align_kabsch( P2 );
				S->outputFlags |= MDTRA_PSOF_ALIGNED;
			}
			fvalue = P->get_rmsd( P2 );
		}
		lua_pushnumber( L, fvalue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, sas, 0)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );	
	
		float fvalue = 0.0f;
		if ( P ) {
			P->set_flag( 0, NULL, PDB_FLAG_SAS );
			fvalue = P->get_sas( S->index );
		}
		lua_pushnumber( L, fvalue );
	}
}
LUAFUNC_END()


LUAFUNC_BEGIN_OVERLOAD(global, coord)
{
	int argc = lua_gettop( L );
	if (argc > 3) return luaL_error( L, "'%s' : called with %d argumens, maximum %d expected",__func_name__,argc,2);
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );
		memset( result->v, 0, sizeof(result->v) );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );
		const MDTRA_PDB_Atom* at = NULL;
		MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );
		memset( result->v, 0, sizeof(result->v) );
		if ( P ) {
			if ( argc == 1 ) {
				int serial = luaL_checkinteger( L, 1 );
				at = P->fetchAtomBySerialNumber( serial );
			} else {
				int chain = luaL_checkinteger( L, 1 );
				int residue = luaL_checkinteger( L, 2 );
				const char *title = luaL_checklstring( L, 3, NULL );
				at = P->fetchAtomByDesc( chain, residue, title );
			}
		}
		if ( at ) {
			result->v[0] = at->original_xyz[0];
			result->v[1] = at->original_xyz[1];
			result->v[2] = at->original_xyz[2];
		}
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN_OVERLOAD(global, radius)
{
	int argc = lua_gettop( L );
	if (argc > 3) return luaL_error( L, "'%s' : called with %d argumens, maximum %d expected",__func_name__,argc,2);
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );
		const MDTRA_PDB_Atom* at = NULL;
		float vdwr = 0.0f;
		if ( P ) {
			if ( argc == 1 ) {
				int serial = luaL_checkinteger( L, 1 );
				at = P->fetchAtomBySerialNumber( serial );
			} else {
				int chain = luaL_checkinteger( L, 1 );
				int residue = luaL_checkinteger( L, 2 );
				const char *title = luaL_checklstring( L, 3, NULL );
				at = P->fetchAtomByDesc( chain, residue, title );
			}
		}
		if ( at ) vdwr = at->vdwRadius;
		lua_pushnumber( L, vdwr );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, serial, 3)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushinteger( L, 0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );
		const MDTRA_PDB_Atom* at = NULL;
		int serial = 0;
		int chain = luaL_checkinteger( L, 1 );
		int residue = luaL_checkinteger( L, 2 );
		const char *title = luaL_checklstring( L, 3, NULL );
		if ( P ) at = P->fetchAtomByDesc( chain, residue, title );
		if ( at ) serial = at->serialnumber;
		lua_pushinteger( L, serial );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, residue, 1)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushinteger( L, 0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );
		int serial = luaL_checkinteger( L, 1 );
		int residue = 0;
		const MDTRA_PDB_Atom* at;
		if ( P && (at = P->fetchAtomBySerialNumber( serial )) ) { 
			residue = at->residuenumber;
		}
		lua_pushinteger( L, residue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, atomflags, 1)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushinteger( L, 0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );
		int serial = luaL_checkinteger( L, 1 );
		int atFlags = 0;
		const MDTRA_PDB_Atom* at;
		if ( P && (at = P->fetchAtomBySerialNumber( serial )) ) { 
			atFlags = at->atomFlags;
		}
		lua_pushinteger( L, atFlags );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, checkflags, 2)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushinteger( L, 0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );
		int serial = luaL_checkinteger( L, 1 );
		int checkf = luaL_checkinteger( L, 2 );
		int atFlags = 0;
		const MDTRA_PDB_Atom* at;
		if ( P && (at = P->fetchAtomBySerialNumber( serial )) ) { 
			atFlags = at->atomFlags;
		}
		lua_pushboolean( L, (atFlags & checkf) ? 1 : 0 );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, sqdev, 1)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );	
		MDTRA_PDB_File* P2 = PROGSTATE_TO_PDBREF(S);
		if (!P2 && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : reference PDB file is NULL", __func_name__ );	

		int serial = luaL_checkinteger( L, 1 );
		float fvalue = 0.0f;

		if ( P && P2 ) {
			const MDTRA_PDB_Atom* atCurrent;
			const MDTRA_PDB_Atom* atRef;
			if (!( S->outputFlags & MDTRA_PSOF_ALIGNED )) {
				P->move_to_centroid();
				P->align_kabsch( P2 );
				S->outputFlags |= MDTRA_PSOF_ALIGNED;
			}
			atCurrent = P->fetchAtomBySerialNumber( serial );
			atRef = P2->fetchAtomBySerialNumber( serial );
			if ( atCurrent && atRef ) {
				fvalue = (atCurrent->xyz[0]-atRef->xyz[0])*(atCurrent->xyz[0]-atRef->xyz[0]) +
						 (atCurrent->xyz[1]-atRef->xyz[1])*(atCurrent->xyz[1]-atRef->xyz[1]) +
						 (atCurrent->xyz[2]-atRef->xyz[2])*(atCurrent->xyz[2]-atRef->xyz[2]);
			}
		}
		lua_pushnumber( L, fvalue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, distance, 2)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );	
		int serial1 = luaL_checkinteger( L, 1 );
		int serial2 = luaL_checkinteger( L, 2 );
		float fvalue = P ? (P->get_distance( serial1, serial2 )) : 0.0f;
		lua_pushnumber( L, fvalue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, angle, 3)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );
		int serial1 = luaL_checkinteger( L, 1 );
		int serial2 = luaL_checkinteger( L, 2 );
		int serial3 = luaL_checkinteger( L, 3 );
		float fvalue = P ? (P->get_angle( serial1, serial2, serial3 )) : 0.0f;
		lua_pushnumber( L, fvalue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, torsion, 4)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );
		int serial1 = luaL_checkinteger( L, 1 );
		int serial2 = luaL_checkinteger( L, 2 );
		int serial3 = luaL_checkinteger( L, 3 );
		int serial4 = luaL_checkinteger( L, 4 );
		float fvalue = P ? (P->get_torsion( serial1, serial2, serial3, serial4, true )) : 0.0f;
		lua_pushnumber( L, fvalue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, utorsion, 4)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );
		int serial1 = luaL_checkinteger( L, 1 );
		int serial2 = luaL_checkinteger( L, 2 );
		int serial3 = luaL_checkinteger( L, 3 );
		int serial4 = luaL_checkinteger( L, 4 );
		float fvalue = P ? (P->get_torsion( serial1, serial2, serial3, serial4, false )) : 0.0f;
		lua_pushnumber( L, fvalue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, dihedral, 4)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );
		int serial1 = luaL_checkinteger( L, 1 );
		int serial2 = luaL_checkinteger( L, 2 );
		int serial3 = luaL_checkinteger( L, 3 );
		int serial4 = luaL_checkinteger( L, 4 );
		float fvalue = P ? (P->get_dihedral( serial1, serial2, serial3, serial4, false )) : 0.0f;
		lua_pushnumber( L, fvalue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, force, 1)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );
		int serial1 = luaL_checkinteger( L, 1 );
		float fvalue = P ? (P->get_force( serial1 )) : 0.0f;
		lua_pushnumber( L, fvalue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, resultant, 2)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );
		int serial1 = luaL_checkinteger( L, 1 );
		int serial2 = luaL_checkinteger( L, 2 );
		float fvalue = P ? (P->get_resultant_force( serial1, serial2 )) : 0.0f;
		lua_pushnumber( L, fvalue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, hbenergy, 2)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		lua_pushnumber( L, 0.0 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );	
		int serial1 = luaL_checkinteger( L, 1 );
		int serial2 = luaL_checkinteger( L, 2 );
		float fvalue = P ? (P->get_hbenergy( serial1, serial2 )) : 0.0f;
		lua_pushnumber( L, fvalue );
	}
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, dot, 2)
{
	// overloads:
	// dot( vec2, vec2 )
	// dot( vec3, vec3 )
	void *ud1 = MDTRA_ProgTypedef_Check( L, 1, "vec2");
	void *ud2 = MDTRA_ProgTypedef_Check( L, 2, "vec2");
	if ( ud1 && ud2 ) {
		MAKETYPEDEF(vec2)* v = (MAKETYPEDEF(vec2)*)ud1;
		MAKETYPEDEF(vec2)* v2 = (MAKETYPEDEF(vec2)*)ud2;
		double result = v->v[0]*v2->v[0] + v->v[1]*v2->v[1];
		lua_pushnumber( L, result );
		return 1;
	}
	ud1 = MDTRA_ProgTypedef_Check( L, 1, "vec3");
	ud2 = MDTRA_ProgTypedef_Check( L, 2, "vec3");
	if ( ud1 && ud2 ) {
		MAKETYPEDEF(vec3)* v = (MAKETYPEDEF(vec3)*)ud1;
		MAKETYPEDEF(vec3)* v2 = (MAKETYPEDEF(vec3)*)ud2;
		double result = v->v[0]*v2->v[0] + v->v[1]*v2->v[1] + v->v[2]*v2->v[2];
		lua_pushnumber( L, result );
		return 1;
	}
	return luaL_error( L, "'%s' : cannot calculate dot product of %s and %s",__func_name__,MDTRA_ProgTypedef_Typename(L,1),MDTRA_ProgTypedef_Typename(L,2));
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, length, 1)
{
	// overloads:
	// length( vec2 )
	// length( vec3 )
	void *ud1 = MDTRA_ProgTypedef_Check( L, 1, "vec2");
	if ( ud1 ) {
		MAKETYPEDEF(vec2)* v = (MAKETYPEDEF(vec2)*)ud1;
		double result = sqrt( v->v[0]*v->v[0] + v->v[1]*v->v[1] );
		lua_pushnumber( L, result );
		return 1;
	}
	ud1 = MDTRA_ProgTypedef_Check( L, 1, "vec3");
	if ( ud1 ) {
		MAKETYPEDEF(vec3)* v = (MAKETYPEDEF(vec3)*)ud1;
		double result = sqrt( v->v[0]*v->v[0] + v->v[1]*v->v[1] + v->v[2]*v->v[2] );
		lua_pushnumber( L, result );
		return 1;
	}
	return luaL_error( L, "'%s' : cannot calculate length of %s",__func_name__,MDTRA_ProgTypedef_Typename(L,1));
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, normalize, 1)
{
	// overloads:
	// normalize( vec2 )
	// normalize( vec3 )
	void *ud1 = MDTRA_ProgTypedef_Check( L, 1, "vec2");
	if ( ud1 ) {
		MAKETYPEDEF(vec2)* v = (MAKETYPEDEF(vec2)*)ud1;
		MAKETYPEDEF(vec2)* result = CALL_CREATE( L, vec2 );
		double lensq = v->v[0]*v->v[0]+v->v[1]*v->v[1]; 
		if (lensq == 0) { 
			result->v[0] = 0.0;
			result->v[1] = 1.0;
		} else { 
			double invlen = 1.0 / sqrt( lensq ); 
			result->v[0] = v->v[0]*invlen; 
			result->v[1] = v->v[1]*invlen; 
		}
		return 1;
	}
	ud1 = MDTRA_ProgTypedef_Check( L, 1, "vec3");
	if ( ud1 ) {
		MAKETYPEDEF(vec3)* v = (MAKETYPEDEF(vec3)*)ud1;
		MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );
		double lensq = v->v[0]*v->v[0]+v->v[1]*v->v[1]+v->v[2]*v->v[2]; 
		if (lensq == 0) { 
			result->v[0] = 0.0;
			result->v[1] = 1.0;
			result->v[2] = 0.0;
		} else { 
			double invlen = 1.0 / sqrt( lensq ); 
			result->v[0] = v->v[0]*invlen; 
			result->v[1] = v->v[1]*invlen; 
			result->v[2] = v->v[2]*invlen; 
		}
		return 1;
	}
	return luaL_error( L, "'%s' : cannot normalize %s",__func_name__,MDTRA_ProgTypedef_Typename(L,1));
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, cross, 2)
{
	// overloads:
	// cross( vec3, vec3 )
	void *ud1 = MDTRA_ProgTypedef_Check( L, 1, "vec3");
	void *ud2 = MDTRA_ProgTypedef_Check( L, 2, "vec3");
	if ( ud1 && ud2 ) {
		MAKETYPEDEF(vec3)* v = (MAKETYPEDEF(vec3)*)ud1;
		MAKETYPEDEF(vec3)* v2 = (MAKETYPEDEF(vec3)*)ud2;
		MAKETYPEDEF(vec3)* result = CALL_CREATE( L, vec3 );
		result->v[0] = v->v[1]*v2->v[2] - v->v[2]*v2->v[1]; 
		result->v[1] = v->v[2]*v2->v[0] - v->v[0]*v2->v[2]; 
		result->v[2] = v->v[0]*v2->v[1] - v->v[1]*v2->v[0];
		return 1;
	}
	return luaL_error( L, "'%s' : cannot calculate cross product of %s and %s",__func_name__,MDTRA_ProgTypedef_Typename(L,1),MDTRA_ProgTypedef_Typename(L,2));
}
LUAFUNC_END()

LUAFUNC_BEGIN(global, get_residue_transform, 1)
{
	MDTRA_ProgState* S = LUASTATE_TO_PROGSTATE(L);
	if (S->inputFlags & MDTRA_PSIF_REDUCE) {
		if ( S->inputFlags & MDTRA_PSIF_COMPILE )
			return luaL_error( L, "'%s' : may not be called from reduce function", __func_name__ );
		CALL_CREATE( L, mat3 );
		CALL_CREATE( L, vec3 );
	} else {
		MDTRA_PDB_File* P = PROGSTATE_TO_PDBFILE(S);
		if (!P && !(S->inputFlags & MDTRA_PSIF_COMPILE))
			return luaL_error( L, "'%s' : current PDB file is NULL", __func_name__ );	
		int serial1 = luaL_checkinteger( L, 1 );

		float rot[9];
		float org[3];

		MAKETYPEDEF(mat3)* r = CALL_CREATE( L, mat3 );
		MAKETYPEDEF(vec3)* o = CALL_CREATE( L, vec3 );

		if ( P->get_residue_transform( serial1, rot, org ) ) {
			for ( int counter = 0; counter < 9; counter++ )
				r->v[counter] = rot[counter];
			for ( int counter = 0; counter < 3; counter++ )
				o->v[counter] = org[counter];
		}
	}
}
LUAFUNC_END2()

LUAFUNC_BEGIN(global, build_rotation_matrix, 2)
{
	// overloads:
	// build_rotation_matrix( vec3, float )
	void *ud1 = MDTRA_ProgTypedef_Check( L, 1, "vec3");
	double angle = luaL_checknumber( L, 2 );
	
	if ( ud1 ) {
		MAKETYPEDEF(vec3)* v = (MAKETYPEDEF(vec3)*)ud1;
		MAKETYPEDEF(mat3)* result = CALL_CREATE( L, mat3 );

		double angle_cos = cos( angle );
		double angle_sin = sin( angle );

		result->v[0] = angle_cos + (1.0-angle_cos)*v->v[0]*v->v[0];
		result->v[1] = (1.0-angle_cos)*v->v[0]*v->v[1] - angle_sin*v->v[2];
		result->v[2] = (1.0-angle_cos)*v->v[0]*v->v[2] + angle_sin*v->v[1];
		result->v[3] = (1.0-angle_cos)*v->v[0]*v->v[1] + angle_sin*v->v[2];
		result->v[4] = angle_cos + (1.0-angle_cos)*v->v[1]*v->v[1];
		result->v[5] = (1.0-angle_cos)*v->v[1]*v->v[2] - angle_sin*v->v[0];
		result->v[6] = (1.0-angle_cos)*v->v[0]*v->v[2] - angle_sin*v->v[1];
		result->v[7] = (1.0-angle_cos)*v->v[1]*v->v[2] + angle_sin*v->v[0];
		result->v[8] = angle_cos + (1.0-angle_cos)*v->v[2]*v->v[2];
		return 1;
	}
	return luaL_error( L, "'%s' : bad argument types %s and %s",__func_name__,MDTRA_ProgTypedef_Typename(L,1),MDTRA_ProgTypedef_Typename(L,2));
}
LUAFUNC_END()
/*------------------------------------------------------------*/
/*					TABLE DECLARATION						  */
/*------------------------------------------------------------*/

LUAREG_BEGIN_FUNCS( global )
	LUAFUNC_DEF( global, datapos ),
	LUAFUNC_DEF( global, datasize ),
	LUAFUNC_DEF( global, dataread ),
	LUAFUNC_DEF( global, datawrite ),
	LUAFUNC_DEF( global, rboutput ),
	LUAFUNC_DEF( global, serial ),
	LUAFUNC_DEF( global, residue ),
	LUAFUNC_DEF( global, atomflags ),
	LUAFUNC_DEF( global, checkflags ),
	LUAFUNC_DEF( global, numatoms ),
	LUAFUNC_DEF( global, numresidues ),
	LUAFUNC_DEF( global, coord ),
	LUAFUNC_DEF( global, radius ),
	LUAFUNC_DEF( global, sqdev ),
	LUAFUNC_DEF( global, rmsd ),
	LUAFUNC_DEF( global, sas ),
	LUAFUNC_DEF( global, distance ),
	LUAFUNC_DEF( global, angle ),
	LUAFUNC_DEF( global, torsion ),
	LUAFUNC_DEF( global, utorsion ),
	LUAFUNC_DEF( global, dihedral ),
	LUAFUNC_DEF( global, force ),
	LUAFUNC_DEF( global, resultant ),
	LUAFUNC_DEF( global, hbenergy ),
	LUAFUNC_DEF( global, dot ),
	LUAFUNC_DEF( global, length ),
	LUAFUNC_DEF( global, normalize ),
	LUAFUNC_DEF( global, cross ),
	LUAFUNC_DEF( global, get_residue_transform ),
	LUAFUNC_DEF( global, build_rotation_matrix ),
	LUAFUNC_NONE()
LUAREG_END()

LUAREG_BEGIN_FUNCS( empty )
	LUAFUNC_NONE()
LUAREG_END()

LUAREG_BEGIN_META( vec2 )
	LUAFUNC_DEF( vec2, __call ),
	LUAFUNC_DEF( vec2, __index ),
	LUAFUNC_DEF( vec2, __newindex ),
	LUAFUNC_DEF( vec2, __len ),
	LUAFUNC_DEF( vec2, __unm ),
	LUAFUNC_DEF( vec2, __add ),
	LUAFUNC_DEF( vec2, __sub ),
	LUAFUNC_DEF( vec2, __mul ),
	LUAFUNC_DEF( vec2, __div ),
	LUAFUNC_DEF( vec2, __eq ),
	LUAFUNC_DEF( vec2, __lt ),
	LUAFUNC_DEF( vec2, __le ),
	LUAFUNC_DEF( vec2, __tostring ),
	LUAFUNC_NONE()
LUAREG_END()

LUAREG_BEGIN_META( vec3 )
	LUAFUNC_DEF( vec3, __call ),
	LUAFUNC_DEF( vec3, __index ),
	LUAFUNC_DEF( vec3, __newindex ),
	LUAFUNC_DEF( vec3, __len ),
	LUAFUNC_DEF( vec3, __unm ),
	LUAFUNC_DEF( vec3, __add ),
	LUAFUNC_DEF( vec3, __sub ),
	LUAFUNC_DEF( vec3, __mul ),
	LUAFUNC_DEF( vec3, __div ),
	LUAFUNC_DEF( vec3, __eq ),
	LUAFUNC_DEF( vec3, __lt ),
	LUAFUNC_DEF( vec3, __le ),
	LUAFUNC_DEF( vec3, __tostring ),
	LUAFUNC_NONE()
LUAREG_END()

LUAREG_BEGIN_META( mat3 )
	LUAFUNC_DEF( mat3, __call ),
	LUAFUNC_DEF( mat3, __index ),
	LUAFUNC_DEF( mat3, __newindex ),
	LUAFUNC_DEF( mat3, __len ),
	LUAFUNC_DEF( mat3, __unm ),
	LUAFUNC_DEF( mat3, __add ),
	LUAFUNC_DEF( mat3, __sub ),
	LUAFUNC_DEF( mat3, __mul ),
	LUAFUNC_DEF( mat3, __tostring ),
	LUAFUNC_NONE()
LUAREG_END()

/*------------------------------------------------------------*/
/*------------------- END MDTRA -> LUA API -------------------*/
/*------------------------------------------------------------*/

static bool PR_CopyType( lua_State* fromL, lua_State* toL, int argn )
{
	void *ud;
	ud = MDTRA_ProgTypedef_Check( fromL, argn, "vec2" );
	if ( ud ) { 
		GETTYPEDEF( fromL, vec2, argn, v );
		MAKETYPEDEF(vec2)* copy = CALL_CREATE( toL, vec2 );
		memcpy( copy->v, v->v, sizeof(v->v) );
		return true;
	}
	ud = MDTRA_ProgTypedef_Check( fromL, argn, "vec3" );
	if ( ud ) { 
		GETTYPEDEF( fromL, vec3, argn, v );
		MAKETYPEDEF(vec3)* copy = CALL_CREATE( toL, vec3 );
		memcpy( copy->v, v->v, sizeof(v->v) );
		return true;
	}
	ud = MDTRA_ProgTypedef_Check( fromL, argn, "mat3" );
	if ( ud ) { 
		GETTYPEDEF( fromL, mat3, argn, v );
		MAKETYPEDEF(mat3)* copy = CALL_CREATE( toL, mat3 );
		memcpy( copy->v, v->v, sizeof(v->v) );
		return true;
	}
	return false;
}

static void PR_RegisterGlobals( MDTRA_ProgState& state )
{
	luaL_register( state.luaState, "vec2", LUAREG_NAME_FUNCS(empty) );
	luaL_newmetatable( state.luaState, "vec2" );
	luaL_register( state.luaState, NULL, LUAREG_NAME_META(vec2) );
	lua_setmetatable( state.luaState, -2 );
	luaL_register( state.luaState, "vec3", LUAREG_NAME_FUNCS(empty) );
	luaL_newmetatable( state.luaState, "vec3" );
	luaL_register( state.luaState, NULL, LUAREG_NAME_META(vec3) );
	lua_setmetatable( state.luaState, -2 );
	luaL_register( state.luaState, "mat3", LUAREG_NAME_FUNCS(empty) );
	luaL_newmetatable( state.luaState, "mat3" );
	luaL_register( state.luaState, NULL, LUAREG_NAME_META(mat3) );
	lua_setmetatable( state.luaState, -2 );
	lua_settop( state.luaState, 0 );
 
	lua_pushvalue( state.luaState, LUA_GLOBALSINDEX );
	luaL_register( state.luaState, NULL, LUAREG_NAME_FUNCS(global) );

	char argname[8];
	memset( argname, 0, sizeof(argname) );
	for (int i = 0; i < MAX_DATA_SOURCE_ARGS; i++) {
		sprintf_s( argname, 7, "arg%d", i+1 );
		lua_pushinteger( state.luaState, state.args[i] );
		lua_setfield( state.luaState, -2, argname );
	}
	lua_settop( state.luaState, 0 );
}

void PR_OpenState( MDTRA_ProgState& state, int* args, int flags )
{
	assert( state.luaState == NULL );
	state.luaState = lua_open();
	assert( state.luaState != NULL );

#if 1
	luaL_openlibs( state.luaState );
#else
	luaopen_base( state.luaState );
	luaopen_table( state.luaState );
	luaopen_string( state.luaState );
	luaopen_math( state.luaState );
#endif
	luaB_printevent( NULL );

	state.inputFlags = flags;
	if (args) memcpy( state.args, args, sizeof(state.args) );

	PR_RegisterGlobals( state );
}

void PR_CloseState( MDTRA_ProgState& state )
{
	if ( state.luaState ) {
		lua_close( state.luaState );
		state.luaState = NULL;
	}
}

void PR_ReduceState( MDTRA_ProgState* threadstates, int numthreadstates )
{
	if ( numthreadstates <= 0 )
		return;

	//State reducer does the following steps:
	// 1) collects all global variables from each thread state
	// 2) for each variable, creates an indexeable table with thread values
	// 3) removes global variables from thread state 0, if any
	// 4) registers a table in thread state 0
	lua_State* L = threadstates[0].luaState;
	lua_State* L2;

	lua_pushnil( L );
	while ( lua_next( L, LUA_GLOBALSINDEX ) ) {
		// key: -2
		// value: -1
		if ( lua_type( L, -2 ) != LUA_TSTRING ) {
			lua_pop( L, 1 );
			continue;
		}
		if ( (lua_type( L, -1 ) != LUA_TNUMBER) &&
			 (lua_type( L, -1 ) != LUA_TSTRING) && 
			 (lua_type( L, -1 ) != LUA_TUSERDATA) ) {
			lua_pop( L, 1 );
			continue;
		}
		if ( !MDTRA_ProgTypedef_AcceptableType( L, -1 ) ) {
			lua_pop( L, 1 );
			continue;
		}

		const char* key = lua_tostring( L, -2 );
		if (!strlen(key)) {
			lua_pop( L, 1 );
			continue;
		}
		if (!strcmp( key, "_VERSION" )) {
			//ignore predefined args
			lua_pop( L, 1 );
			continue;
		}
		if (strlen(key) == 4 && 
			!_strnicmp(key,"arg",3) && 
			(key[3] >= '1' && key[3] <= '6')) {
			//ignore arg1-arg6
			lua_pop( L, 1 );
			continue;
		}

		lua_createtable( L, numthreadstates, 0 );
		lua_pushinteger( L, 1 );
		lua_pushvalue( L, -3 );
		lua_rawset( L, -3 );

		for ( int i = 1; i < numthreadstates; i++ ) {
			L2 = threadstates[i].luaState;
			if (!L2) continue;

			lua_pushinteger( L, i+1 );
			lua_getglobal( L2, key );
			if ( lua_type( L2, -1 ) == LUA_TNUMBER ) {
				lua_Number n = lua_tonumber( L2, -1 );
				lua_pushnumber( L, n );
			} else if ( lua_type( L2, -1 ) == LUA_TSTRING ) {
				const char *s = lua_tostring( L2, -1 );
				lua_pushstring( L, s );
			} else if ( lua_type( L2, -1 ) == LUA_TUSERDATA ) {
				PR_CopyType( L2, L, -1 );
			} else {
				lua_pushnil( L );
			}
			lua_rawset( L, -3 );
		}

		lua_setglobal( L, key );
		lua_pop( L, 1 );
     }

	threadstates[0].inputFlags |= MDTRA_PSIF_REDUCE;
}