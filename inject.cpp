#include <cstdio>
#include <windows.h>
#include <string>

void printerr(){
	printf("%d\n", GetLastError());
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
	if (argc<=1) {
		printf("Usage: %s program <parameters>", argv[0]);
		printf("Installs a hook to the program, that fakes the System call GetComputerNameA which returns the env var of CLIENTNAME or COMPUTERNAME if set");
		printf("can be used to fake the computername to the clientname on a terminal server");
		return 1;
	}
	for (int i=1; i<argc; i++) {
		if (str.size()!=0)
			str.append(" ");
		str.append(argv[i]);
	}

	STARTUPINFO siStartupInfo;
	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	siStartupInfo.cb = sizeof(siStartupInfo); 
	PROCESS_INFORMATION piProcessInfo;
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	printf("Creating paused process: %s\n", str.c_str());
	if (!CreateProcess(NULL, (LPSTR)str.c_str(), 0, 0, false, CREATE_DEFAULT_ERROR_MODE|CREATE_SUSPENDED, 0, 0, &siStartupInfo, &piProcessInfo )) {
		printerr();
		return 1;
	}
	const char* injectdll = "libhook.dll";
	printf("Injecting hook\n");
	InjectDll(piProcessInfo.dwProcessId, injectdll);
	printf("resuming process\n");
	ResumeThread(piProcessInfo.hThread);
	return 0;
}
