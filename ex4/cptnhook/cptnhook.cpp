// cptnhook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "cptnhook.h"
#define MAX_SIZE 1024

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

LPCWSTR WIN_CLASS = (LPCWSTR)"Untitled - Notepad";
HWND _windowHandle = NULL;

char* cheatCode = "BAZINGA";

BOOL cheatActive = FALSE;
int cheatLen = strlen(cheatCode);
int cheatIndex = 0; //where we are on the bazingaX
int cheatStreamIndex = -1; //X of bazingaX

char cheatStream[MAX_SIZE];
size_t streamLen = -1;
int streamIndex = 0;

char* notepadPath = "C:\\windows\\system32\\notepad.exe:%d";

FILE * f;

HWND findWindow() {
	if (NULL != _windowHandle) {
		return _windowHandle;
	}
	_windowHandle = FindWindow(WIN_CLASS, NULL);
	return _windowHandle;
}

LRESULT CALLBACK HookProc ( int code, WPARAM wParam, LPARAM lParam) {

	//f = fopen("c:\\temp.txt", "a");
	//fprintf(f, "IN PROC");
	//fclose(f);
	if (code < 0 ) {
		return CallNextHookEx(NULL, code, wParam, lParam);
	}
	char keyCode = (char)wParam;
	int tmp = lParam;
	int repeatCount = tmp & 0xFFFF; tmp >>= 16;	// The repeat count. The value is the number of times the keystroke is repeated as a result of the user's holding down the key.
	int scanCode = tmp & 0xFF; tmp >>= 8;		// The scan code. The value depends on the OEM.
	int extended = tmp & 0x1; tmp >>= 1;		// Indicates whether the key is an extended key, such as a function key or a key on the numeric keypad. The value is 1 if the key is an extended key; otherwise, it is 0.
	int reserved = tmp & 0xF; tmp >>= 4;		// Reserved.
	int context = tmp & 0x1; tmp >>= 1;			// The context code. The value is 1 if the ALT key is down; otherwise, it is 0.
	int previousState = tmp & 0x1; tmp >>= 1;	// The previous key state. The value is 1 if the key is down before the message is sent; it is 0 if the key is up.
	int transition = tmp & 0x1; tmp >>= 1;	// The transition state. The value is 0 if the key is being pressed and 1 if it is being released.


	//f = fopen("c:\\temp.txt", "a");
	//fprintf(f, "got %c :%d %d %d %d %d %d %d : %d\n", keyCode,repeatCount,scanCode,extended,reserved,context,previousState,transition,lParam);
	//fclose(f);

	if (transition == 0 && shouldReplace(keyCode)) {
		char newC = replaceChar(keyCode);
		
		f = fopen("c:\\temp.txt", "a");
		fprintf(f, "%c -> %c \n", keyCode,newC);
		fclose(f);
		
		sendKeystroke(newC,lParam);
		return 1;
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}

void sendKeystroke(char c,int lp)
{
	HWND handle = GetWindow(GetActiveWindow(),GW_CHILD); //the edit is the first child of the active window.

	f = fopen("c:\\temp.txt", "a");
	fprintf(f, "handle: %x \n", handle);
	fclose(f);
	
	if(NULL != handle)
	{
		//SendMessage(handle,WM_KEYDOWN,c,2949121);
		//SendMessage(handle,WM_KEYUP,c,-1070792703);
		//SendMessage(handle,WM_KEYDOWN,c,1);
		//SendMessage(handle,WM_KEYUP,c,0xC0000001);
		//PostMessage(handle,WM_KEYDOWN,c,0);
		PostMessage(handle,WM_KEYUP,c,0);
	}
	//1337 H4X0Rz
}

BOOL checkCheat(char c)
{

	BOOL ret = FALSE;
	if (cheatIndex == cheatLen && (c >='0' || c <='9')) 
	{
		cheatStreamIndex = c - '0';
		cheatIndex = 0;
		ret = TRUE;
	}
	else if ( cheatIndex < cheatLen && cheatCode[cheatIndex] == c )
	{
		cheatIndex++;
		ret = FALSE;
	}
	else
	{
		cheatIndex = 0;
		ret = FALSE;
	}
	
	f = fopen("c:\\temp.txt", "a");
	fprintf(f, "got %c : %d\n", c,cheatIndex);
	fclose(f);

	return ret;
}

bool shouldReplace(char c)
{



	if(cheatActive)
	{
		cheatActive = streamIndex < streamLen;
	}
	else if (checkCheat(c))
	{
		
		char path[MAX_PATH];
		sprintf(path,notepadPath,cheatStreamIndex);


		//FILE* f = fopen(path,"r");
		//int er = GetLastError();
		//if (NULL != f)
		//{
		//	streamLen = fread(cheatStream,sizeof(char),MAX_SIZE,f);
		//	fclose(f);
		//}
		OFSTRUCT of;
		HFILE hf = OpenFile(path,&of,OF_READ);
		int er = GetLastError();
		if (HFILE_ERROR != hf)
		{
			//streamLen = fread(cheatStream,sizeof(char),MAX_SIZE,f);
			er = ReadFile((HANDLE)hf,cheatStream,MAX_SIZE,(LPDWORD)&streamLen,NULL);
			CloseHandle((HANDLE)hf);
		}
		
		cheatActive = streamLen >0;

		FILE* f = fopen("c:\\temp.txt", "a");
		fprintf(f, "in:%s: %s : %d er:%d hf:%d\n", path,cheatStream,streamLen,er,hf);
		fclose(f);

		return FALSE;
	}
	else
	{
		cheatActive = FALSE;
	}
	return cheatActive;
}

char replaceChar(char c)
{

	return streamIndex < streamLen ? cheatStream[streamIndex++] : c;
}