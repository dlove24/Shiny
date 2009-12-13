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

#include "Shiny.h"

//#include "ShinyManager.h"

#include <fstream>
#include <memory.h>
#include <stdio.h>

#if SHINY_PROFILER == TRUE
namespace Shiny {


//-----------------------------------------------------------------------------

	ProfileManager ProfileManager::instance = {
#if SHINY_PROFILER_HASENABLED == TRUE
		/* enabled = */ false,
#endif

		/* _lastTick = */ 0,
		/* _curNode = */ &instance.rootNode,
		/* _tableMask = */ 0,
		/* _nodeTable = */ ProfileManager::_dummyNodeTable,
#if SHINY_PROFILER_LOOKUPRATE == TRUE
		/* _lookupCount = */ 0,
		/* _lookupSuccessCount = */ 0,
#endif
		/* _tableSize = */ 1,
		/* nodeCount = */ 1,
		/* zoneCount = */ 1,
		/* _lastZone = */ &instance.rootZone,
		/* _lastNodePool = */ NULL,
		/* _firstNodePool = */ NULL,
		/* rootNode = */ {
			/* _last = */ { 0, 0 },
			/* zone = */ &instance.rootZone,
			/* parent = */ &instance.rootNode,
			/* nextSibling = */ NULL,
			/* firstChild = */ NULL,
			/* lastChild = */ NULL,
			/* childCount = */ 0,
			/* entryLevel = */ 0,
			/* _cache = */ NULL,
			/* data = */ { { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		/* rootZone = */ {
			/* next = */ NULL,
			/* _state = */ ProfileZone::STATE_HIDDEN,
			/* name = */ "<root>",
			/* data = */ { { 0, 0 }, { 0, 0 }, { 0, 0 } }
		},
		/* _initialized = */ false,
		/* _firstUpdate = */ true
	};

	ProfileNode* ProfileManager::_dummyNodeTable[] = { NULL };


//-----------------------------------------------------------------------------

	/* Robert Jenkins' 32 bit integer hash function

	SHINY_INLINE uint32_t hash_value(ProfileNode* a_pParent, ProfileZone* a_pZone) {
		uint32_t a = ptr32(a_pParent) + ptr32(a_pZone);

		a = (a+0x7ed55d16) + (a<<12);
		a = (a^0xc761c23c) ^ (a>>19);
		a = (a+0x165667b1) + (a<<5);
		a = (a+0xd3a2646c) ^ (a<<9);
		a = (a+0xfd7046c5) + (a<<3);
		a = (a^0xb55a4f09) ^ (a>>16);
		return a;
	}
	*/

	/* Old hash function
	
	SHINY_INLINE uint32_t hash_index(ProfileNode* a_pParent, ProfileZone* a_pZone) {
		uint32_t a = ptr32(a_pParent) + ptr32(a_pZone);
		return (a << 8) - (a >> 4);
	}
	*/

	// primary hash function
	SHINY_INLINE uint32_t hash_value(ProfileNode* a_pParent, ProfileZone* a_pZone) {
		uint32_t a = ptr32(a_pParent) + ptr32(a_pZone);

		a = (a+0x7ed55d16) + (a<<12);
		a = (a^0xc761c23c) ^ (a>>19);
		return a;
	}

	// secondary hash used as index offset: force it to be odd
	// so it's relatively prime to the power-of-two table size
	SHINY_INLINE uint32_t hash_offset(uint32_t a) {
		return ((a << 8) + (a >> 4)) | 1;
	}


//-----------------------------------------------------------------------------

	void ProfileManager::preLoad(void) {
		if (!_initialized) {
			_init();

			_createNodeTable(TABLE_SIZE_INIT);
			_createNodePool(TABLE_SIZE_INIT / 2);
		}
	}


//-----------------------------------------------------------------------------

	void ProfileManager::update(float a_damping) {
#if SHINY_PROFILER_HASENABLED == TRUE
		if (!enabled) return;
#endif

		_appendTicksToCurNode();

		if (!_firstUpdate) {
			rootZone.preUpdateChain();
			rootNode.updateTree(a_damping);
			rootZone.updateChain(a_damping);

		} else {
			_firstUpdate = false;
			rootZone.preUpdateChain();
			rootNode.updateTree(0);
			rootZone.updateChain(0);
		}
	}


//-----------------------------------------------------------------------------

	void ProfileManager::clear(void) {
		destroy();
		preLoad();
	}


//-----------------------------------------------------------------------------

	void ProfileManager::destroy(void) {
		_resetZones();
		_destroyNodes();
		_uninit();
	}


//-----------------------------------------------------------------------------

	ProfileNode* ProfileManager::_lookupNode(ProfileNodeCache* a_cache, ProfileZone* a_zone) {
		uint32_t nHash = hash_value(_curNode, a_zone);
		uint32_t nIndex = nHash & _tableMask;
		ProfileNode* pNode = _nodeTable[nIndex];

		_incLookup();
		_incLookupSuccess();

		if (pNode) {
			if (pNode->isEqual(_curNode, a_zone)) return pNode; // found it!
			
			// hash collision:

			// compute a secondary hash function for stepping
			uint32_t nStep = hash_offset(nHash);

			for (;;) {
				_incLookup();

				nIndex = (nIndex + nStep) & _tableMask;
				pNode = _nodeTable[nIndex];

				if (!pNode) break;
				else if (pNode->isEqual(_curNode, a_zone)) return pNode;
			}

			// loop is guaranteed to end because the hash table is never full
		}

		if (!a_zone->isInited()) { // zone is not initialized
			a_zone->init(_lastZone);

			_lastZone = a_zone;
			zoneCount++;

			if (_initialized == false) { // first time init
				_init();

				_createNodeTable(TABLE_SIZE_INIT);
				_createNodePool(TABLE_SIZE_INIT / 2);

				// initialization has invalidated nIndex
				// we must compute nIndex again
				return _createNode(a_cache, a_zone);
			}
		}

		// YES nodeCount is not updated
		// but it includes rootNode so it adds up.

		// check if we need to grow the table
		// we keep it at most 1/2 full to be very fast
		if (_tableSize < 2 * nodeCount) {

			_resizeNodeTable(2 * _tableSize);
			_resizeNodePool(nodeCount - 1);

			// expansion has invalidated nIndex
			// we must compute nIndex again
			return _createNode(a_cache, a_zone);
		}
		
		nodeCount++;

		ProfileNode* pNewNode = _lastNodePool->newItem();
		pNewNode->init(_curNode, a_zone, a_cache);

		_nodeTable[nIndex] = pNewNode;
		return pNewNode;
	}


//-----------------------------------------------------------------------------

	ProfileNode* ProfileManager::_createNode(ProfileNodeCache* a_cache, ProfileZone* a_pZone) {
		ProfileNode* pNewNode = _lastNodePool->newItem();
		pNewNode->init(_curNode, a_pZone, a_cache);

		nodeCount++;
		_insertNode(pNewNode);
		return pNewNode;
	}


//-----------------------------------------------------------------------------

	void ProfileManager::_insertNode(ProfileNode* a_pNode) {
		uint32_t nHash = hash_value(a_pNode->parent, a_pNode->zone);
		uint32_t nIndex = nHash & _tableMask;

		if (_nodeTable[nIndex]) {
			uint32_t nStep = hash_offset(nHash);

			while (_nodeTable[nIndex])
				nIndex = (nIndex + nStep) & _tableMask;
		}

		_nodeTable[nIndex] = a_pNode;
	}


//-----------------------------------------------------------------------------

	void ProfileManager::_createNodePool(uint32_t a_nCount) {
		_firstNodePool = ProfileNodePool::createNodePool(a_nCount);
		_lastNodePool = _firstNodePool;
	}


//-----------------------------------------------------------------------------

	void ProfileManager::_resizeNodePool(uint32_t a_nCount) {
		ProfileNodePool* pPool = ProfileNodePool::createNodePool(a_nCount);
		_lastNodePool->nextPool = pPool;
		_lastNodePool = pPool;
	}


//-----------------------------------------------------------------------------

	void ProfileManager::_createNodeTable(uint32_t a_nCount) {
		_tableSize = a_nCount;
		_tableMask = a_nCount - 1;

		_nodeTable = static_cast<ProfileNodeTable*>(
			malloc(sizeof(ProfileNode) * a_nCount));

		memset(_nodeTable, 0, a_nCount * sizeof(ProfileNode*));
	}


//-----------------------------------------------------------------------------

	void ProfileManager::_resizeNodeTable(uint32_t a_nCount) {
		ProfileNodePool* pPool;

		free(_nodeTable);
		_createNodeTable(a_nCount);

		pPool = _firstNodePool;
		while (pPool) {

			ProfileNode *pIter = pPool->firstItem();

			while (pIter != pPool->unusedItem())
				_insertNode(pIter++);

			pPool = pPool->nextPool;
		}
	}


//-----------------------------------------------------------------------------

	void ProfileManager::_resetZones(void) {
		ProfileZone *pZone, *pNextZone;

		pZone = &rootZone;

		for(;;) {
			pZone->uninit();

			pNextZone = pZone->next;
			pZone->next = NULL;
			
			if (!pNextZone) break;
			pZone = pNextZone;
		}

		_lastZone = &rootZone;
		zoneCount = 1;
	}


//-----------------------------------------------------------------------------

	void ProfileManager::_destroyNodes(void) {
		if (_firstNodePool) {
			_firstNodePool->destroy();
			_firstNodePool = NULL;
		}

		if (_nodeTable != instance._dummyNodeTable) {
			free(_nodeTable);

			_nodeTable = instance._dummyNodeTable;
			_tableSize = 1;
			_tableMask = 0;
		}

		_curNode = &rootNode;
		nodeCount = 1;

		_init();
	}


//-----------------------------------------------------------------------------

	bool ProfileManager::output(const char *a_filename) {
		std::ofstream file(a_filename, std::ios_base::out);

		if (!file.is_open()) return false;
		else return output(file);
	}


//-----------------------------------------------------------------------------

	bool ProfileManager::output(std::ostream &a_ostream) {
		if (_firstUpdate) {
			a_ostream << "!!! Profile data not updated !!!" << std::endl;
			return true;

		} else if (!_initialized) {
			a_ostream << "!!! No profile started !!!" << std::endl;
			return true;
		}

		a_ostream << outputZonesAsString().c_str()
		          << "\n\n"
		          << outputNodesAsString().c_str()
		          << "\n\n"
				  << std::flush;

		return true;
	}


} // namespace Shiny

#endif // if SHINY_PROFILER == TRUE


//#include "ShinyNode.h"
//#include "ShinyZone.h"

#include <memory.h>


#if SHINY_PROFILER == TRUE
namespace Shiny {

//-----------------------------------------------------------------------------

	ProfileNode ProfileNode::_dummy = {
		/* _last = */ { 0, 0 },
		/* zone = */ NULL,
		/* parent = */ NULL,
		/* nextSibling = */ NULL,
		/* firstChild = */ NULL,
		/* lastChild = */ NULL
	};


//-----------------------------------------------------------------------------

	void ProfileNode::updateTree(float a_damping) {
		data.selfTicks.cur = _last.selfTicks;
		data.entryCount.cur = _last.entryCount;

		zone->data.selfTicks.cur += _last.selfTicks;
		zone->data.entryCount.cur += _last.entryCount;
		
		data.childTicks.cur = 0;
		_last.selfTicks = 0;
		_last.entryCount = 0;

		if (!zone->isUpdating()) {

			zone->enableUpdating();
			if (firstChild) firstChild->updateTree(a_damping);
			
			zone->data.childTicks.cur += data.childTicks.cur;
			zone->disableUpdating();

		} else {
			zone->data.childTicks.cur -= data.selfTicks.cur;
			if (firstChild) firstChild->updateTree(a_damping);
		}

		data.computeAverage(a_damping);

		if (!isRoot()) parent->data.childTicks.cur += data.selfTicks.cur + data.childTicks.cur;
		if (nextSibling) nextSibling->updateTree(a_damping);
	}


//-----------------------------------------------------------------------------

	const ProfileNode* ProfileNode::findNextInTree(void) const {
		if (firstChild) {
			return firstChild;

		} else if (nextSibling) {
			return nextSibling;

		} else {
			ProfileNode* pParent = parent;

			while (!pParent->isRoot()) {
				if (pParent->nextSibling) return pParent->nextSibling;
				else pParent = pParent->parent;
			}

			return NULL;
		}
	}


//-----------------------------------------------------------------------------

	void ProfileNode::clear(void) {
		memset(this, 0, sizeof(ProfileNode));
	}


} // namespace Shiny
#endif


//#include "ShinyNodePool.h"
//#include "ShinyTools.h"

#include <memory.h>
#include <malloc.h>

#if SHINY_PROFILER == TRUE
namespace Shiny {


//-----------------------------------------------------------------------------

	ProfileNodePool* ProfileNodePool::createNodePool(uint32_t a_items) {
		ProfileNodePool* pPool = static_cast<ProfileNodePool*>(
			malloc(sizeof(ProfileNodePool) + sizeof(T) * (a_items - 1)));

		pPool->nextPool = NULL;
		pPool->_nextItem = &pPool->_items[0];
		pPool->endOfItems = &pPool->_items[a_items];

		memset(&pPool->_items[0], 0, a_items * sizeof(T));
		return pPool;
	}


//-----------------------------------------------------------------------------

	uint32_t ProfileNodePool::memoryUsageChain(void) {
		uint32_t bytes = ptr32(
			reinterpret_cast<void*>(
				  reinterpret_cast<char*>(endOfItems)
				- reinterpret_cast<char*>(this)));

		if (nextPool) bytes += nextPool->memoryUsageChain();
		return bytes;
	}


//-----------------------------------------------------------------------------

	void ProfileNodePool::destroy(void) {
		T* pItem = firstItem();

		while (pItem != unusedItem())
			(pItem++)->destroy();

		if (nextPool) nextPool->destroy();
		free(this);
	}

} // namespace Shiny
#endif


//#include "ShinyOutput.h"

#include <stdio.h>

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	pragma warning(disable: 4996)
#	define snprintf		_snprintf
#	define TRAILING		0

#else
#	define TRAILING		1
#endif

#if SHINY_PROFILER == TRUE
namespace Shiny {


//-----------------------------------------------------------------------------

	void _printHeader(char *dest, const char *a_title) {
		snprintf(dest, OUTPUT_WIDTH_SUM + TRAILING,
			"%-*s %*s %*s %*s",
			OUTPUT_WIDTH_NAME, a_title,
			OUTPUT_WIDTH_HIT, "hits",
			OUTPUT_WIDTH_TIME+4+OUTPUT_WIDTH_PERC+1, "self time",
			OUTPUT_WIDTH_TIME+4+OUTPUT_WIDTH_PERC+1, "total time");
	}


//-----------------------------------------------------------------------------

	void _printData(char *dest, const ProfileData &a_data, float a_topercent) {
		float totalTicksAvg = a_data.totalTicksAvg();
		const TimeUnit *selfUnit = GetTimeUnit(a_data.selfTicks.avg);
		const TimeUnit *totalUnit = GetTimeUnit(totalTicksAvg);

		snprintf(dest, OUTPUT_WIDTH_DATA + TRAILING,
			" %*.1f %*.0f %-2s %*.0f%% %*.0f %-2s %*.0f%%",
			OUTPUT_WIDTH_HIT, a_data.entryCount.avg,
			OUTPUT_WIDTH_TIME, a_data.selfTicks.avg * selfUnit->invTickFreq, selfUnit->suffix,
			OUTPUT_WIDTH_PERC, a_data.selfTicks.avg * a_topercent,
			OUTPUT_WIDTH_TIME, totalTicksAvg * totalUnit->invTickFreq, totalUnit->suffix,
			OUTPUT_WIDTH_PERC, totalTicksAvg * a_topercent);
	}

//-----------------------------------------------------------------------------

	std::string OutputNodesAsString(const ProfileNode *a_root, uint32_t a_count) {
		float fTicksToPc = 100.0f / a_root->data.childTicks.avg;
		std::string str;

		str.resize((1 + a_count) * (OUTPUT_WIDTH_SUM + 1) - 1);
		char *s = &str[0];

		_printHeader(s, "call tree");
		s += OUTPUT_WIDTH_SUM;
		(*s++) = '\n';

		const ProfileNode *node = a_root;

		do {
			int offset = node->entryLevel * 2;
			snprintf(s, OUTPUT_WIDTH_NAME + TRAILING, "%*s%-*s",
				offset, "", OUTPUT_WIDTH_NAME - offset, node->zone->name);

			s += OUTPUT_WIDTH_NAME;

			_printData(s, node->data, fTicksToPc);

			s += OUTPUT_WIDTH_DATA;
			(*s++) = '\n';

			node = node->findNextInTree();
		} while (node);

		*(--s) = '\0';
		return str;
	}


//-----------------------------------------------------------------------------

	std::string OutputZonesAsString(const ProfileZone *a_root, uint32_t a_count) {
		float fTicksToPc = 100.0f / a_root->data.childTicks.avg;
		std::string str;

		str.resize((1 + a_count) * (OUTPUT_WIDTH_SUM + 1) - 1);
		char *s = &str[0];

		_printHeader(s, "flat profile");
		s += OUTPUT_WIDTH_SUM;
		(*s++) = '\n';

		const ProfileZone *zone = a_root;

		do {
			snprintf(s, OUTPUT_WIDTH_NAME + TRAILING, "%-*s",
				OUTPUT_WIDTH_NAME, zone->name);

			s += OUTPUT_WIDTH_NAME;

			_printData(s, zone->data, fTicksToPc);

			s += OUTPUT_WIDTH_DATA;
			(*s++) = '\n';

			zone = zone->next;
		} while (zone);

		*(--s) = '\0';
		return str;
	}

} // namespace Shiny
#endif


//#include "ShinyTools.h"

#if SHINY_PLATFORM == SHINY_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif SHINY_PLATFORM == SHINY_PLATFORM_POSIX
#include <sys/time.h>
#endif

namespace Shiny {


//-----------------------------------------------------------------------------

	const TimeUnit* GetTimeUnit(float ticks) {
		static TimeUnit unit[] = {
			{ GetTickFreq() / 1.0f			, GetTickInvFreq() * 1.0f			, "s" },
			{ GetTickFreq() / 1000.0f		, GetTickInvFreq() * 1000.0f		, "ms" },
			{ GetTickFreq() / 1000000.0f	, GetTickInvFreq() * 1000000.0f		, "us" },
			{ GetTickFreq() / 1000000000.0f	, GetTickInvFreq() * 1000000000.0f	, "ns" }
		};

		if (unit[0].tickFreq < ticks) return &unit[0];
		else if (unit[1].tickFreq < ticks) return &unit[1];
		else if (unit[2].tickFreq < ticks) return &unit[2];
		else return &unit[3];
	}


//-----------------------------------------------------------------------------

#if SHINY_PLATFORM == SHINY_PLATFORM_WIN32

	tick_t _InitTickFreq(void) {
		tick_t freq;

		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&freq));
		return freq;
	}

	void GetTicks(tick_t *p) {
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(p));
	}

	tick_t GetTickFreq(void) {
		static tick_t freq = _InitTickFreq();
		return freq;
	}

	float GetTickInvFreq(void) {
		static float invfreq = 1.0f / GetTickFreq();
		return invfreq;
	}


//-----------------------------------------------------------------------------

#elif SHINY_PLATFORM == SHINY_PLATFORM_POSIX

	void GetTicks(tick_t *p) {
		timeval time;
		gettimeofday(&time, NULL);

		*p = time.tv_sec * 1000000 + time.tv_usec;
	}

	tick_t GetTickFreq(void) {
		return 1000000;
	}

	float GetTickInvFreq(void) {
		return 1.0f / 1000000.0f;
	}

#endif
} // namespace Shiny
