// cptnhook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "cptnhook.h"
#define MAX_SIZE 1024

LPCWSTR WIN_CLASS = (LPCWSTR)"Untitled - Notepad";
HWND _windowHandle = NULL;

const char const * STREAM_TEMPLATE = "C:\\windows\\system32\\notepad.exe:%d";
const char const * CHEAT_CODE = "bazinga";
const int CHEAT_LENGTH = strlen(CHEAT_CODE);

BOOL cheatActive = FALSE;
char cheatStream[MAX_SIZE];
size_t streamLen = -1;
int streamIndex = 0;

int bsCount = 0; // number of backspaces send after cheat (to delete cheat)

FILE * f;

HWND findWindow() {
	if (NULL != _windowHandle) {
		return _windowHandle;
	}
	_windowHandle = FindWindow(WIN_CLASS, NULL);
	return _windowHandle;
}

LRESULT CALLBACK HookProc ( int code, WPARAM wParam, LPARAM lParam) {
	if (code == HC_ACTION) {
		MSG* msg = (MSG*)lParam;
		if (msg->message == WM_CHAR) {
			char charCode = msg->wParam;
			f = fopen("c:\\temp.txt", "a");
			fprintf(f, "got %c : %d\n", charCode);
			fclose(f);
			if (shouldReplace(charCode)) {
				char newChar = replaceChar(charCode);
				msg->wParam = (WPARAM)newChar;
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
	} else if ( cheatPos < CHEAT_LENGTH && CHEAT_CODE[cheatPos] == tolower(c) ) {
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
	if (consumeBackspace(c)) {
		f = fopen("c:\\temp.txt", "a");
		fprintf(f, "bs\n");
		fclose(f);
		return FALSE;
	}
	int streamNumber = -1;
	if(cheatActive) {
		cheatActive = streamIndex < streamLen;
	} else if ((streamNumber = checkCheat(c)) != -1) {
		char path[MAX_PATH];
		sprintf(path, STREAM_TEMPLATE, streamNumber);
		OFSTRUCT of;
		HFILE hf = OpenFile(path,&of,OF_READ);
		int er = GetLastError();
		streamLen = 0;
		if (HFILE_ERROR != hf) {
			er = ReadFile((HANDLE)hf,cheatStream,MAX_SIZE,(LPDWORD)&streamLen,NULL);
			CloseHandle((HANDLE)hf);
		}
		cheatActive = streamLen > 0;
		streamIndex = 0;
		for (int i = 0; i < CHEAT_LENGTH+1; i++) {
			sendKeystroke(VK_BACK);
		}
		f = fopen("c:\\temp.txt", "a");
		fprintf(f, "in:%s: %s : %d/%d er:%d hf:%d\n", path,cheatStream,streamIndex, streamLen,er,hf);
		fclose(f);
		return FALSE;
	} else {
		cheatActive = FALSE;
	}
	return cheatActive;
}

char replaceChar(char c) {
	return streamIndex < streamLen ? cheatStream[streamIndex++] : c;
}
