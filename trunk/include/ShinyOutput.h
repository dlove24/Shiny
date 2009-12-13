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

#ifndef SHINY_OUTPUT_H
#define SHINY_OUTPUT_H

#include "ShinyNode.h"
#include "ShinyZone.h"

#if SHINY_COMPILED == TRUE


//-----------------------------------------------------------------------------

int ShinyPrintNodesSize(uint32_t a_count);
int ShinyPrintZonesSize(uint32_t a_count);

void ShinyPrintNodes(char* output, const ShinyNode *a_root);
void ShinyPrintZones(char* output, const ShinyZone *a_root);


//-----------------------------------------------------------------------------

#if __cplusplus
#include <string>

/*
SHINY_INLINE std::string ShinyNodesToString(const ShinyNode *a_root, uint32_t a_count) {
	std::string str(ShinyPrintNodesSize(a_count) - 1);
	ShinyPrintNodes(&str[0], a_root);
	return str;
}

SHINY_INLINE std::string ShinyZonesToString(const ShinyZone *a_root, uint32_t a_count) {
	std::string str(ShinyPrintZonesSize(a_count) - 1);
	ShinyPrintZones(&str[0], a_root);
	return str;
}
*/
#endif

#endif // if SHINY_COMPILED == TRUE

#endif // ifndef SHINY_*_H
