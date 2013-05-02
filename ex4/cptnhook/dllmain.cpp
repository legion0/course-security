// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#define NOTEPAD_PROC_NAME "notepad.exe"

int str_ends_with(const char * str, const char * suffix) {
	FILE * f = fopen("c:\\temp.txt", "a");


  if( str == NULL || suffix == NULL )
    return 0;

  size_t str_len = strlen(str);
  size_t suffix_len = strlen(suffix);

  	fprintf(f,"long %d:%s\n",str_len,str);

	fprintf(f,"suf %d:%s\n",suffix_len,suffix);
	
	
	fclose(f);

  if(suffix_len > str_len)
    return 0;

  return 0 == strncmp( str + str_len - suffix_len, suffix, suffix_len );
}


BOOL isNotepad() {
    TCHAR buffer[MAX_PATH];
	char cbuffer[MAX_PATH];
    if (GetModuleFileName(NULL, buffer, MAX_PATH)) {
		FILE * f = fopen("c:\\temp.txt", "a");
		int i = 0;
		for (i =0; i< MAX_PATH; i++)
		{
			cbuffer[i] = (char)buffer[i];
		}

		fprintf(f, "%d\n",str_ends_with(cbuffer, NOTEPAD_PROC_NAME));	
		fclose(f);
		return str_ends_with(cbuffer, NOTEPAD_PROC_NAME);
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

