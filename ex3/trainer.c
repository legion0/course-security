#include <stdio.h>
#include <iostream>
#include <string>
#include <windows.h>

#include <TlHelp32.h>

#include "trainer.h"

#define PROC_NAME "winmine.exe"
#define WIN_NAME "Minesweeper"
#define BASE_ADDRESS 0x1005000
#define STATE_OFFSET 0x0
#define STATE_GAMEOVER 0x10
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

#define MSG_WM_LBUTTONUP 0x0202
#define MSG_WM_LBUTTONDOWN 0x0201
#define MOUSE_BASE_X 19
#define MOUSE_BASE_Y 63
#define MOUSE_SQUARE_SIZE 16

#define EXEPTION_FIND_WINDOW -1
#define EXEPTION_OPEN_PROCESS -2
#define EXEPTION_READ_PROCESS_MEMORY -3
#define EXEPTION_VERY_BAD_STATE -5

#define SYM_UNREVEALED_EMPTY 0x0F
#define SYM_UNREVEALED_EMPTY_CHAR '.'
#define SYM_REVEALED_MINE 0x8A
#define SYM_REVEALED_MINE_CHAR 'M'
#define SYM_UNREVEALED_MINE_QUESTION 0x8D
#define SYM_UNREVEALED_MINE_QUESTION_CHAR '!'
#define SYM_UNREVEALED_MINE_FLAG 0x8E
#define SYM_UNREVEALED_MINE_FLAG_CHAR '$'
#define SYM_UNREVEALED_MINE 0x8F
#define SYM_UNREVEALED_MINE_CHAR '*'
#define SYM_QUESTION_MARK 0x0D
#define SYM_QUESTION_MARK_CHAR '?'
#define SYM_FLAG 0x0E
#define SYM_FLAG_CHAR 'F'
#define SYM_TRIGGERED_MINE 0xCC
#define SYM_TRIGGERED_MINE_CHAR 'D'
#define SYM_WRONG_FLAG 0x0B
#define SYM_WRONG_FLAG_CHAR 'E'
#define SYM_OTHER_CHAR '#'
#define SYM_NUM_0 0x40
#define SYM_NUM_8 0x48

#define ARG_HINT_STR "-v"
#define ARG_CHEAT_STR "-s"
#define USAGE_MSG "Usage: trainer.exe -[v|s]"

#define WAIT_MILIS 50

#define STATE_GAME_CLEAN 0
#define STATE_GAME_PLAYING 1
#define STATE_GAME_OVER 3

int main(int argc, char **argv) {
	int doHint = 0, doCheat = 0;
	if (argc == 1 || argc > 2 || ( strncmp(ARG_HINT_STR, argv[1], 3) != 0 && strncmp(ARG_CHEAT_STR, argv[1], 3) != 0)) {
		die(USAGE_MSG);
	}
	if (strncmp(ARG_HINT_STR, argv[1], 3) == 0) {
		doHint = 1;
	}
	if (strncmp(ARG_CHEAT_STR, argv[1], 3) == 0) {
		doCheat = 1;
	}
	//get the window handle
	HWND windowHandle = FindWindow(NULL, WIN_NAME);
	if (windowHandle == NULL) {
		std::string msg = "ERROR: Cannot Find ";
		msg += WIN_NAME;
		msg += " window.";
		die((char*)msg.c_str());
	}
	MineMap* mapPtr = NULL;
	try {
		mapPtr = new MineMap();
	} catch (int) {
		die("ERROR: Failed to init.");
	}
	//print the map
	if (doHint) {
		for (int j = 0; j < mapPtr->height(); j++) {
			for (int i = 0; i < mapPtr->width(); i++) {
				printf("%c ", byteToPrettyChar(mapPtr->getByte(i,j)));
			}
			std::cout << std::endl;
		}
	}
	//solve the board
	if (doCheat) {
		int solved = 0, clicked;
		if (mapPtr->state() == STATE_GAME_CLEAN) {
			clickOrDie(windowHandle, mapPtr->width()-1, mapPtr->height()-1);
		}
		while(!solved) {
			clicked = 0;
			for (int i = mapPtr->width()-1; i >= 0; i--) {
				for (int j = mapPtr->height()-1; j >= 0; j--) {
					if (SYM_UNREVEALED_EMPTY == mapPtr->getByte(i,j)) {
						clickOrDie(windowHandle, i, j);
						Sleep(WAIT_MILIS);
						clicked = 1;
						break;
					}
				}
				if (clicked) {
					break;
				}
			}
			if (clicked) {
				delete mapPtr;
				try {
					mapPtr = new MineMap();
				} catch (int) {
					die("ERROR: Failed to read board.");
				}
			} else {
				break;
			}
			if (mapPtr != NULL && mapPtr->state() == STATE_GAME_OVER) {
				solved = 1;
			}
		}
	}
	if (mapPtr != NULL) {
		delete mapPtr;
	}
	return 0;
};

