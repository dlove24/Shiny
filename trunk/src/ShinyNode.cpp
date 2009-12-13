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

#include "ShinyNode.h"
#include "ShinyZone.h"

#include <memory.h>
#include <stack>


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

	struct ProfileNodeState {
		ProfileNode *node;
		bool zoneUpdating;

		ProfileNodeState(ProfileNode *a_node);
		ProfileNode* finishAndGetNext(float a_damping);
		ProfileNode* finishAndGetNext(void);
	};


//-----------------------------------------------------------------------------

	ProfileNodeState::ProfileNodeState(ProfileNode *a_node) : node(a_node) {
		ProfileZone *zone = node->zone;
		ProfileData &data = node->data;
		ProfileLastData &_last = node->_last;

		data.selfTicks.cur = _last.selfTicks;
		data.entryCount.cur = _last.entryCount;

		zone->data.selfTicks.cur += _last.selfTicks;
		zone->data.entryCount.cur += _last.entryCount;
		
		data.childTicks.cur = 0;
		_last.selfTicks = 0;
		_last.entryCount = 0;

		zoneUpdating = !zone->isUpdating();
		if (zoneUpdating) {
			zone->enableUpdating();
		} else {
			zone->data.childTicks.cur -= data.selfTicks.cur;
		}
	}


//-----------------------------------------------------------------------------

	ProfileNode* ProfileNodeState::finishAndGetNext(float a_damping) {
		ProfileZone *zone = node->zone;
		ProfileData &data = node->data;

		if (zoneUpdating) {					
			zone->data.childTicks.cur += data.childTicks.cur;
			zone->disableUpdating();
		}

		data.computeAverage(a_damping);

		if (!node->isRoot())
			node->parent->data.childTicks.cur += data.selfTicks.cur + data.childTicks.cur;

		return node->nextSibling;
	}


//-----------------------------------------------------------------------------

	ProfileNode* ProfileNodeState::finishAndGetNext(void) {
		ProfileZone *zone = node->zone;
		ProfileData &data = node->data;

		if (zoneUpdating) {					
			zone->data.childTicks.cur += data.childTicks.cur;
			zone->disableUpdating();
		}

		data.copyAverage();

		if (!node->isRoot())
			node->parent->data.childTicks.cur += data.selfTicks.cur + data.childTicks.cur;

		return node->nextSibling;
	}


//-----------------------------------------------------------------------------

	void ProfileNode::updateTree(float a_damping) {
		std::stack<ProfileNodeState> up;
		ProfileNode *node = this;

		for (;;) {
			do {
				up.push(ProfileNodeState(node));
				node = node->firstChild;
			} while (node);

			for (;;) {
				node = up.top().finishAndGetNext(a_damping);
				up.pop();

				if (node) break;
				else if (up.empty()) return;
			}
		}
	}


//-----------------------------------------------------------------------------

	void ProfileNode::updateTree(void) {
		std::stack<ProfileNodeState> up;
		ProfileNode *node = this;

		for (;;) {
			do {
				up.push(ProfileNodeState(node));
				node = node->firstChild;
			} while (node);

			for (;;) {
				node = up.top().finishAndGetNext();
				up.pop();

				if (node) break;
				else if (up.empty()) return;
			}
		}
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
