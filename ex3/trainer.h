#ifndef __INCLUDE_TRAINER_H__
#define __INCLUDE_TRAINER_H__

char byteToPrettyChar(byte);
void clickOrDie(HWND, int, int);
void die(char*, int);
void die(char*);

class MineMap {
	private:
		int _width, _height, _count, _state;
		byte ** _map;
	public:
		MineMap();
		int width();
		int height();
		byte getByte(int, int);
		int state();
		~MineMap();
};

#endif
