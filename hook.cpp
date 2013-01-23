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
 
#define WINVER 0x0501

#include "chrom.h"
#include <stdlib.h> //getenv
#include <wtsapi32.h>
#include <string>

#include <stdio.h>
Hook hk; // Hook struct

typedef BOOL (*prGetComputerNameA(LPTSTR, LPDWORD)); // original function

std::string GetEnvName() {
	std::string clientname;
	char* name = getenv("FAKECOMPUTERNAME");
	if (name!=NULL) {
		clientname = std::string(name, strlen(name));
		printf("GetComputerNameA(): FAKECOMPUTERNAME: %s\n", clientname.c_str());
		return clientname;
	}
	LPTSTR ppBuffer = NULL;
	DWORD pBytesReturned = 0;
	if (WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION, WTSClientName, &ppBuffer, &pBytesReturned)) {
		clientname = std::string(ppBuffer, pBytesReturned);
		WTSFreeMemory(ppBuffer);
		if (pBytesReturned>1) {
			printf("GetComputerNameA(): WTSCLIENTNAME: %s\n", clientname.c_str());
			return clientname;
		}
	}
	name = getenv("COMPUTERNAME");
	if (name!=NULL) {
		clientname = std::string(name, strlen(name));
		printf("GetComputerNameA(): COMPUTERNAME: %s\n", clientname.c_str());
		return clientname;
	}
	return clientname;
}

//hook function which gets called instead of GetComputerNameA
extern "C" BOOL APIENTRY PR_GetComputerName_H(LPTSTR lpBuffer, LPDWORD lpnSize)
{
	const std::string computername = GetEnvName();
	if (computername.empty()) {
		return false;
	}
	if (*lpnSize<computername.size())
		return false;

	strcpy(lpBuffer, computername.c_str());
	lpBuffer[computername.size()]=0;
	*lpnSize = computername.size();
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
