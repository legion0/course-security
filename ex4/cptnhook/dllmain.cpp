// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#define NOTEPAD_PROC_NAME "notepad.exe"

BOOL isNotepad() {
    TCHAR buffer[MAX_PATH];
    if (GetModuleFileName(NULL, buffer, MAX_PATH)) {
		return NULL != strstr((char*)buffer, NOTEPAD_PROC_NAME);
    }
    return FALSE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 ) {
	FILE * f = fopen("c:\\temp.txt", "a");
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		fprintf(f, "something something dark side process... %d \n", isNotepad());
		break;
	case DLL_THREAD_ATTACH:
		fprintf(f, "something something dark side thread... %d \n", isNotepad());
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	fclose(f);
	return TRUE;
}