//send the click command to the cell at x,y
void clickOrDie(HWND windowHandle, int x, int y) {
	LRESULT retVal = SendMessage(windowHandle, MSG_WM_LBUTTONDOWN,
		0,
		((MOUSE_BASE_Y + y * MOUSE_SQUARE_SIZE) << 16) + (MOUSE_BASE_X + x * MOUSE_SQUARE_SIZE)
	);
	if (retVal != 0) {
		die("Failed to click down :(");
	}
	retVal = SendMessage(windowHandle, MSG_WM_LBUTTONUP,
		0,
		((MOUSE_BASE_Y + y * MOUSE_SQUARE_SIZE) << 16) + (MOUSE_BASE_X + x * MOUSE_SQUARE_SIZE)
	);
	if (retVal != 0) {
		die("Failed to click up :(");
	}
}

//quit the program with message.
void die(char * arg, int retVal) {
	if (retVal != 0) {
		std::cerr << arg << std::endl;
	} else {
		std::cout << arg << std::endl;
	}
	exit(retVal);
};
void die(char * arg) {
	die(arg, -1);
};

//convert the byte from memory to the appropriate printable char
char byteToPrettyChar(byte b) {
	if (SYM_NUM_0 <= b && b <= SYM_NUM_8) {
		return '0'+(b-SYM_NUM_0);
	}
	switch (b) {
		case SYM_UNREVEALED_EMPTY:
			return SYM_UNREVEALED_EMPTY_CHAR;
		case SYM_UNREVEALED_MINE:
			return SYM_UNREVEALED_MINE_CHAR;
		case SYM_UNREVEALED_MINE_FLAG:
			return SYM_UNREVEALED_MINE_FLAG_CHAR;
		case SYM_UNREVEALED_MINE_QUESTION:
			return SYM_UNREVEALED_MINE_QUESTION_CHAR;
		case SYM_QUESTION_MARK:
			return SYM_QUESTION_MARK_CHAR;
		case SYM_FLAG:
			return SYM_FLAG_CHAR;
		case SYM_REVEALED_MINE:
			return SYM_REVEALED_MINE_CHAR;
		case SYM_TRIGGERED_MINE:
			return SYM_TRIGGERED_MINE_CHAR;
		case SYM_WRONG_FLAG:
			return SYM_WRONG_FLAG_CHAR;
	}
	return SYM_OTHER_CHAR;
}
//class that represnts the mine map in the memory.
MineMap::MineMap () {
	_map = NULL;
	HWND windowHandle = FindWindow(NULL, WIN_NAME);
	if (windowHandle == NULL) {
		throw EXEPTION_FIND_WINDOW;
	}
	DWORD processID;
	GetWindowThreadProcessId(windowHandle, &processID); // I Can't Fail :)
	HANDLE phandle = OpenProcess(PROCESS_VM_READ, FALSE, processID);
	if(phandle == NULL) {
		throw EXEPTION_OPEN_PROCESS;
	}
	byte buffer[BUFFER_SIZE];
	SIZE_T bytesRead = 0;
	int retVal = ReadProcessMemory(phandle, (LPCVOID) BASE_ADDRESS, (void*)&buffer, BUFFER_SIZE, &bytesRead);
	if (retVal == 0) {
		CloseHandle(phandle);
		throw EXEPTION_READ_PROCESS_MEMORY;
	}
	//check game state (running/gameover timer on/off)
	if (buffer[STATE_OFFSET] == STATE_RUNNING && buffer[TIMER_STATE_OFFSET] == TIMER_STATE_OFF) {
		_state = STATE_GAME_CLEAN;
	} else if (buffer[STATE_OFFSET] == STATE_RUNNING && buffer[TIMER_STATE_OFFSET] == TIMER_STATE_ON) {
		_state = STATE_GAME_PLAYING;
	} else if (buffer[STATE_OFFSET] == STATE_GAMEOVER && buffer[TIMER_STATE_OFFSET] == TIMER_STATE_OFF) {
		_state = STATE_GAME_OVER;
	} else {
		CloseHandle(phandle);
		throw EXEPTION_VERY_BAD_STATE;
	}
	
	_width = buffer[WIDTH_OFFSET];
	_height = buffer[HEIGHT_OFFSET];
	_count = buffer[MINES_COUNT_OFFSET];
	
	//initiate the matix for our data
	_map = new byte*[_width];
	for (int i = 0; i < _width; i++) {
		_map[i] = new byte[_height];
	}
	//copy memory to the local var
	for (int i = 0; i < _width; i++) {
		for (int j = 0; j < _height; j++) {
			_map[i][j] = buffer[MAP_OFFSET + ROW_MAX_WIDTH*(j+1)+(i+1)]; //adjust for 1 based index windows ppl are !@#$%^&*()_+
		}
	}
	CloseHandle(phandle);
};

int MineMap::width() {
	return _width;
};

int MineMap::height() {
	return _height;
};

byte MineMap::getByte(int x, int y) {
	return _map[x][y];
};

int MineMap::state() {
	return _state;
};

MineMap::~MineMap() {
	if (_map != NULL) {
		for (int i = 0; i < _width; i++) {
			delete[] _map[i];
		}
		delete[] _map;
	}
};
