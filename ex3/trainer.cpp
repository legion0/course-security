#include <stdio.h>
#include <iostream>
#include <string>
#include <windows.h>

#include <TlHelp32.h>

#define PROC_NAME "winmine.exe"
#define WIN_NAME "Minesweeper"
#define BASE_ADDRESS 0x1005000
#define STATE_OFFSET 0x0
#define STATE_GAMEOVER 10
#define STATE_RUNNING 1
#define TIMER_STATE_OFFSET 0x164
#define TIMER_STATE_OFF 0
#define TIMER_STATE_ON 1

#define MINES_COUNT_OFFSET 0x330
#define WIDTH_OFFSET 0x334
#define HEIGHT_OFFSET 0x338
#define MAP_OFFSET 0x340

#define MINE_STATE_OK 0
#define MINE_STATE_BAD -1

#define ROW_MAX_WIDTH 0x20

#define BUFFER_SIZE 0x680

char byteToPrettyChar(byte);

class MineMap {
	private:
		int _width, _height, _count;
		byte ** _map;
	public:
		MineMap();
		byte getByte(int, int);
		int width();
		int height();
		~MineMap() {
			if (_map != NULL) {
				for (int i = 0; i < _width; i++) {
					delete _map[i];
				}
				delete _map;
			}
		};
};

int main(int argc, char **argv) {
	MineMap map;
	for (int i = 0; i < map.width(); i++) {
		for (int j = 0; j < map.height(); j++) {
			printf("%c ", byteToPrettyChar(map.getByte(i,j)));
		}
		printf("\n");
	}
};

void die(char * arg, int retVal) {
	printf(arg);
	exit(retVal);
};
void die(char * arg) {
	die(arg, -1);
};

byte MineMap::getByte(int x, int y) {
	return _map[x][y];
}

int MineMap::width() {
	return _width;
}

int MineMap::height() {
	return _height;
}

char byteToPrettyChar(byte b) {
	if (0x40 <= b && b <= 0x48) {
		return '0'+(b-0x40);
	}
	switch (b) {
		case 0x0F:
			return '.'; // unrevealed empty
		case 0x8F:
			return '*'; // unrevealed mine
		case 0x0D:
			return '?'; // question mark
		case 0x0E:
			return 'F'; // flag
		case 0x8A:
			return 'M'; // revealed mine
		case 0xCC:
			return 'D'; // triggered mine
		case 0x0B:
			return 'E'; // wrong flag (flag over non mine)
	}
	return '#';
}

MineMap::MineMap () {
	_map = NULL;
	HWND windowHandle = FindWindow(NULL, WIN_NAME);
	if (windowHandle == NULL) {
		die("XXX: FU FindWindow\n");
	}
	DWORD processID;
	GetWindowThreadProcessId(windowHandle, &processID); // I Can't Fail :)
	HANDLE phandle = OpenProcess(PROCESS_VM_READ, FALSE, processID);
	if(phandle == NULL) {
		die("XXX: FU OpenProcess\n");
	}
	byte buffer[BUFFER_SIZE];
	SIZE_T bytesRead = 0;
	int retVal = ReadProcessMemory(phandle, (LPCVOID) BASE_ADDRESS, (void*)&buffer, BUFFER_SIZE, &bytesRead);
	if (retVal == 0) {
		CloseHandle(phandle);
		die("XXX: FU ReadProcessMemory\n");
	}
	// for (int i = 0; i < 10; i++) {
		// printf("%d:\t%u\n", i, buffer[i]);
	// }
	if (!(buffer[STATE_OFFSET] == STATE_RUNNING && buffer[TIMER_STATE_OFFSET] == TIMER_STATE_ON)) {
		CloseHandle(phandle);
		die("XXX: FU BAD STATE\n");
	}
	_width = buffer[WIDTH_OFFSET];
	_height = buffer[HEIGHT_OFFSET];
	_count = buffer[MINES_COUNT_OFFSET];
	_map = new byte*[_width];
	for (int i = 0; i < _width; i++) {
		_map[i] = new byte[_height];
	}
	for (int i = 0; i < _width; i++) {
		for (int j = 0; j < _height; j++) {
			_map[i][j] = buffer[MAP_OFFSET + ROW_MAX_WIDTH*(j+1)+(i+1)]; // windows ppl are !@#$%^&*()_+
		}
	}
	CloseHandle(phandle);
};
