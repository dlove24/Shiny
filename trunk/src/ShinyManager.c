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

#include "ShinyManager.h"

#include <malloc.h>
#include <memory.h>
#include <stdio.h>

#if SHINY_COMPILED == TRUE


//-----------------------------------------------------------------------------

#define TABLE_SIZE_INIT		256

//-----------------------------------------------------------------------------

ShinyManager Shiny_instance = {
#if SHINY_HAS_ENABLED == TRUE
	/* enabled = */ false,
#endif
	/* _lastTick = */ 0,
	/* _curNode = */ &Shiny_instance.rootNode,
	/* _tableMask = */ 0,
	/* _nodeTable = */ _ShinyManager_dummyNodeTable,
#if SHINY_LOOKUP_RATE == TRUE
	/* _lookupCount = */ 0,
	/* _lookupSuccessCount = */ 0,
#endif
	/* _tableSize = */ 1,
	/* nodeCount = */ 1,
	/* zoneCount = */ 1,
	/* _lastZone = */ &Shiny_instance.rootZone,
	/* _lastNodePool = */ NULL,
	/* _firstNodePool = */ NULL,
	/* rootNode = */ {
		/* _last = */ { 0, 0 },
		/* zone = */ &Shiny_instance.rootZone,
		/* parent = */ &Shiny_instance.rootNode,
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
		/* _state = */ SHINY_ZONE_STATE_HIDDEN,
		/* name = */ "<Unprofiled>",
		/* data = */ { { 0, 0 }, { 0, 0 }, { 0, 0 } }
	},
	/* damping = */ 0.9f,
	/* _initialized = */ FALSE,
	/* _firstUpdate = */ TRUE
};

ShinyNode* _ShinyManager_dummyNodeTable[] = { NULL };


//-----------------------------------------------------------------------------

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning(disable: 4311)
#endif

// primary hash function
SHINY_INLINE uint32_t hash_value(ShinyNode* a_pParent, ShinyZone* a_pZone) {
	uint32_t a = (uint32_t) a_pParent + (uint32_t) a_pZone;

	a = (a+0x7ed55d16) + (a<<12);
	a = (a^0xc761c23c) ^ (a>>19);
	return a;
}

// secondary hash used as index offset: force it to be odd
// so it's relatively prime to the power-of-two table size
SHINY_INLINE uint32_t hash_offset(uint32_t a) {
	return ((a << 8) + (a >> 4)) | 1;
}

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	pragma warning (pop)
#endif


//-----------------------------------------------------------------------------

void ShinyManager_preLoad(ShinyManager *self) {
	if (!self->_initialized) {
		_ShinyManager_init(self);

		_ShinyManager_createNodeTable(self, TABLE_SIZE_INIT);
		_ShinyManager_createNodePool(self, TABLE_SIZE_INIT / 2);
	}
}


//-----------------------------------------------------------------------------

void ShinyManager_update(ShinyManager *self) {
#if SHINY_HAS_ENABLED == TRUE
	if (!enabled) return;
#endif

	_ShinyManager_appendTicksToCurNode(self);
	ShinyZone_preUpdateChain(&self->rootZone);

	if (self->_firstUpdate || self->damping == 0) {
		self->_firstUpdate = FALSE;
		ShinyNode_updateTreeSimple(&self->rootNode);
		ShinyZone_updateChainSimple(&self->rootZone);

	} else {
		ShinyNode_updateTree(&self->rootNode, self->damping);
		ShinyZone_updateChain(&self->rootZone, self->damping);
	}
}


//-----------------------------------------------------------------------------

void ShinyManager_clear(ShinyManager *self) {
	ShinyManager_destroy(self);
	ShinyManager_preLoad(self);
}


//-----------------------------------------------------------------------------

void ShinyManager_destroy(ShinyManager *self) {
	ShinyManager_destroyNodes(self);
	ShinyManager_resetZones(self);
	_ShinyManager_uninit(self);
}


