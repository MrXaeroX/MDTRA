#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lscriptlib_c
#define LUA_CORE

#include "lua.h"
#include "lauxlib.h"

#define FINALCODE_INITIAL_SIZE	4096
#define FINALCODE_GROW_SIZE		2048

char *finalcode = NULL;
size_t finalcodesize = 0;
size_t finalcodemaxsize = 0;

extern lua_loadscriptfile_fn pfnLoadScriptFile;
extern lua_freescriptfile_fn pfnFreeScriptFile;
extern char *lua_parse (char *data, char *token, int skipwhite, int macrostr, int crossline, int chars_not_separate);

typedef struct define_s
{
	char			 name[512];
	char			 value[512];
	struct define_s* next;
} define_t;

define_t *pDefineList = NULL;

//////////////////////////////////
// DEFINE CODE
//////////////////////////////////
void _remove_all_defines(void)
{
	define_t* d = pDefineList;
	while (d) {
		define_t* tmp = d->next;
		free(d);
		d = tmp;
	}
	pDefineList = NULL;
}

void _dump_defines(void)
{
	define_t* d = pDefineList;
	while (d) {
		printf("%s = \"%s\"\n", d->name, d->value);
		d = d->next;
	}
}

define_t* _lookup_define(const char *name)
{
	define_t* d = pDefineList;
	while (d) {
		if (!strcmp(d->name, name))
			return d;
		d = d->next;
	}
	return NULL;
}

void _alloc_define(lua_State *L, const char *name, const char *value)
{
	define_t* d = _lookup_define(name);
	if (d) {
		luaL_error(L,"\"%s\" already defined", name);
		return;
	}
	d = (define_t*)calloc(sizeof(define_t),1);
	strncpy(d->name, name, 511);
	strncpy(d->value, value, 511);
	d->next = pDefineList;
	pDefineList = d;
}

//////////////////////////////////
void _check_finalcode(int more)
{
	if (finalcodesize + more >= finalcodemaxsize) {
		finalcodemaxsize += FINALCODE_GROW_SIZE;
		finalcode = (char*)realloc(finalcode, finalcodemaxsize);
	}
}

void _write_finalcode(char *str)
{
	int str_len = strlen(str);
	_check_finalcode(str_len);

	memcpy(finalcode+finalcodesize,str,str_len);
	finalcodesize+=str_len;
}

