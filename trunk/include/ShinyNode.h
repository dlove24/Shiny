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

#ifndef SHINY_NODE_H
#define SHINY_NODE_H

#include "ShinyData.h"
#include "ShinyTools.h"

#if SHINY_COMPILED == TRUE


//-----------------------------------------------------------------------------

typedef struct _ShinyNode {

	ShinyLastData _last;

	struct _ShinyZone* zone;
	struct _ShinyNode* parent;
	struct _ShinyNode* nextSibling;

	struct _ShinyNode* firstChild;
	struct _ShinyNode* lastChild;

	uint32_t childCount;
	uint32_t entryLevel;

	ProfileNodeCache* _cache;

	ShinyData data;

	//
} ShinyNode;


//-----------------------------------------------------------------------------

extern ShinyNode ShinyNode_dummy;


//-----------------------------------------------------------------------------

SHINY_INLINE void ShinyNode_addChild(ShinyNode* self,  ShinyNode* a_child) {
	if (self->childCount++) {
		self->lastChild->nextSibling = a_child;
		self->lastChild = a_child;

	} else {
		self->lastChild = a_child;
		self->firstChild = a_child;
	}
}

SHINY_INLINE void ShinyNode_init(ShinyNode* self, ShinyNode* a_parent, struct _ShinyZone* a_zone, ProfileNodeCache* a_cache) {
	// NOTE: all member variables are assumed to be zero when allocated

	self->zone = a_zone;
	self->parent = a_parent;

	self->entryLevel = a_parent->entryLevel + 1;
	ShinyNode_addChild(a_parent, self);

	self->_cache = a_cache;
}

void ShinyNode_updateTree(ShinyNode* self, float a_damping);
void ShinyNode_updateTreeSimple(ShinyNode* self);

SHINY_INLINE void ShinyNode_destroy(ShinyNode* self) {
	*(self->_cache) = &ShinyNode_dummy;
}

SHINY_INLINE void ShinyNode_appendTicks(ShinyNode* self, tick_t a_elapsedTicks) {
	self->_last.selfTicks += a_elapsedTicks;
}

SHINY_INLINE void ShinyNode_beginEntry(ShinyNode* self) {
	self->_last.entryCount++;
}

SHINY_INLINE int ShinyNode_isRoot(ShinyNode* self) {
	return (self->entryLevel == 0);
}

SHINY_INLINE int ShinyNode_isDummy(ShinyNode* self) {
	return (self == &ShinyNode_dummy);
}

SHINY_INLINE int ShinyNode_isEqual(ShinyNode* self, const ShinyNode* a_parent, const struct _ShinyZone* a_zone) {
	return (self->parent == a_parent && self->zone == a_zone);
}

const ShinyNode* ShinyNode_findNextInTree(const ShinyNode* self);

void ShinyNode_clear(ShinyNode* self);

void ShinyNode_enumerateNodes(const ShinyNode* a_node, void (*a_func)(const ShinyNode*));

#if __cplusplus
template <class T>
void ShinyNode_enumerateNodes(const ShinyNode* a_node, T* a_this, void (T::*a_func)(const ShinyNode*)) {
	(a_this->*a_func)(a_node);

	if (a_node->firstChild) ShinyNode_enumerateNodes(a_node->firstChild, a_this, a_func);
	if (a_node->nextSibling) ShinyNode_enumerateNodes(a_node->nextSibling, a_this, a_func);
}
#endif

#endif // if SHINY_COMPILED == TRUE

#endif // ifndef SHINY_*_H
