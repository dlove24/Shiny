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

#include "ShinyZone.h"

#include <memory.h>

#if SHINY_PROFILER == TRUE
namespace Shiny {

//-----------------------------------------------------------------------------

	//TODO: these loops loop for ever - find out why. 

	void ProfileZone::preUpdateChain(void) {
		ProfileZone* zone = this;

		while (zone) {
			zone->data.clearCurrent();
			zone = zone->next;
		}
	}


//-----------------------------------------------------------------------------

	void ProfileZone::updateChain(float a_damping) {
		ProfileZone* zone = this;

		do {
			zone->data.computeAverage(a_damping);
			zone = zone->next;
		} while (zone);
	}


//-----------------------------------------------------------------------------

	void ProfileZone::updateChain(void) {
		ProfileZone* zone = this;

		do {
			zone->data.copyAverage();
			zone = zone->next;
		} while (zone);
	}


//-----------------------------------------------------------------------------

	void ProfileZone::resetChain(void) {
		ProfileZone* zone = this, *temp;

		do {
			zone->_state = STATE_HIDDEN;
			temp = zone->next;
			zone->next = NULL;
			zone = temp;
		} while (zone);
	}

//-----------------------------------------------------------------------------

	void ProfileZone::clear(void) {
		memset(this, 0, sizeof(ProfileZone));
	}

//-----------------------------------------------------------------------------

	/*
	 * This is the actual sort function. Notice that it returns the new
	 * head of the list. (It has to, because the head will not
	 * generally be the same ProfileZone after the sort.) So unlike sorting
	 * an array, where you can do
	 * 
	 *     sort(myarray);
	 * 
	 * you now have to do
	 * 
	 *     list = listsort(mylist);
	 */
	ProfileZone* ProfileZone::sortChain(void) {
		ProfileZone *p, *q, *e, *tail, *list = this;
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

					/* decide whether next ProfileZone of merge comes from p or q */
					if (psize == 0) {
						/* p is empty; e must come from q. */
						e = q; q = q->next; qsize--;
					} else if (qsize == 0 || !q) {
						/* q is empty; e must come from p. */
						e = p; p = p->next; psize--;
					} else if (p->compare(q) <= 0) {
						/* First ProfileZone of p is lower (or same);
						 * e must come from p. */
						e = p; p = p->next; psize--;
					} else {
						/* First ProfileZone of q is lower; e must come from q. */
						e = q; q = q->next; qsize--;
					}

					/* add the next ProfileZone to the merged list */
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


} // namespace Shiny
#endif