//-----------------------------------------------------------------------------

ShinyNode* _ShinyManager_lookupNode(ShinyManager *self, ProfileNodeCache* a_cache, ShinyZone* a_zone) {
	uint32_t nHash = hash_value(self->_curNode, a_zone);
	uint32_t nIndex = nHash & self->_tableMask;
	ShinyNode* pNode = self->_nodeTable[nIndex];

	_ShinyManager_incLookup(self);
	_ShinyManager_incLookupSuccess(self);

	if (pNode) {
		uint32_t nStep;

		if (ShinyNode_isEqual(pNode, self->_curNode, a_zone)) return pNode; // found it!
		
		// hash collision:

		// compute a secondary hash function for stepping
		nStep = hash_offset(nHash);

		for (;;) {
			_ShinyManager_incLookup(self);

			nIndex = (nIndex + nStep) & self->_tableMask;
			pNode = self->_nodeTable[nIndex];

			if (!pNode) break; // found empty slot
			else if (ShinyNode_isEqual(pNode, self->_curNode, a_zone)) return pNode; // found it!
		}

		// loop is guaranteed to end because the hash table is never full
	}

	if (a_zone->_state == SHINY_ZONE_STATE_HIDDEN) { // zone is not initialized
		ShinyZone_init(a_zone, self->_lastZone);

		self->_lastZone = a_zone;
		self->zoneCount++;

		if (self->_initialized == FALSE) { // first time init
			_ShinyManager_init(self);

			_ShinyManager_createNodeTable(self, TABLE_SIZE_INIT);
			_ShinyManager_createNodePool(self, TABLE_SIZE_INIT / 2);

			// initialization has invalidated nIndex
			// we must compute nIndex again
			return _ShinyManager_createNode(self, a_cache, a_zone);
		}
	}

	// Althouth nodeCount is not updated
	// it includes rootNode so it adds up.

	// check if we need to grow the table
	// we keep it at most 1/2 full to be very fast
	if (self->_tableSize < 2 * self->nodeCount) {

		_ShinyManager_resizeNodeTable(self, 2 * self->_tableSize);
		_ShinyManager_resizeNodePool(self, self->nodeCount - 1);

		// resize has invalidated nIndex
		// we must compute nIndex again
		return _ShinyManager_createNode(self, a_cache, a_zone);
	}
	
	self->nodeCount++;

	{
		ShinyNode* pNewNode = ShinyNodePool_newItem(self->_lastNodePool);
		ShinyNode_init(pNewNode, self->_curNode, a_zone, a_cache);

		self->_nodeTable[nIndex] = pNewNode;
		return pNewNode;
	}
}


//-----------------------------------------------------------------------------

void _ShinyManager_insertNode(ShinyManager *self, ShinyNode* a_pNode) {
	uint32_t nHash = hash_value(a_pNode->parent, a_pNode->zone);
	uint32_t nIndex = nHash & self->_tableMask;

	if (self->_nodeTable[nIndex]) {
		uint32_t nStep = hash_offset(nHash);

		while (self->_nodeTable[nIndex])
			nIndex = (nIndex + nStep) & self->_tableMask;
	}

	self->_nodeTable[nIndex] = a_pNode;
}


//-----------------------------------------------------------------------------

ShinyNode* _ShinyManager_createNode(ShinyManager *self, ProfileNodeCache* a_cache, ShinyZone* a_pZone) {
	ShinyNode* pNewNode = ShinyNodePool_newItem(self->_lastNodePool);
	ShinyNode_init(pNewNode, self->_curNode, a_pZone, a_cache);

	self->nodeCount++;
	_ShinyManager_insertNode(self, pNewNode);
	return pNewNode;
}


//-----------------------------------------------------------------------------

