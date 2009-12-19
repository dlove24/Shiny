/*
The MIT License

Copyright (c) 2007-2009 Aidin Abedi http://code.google.com/p/shinyprofiler/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "ShinyOutput.h"

#include <stdio.h>

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	pragma warning(disable: 4996)
#	define snprintf		_snprintf
#	define TRAILING		0

#else
#	define TRAILING		1
#endif

#if SHINY_IS_COMPILED == TRUE


//-----------------------------------------------------------------------------

#define OUTPUT_WIDTH_CALL	6
#define OUTPUT_WIDTH_TIME	6
#define OUTPUT_WIDTH_PERC	4
#define OUTPUT_WIDTH_SUM	79

#define OUTPUT_WIDTH_DATA	(1+OUTPUT_WIDTH_CALL + 1 + 2*(OUTPUT_WIDTH_TIME+4+OUTPUT_WIDTH_PERC+1) + 1)
#define OUTPUT_WIDTH_NAME	(OUTPUT_WIDTH_SUM - OUTPUT_WIDTH_DATA)


//-----------------------------------------------------------------------------

void _printHeader(char *dest, const char *a_title) {
	snprintf(dest, OUTPUT_WIDTH_SUM + TRAILING,
		"%-*s %*s %*s %*s",
		OUTPUT_WIDTH_NAME, a_title,
		OUTPUT_WIDTH_CALL, "calls",
		OUTPUT_WIDTH_TIME+4+OUTPUT_WIDTH_PERC+1, "self time",
		OUTPUT_WIDTH_TIME+4+OUTPUT_WIDTH_PERC+1, "total time");
}


//-----------------------------------------------------------------------------

void _printData(char *dest, const ShinyData *a_data, float a_topercent) {
	float totalTicksAvg = ShinyData_totalTicksAvg(a_data);
	const ShinyTimeUnit *selfUnit = ShinyGetTimeUnit(a_data->selfTicks.avg);
	const ShinyTimeUnit *totalUnit = ShinyGetTimeUnit(totalTicksAvg);

	snprintf(dest, OUTPUT_WIDTH_DATA + TRAILING,
		" %*.1f %*.0f %-2s %*.0f%% %*.0f %-2s %*.0f%%",
		OUTPUT_WIDTH_CALL, a_data->entryCount.avg,
		OUTPUT_WIDTH_TIME, a_data->selfTicks.avg * selfUnit->invTickFreq, selfUnit->suffix,
		OUTPUT_WIDTH_PERC, a_data->selfTicks.avg * a_topercent,
		OUTPUT_WIDTH_TIME, totalTicksAvg * totalUnit->invTickFreq, totalUnit->suffix,
		OUTPUT_WIDTH_PERC, totalTicksAvg * a_topercent);
}


//-----------------------------------------------------------------------------

int ShinyPrintNodesSize(uint32_t a_count) {
	return (1 + a_count) * (OUTPUT_WIDTH_SUM + 1);
}


//-----------------------------------------------------------------------------

int ShinyPrintZonesSize(uint32_t a_count) {
	return (1 + a_count) * (OUTPUT_WIDTH_SUM + 1);
}


//-----------------------------------------------------------------------------

void ShinyPrintNodes(char* output, const ShinyNode *a_root) {
	float fTicksToPc = 100.0f / a_root->data.childTicks.avg;
	const ShinyNode *node = a_root;
	char *s = output;

	_printHeader(s, "call tree");
	s += OUTPUT_WIDTH_SUM;
	(*s++) = '\n';

	do {
		int offset = node->entryLevel * 2;
		snprintf(s, OUTPUT_WIDTH_NAME + TRAILING, "%*s%-*s",
			offset, "", OUTPUT_WIDTH_NAME - offset, node->zone->name);

		s += OUTPUT_WIDTH_NAME;

		_printData(s, &(node->data), fTicksToPc);

		s += OUTPUT_WIDTH_DATA;
		(*s++) = '\n';

		node = ShinyNode_findNextInTree(node);
	} while (node);

	*(--s) = '\0';
}


//-----------------------------------------------------------------------------

void ShinyPrintZones(char* output, const ShinyZone *a_root) {
	float fTicksToPc = 100.0f / a_root->data.childTicks.avg;
	const ShinyZone *zone = a_root;
	char *s = output;

	_printHeader(s, "flat profile");
	s += OUTPUT_WIDTH_SUM;
	(*s++) = '\n';

	do {
		snprintf(s, OUTPUT_WIDTH_NAME + TRAILING, "%-*s",
			OUTPUT_WIDTH_NAME, zone->name);

		s += OUTPUT_WIDTH_NAME;

		_printData(s, &(zone->data), fTicksToPc);

		s += OUTPUT_WIDTH_DATA;
		(*s++) = '\n';

		zone = zone->next;
	} while (zone);

	*(--s) = '\0';
}

#endif
