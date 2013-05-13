// cptnhook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "cptnhook.h"
#define MAX_SIZE 1024

// This is an example of an exported variable
CPTNHOOK_API int ncptnhook=0;

// This is an example of an exported function.
int fncptnhook(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see cptnhook.h for the class definition
Ccptnhook::Ccptnhook()
{
	return;
}

LPCWSTR WIN_CLASS = (LPCWSTR)"Untitled - Notepad";
HWND _windowHandle = NULL;

char* cheatCode = "bazinga";

BOOL cheatActive = FALSE;
int cheatLen = strlen(cheatCode);
int cheatIndex = 0; //where we are on the bazingaX
int cheatStreamIndex = -1; //X of bazingaX

char cheatStream[MAX_SIZE];
size_t streamLen = -1;
int streamIndex = 0;

char* notepadPath = "C:\\windows\\system32\\notepad.exe:%d";

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
	f = fopen("c:\\temp.txt", "a");
	fprintf(f, "handle: %x \n", handle);
	fclose(f);
	if(NULL != handle) {
		PostMessage(handle,WM_CHAR,c,0);
	}
}

BOOL checkCheat(char c) {
	BOOL ret = FALSE;
	if (cheatIndex == cheatLen && (c >='0' || c <='9')) {
		for (int i = 0; i < cheatLen+1; i++) {
			sendKeystroke(VK_BACK);
		}
		cheatStreamIndex = c - '0';
		cheatIndex = 0;
		ret = TRUE;
	}
	else if ( cheatIndex < cheatLen && cheatCode[cheatIndex] == tolower(c) ) {
		cheatIndex++;
		ret = FALSE;
	} else {
		cheatIndex = 0;
		ret = FALSE;
	}
	f = fopen("c:\\temp.txt", "a");
	fprintf(f, "got %c : %d\n", c,cheatIndex);
	fclose(f);
	return ret;
}

int bsCount = 0;
bool shouldReplace(char c) {
	if (0 == streamIndex && '\b' == c && bsCount < cheatLen + 1) {
		bsCount++;
		return FALSE;
	} else {
		bsCount = 0;
	}
	if(cheatActive) {
		cheatActive = streamIndex < streamLen;
	}
	else if (checkCheat(c)) {
		char path[MAX_PATH];
		sprintf(path,notepadPath,cheatStreamIndex);
		OFSTRUCT of;
		HFILE hf = OpenFile(path,&of,OF_READ);
		int er = GetLastError();
		if (HFILE_ERROR != hf) {
			er = ReadFile((HANDLE)hf,cheatStream,MAX_SIZE,(LPDWORD)&streamLen,NULL);
			CloseHandle((HANDLE)hf);
		}
		cheatActive = streamLen > 0;
		FILE* f = fopen("c:\\temp.txt", "a");
		fprintf(f, "in:%s: %s : %d er:%d hf:%d\n", path,cheatStream,streamLen,er,hf);
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
