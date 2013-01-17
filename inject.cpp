#include <cstdio>
#include <windows.h>
#include <tlhelp32.h>
#include <string>


void EnableDebugPriv() {
    HANDLE hToken;
    LUID luid;
    TOKEN_PRIVILEGES tkp;

    OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken );

    LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &luid );

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = luid;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges( hToken, false, &tkp, sizeof( tkp ), NULL, NULL );

    CloseHandle( hToken ); 
}
/*
int main( int, char *[] ) {
    PROCESSENTRY32 entry;
    entry.dwFlags = sizeof( PROCESSENTRY32 );

    HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

    if ( Process32First( snapshot, &entry ) == TRUE ) {
        while ( Process32Next( snapshot, &entry ) == TRUE ) {
			printf("%s", entry.szExeFile);
            if ( stricmp( entry.szExeFile, "target.exe" ) == 0 ) {
                EnableDebugPriv();

                char dirPath[MAX_PATH];
                char fullPath[MAX_PATH];

                GetCurrentDirectory( MAX_PATH, dirPath );

                snprintf( fullPath, MAX_PATH, "%s\\libhook.dll", dirPath );

                HANDLE hProcess = OpenProcess( PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, entry.th32ProcessID );
                LPVOID libAddr = (LPVOID)GetProcAddress( GetModuleHandle( "kernel32.dll" ), "LoadLibraryA" );
                LPVOID llParam = (LPVOID)VirtualAllocEx( hProcess, NULL, strlen( fullPath ), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );

                WriteProcessMemory( hProcess, llParam, fullPath, strlen( fullPath ), NULL );
                CreateRemoteThread( hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)libAddr, llParam, 0, NULL );
                CloseHandle( hProcess );
            }
        }
    }

    CloseHandle( snapshot );

    return 0;
}
*/

void printerr(){
	printf("%d\n", GetLastError());
}

#define NULL_CHECK(var) \
	if (var==NULL) { \
		printf(#var" is null!");\
		return false;\
	}
//We will be writing our own little function called CreateRemoteThreadInject
BOOL CreateRemoteThreadInject(DWORD ID, const char * dll) 
{ 
	printf("dll: %s\n", dll);
	//Declare the handle of the process.
	HANDLE Process;

	//Declare the memory we will be allocating
	LPVOID Memory;

	//Declare LoadLibrary
	LPVOID LoadLibrary; 

	LPDWORD thread;

	//If there's no process ID we return false.
	NULL_CHECK(ID);
	//Open the process with read , write and execute priviledges
	Process = OpenProcess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_VM_WRITE|PROCESS_VM_OPERATION, FALSE, ID); 
	
	NULL_CHECK(Process);

	//Get the address of LoadLibraryA
	LoadLibrary = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA"); 
	NULL_CHECK(LoadLibrary);

	// Allocate space in the process for our DLL 
	Memory = (LPVOID)VirtualAllocEx(Process, NULL, strlen(dll)+1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); 
	NULL_CHECK(Memory);

	// Write the string name of our DLL in the memory allocated 
	if (!WriteProcessMemory(Process, (LPVOID)Memory, dll, strlen(dll)+1, NULL)) {
		printf("WriteProcessMemory failed\n");
	}

	// Load our DLL 
	HANDLE hRemoteThread = CreateRemoteThread(Process, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, (LPVOID)Memory, 0, thread); 

    WaitForSingleObject(hRemoteThread, INFINITE);
    DWORD ThreadTeminationStatus;
    GetExitCodeThread(hRemoteThread, &ThreadTeminationStatus);
	
	//Let the program regain control of itself
	CloseHandle(Process);

	//Terminate The Thread
	TerminateThread(thread , 0);

	//Free the allocated memory
	VirtualFreeEx(Process , (LPVOID)Memory , 0, MEM_RELEASE);

	return true;
}

bool InjectDll(DWORD ProcessID, const char* DllFilePath)
{
    DWORD ThreadTeminationStatus;
    LPVOID VirtualMem;
    HANDLE hProcess, hRemoteThread;
    HMODULE hModule;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
    if (hProcess == NULL)
        return false;

    VirtualMem = VirtualAllocEx (hProcess, NULL, strlen(DllFilePath), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (VirtualMem == NULL)
        return false;

    if (WriteProcessMemory(hProcess, (LPVOID)VirtualMem, DllFilePath, strlen(DllFilePath), NULL) == 0)
    {
        VirtualFreeEx(hProcess, NULL, (size_t)strlen(DllFilePath), MEM_RESERVE|MEM_COMMIT);
        CloseHandle(hProcess); 
        return false;
    }

    hModule = GetModuleHandle("kernel32.dll");
    hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, 
                        (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryA"),
                          (LPVOID)VirtualMem, 0, NULL);

    if (hRemoteThread == NULL)
    {
        FreeLibrary(hModule);
        VirtualFreeEx(hProcess, NULL, (size_t)strlen(DllFilePath), MEM_RESERVE | MEM_COMMIT);
        CloseHandle(hProcess); 
        return false;
    }

    WaitForSingleObject(hRemoteThread, INFINITE);
    GetExitCodeThread(hRemoteThread, &ThreadTeminationStatus);
    FreeLibrary(hModule);

    VirtualFreeEx(hProcess, NULL, (size_t)strlen(DllFilePath), MEM_RESERVE | MEM_COMMIT);
    CloseHandle(hRemoteThread);
    CloseHandle(hProcess); 
    return true;
}

int main(int argc, char** argv){
	std::string str;
	for (int i=1; i<argc; i++)
		str.append(argv[i]);

	STARTUPINFO siStartupInfo;
	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	siStartupInfo.cb = sizeof(siStartupInfo); 
	PROCESS_INFORMATION piProcessInfo;
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	printf("Running %s\n", str.c_str());
	if (!CreateProcess(NULL, (LPSTR)str.c_str(), 0, 0, false, CREATE_DEFAULT_ERROR_MODE, 0, 0, &siStartupInfo, &piProcessInfo )) {
		printerr();
		return 1;
	}
//	EnableDebugPriv();
//	HANDLE hProcess = piProcessInfo.hProcess;

//	Sleep(1000);

	const char* injectdll = "\\\\swczds1\\hauptverwaltung\\edv\\coding\\schliesssystem\\libhook.dll";
//	HMODULE lib = LoadLibrary(injectdll);
/*	typedef BOOL (WINAPI* tDllMain)( HMODULE , DWORD , LPVOID );
	tDllMain p = (tDllMain) GetProcAddress(lib, "DllMain");
	p(0,0,0);
*/
	
	//CreateRemoteThreadInject(piProcessInfo.dwProcessId, injectdll);
	InjectDll(piProcessInfo.dwProcessId, injectdll);
/*	LPVOID libAddr = (LPVOID)GetProcAddress( GetModuleHandle( "kernel32.dll" ), "LoadLibraryA" );
	LPVOID llParam = (LPVOID)VirtualAllocEx( hProcess, NULL, strlen( injectdll ), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
	WriteProcessMemory( hProcess, llParam, injectdll, strlen( injectdll ), NULL );
	CreateRemoteThread( hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)libAddr, llParam, 0, NULL );
	CloseHandle( hProcess );
*/
	return 0;
}

//http://easyhook.codeplex.com/