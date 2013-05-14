#pragma once

#include <windows.h>

#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#pragma comment(lib, "Psapi")

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif