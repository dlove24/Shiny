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
*** \file config_header.h
*** \brief Header Includes: these are conditional, based on the results of the
**  		                    CMake header checks in \code config_cmake.h
***
*** \author David Love
*** \date June 2011
**/

#ifndef CONFIG_HEADER_H
#define CONFIG_HEADER_H

/**
*** Header Includes: these are conditional, based on the results of the
**  								 header checks above
**/

/* Look for the C99 integer types */
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#else
# error "Cannot build Shiny on this platform: The C99 integer types are undefined"
#endif

/* Look for the header files defining the constants for type lengths */
#ifdef HAVE_VALUES_H
# include <values.h>
#endif

#ifdef HAVE_FLOAT_H
# include <float.h>
#endif

#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif

#endif /* CONFIG_HEADER_H */