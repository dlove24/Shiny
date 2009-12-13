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

#ifndef SHINY_NODE_POOL_H
#define SHINY_NODE_POOL_H

#include "ShinyNode.h"

#if SHINY_COMPILED == TRUE


//-----------------------------------------------------------------------------

typedef struct _ShinyNodePool {

	struct _ShinyNodePool* nextPool;

	ShinyNode *_nextItem;
	ShinyNode *endOfItems;

	ShinyNode _items[1];

} ShinyNodePool;


//-----------------------------------------------------------------------------

SHINY_INLINE ShinyNode* ShinyNodePool_firstItem(ShinyNodePool *self) {
	return &(self->_items[0]);
}

SHINY_INLINE ShinyNode* ShinyNodePool_newItem(ShinyNodePool *self) {
	return self->_nextItem++;
}

ShinyNodePool* ShinyNodePool_create(uint32_t a_items);
void ShinyNodePool_destroy(ShinyNodePool *self);

uint32_t ShinyNodePool_memoryUsageChain(ShinyNodePool *first);


#endif // if SHINY_COMPILED == TRUE

#endif // ifndef SHINY_*_H
