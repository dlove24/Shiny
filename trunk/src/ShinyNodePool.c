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

#include "ShinyNodePool.h"
#include "ShinyTools.h"

#include <memory.h>
#include <malloc.h>

#if SHINY_IS_COMPILED == TRUE


//-----------------------------------------------------------------------------

ShinyNodePool* ShinyNodePool_create(uint32_t a_items) {
	ShinyNodePool* pPool = (ShinyNodePool*)
		malloc(sizeof(ShinyNodePool) + sizeof(ShinyNode) * (a_items - 1));

	pPool->nextPool = NULL;
	pPool->_nextItem = &pPool->_items[0];
	pPool->endOfItems = &pPool->_items[a_items];

	memset(&pPool->_items[0], 0, a_items * sizeof(ShinyNode));
	return pPool;
}


//-----------------------------------------------------------------------------

uint32_t ShinyNodePool_memoryUsageChain(ShinyNodePool *first) {
	uint32_t bytes = (uint32_t) ((char*) first->endOfItems - (char*) first);
	ShinyNodePool *pool = first->nextPool;

	while (pool) {
		bytes += (uint32_t) ((char*) pool->endOfItems - (char*) pool);
		pool = pool->nextPool;
	}

	return bytes;
}


//-----------------------------------------------------------------------------

void ShinyNodePool_destroy(ShinyNodePool *self) {
	ShinyNode* firstNode = ShinyNodePool_firstItem(self);
	ShinyNode* lastNode = self->_nextItem;

	while (firstNode != lastNode)
		ShinyNode_destroy(firstNode++);

	// TODO: make this into a loop or a tail recursion
	if (self->nextPool) ShinyNodePool_destroy(self->nextPool);
	free(self);
}

#endif
