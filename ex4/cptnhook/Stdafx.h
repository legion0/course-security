// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <cstdio>
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "Psapi")

char _processName[MAX_PATH];
#define NOTEPAD_PROC_NAME "notepad.exe"
#define LOADER_PROC_NAME "explorer.exe"
#define DEBUG_DETECT_SLEEP_DURATION 10 // TODO: this should be 5
#define DEBUG_COUNT_MAX 2 // TODO: this should be 10


// TODO: reference additional headers your program requires here

void decodeStrings();
