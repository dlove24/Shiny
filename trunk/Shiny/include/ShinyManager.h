/*
The zlib/libpng License

Copyright (c) 2007-2009 Aidin Abedi, http://shinyprofiler.sourceforge.net

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

#include <stdio.h>

#if SHINY_IS_COMPILED == TRUE


//-----------------------------------------------------------------------------

typedef struct {
#if SHINY_HAS_ENABLED == TRUE
	bool enabled;
#endif

	shinytick_t _lastTick;

	ShinyNode* _curNode;

	uint32_t _tableMask; // = _tableSize - 1

	ShinyNodeTable* _nodeTable;

#if SHINY_LOOKUP_RATE == TRUE
	uint64_t _lookupCount;
	uint64_t _lookupSuccessCount;
#endif

	uint32_t _tableSize;

	uint32_t nodeCount;
	uint32_t zoneCount;

	ShinyZone* _lastZone;

	ShinyNodePool* _lastNodePool;
	ShinyNodePool* _firstNodePool;

	ShinyNode rootNode;
	ShinyZone rootZone;

	float damping;

	int _initialized;
	int _firstUpdate;
} ShinyManager;


//-----------------------------------------------------------------------------

extern ShinyNode* _ShinyManager_dummyNodeTable[];

extern ShinyManager Shiny_instance;


//-----------------------------------------------------------------------------

SHINY_INLINE void _ShinyManager_appendTicksToCurNode(ShinyManager *self) {
	shinytick_t curTick;
	ShinyGetTicks(&curTick);

	ShinyNode_appendTicks(self->_curNode, curTick - self->_lastTick);
	self->_lastTick = curTick;
}

ShinyNode* _ShinyManager_lookupNode(ShinyManager *self, ShinyNodeCache* a_cache, ShinyZone* a_zone);

void _ShinyManager_createNodeTable(ShinyManager *self, uint32_t a_count);
void _ShinyManager_resizeNodeTable(ShinyManager *self, uint32_t a_count);

void _ShinyManager_createNodePool(ShinyManager *self, uint32_t a_count);
void _ShinyManager_resizeNodePool(ShinyManager *self, uint32_t a_count);

ShinyNode* _ShinyManager_createNode(ShinyManager *self, ShinyNodeCache* a_cache, ShinyZone* a_pZone);
void _ShinyManager_insertNode(ShinyManager *self, ShinyNode* a_pNode);

SHINY_INLINE void _ShinyManager_init(ShinyManager *self) {
	self->_initialized = TRUE;

	self->rootNode._last.entryCount = 1;
	self->rootNode._last.selfTicks = 0;
	ShinyGetTicks(&self->_lastTick);
}

SHINY_INLINE void _ShinyManager_uninit(ShinyManager *self) {
	self->_initialized = FALSE;

	ShinyNode_clear(&self->rootNode);
	self->rootNode.parent = &self->rootNode;
	self->rootNode.zone = &self->rootZone;
}

#if SHINY_LOOKUP_RATE == TRUE
SHINY_INLINE void _ShinyManager_incLookup(ShinyManager *self) { self->_lookupCount++; }
SHINY_INLINE void _ShinyManager_incLookupSuccess(ShinyManager *self) { self->_lookupSuccessCount++; }
SHINY_INLINE float ShinyManager_getLookupRate(const ShinyManager *self) { return ((float) self->_lookupSuccessCount) / ((float) self->_lookupCount); }

#else
SHINY_INLINE void _ShinyManager_incLookup(ShinyManager *self) {}
SHINY_INLINE void _ShinyManager_incLookupSuccess(ShinyManager *self) {}
SHINY_INLINE float ShinyManager_getLookupRate(const ShinyManager *self) { return -1; }
#endif

void ShinyManager_resetZones(ShinyManager *self);
void ShinyManager_destroyNodes(ShinyManager *self);

SHINY_INLINE float ShinyManager_tableUsage(const ShinyManager *self)  {
	return ((float) self->nodeCount) / ((float) self->_tableSize);
}

SHINY_INLINE uint32_t ShinyManager_allocMemInBytes(const ShinyManager *self) {
	return self->_tableSize * sizeof(ShinyNode*)
		 + (self->_firstNodePool)? ShinyNodePool_memoryUsageChain(self->_firstNodePool) : 0;
}

SHINY_INLINE void ShinyManager_beginNode(ShinyManager *self, ShinyNode* a_node) {
	ShinyNode_beginEntry(a_node);

	_ShinyManager_appendTicksToCurNode(self);
	self->_curNode = a_node;
}

SHINY_INLINE void ShinyManager_lookupAndBeginNode(ShinyManager *self, ShinyNodeCache* a_cache, ShinyZone* a_zone) {
#if SHINY_HAS_ENABLED == TRUE
	if (!self->enabled) return;
#endif

	if (self->_curNode != (*a_cache)->parent)
		*a_cache = _ShinyManager_lookupNode(self, a_cache, a_zone);

	ShinyManager_beginNode(self, *a_cache);
}

SHINY_INLINE void ShinyManager_endCurNode(ShinyManager *self) {
#if SHINY_HAS_ENABLED == TRUE
	if (!enabled) return;
#endif

	_ShinyManager_appendTicksToCurNode(self);
	self->_curNode = self->_curNode->parent;
}

//

void ShinyManager_preLoad(ShinyManager *self);

void ShinyManager_updateClean(ShinyManager *self);
void ShinyManager_update(ShinyManager *self);

void ShinyManager_clear(ShinyManager *self);
void ShinyManager_destroy(ShinyManager *self);

SHINY_INLINE void ShinyManager_sortZones(ShinyManager *self) {
	if (self->rootZone.next)
		self->rootZone.next = ShinyZone_sortChain(self->rootZone.next);
}

const char* ShinyManager_getOutputErrorString(ShinyManager *self);

int ShinyManager_outputToFile(ShinyManager *self, const char *a_filename);
void ShinyManager_outputToStream(ShinyManager *self, FILE *stream);

#if __cplusplus
} // end of extern "C"

SHINY_INLINE std::string ShinyManager_outputNodesToString(ShinyManager *self) {
	const char* error = ShinyManager_getOutputErrorString(self);
	if (!error) return ShinyNodesToString(&self->rootNode, self->nodeCount);
	else return error;
}

SHINY_INLINE std::string ShinyManager_outputZonesToString(ShinyManager *self) {
	const char* error = ShinyManager_getOutputErrorString(self);
	if (!error) return ShinyZonesToString(&self->rootZone, self->zoneCount);
	else return error;
}

extern "C" { // end of c++
#endif

SHINY_INLINE int ShinyManager_isSelfZoneGreaterEqual(ShinyManager *self, ShinyZone* a_zone, float a_percentage) {
	return a_percentage * (float) self->rootZone.data.childTicks.cur
		<= (float) a_zone->data.selfTicks.cur; 
}

SHINY_INLINE int ShinyManager_isTotalZoneGreaterEqual(ShinyManager *self, ShinyZone* a_zone, float a_percentage) {
	return a_percentage * (float) self->rootZone.data.childTicks.cur
		<= (float) ShinyData_totalTicksCur(&a_zone->data); 
}

//

SHINY_INLINE void ShinyManager_enumerateNodes(ShinyManager *self, void (*a_func)(const ShinyNode*)) {
	ShinyNode_enumerateNodes(&self->rootNode, a_func);
}

SHINY_INLINE void ShinyManager_enumerateZones(ShinyManager *self, void (*a_func)(const ShinyZone*)) {
	ShinyZone_enumerateZones(&self->rootZone, a_func);
}

#if __cplusplus
} // end of extern "C"

template <class T> void ShinyManager_enumerateNodes(ShinyManager *self, T* a_this, void (T::*a_func)(const ShinyNode*)) {
	ShinyNode_enumerateNodes(&self->rootNode, a_this, a_func);
}

template <class T> void ShinyManager_enumerateZones(ShinyManager *self, T* a_this, void (T::*a_func)(const ShinyZone*)) {
	ShinyZone_enumerateZones(&self->rootZone, a_this, a_func);
}

extern "C" { // end of c++
#endif


//-----------------------------------------------------------------------------

#if __cplusplus
} // end of extern "C"

class ShinyEndNodeOnDestruction {
public:

	SHINY_INLINE ~ShinyEndNodeOnDestruction() {
		ShinyManager_endCurNode(&Shiny_instance);
	}
};

extern "C" { // end of c++
#endif


#endif // if SHINY_IS_COMPILED == TRUE

#endif // ifndef SHINY_*_H
