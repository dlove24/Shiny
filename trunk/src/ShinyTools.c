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

#include "ShinyTools.h"

#if SHINY_PLATFORM == SHINY_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif SHINY_PLATFORM == SHINY_PLATFORM_POSIX
#include <sys/time.h>
#endif


//-----------------------------------------------------------------------------

const ShinyTimeUnit* ShinyGetTimeUnit(float ticks) {
	static ShinyTimeUnit units[4] = { 0 };

	if (units[0].tickFreq == 0) { // auto initialize first time
		units[0].tickFreq = ShinyGetTickFreq() / 1.0f;
		units[0].invTickFreq = ShinyGetTickInvFreq() * 1.0f;
		units[0].suffix = "s";

		units[1].tickFreq = ShinyGetTickFreq() / 1000.0f;
		units[1].invTickFreq = ShinyGetTickInvFreq() * 1000.0f;
		units[1].suffix = "ms";

		units[2].tickFreq = ShinyGetTickFreq() / 1000000.0f;
		units[2].invTickFreq = ShinyGetTickInvFreq() * 1000000.0f;
		units[2].suffix = "us";

		units[3].tickFreq = ShinyGetTickFreq() / 1000000000.0f;
		units[3].invTickFreq = ShinyGetTickInvFreq() * 1000000000.0f;
		units[3].suffix = "ns";
	}

	if (units[0].tickFreq < ticks) return &units[0];
	else if (units[1].tickFreq < ticks) return &units[1];
	else if (units[2].tickFreq < ticks) return &units[2];
	else return &units[3];
}


//-----------------------------------------------------------------------------

#if SHINY_PLATFORM == SHINY_PLATFORM_WIN32

void ShinyGetTicks(tick_t *p) {
	QueryPerformanceCounter((LARGE_INTEGER*)(p));
}

tick_t ShinyGetTickFreq(void) {
	static tick_t freq = 0;
	if (freq == 0) QueryPerformanceFrequency((LARGE_INTEGER*)(&freq));
	return freq;
}

float ShinyGetTickInvFreq(void) {
	static float invfreq = 0;
	if (invfreq == 0) invfreq = 1.0f / ShinyGetTickFreq();
	return invfreq;
}


//-----------------------------------------------------------------------------

#elif SHINY_PLATFORM == SHINY_PLATFORM_POSIX

void ShinyGetTicks(tick_t *p) {
	timeval time;
	gettimeofday(&time, NULL);

	*p = time.tv_sec * 1000000 + time.tv_usec;
}

const tick_t& ShinyGetTickFreq(void) {
	return 1000000;
}

float ShinyGetTickInvFreq(void) {
	return 1.0f / 1000000.0f;
}

#endif
