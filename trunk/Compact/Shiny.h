/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (http://shinyprofiler.sourceforge.net)

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

#ifndef SHINY_H
#define SHINY_H

//-----------------------------------------------------------------------------

#include "ShinyConfig.h"
#include "ShinyMacros.h"
#include "ShinyManager.h"

#endif // ifndef SHINY_*_H
/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (www.*)

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

#ifndef SHINY_CONFIG_H
#define SHINY_CONFIG_H


//-----------------------------------------------------------------------------

#ifndef SHINY_PROFILER
#define SHINY_PROFILER				TRUE
#endif

#ifndef SHINY_PROFILER_LOOKUPRATE
#define SHINY_PROFILER_LOOKUPRATE	FALSE
#endif

#ifndef SHINY_PROFILER_HASENABLED
#define SHINY_PROFILER_HASENABLED	FALSE
#endif

//-----------------------------------------------------------------------------

#ifndef FALSE
#define FALSE	0x0
#endif

#ifndef TRUE
#define TRUE	0x1
#endif

#ifndef NULL
#define NULL	0
#endif


//-----------------------------------------------------------------------------

#define SHINY_PLATFORM_WIN32	0x1
#define SHINY_PLATFORM_POSIX	0x2

#if defined (_WIN32)
#   define SHINY_PLATFORM	SHINY_PLATFORM_WIN32

#else
//	ASSUME: POSIX-compliant OS
#   define SHINY_PLATFORM	SHINY_PLATFORM_POSIX
#endif


//-----------------------------------------------------------------------------

#define SHINY_COMPILER_MSVC		0x1
#define SHINY_COMPILER_GNUC		0x2
#define SHINY_COMPILER_OTHER	0x3

#if defined (_MSC_VER)
#   define SHINY_COMPILER	SHINY_COMPILER_MSVC

#elif defined (__GNUG__)
#   define SHINY_COMPILER	SHINY_COMPILER_GNUC

#else
#   define SHINY_COMPILER	SHINY_COMPILER_OTHER
#endif

#endif // ifndef SHINY_*_H
/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (http://shinyprofiler.sourceforge.net)

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

#ifndef SHINY_PREREQS_H
#define SHINY_PREREQS_H

#include "ShinyConfig.h"

#if SHINY_COMPILER == SHINY_COMPILER_GNUC
#include <sys/types.h>
#include <stdint.h>
#endif

namespace Shiny {


//-----------------------------------------------------------------------------

#if SHINY_PROFILER == TRUE
	struct ProfileNode;
	struct ProfileZone;

	typedef ProfileNode* ProfileNodeCache;
	typedef ProfileNode* ProfileNodeTable;
#endif


//-----------------------------------------------------------------------------


#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	define SHINY_INLINE		inline
#	define SHINY_UNUSED

#elif SHINY_COMPILER == SHINY_COMPILER_GNUC
#	define SHINY_INLINE		inline
#	define SHINY_UNUSED		__attribute__ ((unused))

#elif SHINY_COMPILER == SHINY_COMPILER_OTHER
#	define SHINY_INLINE		inline
#	define SHINY_UNUSED
#endif

//-----------------------------------------------------------------------------

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
	typedef int					int32_t;
	typedef unsigned int		uint32_t;

	typedef __int64				int64_t;
	typedef unsigned __int64	uint64_t;

/*
#elif defined(__CYGWIN__)
	typedef u_int32_t			uint32_t;
	typedef u_int64_t			uint64_t;
*/
#endif

	typedef uint64_t			tick_t;

} // namespace Shiny

#endif // ifndef SHINY_*_H
/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (http://shinyprofiler.sourceforge.net)

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

#ifndef SHINY_DATA_H
#define SHINY_DATA_H

#include "ShinyPrereqs.h"

namespace Shiny {


//-----------------------------------------------------------------------------
	
	struct ProfileLastData {
		uint32_t entryCount;
		tick_t selfTicks;
	};


//-----------------------------------------------------------------------------

	struct ProfileData {

		template <typename T>
		struct Data {
			T cur;
			float avg;

			void computeAverage(float a_damping) { avg = a_damping * (avg - cur) + cur; }
			void clear(void) { cur = 0; avg = 0; }
		};


		Data<uint32_t> entryCount;
		Data<tick_t> selfTicks;
		Data<tick_t> childTicks;


		tick_t totalTicksCur(void) const { return selfTicks.cur + childTicks.cur; }
		float totalTicksAvg(void) const { return selfTicks.avg + childTicks.avg; }

		void computeAverage(float a_damping) {
			entryCount.computeAverage(a_damping);
			selfTicks.computeAverage(a_damping);
			childTicks.computeAverage(a_damping);
		}

		void clearAll(void) {
			entryCount.clear();
			selfTicks.clear();
			childTicks.clear();
		}

		void clearCurrent(void) {
			entryCount.cur = 0;
			selfTicks.cur = 0;
			childTicks.cur = 0;
		}
	};


} // namespace Shiny

#endif // ifndef SHINY_*_H
/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (http://shinyprofiler.sourceforge.net)

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

#ifndef SHINY_TOOLS_H
#define SHINY_TOOLS_H

#include "ShinyPrereqs.h"


namespace Shiny {


//-----------------------------------------------------------------------------

	struct TimeUnit {
		float tickFreq;
		float invTickFreq;
		const char* suffix;
	};

	const TimeUnit* GetTimeUnit(float ticks);


//-----------------------------------------------------------------------------

	void GetTicks(tick_t *p);

	tick_t GetTickFreq(void);

	float GetTickInvFreq(void);


//-----------------------------------------------------------------------------

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning(disable: 4311)
#endif

	inline uint32_t ptr32(const void *a_Ptr) {
		return reinterpret_cast<uint32_t>(a_Ptr);
	}

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	pragma warning (pop)
#endif


} // namespace Shiny

#endif // ifndef SHINY_*_H
/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (http://shinyprofiler.sourceforge.net)

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

#ifndef SHINY_NODE_H
#define SHINY_NODE_H

#include "ShinyData.h"
#include "ShinyTools.h"

#if SHINY_PROFILER == TRUE
namespace Shiny {


//-----------------------------------------------------------------------------

	struct ProfileNode {

		//NOTE: data-members are intentionally public because the
		//		class needs to fulfil the definition of an aggregate


		ProfileLastData _last;

		ProfileZone* zone;
		ProfileNode* parent;
		ProfileNode* nextSibling;

		ProfileNode* firstChild;
		ProfileNode* lastChild;

		uint32_t childCount;
		uint32_t entryLevel;

		ProfileNodeCache* _cache;

		ProfileData data;

		static ProfileNode _dummy;

		//

		void init(ProfileNode* a_parent, ProfileZone* a_zone, ProfileNodeCache* a_cache) {
			// NOTE: all member variables are assumed to be zero when allocated

			zone = a_zone;
			parent = a_parent;

			entryLevel = a_parent->entryLevel + 1;
			a_parent->addChild(this);

			_cache = a_cache;
		}

		void addChild(ProfileNode* a_child) {
			if (childCount++) {
				lastChild->nextSibling = a_child;
				lastChild = a_child;

			} else {
				lastChild = a_child;
				firstChild = a_child;
			}
		}

		void updateTree(float a_damping);

		void destroy(void) { *_cache = &_dummy; }

		SHINY_INLINE void appendTicks(tick_t a_elapsedTicks) { _last.selfTicks += a_elapsedTicks; }
		SHINY_INLINE void beginEntry(void) { _last.entryCount++; }

		bool isRoot(void) const { return (entryLevel == 0); }
		bool isDummy(void) const { return (this == &_dummy); }

		bool isEqual(const ProfileNode* a_parent, const ProfileZone* a_zone) const {
			return (parent == a_parent && zone == a_zone);
		}

		const ProfileNode* findNextInTree(void) const;

		void clear(void);
	};

} // namespace Shiny
#endif // if SHINY_PROFILER == TRUE

#endif // ifndef SHINY_*_H
/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (http://shinyprofiler.sourceforge.net)

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

#ifndef SHINY_ZONE_H
#define SHINY_ZONE_H

#include "ShinyData.h"

#if SHINY_PROFILER == TRUE
namespace Shiny {


//-----------------------------------------------------------------------------

	struct ProfileZone {

		enum STATE {
			STATE_HIDDEN = 0,
			STATE_INITIALIZED,
			STATE_UPDATING
		};

		//NOTE: data-members are intentionally public because the
		//		class needs to fulfil the definition of an aggregate

		ProfileZone* next;

		STATE _state;

		mutable const char* name;

		ProfileData data;

		//

		bool isInited(void) const { return _state != 0; }

		void init(ProfileZone* a_prev) {
			_state = STATE_INITIALIZED;

			a_prev->next = this;
		}

		void uninit(void) {
			_state = STATE_HIDDEN;
		}

		void preUpdateChain(void) {
			data.clearCurrent();
			if (next) next->preUpdateChain();
		}

		void updateChain(float a_damping) {
			data.computeAverage(a_damping);
			if (next) next->updateChain(a_damping);
		}

		bool isUpdating(void) const { return _state == STATE_UPDATING; }

		void enableUpdating(void) { _state = STATE_UPDATING; }
		void disableUpdating(void) { _state = STATE_INITIALIZED; }
	};

} // namespace Shiny
#endif // if SHINY_PROFILER == TRUE

#endif // ifndef SHINY_*_H
/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (http://shinyprofiler.sourceforge.net)

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

#ifndef SHINY_NODE_POOL_H
#define SHINY_NODE_POOL_H

#include "ShinyNode.h"

#if SHINY_PROFILER == TRUE
namespace Shiny {


//-----------------------------------------------------------------------------

	struct ProfileNodePool {

		typedef ProfileNode T;

		ProfileNodePool* nextPool;

		T *_nextItem;
		T *endOfItems;

		T _items[1];


		T* firstItem(void) { return &_items[0]; }
		T* newItem(void) { return _nextItem++; }
		const T* unusedItem(void) const { return _nextItem; }

		static ProfileNodePool* createNodePool(uint32_t a_items);

		uint32_t memoryUsageChain(void);
		void destroy(void);
	};

} // namespace Shiny
#endif // if SHINY_PROFILER == TRUE

#endif // ifndef SHINY_*_H
/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (http://shinyprofiler.sourceforge.net)

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

#ifndef SHINY_OUTPUT_H
#define SHINY_OUTPUT_H

#include "ShinyNode.h"
#include "ShinyZone.h"

#include <string>

#if SHINY_PROFILER == TRUE
namespace Shiny {


//-----------------------------------------------------------------------------

	enum OUTPUT_WIDTH {
		OUTPUT_WIDTH_HIT = 6,
		OUTPUT_WIDTH_TIME = 6,
		OUTPUT_WIDTH_PERC = 4,
		OUTPUT_WIDTH_SUM = 79,

		OUTPUT_WIDTH_DATA = 1+OUTPUT_WIDTH_HIT + 1 + 2*(OUTPUT_WIDTH_TIME+4+OUTPUT_WIDTH_PERC+1) + 1,
		OUTPUT_WIDTH_NAME = OUTPUT_WIDTH_SUM - OUTPUT_WIDTH_DATA
	};


//-----------------------------------------------------------------------------

	std::string OutputNodesAsString(const ProfileNode *a_root, uint32_t a_count);
	std::string OutputZonesAsString(const ProfileZone *a_root, uint32_t a_count);

} // namespace Shiny
#endif // if SHINY_PROFILER == TRUE

#endif // ifndef SHINY_*_H
/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (www.*)

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

#ifndef SHINY_MANAGER_H
#define SHINY_MANAGER_H

#include "ShinyZone.h"
#include "ShinyNode.h"
#include "ShinyNodePool.h"
#include "ShinyTools.h"
#include "ShinyOutput.h"

#include <iostream>


#if SHINY_PROFILER == TRUE
namespace Shiny {


//-----------------------------------------------------------------------------

	struct ProfileManager {
		//NOTE: data-members are intentionally public because the
		//		class needs to fulfil the definition of an aggregate

		enum TABLE_SIZE {
			TABLE_SIZE_INIT = 256
		};

#if SHINY_PROFILER_HASENABLED == TRUE
		bool enabled;
#endif

		tick_t _lastTick;

		ProfileNode* _curNode;

		uint32_t _tableMask; // = _tableSize - 1

		ProfileNodeTable* _nodeTable;

#if SHINY_PROFILER_LOOKUPRATE == TRUE
		uint64_t _lookupCount;
		uint64_t _lookupSuccessCount;
#endif

		uint32_t _tableSize;

		uint32_t nodeCount;
		uint32_t zoneCount;

		ProfileZone* _lastZone;

		ProfileNodePool* _lastNodePool;
		ProfileNodePool* _firstNodePool;

		ProfileNode rootNode;
		ProfileZone rootZone;

		bool _initialized;
		bool _firstUpdate;

		static ProfileNode* _dummyNodeTable[];

		static ProfileManager instance;

		//

		SHINY_INLINE void _appendTicksToCurNode(void) {
			register tick_t curTick;
			GetTicks(&curTick);

			_curNode->appendTicks(curTick - _lastTick);
			_lastTick = curTick;
		}

		ProfileNode* _lookupNode(ProfileNodeCache* a_cache, ProfileZone* a_zone);

		void _createNodeTable(uint32_t a_count);
		void _resizeNodeTable(uint32_t a_count);

		void _createNodePool(uint32_t a_count);
		void _resizeNodePool(uint32_t a_count);

		ProfileNode* _createNode(ProfileNodeCache* a_cache, ProfileZone* a_pZone);
		void _insertNode(ProfileNode* a_pNode);

		void _init(void) {
			_initialized = true;

			rootNode.beginEntry();
			GetTicks(&_lastTick);
		}

		void _uninit(void) {
			_initialized = false;

			rootNode.clear();
			rootNode.parent = &rootNode;
			rootNode.zone = &rootZone;
		}

#if SHINY_PROFILER_LOOKUPRATE == TRUE
		SHINY_INLINE void _incLookup(void) { _lookupCount++; }
		SHINY_INLINE void _incLookupSuccess(void) { _lookupSuccessCount++; }
		SHINY_INLINE float lookupSuccessRate(void) const { return ((float) _lookupSuccessCount) / ((float) _lookupCount); }

#else
		SHINY_INLINE void _incLookup(void) {}
		SHINY_INLINE void _incLookupSuccess(void) {}
		SHINY_INLINE float lookupSuccessRate(void) const { return -1; }
#endif

		void _resetZones(void);
		void _destroyNodes(void);

		SHINY_INLINE float tableUsage(void) const { return ((float) nodeCount) / ((float) _tableSize); }

		uint32_t staticMemInBytes(void) {
			// ASSUME: zones and cache are used as intended; throught the macros

			return sizeof(instance) + sizeof(_dummyNodeTable[0]) + sizeof(ProfileNode::_dummy)
				 + (zoneCount - 1) * (sizeof(ProfileZone) + sizeof(ProfileNodeCache));
		}

		uint32_t allocMemInBytes(void) {
			return _tableSize * sizeof(ProfileNode*)
				 + (_firstNodePool)? _firstNodePool->memoryUsageChain() : 0;
		}

		SHINY_INLINE void _beginNode(ProfileNodeCache* a_cache, ProfileZone* a_zone) {
#if SHINY_PROFILER_HASENABLED == TRUE
			if (!enabled) return;
#endif

			if (_curNode != (*a_cache)->parent)
				*a_cache = _lookupNode(a_cache, a_zone);

			_beginNode(*a_cache);
		}

		SHINY_INLINE void _beginNode(ProfileNode* a_node) {
#if SHINY_PROFILER_HASENABLED == TRUE
			if (!enabled) return;
#endif

			a_node->beginEntry();

			_appendTicksToCurNode();
			_curNode = a_node;
		}

		SHINY_INLINE void _endCurNode(void) {
#if SHINY_PROFILER_HASENABLED == TRUE
			if (!enabled) return;
#endif

			_appendTicksToCurNode();
			_curNode = _curNode->parent;
		}

		//

		void preLoad(void);

		void updateClean(void);
		void update(float a_damping = 0.9f);

		void clear(void);
		void destroy(void);

		bool output(const char *a_filename);
		bool output(std::ostream &a_ostream = std::cout);

		SHINY_INLINE std::string outputNodesAsString(void) {
			return OutputNodesAsString(&rootNode, nodeCount);
		}

		SHINY_INLINE std::string outputZonesAsString(void) {
			return OutputZonesAsString(&rootZone, zoneCount);
		}

		//

		static void enumerateNodes(void (*a_func)(const ProfileNode*),
			const ProfileNode* a_node = &instance.rootNode)
		{
			a_func(a_node);

			if (a_node->firstChild) enumerateNodes(a_func, a_node->firstChild);
			if (a_node->nextSibling) enumerateNodes(a_func, a_node->nextSibling);
		}

		template <class T>
		static void enumerateNodes(T* a_this, void (T::*a_func)(const ProfileNode*),
			const ProfileNode* a_node = &instance.rootNode)
		{
			(a_this->*a_func)(a_node);

			if (a_node->firstChild) enumerateNodes(a_this, a_func, a_node->firstChild);
			if (a_node->nextSibling) enumerateNodes(a_this, a_func, a_node->nextSibling);
		}

		static void enumerateZones(void (*a_func)(const ProfileZone*),
			const ProfileZone* a_zone = &instance.rootZone)
		{
			a_func(a_zone);

			if (a_zone->next) enumerateZones(a_func, a_zone->next);
		}

		template <class T>
		static void enumerateZones(T* a_this, void (T::*a_func)(const ProfileZone*),
			const ProfileZone* a_zone = &instance.rootZone)
		{
			(a_this->*a_func)(a_zone);

			if (a_zone->next) enumerateZones(a_this, a_func, a_zone->next);
		}
	};


//-----------------------------------------------------------------------------

	class ProfileAutoEndNode {
	public:

		SHINY_INLINE ~ProfileAutoEndNode() {
			ProfileManager::instance._endCurNode();
		}
	};

} // namespace Shiny

#endif // if SHINY_PROFILER == TRUE

#endif // ifndef SHINY_*_H
/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (www.*)

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

#if SHINY_PROFILER == TRUE


//-----------------------------------------------------------------------------
// public preprocessors

#define PROFILE_UPDATE_ALL													\
	Shiny::ProfileManager::instance.update

#define PROFILE_OUTPUT_ALL													\
	Shiny::ProfileManager::instance.output

#define PROFILE_GET_TREE_OUTPUT()											\
	Shiny::ProfileManager::instance.outputNodesAsString()

#define PROFILE_GET_FLAT_OUTPUT()											\
	Shiny::ProfileManager::instance.outputZonesAsString()

#define PROFILE_DESTROY_ALL()												\
	Shiny::ProfileManager::instance.destroy()


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_GET_ROOT_DATA()												\
	Shiny::ProfileManager::instance.rootZone.data


//-----------------------------------------------------------------------------
// public preprocessor

#define PROFILE_END()														\
	Shiny::ProfileManager::instance._endCurNode()


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

#if SHINY_PROFILER_HASENABLED == TRUE
#define PROFILE_SET_ENABLED( boolean )										\
	Shiny::ProfileManager::instance.enabled = boolean
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
	Shiny::ProfileZone id = {												\
		NULL, Shiny::ProfileZone::STATE_HIDDEN, string,						\
		{ { 0, 0 }, { 0, 0 }, { 0, 0 } }									\
	}


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_ZONE_DECLARE( prefix, id )									\
																			\
	prefix Shiny::ProfileZone id


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_BLOCK_DEFINE( id )											\
																			\
	Shiny::ProfileAutoEndNode SHINY_UNUSED id


//-----------------------------------------------------------------------------
// internal preprocessor

#define _PROFILE_ZONE_BEGIN( id )											\
	{																		\
		static Shiny::ProfileNodeCache cache =								\
			&Shiny::ProfileNode::_dummy;									\
																			\
		Shiny::ProfileManager::instance._beginNode(&cache, &id);			\
	}

//-----------------------------------------------------------------------------

#else // #if SHINY_PROFILER == TRUE

namespace Shiny {

	SHINY_INLINE ProfileData GetEmptyData() {
		ProfileData a = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
		return a;
	}
}

#define PROFILE_UPDATE_ALL(...)
#define PROFILE_OUTPUT_ALL(...)
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

#if SHINY_PROFILER_HASENABLED == TRUE
#define PROFILE_SET_ENABLED(boolean)
#endif

#endif

#endif // ifndef SHINY_*_H
