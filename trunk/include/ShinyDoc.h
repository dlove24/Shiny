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

#ifndef SHINY_PROFILER // do not include this file


//-----------------------------------------------------------------------------
/*! \mainpage

\section Introduction
Goto the documentation here: \ref ShinyDoc.h

\n
\subsection sub1 What is Shiny?

Shiny is a low overhead, well documented and lightning fast C++ profiler. Easy to use on existing projects, no extensive surgery, just have fun with it. Nicely formatted output provides call tree (call graph), flat profile and is simple to custom display.

\n
\subsection sub-1 What is this document?

\ref ShinyDoc.h is a complete description of all the Shiny macros. Macro is a preprocessor directive that provides a mechanism for token replacement in your source code. Shiny uses macros to simplify interface and remain powerful. If the preprocessor SHINY_PROFILER is FALSE macro is ignored unless specified otherwise.

\n
\subsection sub2 Why did you create Shiny?

I was working on a graphics engine and I wanted to see the fruit of my optimizations and growth to each critical part. I needed something accurate, adapted to my main-loop & display and disgusting fast.

\n
\subsection sub3 What's the difference between other profilers?

Most profilers are external tools and can't adapt to the way your project is designed. The rest aren't really powerful enough. Shiny can adapt to your code flow and your display by your choice. Optimized to the bone, delivers both call tree/graph and flat profile and is so easy to use you'll never look at another profiler again.

\n
\subsection sub4 What features does Shiny support?

Where are some key features:
-    Lightning fast and accurate profiling.
-    Self-initialization: no explicit starting point.
-    Ridiculously easy and minimal interface.
-    Nicely formatted output with customizable display.
-    Call-tree with recursion and child-to-parent relationship timing.
-    Smooth averages support for main-loop projects, typically games.
-    Static combinded with internal pool memory manager for negligible presence.
-    Platform-independent clean source code without GPL restrictions.
-    Well documentation and example rich.


\n
\subsection sub5 How to switch Shiny on and off?

Shiny is turned on or off at compile time. Define the pre-processor SHINY_PROFILER to TRUE or FALSE to enable or disable, before including header Shiny.h. Shiny can also be enabled or disabled in ShinyConfig.h. Default is on.

\n
\subsection sub6 Does Shiny support C language?

No. I currently have no future plans to support C, although it is easy to adapt Shiny for C without touching the interface.

\n
\subsection sub7 Is Shiny thread-safe?

Not yet.

\n
\subsection sub8 Why is profile data zero?

PROFILER_UPDATE must be called before outputting profile data.

\n
\subsection sub9 How to output latest profile data instead of averages?

Set damping to zero when calling PROFILER_UPDATE.
In code: PROFILER_UPDATE(0);

\n
\subsection sub10 Why is profile data "hit" count a floating-point?

Shiny uses the exponential moving average to calculate floating-point values for the latest profile data values. This can be customized by the damping parameter for PROFILER_UPDATE.

\n
\subsection sub11 Why is some profile data leaping seconds back or forward in time?

Your platform has broken support for multi-core or multi-processor high-performance timing. You can solve this in three ways in Windows:
(Recommended) Download the fix for this from Microsoft website.
-    Call SetProcessAffinityMask or SetThreadAffinityMask at start of program to lock to a specific CPU.
-    (Not Recommended) Use timeGetTime instead of QueryPerformanceCounter in the Shiny source code.

*/

//-----------------------------------------------------------------------------
//! Profile the caller function
/*!
This will create and begin a profile with the caller function name until end of block. Call this in the beginning of your function.

\note Must only be called at most once per function

\Example
\code
{working example}
\b Output:
{output from working example}
\endcode

\see
PROFILE_CODE
PROFILE_BLOCK
PROFILE_BEGIN
PROFILE_UPDATE_ALL
PROFILE_OUTPUT_ALL
*/

#define PROFILE_FUNC()


//-----------------------------------------------------------------------------
//! Profile and execute the specified code
/*! 
This will create and begin a profile, execute code, and end profile. PROFILE_CODE is typically used when you want to profile a function call without touching the function source code or profiling every call.

\note Any return value from the executed code will be lost

\param Code to be executed and profiled

\Example
\code
{working example}
\b Output:
{output from working example}
\endcode

\see
PROFILE_FUNC
PROFILE_BLOCK
PROFILE_BEGIN
PROFILE_UPDATE_ALL
PROFILE_OUTPUT_ALL
*/

#define PROFILE_CODE(code)


//-----------------------------------------------------------------------------
//! Profile the caller block
/*! 
This will create and begin a profile with the specified name until end of block. Call this in the beginning of your code block (function, for-loop, if-statment, etc).

\note Must only be called at most once per code block

\param Name of profile to be created. Name is an identifier (not a string) and uses C++ naming rules

\Example
\code
{working example}
\b Output:
{output from working example}
\endcode

\see
PROFILE_FUNC
PROFILE_CODE
PROFILE_BEGIN
PROFILE_UPDATE_ALL
PROFILE_OUTPUT_ALL
*/

#define PROFILE_BLOCK(name)


//-----------------------------------------------------------------------------
//! Profile until PROFILE_END
/*! 
This will create and begin a profile with the specified name until PROFILE_END is called. Call PROFILE_END to end profile.

\note When the profile is finished program must call PROFILE_END.

\param Name of profile to be created. Name is an identifier (not a string) and uses C++ naming rules

\Example
\code
{working example}
\b Output:
{output from working example}
\endcode

\see
PROFILE_FUNC
PROFILE_CODE
PROFILE_BLOCK
PROFILE_UPDATE_ALL
PROFILE_OUTPUT_ALL
*/

#define PROFILE_BEGIN()


//-----------------------------------------------------------------------------
//! short
/*! 
{Specialisation offered by the function A}

\note Must only be called at most once per function

\param {details of 1st parameters used}

\param {details of 2nd parameters used}

\param {etc...}

\return {details of any return values}

\Example
\code
{working example}
\b Output:
{output from working example}
\endcode

\see
PROFILE_CODE
PROFILE_BLOCK
PROFILE_BEGIN
PROFILE_UPDATE_ALL
PROFILE_OUTPUT_ALL
*/

#define PROFILE_()

//-----------------------------------------------------------------------------


#endif
