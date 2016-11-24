#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <excpt.h>
#endif

#define lcompiler_c
#define LUA_CORE

#include "lua.h"
#include "lauxlib.h"

#include "ldo.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstring.h"
#include "lundump.h"

char *g_sourcecode = NULL;
char *g_bytecode = NULL;
int g_bytecodemaxsize = 0;
int g_bufferpos = 0;
int g_stripping = 0;

#define BYTECODE_INITIAL_SIZE	4096
#define BYTECODE_GROW_SIZE		2048


#define toproto(L,i) (clvalue(L->top+(i))->l.p)

static int writer(lua_State* L, const void* p, size_t size, void* u)
{
	UNUSED(L);

	if (g_bufferpos + (int)size >= g_bytecodemaxsize) {
		g_bytecodemaxsize += BYTECODE_GROW_SIZE;
		g_bytecode = realloc(g_bytecode, g_bytecodemaxsize);
	}

	memcpy(g_bytecode + g_bufferpos, (char*)p, size);
	g_bufferpos += size;

	return 0;
}

static int compilerstub(lua_State* L)
{
	const Proto* f;
	
	luaL_loadstring(L, g_sourcecode);
	f=toproto(L,-1);

	g_bufferpos = 0;

	lua_lock(L);

#ifdef _WIN32
	__try {
#endif
		luaU_dump(L,f,writer,g_bytecode,g_stripping);
#ifdef _WIN32
	} 
	__except(EXCEPTION_EXECUTE_HANDLER) {}
#endif

	lua_unlock(L);
	
	return 0;
}

LUA_API int lua_compile(lua_State *L, char *sourcecode, char **bytecode, int stripping )
{
	int srclen = strlen(sourcecode);
	if (!srclen)
		return 0;

	g_sourcecode = (char*)calloc(srclen+1,1);
	strcpy(g_sourcecode, sourcecode);

	g_bytecodemaxsize = BYTECODE_INITIAL_SIZE;
	g_bytecode = (char*)calloc(g_bytecodemaxsize,1);

	g_stripping = stripping;

	if (lua_cpcall(L,compilerstub,NULL)!=0)
	{
		luaL_error(L, lua_tostring(L,-1));
		free(g_sourcecode);
		return 0;
	}

	free(g_sourcecode);

	g_bytecode[g_bufferpos] = '\0';

	*bytecode = g_bytecode;
	return g_bufferpos;
}
