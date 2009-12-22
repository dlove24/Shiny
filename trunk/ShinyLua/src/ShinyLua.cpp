/*
** LuaProfiler
** Copyright Kepler Project 2005-2007 (http://www.keplerproject.org/luaprofiler)
** $Id: lua50_profiler.c,v 1.13 2008/05/19 18:36:23 mascarenhas Exp $
*/

/*****************************************************************************
lua50_profiler.c:
   Lua version dependent profiler interface
*****************************************************************************/

#include "ShinyLua.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include <stdio.h>
#include <stdarg.h>

#include <string.h>
#include <string>

// Note - not in the original STL, but exists in SGI STL and STLport
#if (SHINY_COMPILER == SHINY_COMPILER_GNUC) && !defined(STLPORT)
#   include <ext/hash_map>
#   include <ext/hash_set>
#else
#   include <hash_set>
#   include <hash_map>
#endif

#if SHINY_COMPILER == SHINY_COMPILER_GNUC && __GNUC__ >= 3 && __GNUC_MINOR__ >= 1 && !defined(STLPORT)
#   define HASHMAP ::__gnu_cxx::hash_map
#else
#   if SHINY_COMPILER == SHINY_COMPILER_MSVC
#       if _MSC_VER > 1300 && !defined(_STLP_MSVC)
#           define HASHMAP ::stdext::hash_map
#       else
#           define HASHMAP ::std::hash_map
#       endif
#   else
#       define HASHMAP ::std::hash_map
#   endif
#endif


//-----------------------------------------------------------------------------

struct Profile {
	ShinyZone zone;
	ShinyNodeCache cache;
	std::string name;

	Profile(void) {
		ShinyZone_clear(&zone);
		cache = &_ShinyNode_dummy;
	}
};

typedef HASHMAP<const void*, Profile> ProfileMap;


//-----------------------------------------------------------------------------

int is_running = 0;

ProfileMap profiles;


//-----------------------------------------------------------------------------

std::string StringPrintV(const char* format, va_list args) {
#if (SHINY_COMPILER == SHINY_COMPILER_MSVC)
	int size = _vscprintf(format, args);
	std::string buffer;
	buffer.resize(size);

	vsnprintf(&buffer[0], size, format, args);
	return buffer;

#else
	char* buffer;

	vasprintf(&buffer, format, args);

	std::string str(buffer);
	free(buffer);
	return str;
#endif
}


//-----------------------------------------------------------------------------

std::string StringPrint(const char* format, ...) {
	va_list args;

	va_start(args, format);
	std::string str(StringPrintV(format, args));
	va_end(args);

	return str;
}

//-----------------------------------------------------------------------------

Profile* FindProfile(lua_State *L, lua_Debug *ar) {
	const void *key;

	lua_getinfo(L, "f", ar);
	key = lua_topointer(L, -1);
	lua_pop(L, 1);

	Profile *prof = &profiles[key];

	if (!prof->zone.name) {
		lua_getinfo(L, "nS", ar);
		if (!ar->name) return NULL; // ignore tail calls 

		switch (ar->what[0]) {
			case 'C': /* 'C' */
				prof->name = ar->name;
				break;

			case 'm': /* 'main' */
				prof->name = ar->source;
				break;
				
			default: /* 'Lua' */
				prof->name = StringPrint("%s(%d):%s",
					ar->source, ar->linedefined, ar->name);
		}

		prof->zone.name = prof->name.c_str();
	}

	return prof;
}


//-----------------------------------------------------------------------------

void callhook(lua_State *L, lua_Debug *ar) {
	switch (ar->event) {
		case LUA_HOOKCALL:
			{
				Profile *prof = FindProfile(L, ar);
				ShinyManager_lookupAndBeginNode(&Shiny_instance, &prof->cache, &prof->zone);
				return;
			}

		case LUA_HOOKRET:
			ShinyManager_endCurNode(&Shiny_instance);
			return;

		default:
			return;
	}
}


//-----------------------------------------------------------------------------

int ShinyLua_update(lua_State *L) {
	PROFILE_UPDATE();
	return 0;
}


//-----------------------------------------------------------------------------

int ShinyLua_clear(lua_State *L) {
	PROFILE_CLEAR();
	return 0;
}


//-----------------------------------------------------------------------------

int ShinyLua_damping(lua_State *L) {
	if (lua_gettop(L) == 1) {
		float damping = (float) luaL_checknumber(L, -1);
		PROFILE_SET_DAMPING(damping);

	} else {
		lua_pushnumber(L, PROFILE_GET_DAMPING());
	}

	return 0;
}


//-----------------------------------------------------------------------------

int ShinyLua_enabled(lua_State *L) {
	if (lua_gettop(L) == 1) {
		is_running = lua_isboolean(L, -1)? lua_toboolean(L, -1) : luaL_checkint(L, -1);
		lua_sethook(L, callhook, is_running? (LUA_MASKCALL | LUA_MASKRET) : 0, 0);

	} else {
		lua_pushboolean(L, is_running);
	}

	return 0;
}


//-----------------------------------------------------------------------------

int ShinyLua_output(lua_State *L) {
	const char* outfile = NULL;

	if (lua_gettop(L) == 1)
		outfile = luaL_checkstring(L, -1);

	PROFILE_OUTPUT(outfile);

	return 0;
}


//-----------------------------------------------------------------------------

int ShinyLua_treeString(lua_State *L) {
	lua_pushstring(L, PROFILE_GET_TREE_STRING().c_str());
	return 0;
}


//-----------------------------------------------------------------------------

int ShinyLua_flatString(lua_State *L) {
	lua_pushstring(L, PROFILE_GET_FLAT_STRING().c_str());
	return 0;
}


//-----------------------------------------------------------------------------

int luaopen_profiler(lua_State *L) {
	const luaL_reg funcs[] = {
		{ "update", ShinyLua_update },
		{ "clear", ShinyLua_clear },
		{ "output", ShinyLua_output },
		{ "damping", ShinyLua_damping },
		{ "enabled", ShinyLua_enabled },
		{ "tree_string", ShinyLua_treeString },
		{ "flat_string", ShinyLua_flatString },
		{ NULL, NULL }
	};

	is_running = 1;
	lua_sethook(L, callhook, LUA_MASKCALL | LUA_MASKRET, 0);

	luaL_openlib(L, "shiny", funcs, 0);

	lua_pushliteral (L, "_COPYRIGHT");
	lua_pushliteral (L, SHINY_COPYRIGHT);
	lua_settable (L, -3);
	lua_pushliteral (L, "_DESCRIPTION");
	lua_pushliteral (L, SHINY_DESCRIPTION);
	lua_settable (L, -3);
	lua_pushliteral (L, "_NAME");
	lua_pushliteral (L, SHINY_FULLNAME);
	lua_settable (L, -3);
	lua_pushliteral (L, "_VERSION");
	lua_pushliteral (L, SHINY_VERSION);
	lua_settable (L, -3);
	return 1;
}
