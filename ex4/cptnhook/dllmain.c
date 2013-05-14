// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "cptnhook.h"

char STR_C_TEMP_TXT[] = {0x72,  0x2b,  0x4d,  0x65,  0x74,  0x7c,  0x61,  0x3f,  0x65,  0x69,  0x65,  0x00};
char STR_OLLYDBG_EXE[] = {0x7e,  0x7d,  0x7d,  0x68,  0x75,  0x73,  0x76,  0x3f,  0x74,  0x69,  0x74,  0x00};
char STR_GO_AWAY_DEBUGGER[] = {0x56,  0x7e,  0x31,  0x70,  0x66,  0x70,  0x68,  0x31,  0x75,  0x74,  0x73,  0x64,  0x76,  0x76,  0x74,  0x63,  0x30,  0x00};
char STR_EVIL[] = {0x54,  0x67,  0x78,  0x7d,  0x30,  0x00};
char* _strings[] = {STR_C_TEMP_TXT, STR_OLLYDBG_EXE, STR_GO_AWAY_DEBUGGER, STR_EVIL};
BOOL _stringDecoded = FALSE;

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

HMODULE _hModule = NULL;
HHOOK _hookHandle = NULL;
HANDLE _threadHandle;
DWORD _threadId;
FILE* f;
	//fopen_s(&f, "c:\\temp.txt", "a");
	//fprintf(f, "GetProcessImageFileNameA: %s\n", _processName);
	//fclose(f);

DWORD startRoutine(LPVOID param) {
	BOOL debuggerPresent;
	BOOL ret, hasDebug, theGreatEscape = FALSE;
	DWORD ollyPID, lastError;
	CONTEXT context;
	int debugCount = 0;

	(void)param;

	while (debugCount < DEBUG_COUNT_MAX) {
		hasDebug = FALSE;
		//fopen_s(&f, "c:\\temp.txt", "a");
		//fprintf(f, "hasDebug: 0x%8x\n", hasDebug);
		//fclose(f);
		
		ret = CheckRemoteDebuggerPresent(GetCurrentProcess(), &debuggerPresent);
		//hasDebug |= ret && debuggerPresent;
		
		ollyPID = findProcessByName(STR_OLLYDBG_EXE);
		//hasDebug |= ollyPID != 0;
		
		lastError = GetLastError();
		OutputDebugStringA(STR_GO_AWAY_DEBUGGER);
		//hasDebug |= GetLastError() == lastError;

		ret = GetThreadContext(GetCurrentThread(), &context);
		if (ret) {
			fopen_s(&f, "c:\\temp.txt", "a");
			fprintf(f, "Dr0: 0x%08x, Dr1: 0x%08x, Dr2: 0x%08x, Dr3: 0x%08x, Dr6: 0x%08x, Dr7: 0x%08x\n", context.Dr0, context.Dr1, context.Dr2, context.Dr3, context.Dr6, context.Dr7);
			fclose(f);
			context.Dr0 = 0;
			context.Dr1 = 0;
			context.Dr2 = 0;
			context.Dr3 = 0;
			context.Dr6 = 0;
			context.Dr7 = 0;
			ret = SetThreadContext(GetCurrentThread(), &context);
		}
		if (hasDebug) {
			debugCount++;
			MessageBoxA(NULL, STR_GO_AWAY_DEBUGGER, STR_EVIL, MB_OK);
		} else {
			debugCount = 0;
		}
		if (debugCount == DEBUG_COUNT_MAX) {
			theGreatEscape = TRUE;
			break;
		}
		Sleep(DEBUG_DETECT_SLEEP_DURATION*1000);
	}
	if (theGreatEscape) {
		ExitProcess(0);
	}
	return 0;
}

void createDebugThread() {
	_threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startRoutine, NULL, 0, &_threadId);
}

BOOL APIENTRY DllMain( HMODULE hModule,
			DWORD  ul_reason_for_call,
			LPVOID lpReserved
		) {
	if (!_stringDecoded) {
		_stringDecoded = TRUE;
		decodeStrings(_strings);
	}
	GetModuleBaseNameA(GetCurrentProcess(), NULL, _processName, MAX_PATH);
	(void)lpReserved;
	_hModule = hModule;
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		if (_stricmp(_processName, NOTEPAD_PROC_NAME) == 0) {
			createDebugThread();
		} else if (_stricmp(_processName, LOADER_PROC_NAME) == 0) {
		} else {
			return FALSE;
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
