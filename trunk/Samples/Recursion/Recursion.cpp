/*
The zlib/libpng License

Copyright (c) 2007 Aidin Abedi (http://sourceforge.net/projects/shinyprofiler)

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

#include "Shiny.h"

#ifdef _WIN32
#include <windows.h>
#else // assume POSIX
#include <unistd.h>
#endif

#include <iostream>
using namespace std;


//-----------------------------------------------------------------------------

void millisleep(unsigned int milliseconds) {
#ifdef _WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}


//-----------------------------------------------------------------------------

void Recursion(int calls_left) {
	PROFILE_FUNC(); // begin profile until end of block

	if (calls_left > 0) Recursion(calls_left - 1);
}


//-----------------------------------------------------------------------------

int main() {

	Recursion(12);

	PROFILE_UPDATE(); // update all profiles
	PROFILE_OUTPUT(stdout); // print to cout

#ifdef _WIN32
	system("pause");
#endif
	return 0;
}
