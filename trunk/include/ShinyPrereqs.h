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

#ifndef SHINY_PREREQS_H
#define SHINY_PREREQS_H

#include "ShinyConfig.h"

#if SHINY_COMPILER == SHINY_COMPILER_GNUC
#include <sys/types.h>
#include <stdint.h>
#endif


//-----------------------------------------------------------------------------

#if SHINY_IS_COMPILED == TRUE
	struct _ShinyNode;
	struct _ShinyZone;

	typedef struct _ShinyNode* ShinyNodeCache;
	typedef struct _ShinyNode* ShinyNodeTable;
#endif


//-----------------------------------------------------------------------------


#if SHINY_COMPILER == SHINY_COMPILER_MSVC
#	define SHINY_INLINE		__inline
#	define SHINY_UNUSED

#elif SHINY_COMPILER == SHINY_COMPILER_GNUC
#	define SHINY_INLINE		inline
#	define SHINY_UNUSED		__attribute__ ((unused))

#elif SHINY_COMPILER == SHINY_COMPILER_OTHER
#	define SHINY_INLINE		inline
#	define SHINY_UNUSED
#endif

//-----------------------------------------------------------------------------

#if SHINY_COMPILER == SHINY_COMPILER_MSVC
	typedef int					int32_t;
	typedef unsigned int		uint32_t;

	typedef __int64				int64_t;
	typedef unsigned __int64	uint64_t;

/*
#elif defined(__CYGWIN__)
	typedef u_int32_t			uint32_t;
	typedef u_int64_t			uint64_t;
*/
#endif

	typedef uint64_t			shinytick_t;

#endif // ifndef SHINY_*_H
