// cptnhook.cpp : Defines the exported functions for the DLL application.
//

#include "cptnhook.h"
#define MAX_SIZE 1024
#define DECODE_STRING_XOR_BYTE 0x11
#define DEBUG_DETECT_SLEEP_DURATION 10 // TODO: this should be 5
#define DEBUG_COUNT_MAX 2 // TODO: this should be 10

HWND _windowHandle = NULL;

char STR_NOTEPAD_EXE_D[] = {0x7f,  0x7e,  0x65,  0x74,  0x61,  0x70,  0x75,  0x3f,  0x74,  0x69,  0x74,  0x2b,  0x34,  0x75,  0x00};
char STR_BAZINGA[] = {0x73,  0x70,  0x6b,  0x78,  0x7f,  0x76,  0x70,  0x00};
char STR_C_TEMP_TXT[] = {0x72,  0x2b,  0x4d,  0x65,  0x74,  0x7c,  0x61,  0x3f,  0x65,  0x69,  0x65,  0x00};
char STR_OLLYDBG_EXE[] = {0x7e,  0x7d,  0x7d,  0x68,  0x75,  0x73,  0x76,  0x3f,  0x74,  0x69,  0x74,  0x00};
char STR_GO_AWAY_DEBUGGER[] = {0x56,  0x7e,  0x31,  0x70,  0x66,  0x70,  0x68,  0x31,  0x75,  0x74,  0x73,  0x64,  0x76,  0x76,  0x74,  0x63,  0x30,  0x00};
char STR_EVIL[] = {0x54,  0x67,  0x78,  0x7d,  0x30,  0x00};
char STR_NOTEPAD_EXE[] = {0x7f,  0x7e,  0x65,  0x74,  0x61,  0x70,  0x75,  0x3f,  0x74,  0x69,  0x74,  0x00};
char STR_EXPLORER_EXE[] = {0x74,  0x69,  0x61,  0x7d,  0x7e,  0x63,  0x74,  0x63,  0x3f,  0x74,  0x69,  0x74,  0x00};
char* _strings[] = {STR_NOTEPAD_EXE_D, STR_BAZINGA, STR_C_TEMP_TXT, STR_OLLYDBG_EXE, STR_GO_AWAY_DEBUGGER, STR_EVIL, STR_NOTEPAD_EXE, STR_EXPLORER_EXE};
BOOL _stringDecoded = FALSE;
const int CHEAT_LENGTH = 7;

BOOL cheatActive = FALSE;
char cheatStream[MAX_SIZE];
int streamLen = -1;
int streamIndex = 0;

int bsCount = 0; // number of backspaces send after cheat (to delete cheat)

HMODULE _hModule = NULL;
HHOOK _hookHandle = NULL;
HANDLE _threadHandle;
DWORD _threadId;
char _processName[MAX_PATH];

 //FILE* f;
//fopen_s(&f, "c:\\temp.txt", "a");
//fprintf(f, "GetProcessImageFileNameA: %s\n", _processName);
//fclose(f);

LRESULT CALLBACK HookProc ( int code, WPARAM wParam, LPARAM lParam) {
	if (code == HC_ACTION && _stricmp(_processName, STR_NOTEPAD_EXE) == 0) {
		MSG* msg = (MSG*)lParam;
		if (msg->message == WM_CHAR) {
			char charCode = (char)msg->wParam;
			if (shouldReplace(charCode)) {
				msg->wParam = (WPARAM)replaceChar(charCode);
			}
		}
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}

void sendKeystroke(char c) {
	HWND handle = GetWindow(GetActiveWindow(),GW_CHILD); //the edit is the first child of the active window.
	if(NULL != handle) {
		PostMessage(handle,WM_CHAR,c,0);
	}
}

BOOL checkCheat(char c) {
	static int cheatPos = 0;
	if (cheatPos == CHEAT_LENGTH && ('0' <= c && c <='9')) {
		cheatPos = 0;
		return c - '0';
	} else if ( cheatPos < CHEAT_LENGTH && STR_BAZINGA[cheatPos] == tolower(c) ) {
		cheatPos++;
	} else {
		cheatPos = 0;
	}
	return -1;
}

BOOL consumeBackspace(char c) {
	static int bsCount = 0;
	if (c == '\b' && streamIndex == 0) {
		bsCount++;
		return bsCount <= CHEAT_LENGTH + 1;
	} else {
		bsCount = 0;
		return FALSE;
	}
}

BOOL shouldReplace(char c) {
	int streamNumber = -1;
	OFSTRUCT of;
	int err = 0, i;
	HFILE hf;
	char path[MAX_PATH];
	if (consumeBackspace(c)) {
		return FALSE;
	}
	if(cheatActive) {
		cheatActive = streamIndex < streamLen;
	} else if ((streamNumber = checkCheat(c)) != -1) {
		sprintf_s(path, MAX_PATH, STR_NOTEPAD_EXE_D, streamNumber);
		hf = OpenFile(path,&of,OF_READ);
		err = GetLastError();
		streamLen = 0;
		if (HFILE_ERROR != hf) {
			err = ReadFile((HANDLE)hf,cheatStream,MAX_SIZE,(LPDWORD)&streamLen,NULL);
			CloseHandle((HANDLE)hf);
		}
		cheatActive = streamLen > 0;
		streamIndex = 0;
		for (i = 0; i < CHEAT_LENGTH+1; i++) {
			sendKeystroke(VK_BACK);
		}
		return FALSE;
	} else {
		cheatActive = FALSE;
	}
	return cheatActive;
}

char replaceChar(char c) {
	return streamIndex < streamLen ? cheatStream[streamIndex++] : c;
}

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
			//fopen_s(&f, "c:\\temp.txt", "a");
			//fprintf(f, "Dr0: 0x%08x, Dr1: 0x%08x, Dr2: 0x%08x, Dr3: 0x%08x, Dr6: 0x%08x, Dr7: 0x%08x\n", context.Dr0, context.Dr1, context.Dr2, context.Dr3, context.Dr6, context.Dr7);
			//fclose(f);
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
		decodeStrings(_strings, sizeof(_strings)/sizeof(char*));
	}
	GetModuleBaseNameA(GetCurrentProcess(), NULL, _processName, MAX_PATH);
	(void)lpReserved;
	_hModule = hModule;
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		if (_stricmp(_processName, STR_NOTEPAD_EXE) == 0) {
			createDebugThread();
		} else if (_stricmp(_processName, STR_EXPLORER_EXE) == 0) {
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

void decodeStrings(char**strings, unsigned int stringsAmount) {
	unsigned int i, j;
	for (i = 0; i < stringsAmount; i++) {
		for (j = 0; j < strlen(strings[i]); j++) {
			strings[i][j] = strings[i][j] ^ DECODE_STRING_XOR_BYTE;
		}
	}
};
