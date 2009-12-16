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

#ifndef SHINY_CONFIG_H
#define SHINY_CONFIG_H


//-----------------------------------------------------------------------------

#define SHINY_VERSION			"Shiny Profiler 2.5"


//-----------------------------------------------------------------------------

// SHINY_IS_COMPILED is the master on or off swith at compile time. Define it to TRUE or FALSE before including header Shiny.h or inside ShinyConfig.h. Default is TRUE.
#ifndef SHINY_IS_COMPILED
#define SHINY_IS_COMPILED		TRUE
#endif

// if SHINY_LOOKUP_RATE is defined to TRUE then Shiny will record the success of its hash function. This is useful for debugging. Default is FALSE.
#ifndef SHINY_LOOKUP_RATE
#define SHINY_LOOKUP_RATE		FALSE
#endif

// if SHINY_HAS_ENABLED is defined to TRUE then Shiny can be enabled and disabled at runtime. TODO: bla bla...
#ifndef SHINY_HAS_ENABLED
#define SHINY_HAS_ENABLED		FALSE
#endif

// TODO:
#define SHINY_OUTPUT_MODE_FLAT	0x1

// TODO:
#define SHINY_OUTPUT_MODE_TREE	0x2

// TODO:
#define SHINY_OUTPUT_MODE_BOTH	0x3

// TODO:
#ifndef SHINY_OUTPUT_MODE
#define SHINY_OUTPUT_MODE		SHINY_OUTPUT_MODE_BOTH
#endif


//-----------------------------------------------------------------------------

#ifndef FALSE
#define FALSE	0x0
#endif

#ifndef TRUE
#define TRUE	0x1
#endif

#ifndef NULL
#define NULL	0
#endif


//-----------------------------------------------------------------------------

#define SHINY_PLATFORM_WIN32	0x1
#define SHINY_PLATFORM_POSIX	0x2

#if defined (_WIN32)
#   define SHINY_PLATFORM	SHINY_PLATFORM_WIN32

#else
//	ASSUME: POSIX-compliant OS
#   define SHINY_PLATFORM	SHINY_PLATFORM_POSIX
#endif


//-----------------------------------------------------------------------------

#define SHINY_COMPILER_MSVC		0x1
#define SHINY_COMPILER_GNUC		0x2
#define SHINY_COMPILER_OTHER	0x3

#if defined (_MSC_VER)
#   define SHINY_COMPILER	SHINY_COMPILER_MSVC

#elif defined (__GNUG__)
#   define SHINY_COMPILER	SHINY_COMPILER_GNUC

#else
#   define SHINY_COMPILER	SHINY_COMPILER_OTHER
#endif

#endif // ifndef SHINY_*_H
