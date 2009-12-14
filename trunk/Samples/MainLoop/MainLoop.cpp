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
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h> // Sleep
#else // assume POSIX
#include <unistd.h> // usleep
#endif


//-----------------------------------------------------------------------------

void millisleep(unsigned int milliseconds) {
#ifdef _WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}


//-----------------------------------------------------------------------------

PROFILE_SHARED_DEFINE(Physics);

void DoPhysicsSimulation() {
	PROFILE_SHARED_BLOCK(Physics);
}

void CheckPhysicsRaycast() {
	PROFILE_SHARED_BEGIN(Physics);
	PROFILE_END();
}


//-----------------------------------------------------------------------------

PROFILE_SHARED_DEFINE(Animations);

void UpdateAllCharacters() {

	PROFILE_BEGIN(Gamelogic);

	PROFILE_BEGIN(AI);
	CheckPhysicsRaycast();

	PROFILE_SHARED_BEGIN(Animations);
	PROFILE_END();

	PROFILE_END();

	PROFILE_BEGIN(Player);
	PROFILE_SHARED_BEGIN(Animations);
	PROFILE_END();
	PROFILE_END();

	PROFILE_END();
}


//-----------------------------------------------------------------------------

void DrawWorldObjects() {
	for (int i = 0; i < 10; i++) {
		PROFILE_BLOCK(Graphics);
	}
}


//-----------------------------------------------------------------------------


int main() {
	int loops = 100;

	while (loops--) {
		UpdateAllCharacters();
		DoPhysicsSimulation();
		DrawWorldObjects();
	}

	PROFILE_UPDATE(); // update all profiles
	PROFILE_OUTPUT(stdout); // print to cout

#ifdef _WIN32
	system("pause");
#endif
	return 0;
}
