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

#include "ShinyNode.h"
#include "ShinyZone.h"
#include "ShinyNodeState.h"

#include <memory.h>


#if SHINY_IS_COMPILED == TRUE

//-----------------------------------------------------------------------------

ShinyNode _ShinyNode_dummy = {
	/* _last = */ { 0, 0 },
	/* zone = */ NULL,
	/* parent = */ NULL,
	/* nextSibling = */ NULL,
	/* firstChild = */ NULL,
	/* lastChild = */ NULL
};


//-----------------------------------------------------------------------------

void ShinyNode_updateTree(ShinyNode* first, float a_damping) {
	ShinyNodeState *top = NULL;
	ShinyNode *node = first;

	for (;;) {
		do {
			top = ShinyNodeState_push(top, node);
			node = node->firstChild;
		} while (node);

		for (;;) {
			node = ShinyNodeState_finishAndGetNext(top, a_damping);
			top = ShinyNodeState_pop(top);

			if (node) break;
			else if (!top) return;
		}
	}
}


//-----------------------------------------------------------------------------

void ShinyNode_updateTreeClean(ShinyNode* first) {
	ShinyNodeState *top = NULL;
	ShinyNode *node = first;

	for (;;) {
		do {
			top = ShinyNodeState_push(top, node);
			node = node->firstChild;
		} while (node);

		for (;;) {
			node = ShinyNodeState_finishAndGetNextClean(top);
			top = ShinyNodeState_pop(top);

			if (node) break;
			else if (!top) return;
		}
	}
}


//-----------------------------------------------------------------------------

const ShinyNode* ShinyNode_findNextInTree(const ShinyNode* self) {
	if (self->firstChild) {
		return self->firstChild;

	} else if (self->nextSibling) {
		return self->nextSibling;

	} else {
		ShinyNode* pParent = self->parent;

		while (!ShinyNode_isRoot(pParent)) {
			if (pParent->nextSibling) return pParent->nextSibling;
			else pParent = pParent->parent;
		}

		return NULL;
	}
}


//-----------------------------------------------------------------------------

void ShinyNode_clear(ShinyNode* self) {
	memset(self, 0, sizeof(ShinyNode));
}


//-----------------------------------------------------------------------------

void ShinyNode_enumerateNodes(const ShinyNode* a_node, void (*a_func)(const ShinyNode*)) {
	a_func(a_node);

	if (a_node->firstChild) ShinyNode_enumerateNodes(a_node->firstChild, a_func);
	if (a_node->nextSibling) ShinyNode_enumerateNodes(a_node->nextSibling, a_func);
}

#endif
