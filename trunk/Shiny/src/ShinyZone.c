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

#include "ShinyZone.h"

#include <memory.h>

#if SHINY_IS_COMPILED == TRUE

//-----------------------------------------------------------------------------

//TODO: these loops loop for ever - find out why. 

void ShinyZone_preUpdateChain(ShinyZone *first) {
	ShinyZone* zone = first;

	while (zone) {
		ShinyData_clearCurrent(&(zone->data));
		zone = zone->next;
	}
}


//-----------------------------------------------------------------------------

void ShinyZone_updateChain(ShinyZone *first, float a_damping) {
	ShinyZone* zone = first;

	do {
		ShinyData_computeAverage(&(zone->data), a_damping);
		zone = zone->next;
	} while (zone);
}


//-----------------------------------------------------------------------------

void ShinyZone_updateChainClean(ShinyZone *first) {
	ShinyZone* zone = first;

	do {
		ShinyData_copyAverage(&(zone->data));
		zone = zone->next;
	} while (zone);
}


//-----------------------------------------------------------------------------

void ShinyZone_resetChain(ShinyZone *first) {
	ShinyZone* zone = first, *temp;

	do {
		zone->_state = SHINY_ZONE_STATE_HIDDEN;
		temp = zone->next;
		zone->next = NULL;
		zone = temp;
	} while (zone);
}

//-----------------------------------------------------------------------------

/*
 * This is the actual sort function. Notice that it returns the new
 * head of the list. (It has to, because the head will not
 * generally be the same ShinyZone after the sort.) So unlike sorting
 * an array, where you can do
 * 
 *     sort(myarray);
 * 
 * you now have to do
 * 
 *     list = listsort(mylist);
 */
ShinyZone* ShinyZone_sortChain(ShinyZone *first) {
	ShinyZone *p, *q, *e, *tail, *list = first;
	int insize, nmerges, psize, qsize, i;

	insize = 1;

	while (1) {
		p = list;
		list = NULL;
		tail = NULL;

		nmerges = 0;  /* count number of merges we do in this pass */

		while (p) {
			nmerges++;  /* there exists a merge to be done */
			/* step `insize' places along from p */
			q = p;
			psize = 0;
			for (i = 0; i < insize; i++) {
				psize++;
				q = q->next;
				if (!q) break;
			}

			/* if q hasn't fallen off end, we have two lists to merge */
			qsize = insize;

			/* now we have two lists; merge them */
			while (psize > 0 || (qsize > 0 && q)) {

				/* decide whether next ShinyZone of merge comes from p or q */
				if (psize == 0) {
					/* p is empty; e must come from q. */
					e = q; q = q->next; qsize--;
				} else if (qsize == 0 || !q) {
					/* q is empty; e must come from p. */
					e = p; p = p->next; psize--;
				} else if (ShinyZone_compare(p, q) <= 0) {
					/* First ShinyZone of p is lower (or same);
					 * e must come from p. */
					e = p; p = p->next; psize--;
				} else {
					/* First ShinyZone of q is lower; e must come from q. */
					e = q; q = q->next; qsize--;
				}

				/* add the next ShinyZone to the merged list */
				if (tail) {
					tail->next = e;
				} else {
					list = e;
				}
				tail = e;
			}

			/* now p has stepped `insize' places along, and q has too */
			p = q;
		}
		
		tail->next = NULL;

		/* If we have done only one merge, we're finished. */
		if (nmerges <= 1)   /* allow for nmerges==0, the empty list case */
			return list;

		/* Otherwise repeat, merging lists twice the size */
		insize *= 2;
	}
}


//-----------------------------------------------------------------------------

void ShinyZone_clear(ShinyZone* self) {
	memset(self, 0, sizeof(ShinyZone));
}


//-----------------------------------------------------------------------------

void ShinyZone_enumerateZones(const ShinyZone* a_zone, void (*a_func)(const ShinyZone*)) {
	a_func(a_zone);

	if (a_zone->next) ShinyZone_enumerateZones(a_zone->next, a_func);
}

#endif
