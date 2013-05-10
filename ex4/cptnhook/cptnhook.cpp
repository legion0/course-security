// cptnhook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "cptnhook.h"


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

LPCWSTR WIN_CLASS = (LPCWSTR)"Notepad";
HWND _windowHandle = NULL;
HWND findWindow() {
	if (NULL != _windowHandle) {
		return _windowHandle;
	}
	_windowHandle = FindWindow(WIN_CLASS, NULL);
	return _windowHandle;
}

LRESULT CALLBACK HookProc ( int code, WPARAM wParam, LPARAM lParam) {
	FILE * f = fopen("c:\\temp.txt", "a");
	fprintf(f, "IN PROC");
	fclose(f);
	if (code < 0) {
		return CallNextHookEx(NULL, code, wParam, lParam);
	}
	int keyCode = wParam;
	int tmp = lParam;
	int repeatCount = tmp & 0xFFFF; tmp <<= 16;	// The repeat count. The value is the number of times the keystroke is repeated as a result of the user's holding down the key.
	int scanCode = tmp & 0xFF; tmp <<= 8;		// The scan code. The value depends on the OEM.
	int extended = tmp & 0x1; tmp <<= 1;		// Indicates whether the key is an extended key, such as a function key or a key on the numeric keypad. The value is 1 if the key is an extended key; otherwise, it is 0.
	int reserved = tmp & 0xF; tmp <<= 4;		// Reserved.
	int context = tmp & 0x1; tmp <<= 1;			// The context code. The value is 1 if the ALT key is down; otherwise, it is 0.
	int previousState = tmp & 0x1; tmp <<= 1;	// The previous key state. The value is 1 if the key is down before the message is sent; it is 0 if the key is up.
	int transition = tmp & 0x1; tmp <<= 1;	// The transition state. The value is 0 if the key is being pressed and 1 if it is being released.
	if (TRUE) {
		FILE * f = fopen("c:\\temp.txt", "a");
		fprintf(f, "something something dark side... %d \n", keyCode);
		fclose(f);
//		if (keyCode == VK_RETURN) {
//			return 1;
//		}
		return 0;
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}
