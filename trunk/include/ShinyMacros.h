/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi, http://shinyprofiler.sourceforge.net

This software is provided 'as-is', without any express or implied warranty. In no event will
the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial
applications, and to alter it and redistribute it freely, subject to the following
restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that
       you wrote the original software. If you use this software in a product,
       an acknowledgment in the product documentation would be appreciated but is
       not required.

    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/

#ifndef SHINY_MACROS_H
#define SHINY_MACROS_H

#include "ShinyManager.h"

#if SHINY_COMPILED == TRUE


//-----------------------------------------------------------------------------
// public preprocessors

#define PROFILE_UPDATE_ALL													\
	Shiny::ShinyManager::instance.update

#define PROFILE_OUTPUT_ALL													\
	Shiny::ShinyManager::instance.output

#define PROFILE_GET_TREE_OUTPUT()											\
	Shiny::ShinyManager::instance.outputNodesAsString()

#define PROFILE_GET_FLAT_OUTPUT()											\
	Shiny::ShinyManager::instance.outputZonesAsString()

#define PROFILE_DESTROY_ALL()												\
	Shiny::ShinyManager::instance.destroy()

#define PROFILE_CLEAR()														\
	Shiny::ShinyManager::instance.clear()

#define PROFILE_SORT_ZONES()												\
	Shiny::ShinyManager::instance.sortZones()


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_GET_ROOT_DATA()												\
	Shiny::ShinyManager::instance.rootZone.data


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_END()														\
	ShinyManager_endCurNode(Shiny_instance)


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_BEGIN( name )												\
																			\
	static _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE(name), #name);				\
	_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE(name))


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_BLOCK( name )												\
																			\
	_PROFILE_BLOCK_DEFINE(_PROFILE_ID_BLOCK());								\
	PROFILE_BEGIN(name)


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_FUNC()														\
																			\
	_PROFILE_BLOCK_DEFINE(_PROFILE_ID_BLOCK());								\
	static _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_FUNC(), __FUNCTION__);		\
	_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_FUNC())


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_CODE( code )												\
	{																		\
		static _PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_CODE(), #code);		\
		_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_CODE());						\
		{ code; }															\
		PROFILE_END();														\
	}


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_GLOBAL( name )										\
																			\
	_PROFILE_ZONE_DECLARE(extern, _PROFILE_ID_ZONE_SHARED(name))


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_MEMBER( name )										\
																			\
	_PROFILE_ZONE_DECLARE(static, _PROFILE_ID_ZONE_SHARED(name))


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_DEFINE( name )										\
																			\
	_PROFILE_ZONE_DEFINE(_PROFILE_ID_ZONE_SHARED(name), #name)


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_BEGIN( name )										\
																			\
	_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_SHARED(name))


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_SHARED_BLOCK( name )										\
																			\
	_PROFILE_BLOCK_DEFINE(_PROFILE_ID_BLOCK());								\
	_PROFILE_ZONE_BEGIN(_PROFILE_ID_ZONE_SHARED(name))


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_GET_SHARED_DATA( name )										\
																			\
	_PROFILE_ID_ZONE_SHARED(name).data										\


//-----------------------------------------------------------------------------
// public preprocessor

#if SHINY_HAS_ENABLED == TRUE
#define PROFILE_SET_ENABLED( boolean )										\
	Shiny::ShinyManager::instance.enabled = boolean
#endif


//-----------------------------------------------------------------------------
// internal preprocessors

#define _PROFILE_ID_ZONE( name )			__ShinyZone_##name
#define _PROFILE_ID_ZONE_FUNC()				__ShinyZoneFunc
#define _PROFILE_ID_ZONE_CODE()				__ShinyZoneCode
#define _PROFILE_ID_ZONE_SHARED( name )		name##__ShinyZoneShared
#define _PROFILE_ID_BLOCK()					__ShinyBlock


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_ZONE_DEFINE( id, string )									\
																			\
	Shiny::ShinyZone id = {												\
		NULL, SHINY_ZONE_STATE_HIDDEN, string,						\
		{ { 0, 0 }, { 0, 0 }, { 0, 0 } }									\
	}


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_ZONE_DECLARE( prefix, id )									\
																			\
	prefix Shiny::ShinyZone id


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_BLOCK_DEFINE( id )											\
																			\
	Shiny::ShinyEndNodeOnDestruction SHINY_UNUSED id


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_ZONE_BEGIN( id )											\
	{																		\
		static Shiny::ProfileNodeCache cache =								\
			&ShinyNode_dummy;												\
																			\
		ShinyManager_lookupAndBeginNode(Shiny_instance, &cache, &id);			\
	}

//-----------------------------------------------------------------------------

#else // #if SHINY_COMPILED == TRUE

namespace Shiny {

	SHINY_INLINE ShinyData GetEmptyData() {
		ShinyData a = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
		return a;
	}
}

#define PROFILE_UPDATE_ALL(...)
#define PROFILE_OUTPUT_ALL(...)
#define PROFILE_CLEAR(...)
#define PROFILE_GET_TREE_OUTPUT()		std::string()
#define PROFILE_GET_FLAT_OUTPUT()		std::string()
#define PROFILE_DESTROY_ALL	()
#define PROFILE_BEGIN(name)
#define PROFILE_BLOCK(name)
#define PROFILE_FUNC()
#define PROFILE_CODE(code)				{ code; }
#define PROFILE_SHARED_GLOBAL(name)
#define PROFILE_SHARED_MEMBER(name)
#define PROFILE_SHARED_DEFINE(name)
#define PROFILE_SHARED_BEGIN(name)
#define PROFILE_SHARED_BLOCK(name)
#define PROFILE_GET_SHARED_DATA(name)	Shiny::GetEmptyData()
#define PROFILE_GET_ROOT_DATA()			Shiny::GetEmptyData()

#if SHINY_HAS_ENABLED == TRUE
#define PROFILE_SET_ENABLED(boolean)
#endif

#endif

#endif // ifndef SHINY_*_H
