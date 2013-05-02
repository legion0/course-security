// BasicLoader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void _die(char * arg, int retVal) {
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

int _tmain(int argc, _TCHAR* argv[]) {
	HMODULE hMod = LoadLibraryA(DLL_FILE_NAME);
	if (NULL == hMod) {
		_die("Failed to LoadLibraryA !");
	}
	HOOKPROC lpfn = (HOOKPROC)GetProcAddress(hMod, PROC_NAME);
	if (NULL == lpfn) {
		_die("Failed to GetProcAddress !");
	}
	HHOOK hookHandle = SetWindowsHookEx(WH_KEYBOARD, lpfn, hMod, 0);
	if (NULL == hookHandle) {
		_die("Failed to SetWindowsHookEx !");
	}
	int i;
	scanf_s("%d",&i);
	return 0;
}

