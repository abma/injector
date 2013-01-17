/*
    Injector (C) Matthias Ableitner 2013

    This file is part of injector.

    Injector is free software: you can redistribute it and/or modify it under
	the terms of the GNU General Public License as published by the Free
	Software Foundation, either version 2 of the License, or (at your option)
	any later version.

    Injector is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
	or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
	for more details.

    You should have received a copy of the GNU General Public License along
	with Injector. If not, see http://www.gnu.org/licenses/
*/
 
#include "chrom.h"
#include <stdlib.h> //getenv
 
Hook hk; // Hook struct

typedef BOOL (*prGetComputerNameA(LPTSTR, LPDWORD)); // original function

//hook function which gets called instead of GetComputerNameA
extern "C" BOOL APIENTRY PR_GetComputerName_H(LPTSTR lpBuffer, LPDWORD lpnSize)
{
	char* name = getenv("CLIENTNAME");
	if (name == NULL) {
		name = getenv("COMPUTERNAME");
	}
	if (name == NULL) {
		return false;
	}
	const int len=strlen(name);
	if (*lpnSize<len)
		return false;

	strcpy(lpBuffer, name);
	lpBuffer[len]=0;
	*lpnSize = len;
	//MessageBox(0, name, "test" , MB_OK);
	return true;
}

//gets called when dll is loaded and unloaded
extern "C" BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		// initialize hooking, this adds the jump instruction to original function address
		hk.Initialize("GetComputerNameA", "kernel32.dll", (FARPROC)PR_GetComputerName_H);
        // Write jump instruction to original function address
        hk.Start();
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
