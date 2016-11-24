#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define lscriptlib_c
#define LUA_CORE

#include "lua.h"

//FILE IO
//It can be overriden by user
int lengthoffile (FILE *f)
{
	int		pos;
	int		end;
	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);
	return end;
}

char *default_lua_loadscriptfile(const char *name, int *len)
{
	size_t locallen, readcount;
	char *sc;
	FILE *f;
	
	f = fopen(name, "rb");
	if (!f)
		return NULL;

	locallen = lengthoffile(f);
	sc = (char*)calloc(locallen+1,1);

	readcount = fread (sc, 1, locallen, f);
	fclose(f);

	sc[locallen] = '\0';

	if (len)
		*len = locallen;

	return sc;
}

void default_lua_freescriptfile(char *buffer)
{
	if (buffer) free(buffer);
}

lua_loadscriptfile_fn pfnLoadScriptFile = default_lua_loadscriptfile;
lua_freescriptfile_fn pfnFreeScriptFile = default_lua_freescriptfile;

LUA_API void lua_setfileiofn( lua_loadscriptfile_fn ploadfn, lua_freescriptfile_fn pfreefn )
{
	if (ploadfn) pfnLoadScriptFile = ploadfn;
	if (pfreefn) pfnFreeScriptFile = pfreefn;
}

//Sub to delete pointers malloced in dll
LUA_API int lua_free( void *p )
{
	if (p) {
		free(p);
		return 0;
	} else {
		return 1;
	}
}

//Sub to parse code for preprocessor
char *lua_parse (char *data, char *token, int skipwhite, int macrostr, int crossline, int chars_not_separate)
{
	int             c;
	int             len;
	
	len = 0;
	token[0] = 0;

	if (!data)
		return NULL;
		
// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;                    // end of file;

		if (!crossline && c=='\n')
		{
			data++;
			return data;
		}

		if (skipwhite)
			data++;
		else
		{
			token[len++] = c;
			token[len] = 0;
			return data+1;
		}
	}
	
// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}

// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		if (!macrostr) token[len++] = '\"';
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				if (!macrostr) token[len++] = '\"';
				token[len] = 0;
				return data;
			}
			token[len] = c;
			len++;
		}
	}

// handle quoted strings specially
	if (c == '\'')
	{
		data++;
		if (!macrostr) token[len++] = '\'';
		while (1)
		{
			c = *data++;
			if (c=='\'' || !c)
			{
				if (!macrostr) token[len++] = '\'';
				token[len] = 0;
				return data;
			}
			token[len] = c;
			len++;
		}
	}

// handle quoted strings specially
	if (macrostr && (c == '<'))
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='>' || !c)
			{
				token[len] = 0;
				return data;
			}
			token[len++] = c;
		}
	}

// parse single characters
	if (!chars_not_separate)
	{
		if (c=='{' || c=='}'|| c==')'|| c=='('|| c==':'|| 
			c==','|| c=='['|| c==']'|| c==';'||
			c=='+' || c=='-'|| c=='*'|| c=='/'|| c=='%'||
			c=='^' || c=='='|| c=='<'|| c=='>'|| c=='~')
		{
			token[len] = c;
			len++;
			token[len] = 0;
			return data+1;
		}
	}


// parse a regular word
	do
	{
		token[len] = c;
		data++;
		len++;
		c = *data;

		if (!chars_not_separate)
		{
			if (c=='{' || c=='}'|| c==')'|| c=='('|| c==':'|| 
				c==','|| c=='['|| c==']'|| c==';'||
				c=='+' || c=='-'|| c=='*'|| c=='/'|| c=='%'||
				c=='^' || c=='='|| c=='<'|| c=='>'|| c=='~')
				break;
		}
	} while (c>32);

	token[len] = 0;
	return data;
}