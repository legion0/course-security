// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "cptnhook.h"

#define NOTEPAD_PROC_NAME "notepad.exe"
#define LOADER_PROC_NAME "explorer.exe"
#define INJECTOR_PROC_NAME "InjectMe.exe"

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

BOOL isNotepad() {
	TCHAR buffer[MAX_PATH];
	char cbuffer[MAX_PATH];
	if (GetModuleFileName(NULL, buffer, MAX_PATH)) {
		int i = 0;
		for (i =0; i< MAX_PATH; i++) {
			cbuffer[i] = (char)buffer[i];
		}
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
		return str_ends_with(cbuffer, LOADER_PROC_NAME);
	}
	return FALSE;
}

BOOL isInject(){
	TCHAR buffer[MAX_PATH];
	char cbuffer[MAX_PATH];
	if (GetModuleFileName(NULL, buffer, MAX_PATH)) {
		int i = 0;
		for (i =0; i< MAX_PATH; i++) {
			cbuffer[i] = (char)buffer[i];
		}
		return str_ends_with(cbuffer, INJECTOR_PROC_NAME);
	}
	return FALSE;
}
HMODULE _hModule = NULL;
HHOOK _hookHandle = NULL;

HHOOK install(HMODULE hModule) {
	if (hModule == NULL) {
		return NULL;
	}
	_hookHandle = SetWindowsHookEx(WH_GETMESSAGE, HookProc, hModule, 0);
	return _hookHandle;
}

BOOL uninstall(HHOOK hookHandle) {
	if (NULL == hookHandle) {
		return FALSE;
	}
	return UnhookWindowsHookEx(hookHandle);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 ) {

	FILE* f;
	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "Hello World\n");
	fclose(f);

	(void)lpReserved;
	_hModule = hModule;
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		if (!(isNotepad() || isLoader() || isInject())) {
			return TRUE;//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	fopen_s(&f, "c:\\temp.txt", "a");
	fprintf(f, "Loaded\n");
	fclose(f);

	return TRUE;
}

/*
BOOL APIENTRY DllMain( HMODULE hModule,
		       DWORD  ul_reason_for_call,
		       LPVOID lpReserved
					 ) {
	if(ul_reason_for_call == DLL_PROCESS_ATTACH) {
		if (isNotepad()) {
			char dllName[MAX_PATH];
			GetModuleFileName(hModule, (LPWCH)dllName, MAX_PATH);
			LoadLibrary((LPCWSTR)dllName);
			uninstall(_hookHandle);
			return TRUE;
		} else if (isLoader()) {
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}
*/