static int _preprocessor(lua_State *L, char *code)
{
	char token[2048];
	char othertoken[512];
	char *buf = code;
	static int loopbreaker = 256;

	int g_codegen = 1;
	int g_codegenlevel = 0;
	int g_level = 0;

	loopbreaker--;
	if (loopbreaker <= 0) {
		luaL_error(L,"infinite loop (recursive #include?)");
		return 1;
	}


	buf = lua_parse(buf, token, 1, 0, 1, 0);
	while (buf)
	{
		//check for preprocessor macros
		//#INCLUDE
		if (!strcmp(token,"#include")) {
			char *newbuffer;
			int incresult;

			buf = lua_parse(buf, token, 1, 1, 0, 0);
			buf = lua_parse(buf, othertoken, 1, 1, 0, 0);//skip \n

			//include it
			if (g_codegen)
			{
				newbuffer = pfnLoadScriptFile(token, NULL);
				if (!newbuffer) {
					luaL_error(L,"cannot include \"%s\"", token);
					return 1;
				}
				incresult = _preprocessor(L, newbuffer);
				pfnFreeScriptFile(newbuffer);
				if (incresult)
					return incresult;
			}

		//#DEFINE
		} else if (!strcmp(token,"#define")) {
			buf = lua_parse(buf, token, 1, 0, 0, 0);
			buf = lua_parse(buf, othertoken, 1, 0, 0, 1);

			//remember define
			if (g_codegen)
				_alloc_define(L, token, othertoken);

			if (strlen(othertoken))
				buf = lua_parse(buf, othertoken, 1, 0, 0, 0);//skip \n

		//#ERROR
		} else if (!strcmp(token,"#error")) {
			//throw an error
			buf = lua_parse(buf, token, 1, 1, 0, 0);
			buf = lua_parse(buf, othertoken, 1, 0, 0, 1);//skip \n
			if (g_codegen)
			{
				luaL_error(L,"#error: %s", token);
				return 1;
			}

		//#PRAGMA
		} else if (!strcmp(token,"#pragma")) {
			//no pragma options for now
			buf = lua_parse(buf, token, 1, 0, 0, 0);
			buf = lua_parse(buf, othertoken, 1, 0, 0, 1);//skip \n
			if (g_codegen)
			{
				luaL_error(L,"unknown pragma: %s", token);
				return 1;
			}

		//#IFDEF
		} else if (!strcmp(token,"#ifdef")) {
			buf = lua_parse(buf, token, 1, 0, 0, 0);
			buf = lua_parse(buf, othertoken, 1, 0, 0, 0);//skip \n

			g_level++;

			if (g_codegen)
			{
				define_t* d = _lookup_define(token);
				if (!d) {
					g_codegen = 0;
					g_codegenlevel = g_level;
				}
			}
		//#IFNDEF
		} else if (!strcmp(token,"#ifndef")) {
			buf = lua_parse(buf, token, 1, 0, 0, 0);
			buf = lua_parse(buf, othertoken, 1, 0, 0, 0);//skip \n

			g_level++;

			if (g_codegen)
			{
				define_t* d = _lookup_define(token);
				if (d) {
					g_codegen = 0;
					g_codegenlevel = g_level;
				}
			}
			
		//#ELSE
		} else if (!strcmp(token,"#else")) {
			buf = lua_parse(buf, othertoken, 1, 0, 0, 0);//skip \n

			if (!g_level) {
				luaL_error(L,"illegal #else");
				return 1;
			}

			g_codegen = 1 - g_codegen;

		//#ENDIF
		} else if (!strcmp(token,"#endif")) {
			buf = lua_parse(buf, othertoken, 1, 0, 0, 0);//skip \n

			if (!g_level) {
				luaL_error(L,"illegal #endif");
				return 1;
			}

			if (g_codegenlevel==g_level)
			{
				g_codegen = 1;
				g_codegenlevel = 0;
			}

			g_level--;

		//Other tokens
		} else {
			//a generic token

			if (g_codegen)
			{
				//check if it is defined
				define_t* d = _lookup_define(token);
				if (d) {
					_write_finalcode(d->value);
				} else {
					_write_finalcode(token);
				}
			}
		}

		buf = lua_parse(buf, token, 0, 0, 1, 0);
	}

	return 0;
}

LUA_API void lua_preprocessor_define(lua_State *L, const char *name, const char *value )
{
	_alloc_define(L, name, value);
}

LUA_API char *lua_preprocessor_defined(lua_State *L, const char *name )
{
	define_t *d = _lookup_define(name);
	if (d)
		return d->value;
	else
		return NULL;
}

LUA_API void lua_preprocessor_clear(lua_State *L)
{
	_remove_all_defines();
}

LUA_API int lua_preprocessor(lua_State *L, char *sourcecode, size_t sourcelen, char **outfinalcode, size_t *outsize)
{
	if (!sourcelen)
		return 0;

	finalcode = (char*)calloc(FINALCODE_INITIAL_SIZE,1);
	memset(finalcode,0,FINALCODE_INITIAL_SIZE);
	finalcodesize = 0;
	finalcodemaxsize = FINALCODE_INITIAL_SIZE;

	if (_preprocessor(L, sourcecode)) {
		free(finalcode);
		*outfinalcode = NULL;
		*outsize = 0;
		return 1;
	}

//	_dump_defines();

	finalcode[finalcodesize] = '\0';
//	printf("FINALCODE: \"%s\"\n",finalcode);
	*outfinalcode = finalcode;
	*outsize = finalcodesize;
	return 0;
}
