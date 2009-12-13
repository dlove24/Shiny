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

#include "ShinyNodeState.h"
#include "ShinyNode.h"
#include "ShinyZone.h"

#include <malloc.h>


#if SHINY_COMPILED == TRUE

//-----------------------------------------------------------------------------

ShinyNodeState* ShinyNodeState_push(ShinyNodeState *a_top, ShinyNode *a_node) {
	ShinyZone *zone = a_node->zone;
	ShinyNodeState *self = (ShinyNodeState*) malloc(sizeof(ShinyNodeState));
	self->node = a_node;
	self->_prev = a_top;

	a_node->data.selfTicks.cur = a_node->_last.selfTicks;
	a_node->data.entryCount.cur = a_node->_last.entryCount;

	zone->data.selfTicks.cur += a_node->_last.selfTicks;
	zone->data.entryCount.cur += a_node->_last.entryCount;
	
	zone->data.childTicks.cur = 0;
	a_node->_last.selfTicks = 0;
	a_node->_last.entryCount = 0;

	self->zoneUpdating = a_node->zone->_state != SHINY_ZONE_STATE_UPDATING;
	if (self->zoneUpdating) {
		zone->_state = SHINY_ZONE_STATE_UPDATING;
	} else {
		zone->data.childTicks.cur -= zone->data.selfTicks.cur;
	}

	return self;
}

//-----------------------------------------------------------------------------

ShinyNodeState* ShinyNodeState_pop(ShinyNodeState *a_top) {
	ShinyNodeState *prev = a_top->_prev;
	free(a_top);
	return prev;
}

//-----------------------------------------------------------------------------

ShinyNode* ShinyNodeState_finishAndGetNext(ShinyNodeState *self, float a_damping) {
	ShinyNode *node = self->node;
	ShinyZone *zone = node->zone;

	if (self->zoneUpdating) {					
		zone->data.childTicks.cur += zone->data.childTicks.cur;
		zone->_state = SHINY_ZONE_STATE_INITIALIZED;
	}

	ShinyData_computeAverage(&(zone->data), a_damping);

	if (!ShinyNode_isRoot(node))
		node->parent->data.childTicks.cur += zone->data.selfTicks.cur + zone->data.childTicks.cur;

	return node->nextSibling;
}


//-----------------------------------------------------------------------------

ShinyNode* ShinyNodeState_finishAndGetNextSimple(ShinyNodeState *self) {
	ShinyNode *node = self->node;
	ShinyZone *zone = node->zone;

	if (self->zoneUpdating) {					
		zone->data.childTicks.cur += zone->data.childTicks.cur;
		zone->_state = SHINY_ZONE_STATE_INITIALIZED;
	}

	ShinyData_copyAverage(&(zone->data));

	if (!ShinyNode_isRoot(node))
		node->parent->data.childTicks.cur += zone->data.selfTicks.cur + zone->data.childTicks.cur;

	return node->nextSibling;
}

#endif