void _ShinyManager_createNodePool(ShinyManager *self, uint32_t a_nCount) {
	self->_firstNodePool = ShinyNodePool_create(a_nCount);
	self->_lastNodePool = self->_firstNodePool;
}


//-----------------------------------------------------------------------------

void _ShinyManager_resizeNodePool(ShinyManager *self, uint32_t a_nCount) {
	ShinyNodePool* pPool = ShinyNodePool_create(a_nCount);
	self->_lastNodePool->nextPool = pPool;
	self->_lastNodePool = pPool;
}


//-----------------------------------------------------------------------------

void _ShinyManager_createNodeTable(ShinyManager *self, uint32_t a_nCount) {
	self->_tableSize = a_nCount;
	self->_tableMask = a_nCount - 1;

	self->_nodeTable = (ProfileNodeTable*)
		malloc(sizeof(ShinyNode) * a_nCount);

	memset(self->_nodeTable, 0, a_nCount * sizeof(ShinyNode*));
}


//-----------------------------------------------------------------------------

void _ShinyManager_resizeNodeTable(ShinyManager *self, uint32_t a_nCount) {
	ShinyNodePool* pPool;

	free(self->_nodeTable);
	_ShinyManager_createNodeTable(self, a_nCount);

	pPool = self->_firstNodePool;
	while (pPool) {

		ShinyNode *pIter = ShinyNodePool_firstItem(pPool);

		while (pIter != pPool->_nextItem)
			_ShinyManager_insertNode(self, pIter++);

		pPool = pPool->nextPool;
	}
}


//-----------------------------------------------------------------------------

void ShinyManager_resetZones(ShinyManager *self) {
	ShinyZone_resetChain(&self->rootZone);
	self->_lastZone = &self->rootZone;
	self->zoneCount = 1;
}


//-----------------------------------------------------------------------------

void ShinyManager_destroyNodes(ShinyManager *self) {
	if (self->_firstNodePool) {
		ShinyNodePool_destroy(self->_firstNodePool);
		self->_firstNodePool = NULL;
	}

	if (self->_nodeTable != _ShinyManager_dummyNodeTable) {
		free(self->_nodeTable);

		self->_nodeTable = _ShinyManager_dummyNodeTable;
		self->_tableSize = 1;
		self->_tableMask = 0;
	}

	self->_curNode = &self->rootNode;
	self->nodeCount = 1;

	_ShinyManager_init(self);
}


//-----------------------------------------------------------------------------

int ShinyManager_outputToFile(ShinyManager *self, const char *a_filename) {
	FILE *file = fopen(a_filename, "w");

	ShinyManager_outputToStream(self, file);

	fclose(file);
	return TRUE;
}


//-----------------------------------------------------------------------------

void ShinyManager_outputToStream(ShinyManager *self, FILE *a_stream) {
	if (self->_firstUpdate) {
		fprintf(a_stream, "!!! Profile data not updated !!!");
		return;

	} else if (!self->_initialized) {
		fprintf(a_stream, "!!! No profile was collected !!!");
		return;

	}

#if SHINY_OUTPUT_MODE & SHINY_OUTPUT_MODE_FLAT
	{
		int size = ShinyPrintZonesSize(self->zoneCount);
		char *buffer = (char*) malloc(size);
		ShinyPrintZones(buffer, &self->rootZone);
		fwrite(buffer, 1, size - 1, a_stream);
		fwrite("\n\n", 1, 2, a_stream);
		free(buffer);
	}
#endif

#if SHINY_OUTPUT_MODE & SHINY_OUTPUT_MODE_TREE
	{
		int size = ShinyPrintNodesSize(self->zoneCount);
		char *buffer = (char*) malloc(size);
		ShinyPrintNodes(buffer, &self->rootNode);
		fwrite(buffer, 1, size - 1, a_stream);
		fwrite("\n\n", 1, 2, a_stream);
		free(buffer);
	}
#endif
}

#endif // if SHINY_COMPILED == TRUE
