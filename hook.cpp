/*
Firefox_hook (C) Raja Jamwal 2011
Distributed under GNU GPL License
 
Firefox_hook is an example code for Chrom Library, Firefox_hook log every
HTTP/HTTPS requests that firefox makes
 
Chrom, is API/Funtion interception/hijacking library for windows systems
Copyright (C) 2011  Raja Jamwal
 
This file is part of Chrom.
*/
 
#include "chrom.h"
#include <stdlib.h> //getenv
 
Hook hk; // Hook firefox


BOOL WINAPI PR_GetComputerName_H(LPTSTR lpBuffer, LPDWORD lpnSize);
typedef BOOL (*prGetComputerNameA(LPTSTR, LPDWORD)); // original function

// initialize hooking, this adds the jump instruction to original function address
int create_hooks()
{
        // Override PR_Write function in nspr4.dll with our PR_Write_H, 
        // Note nspr4.dll must already be
        // loaded in process space
        //Firefox.Initialize("PR_Write", "nspr4.dll", (void*)PR_Write_H);
		hk.Initialize("GetComputerNameA", "kernel32.dll", (FARPROC)PR_GetComputerName_H);
        // Write jump instruction on original function address
        hk.Start();
        return TRUE;
}

BOOL PR_GetComputerName_H(LPTSTR lpBuffer, LPDWORD lpnSize)
{
	//hk.Reset();
	//prw = (prWrite)Firefox.original_function;
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
	//hk.Place_Hook
	return true;
}

extern "C" BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		create_hooks();
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
