BOOL shouldReplace(char c);
char replaceChar(char c);
BOOL checkCheat(char c);
void sendKeystroke(char c);

LRESULT CALLBACK HookProc(int code, WPARAM wParam, LPARAM lParam);
