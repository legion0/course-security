// cptnhook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "cptnhook.h"
#define MAX_SIZE 1024

HWND _windowHandle = NULL;

char STR_NOTEPAD_EXE_D[] = {0x7f,  0x7e,  0x65,  0x74,  0x61,  0x70,  0x75,  0x3f,  0x74,  0x69,  0x74,  0x2b,  0x34,  0x75,  0x00};
char STR_BAZINGA[] = {0x73,  0x70,  0x6b,  0x78,  0x7f,  0x76,  0x70,  0x00};
char* _strings2[] = {STR_NOTEPAD_EXE_D, STR_BAZINGA};
const int CHEAT_LENGTH = 7;
BOOL _stringDecoded2 = FALSE;

BOOL cheatActive = FALSE;
char cheatStream[MAX_SIZE];
int streamLen = -1;
int streamIndex = 0;
FILE* f;

int bsCount = 0; // number of backspaces send after cheat (to delete cheat)

LRESULT CALLBACK HookProc ( int code, WPARAM wParam, LPARAM lParam) {
	if (!_stringDecoded2) {
		_stringDecoded2 = TRUE;
		decodeStrings(_strings2, sizeof(_strings2)/sizeof(char*));
	}
	if (code == HC_ACTION && _stricmp(_processName, NOTEPAD_PROC_NAME) == 0) {
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
