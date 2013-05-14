// stdafx.cpp : source file that includes just the standard includes
// cptnhook.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#define DECODE_STRING_XOR_BYTE 0x11

void decodeStrings(char**strings, unsigned int stringsAmount) {
	unsigned int i, j;
	for (i = 0; i < stringsAmount; i++) {
		for (j = 0; j < strlen(strings[i]); j++) {
			strings[i][j] = strings[i][j] ^ DECODE_STRING_XOR_BYTE;
		}
	}
};