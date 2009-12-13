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

#ifndef SHINY_ZONE_H
#define SHINY_ZONE_H

#include "ShinyData.h"
#include <memory.h>

#if SHINY_COMPILED == TRUE


//-----------------------------------------------------------------------------

#define SHINY_ZONE_STATE_HIDDEN			0
#define SHINY_ZONE_STATE_INITIALIZED	1
#define SHINY_ZONE_STATE_UPDATING		2


//-----------------------------------------------------------------------------

typedef struct _ShinyZone {
	struct _ShinyZone* next;
	int _state;
	const char* name;
	ShinyData data;
} ShinyZone;


//-----------------------------------------------------------------------------

SHINY_INLINE void ShinyZone_init(ShinyZone *self, ShinyZone* a_prev) {
	self->_state = SHINY_ZONE_STATE_INITIALIZED;
	a_prev->next = self;
}

SHINY_INLINE void ShinyZone_uninit(ShinyZone *self) {
	self->_state = SHINY_ZONE_STATE_HIDDEN;
	self->next = NULL;
}

void ShinyZone_preUpdateChain(ShinyZone *first);
void ShinyZone_updateChain(ShinyZone *first, float a_damping);
void ShinyZone_updateChainSimple(ShinyZone *first);

void ShinyZone_resetChain(ShinyZone *first);

ShinyZone* ShinyZone_sortChain(ShinyZone *first);

SHINY_INLINE float ShinyZone_compare(ShinyZone *a, ShinyZone *b) {
	return b->data.selfTicks.avg - a->data.selfTicks.avg;
}

void ShinyZone_enumerateZones(const ShinyZone* a_zone, void (*a_func)(const ShinyZone*));

#if __cplusplus
template <class T>
void ShinyZone_enumerateZones(const ShinyZone* a_zone, T* a_this, void (T::*a_func)(const ShinyZone*)) {
	(a_this->*a_func)(a_zone);

	if (a_zone->next) ShinyZone_enumerateZones(a_zone->next, a_this, a_func);
}
#endif

#endif // if SHINY_COMPILED == TRUE

#endif // ifndef SHINY_*_H
