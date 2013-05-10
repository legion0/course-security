// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "cptnhook.h"

#define NOTEPAD_PROC_NAME "notepad.exe"
#define LOADER_PROC_NAME "BasicLoader.exe"

int str_ends_with(const char * str, const char * suffix) {
	if( str == NULL || suffix == NULL )
		return 0;
	size_t str_len = strlen(str);
	size_t suffix_len = strlen(suffix);
	if(suffix_len > str_len)
		return 0;
	return 0 == strncmp( str + str_len - suffix_len, suffix, suffix_len );
}


BOOL isNotepad() {
	TCHAR buffer[MAX_PATH];
	char cbuffer[MAX_PATH];
	if (GetModuleFileName(NULL, buffer, MAX_PATH)) {
		int i = 0;
		for (i =0; i< MAX_PATH; i++) {
			cbuffer[i] = (char)buffer[i];
		}
		FILE * f = fopen("c:\\temp.txt", "a");
		fprintf(f, "%s\n", cbuffer);
		fclose(f);
		return str_ends_with(cbuffer, NOTEPAD_PROC_NAME);
	}
	return FALSE;
}
BOOL isLoader() {
	TCHAR buffer[MAX_PATH];
	char cbuffer[MAX_PATH];
	if (GetModuleFileName(NULL, buffer, MAX_PATH)) {
		int i = 0;
		for (i =0; i< MAX_PATH; i++) {
			cbuffer[i] = (char)buffer[i];
		}
		FILE * f = fopen("c:\\temp.txt", "a");
		fprintf(f, "%s\n", cbuffer);
		fclose(f);
		return str_ends_with(cbuffer, LOADER_PROC_NAME);
	}
	return FALSE;
}

HMODULE _hModule = NULL;
HHOOK _hookHandle = NULL;

HHOOK install() {
	if (NULL == _hModule) {
		return NULL;
	}
	_hookHandle = SetWindowsHookEx(WH_KEYBOARD, HookProc, _hModule, 0);
	return _hookHandle;
}

BOOL uninstall() {
	if (NULL == _hookHandle) {
		return FALSE;
	}
	return UnhookWindowsHookEx(_hookHandle);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 ) {
	_hModule = hModule;
	FILE * f = fopen("c:\\temp.txt", "a");
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		fprintf(f, "DLL_PROCESS_ATTACH\n");
		fprintf(f, "isNotepad = %d\n", isNotepad());
		if (!(isNotepad() || isLoader())) {
			fclose(f);
			return FALSE;
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		fprintf(f, "DLL_THREAD_DETACH\n");
		uninstall();
		break;
	case DLL_PROCESS_DETACH:
		fprintf(f, "DLL_PROC_DETACH\n");
		uninstall();
		break;
	}
	fclose(f);
	return TRUE;
}

