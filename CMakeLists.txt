# Copyright (c) 2011 David Love
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

##
## Project Definition
##

# Project Name
project ( shiny )

# Set-up CMake
cmake_minimum_required ( VERSION 2.6 )

# Include the checks for library functions
include ( CheckLibraryExists )

# Include the checks for header files
include ( CheckIncludeFiles )

# Include the checks for type symbols
include ( CheckTypeSize ) 

# Include the checks for functions
include ( cmake/extra/CheckPrototypeDefinition.cmake ) 

##
## Compiler Configuration
##

# Enforce ANSI mode on GCC compilers
if ( ${CMAKE_COMPILER_IS_GNUCC} ) 
  message ( "-- Using GCC compiler profile...")
  # Force ANSI C (C89/C90) mode
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall -Wextra" )
endif ( ${CMAKE_COMPILER_IS_GNUCC} ) 

# Check for CLang (this should be redundant eventually, as a patch
# has been made to the 2.8 branch. However, until that version becomes
# common, this will do)
#
# NOTE: This MUST be done AFTER the GCC check, as Clang also shows up
# as GCC. So test for that first, then Clang. One day this will all go
# away..
if ( "${CMAKE_C_COMPILER_ID}" MATCHES "Clang" )
  message ( "-- Using Clang compiler profile...")
  # Force ANSI C (C89/C90) mode
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall -Wextra" )
endif ( "${CMAKE_C_COMPILER_ID}" MATCHES "Clang" )

##
## Project Configuration
##

# Set the global configure file
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/lib/config/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/lib/config/config.h )

# Add the global configure file to the search path
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/lib/config )

# Look for the math library: on some platforms this is part of the C
# library, on others it needs linking separately
check_library_exists ( m floor "" SEPARATE_LIB_M ) 

if ( SEPARATE_LIB_M )
  # If the math library is separate, then it needs to be included
  # in the global link flags
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lm")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -lm")
endif( SEPARATE_LIB_M )
 
# Look for the "assert.h" header file
check_include_files ( assert.h HAVE_ASSERT_H )

# Look for the "inttypes.h" header file
check_include_files ( inttypes.h HAVE_INTTYPES_H )

# Look for the "stdint.h" header file
check_include_files ( stdint.h HAVE_STDINT_H )

# Look for the header files defining the constants for type lengths
check_include_files ( values.h HAVE_VALUES_H )
check_include_files ( float.h HAVE_FLOAT_H )
check_include_files ( limits.h HAVE_LIMITS_H )

# Work out where timeval lives
set ( CMAKE_EXTRA_INCLUDE_FILES sys/resource.h )
	check_type_size( "struct timeval" TIME_IN_SYS_RESOURCE )
set ( CMAKE_EXTRA_INCLUDE_FILES )

set ( CMAKE_EXTRA_INCLUDE_FILES sys/time.h )
	check_type_size( "struct timeval" TIME_IN_SYS_TIME )
set ( CMAKE_EXTRA_INCLUDE_FILES )

# Work out where gettimeofday lives
check_prototype_definition ( gettimeofday "int gettimeofday(struct timeval *restrict tp, void *restrict tzp)" "NULL" "sys/time.h" GETTIMEOFDAY_IN_SYS_TIME ) 
check_prototype_definition ( gettimeofday "int gettimeofday(struct timeval *restrict tp, void *restrict tzp)" "NULL" "time.h" GETTIMEOFDAY_IN_TIME )

# Check whether we are on a 32-bit or a 64-bit platform for non-Apple
# platforms: Apple platforms have to be checked at compile time

if ( NOT APPLE )
   # check 64 bit
   if ( CMAKE_SIZEOF_VOID_P EQUALS 4 )
     set ( HAVE_64_BIT 0 )
   else ( CMAKE_SIZEOF_VOID_P EQUALS 4 )
     set ( HAVE_64_BIT 1 )
   endif ( CMAKE_SIZEOF_VOID_P EQUALS 4 )

   # check endianness
   include ( TestBigEndian )
   test_big_endian ( HAVE_BIG_ENDIAN )
   if ( HAVE_BIG_ENDIAN )
     set ( HAVE_BIG_ENDIAN 1 )
   else ( HAVE_BIG_ENDIAN )
     set ( HAVE_BIG_ENDIAN 0 )
   endif ( HAVE_BIG_ENDIAN )
endif ( NOT APPLE )

##
## C Application Sources
##

# Add the Shiny header files to the search path
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR}/trunk/Shiny/include )

add_executable ( shiny_tools trunk/Shiny/src/ShinyTools.c )
