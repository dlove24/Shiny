/**
*** Copyright (c) 2001 David Love <david@homeunix.org.uk>
***
*** Permission is hereby granted, free of charge, to any person obtaining a copy
*** of this software and associated documentation files (the "Software"), to deal
*** in the Software without restriction, including without limitation the rights
*** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*** copies of the Software, and to permit persons to whom the Software is
*** furnished to do so, subject to the following conditions:
*** 
*** The above copyright notice and this permission notice shall be included in
*** all copies or substantial portions of the Software.
*** 
*** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*** THE SOFTWARE.
***
*** \file config.h
*** \brief Bundles the CMake checks, platform, library and other environmental
*** needs into a single file. All files relying on the external C environment
*** should include this file
***
*** \author David Love
*** \date June 2011
**/

#ifndef CONFIG_H
#define CONFIG_H

/* Include the platform checks from CMake */
#include "config_cmake.h"

/* Include the platform specific headers */
#include "config_header.h"

/* Include the headers for the needed function prototypes */
#include "config_function.h"

/* Include the library defines */
#include "config_define.h"

/* Include the Shiny version defines */
#include "config_version.h"

#endif /* CONFIG_H */