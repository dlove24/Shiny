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
