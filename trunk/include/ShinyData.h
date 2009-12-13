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

#ifndef SHINY_DATA_H
#define SHINY_DATA_H

#include "ShinyPrereqs.h"

//-----------------------------------------------------------------------------

typedef struct {
	uint32_t entryCount;
	tick_t selfTicks;
} ShinyLastData;


//-----------------------------------------------------------------------------

typedef struct {
	tick_t cur;
	float avg;
} ShinyTickData;

typedef struct {
	uint32_t cur;
	float avg;
} ShinyCountData;

typedef struct {
	ShinyCountData entryCount;
	ShinyTickData selfTicks;
	ShinyTickData childTicks;
} ShinyData;

SHINY_INLINE tick_t ShinyData_totalTicksCur(const ShinyData *self) {
	return self->selfTicks.cur + self->childTicks.cur;
}

SHINY_INLINE float ShinyData_totalTicksAvg(const ShinyData *self) {
	return self->selfTicks.avg + self->childTicks.avg;
}

SHINY_INLINE void ShinyData_computeAverage(ShinyData *self, float a_damping) {
	self->entryCount.avg = self->entryCount.cur +
		a_damping * (self->entryCount.avg - self->entryCount.cur);
	self->selfTicks.avg = self->selfTicks.cur +
		a_damping * (self->selfTicks.avg - self->selfTicks.cur);
	self->childTicks.avg = self->childTicks.cur +
		a_damping * (self->childTicks.avg - self->childTicks.cur);
}

SHINY_INLINE void ShinyData_copyAverage(ShinyData *self) {
	self->entryCount.avg = (float) self->entryCount.cur;
	self->selfTicks.avg = (float) self->selfTicks.cur;
	self->childTicks.avg = (float) self->childTicks.cur;
}

SHINY_INLINE void ShinyData_clearAll(ShinyData *self) {
	self->entryCount.cur = 0;
	self->entryCount.avg = 0;
	self->selfTicks.cur = 0;
	self->selfTicks.avg = 0;
	self->childTicks.cur = 0;
	self->childTicks.avg = 0;
}

SHINY_INLINE void ShinyData_clearCurrent(ShinyData *self) {
	self->entryCount.cur = 0;
	self->selfTicks.cur = 0;
	self->childTicks.cur = 0;
}

#endif // ifndef SHINY_*_H
