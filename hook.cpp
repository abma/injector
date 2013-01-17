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
 
Hook hk; // Hook firefox

/*DWORD PR_Write_H (DWORD *fd,  void *buf,DWORD amount); // this is our overiding-function
typedef DWORD (*prWrite)(DWORD*,void*,DWORD); // definition of our original function
*/

BOOL WINAPI PR_GetComputerName_H(LPTSTR lpBuffer, LPDWORD lpnSize);
typedef BOOL (*prGetComputerNameA(LPTSTR, LPDWORD)); // original function

 
//prGetComputerNameA prw = NULL; // create an original function, we later point this to original function
                    // address
 

 
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
	MessageBox(0, "GetFakeComputerName", "GetFakeComputerName", MB_OK);
	//hk.Place_Hook
	return false;
}

extern "C" BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	printf("dllmain\n");
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		create_hooks();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

/*
// our overriding function
DWORD PR_Write_H (DWORD *fd,  void *buf,DWORD amount)
{
        // reset hooks, this will replace the jump instruction to original data
        Firefox.Reset();
        // You may skip the code shown below and call the original function directly
        // since after calling Firefox.Reset() the address of the original function, 
        // now contains the original function's data
        // point prw(function) to original function (optional)
        prw = (prWrite)Firefox.original_function;
        // log the headers
        write_log(log_file, (char*) buf);
        // call the real PR_Write function
        DWORD ret = prw(fd, buf, amount);
        // again place the jump instruction on the original function
        Firefox.Place_Hook();
        return ret;
}
*/