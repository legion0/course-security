// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CPTNHOOK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CPTNHOOK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CPTNHOOK_EXPORTS
#define CPTNHOOK_API __declspec(dllexport)
#else
#define CPTNHOOK_API __declspec(dllimport)
#endif

// This class is exported from the cptnhook.dll
class CPTNHOOK_API Ccptnhook {
public:
	Ccptnhook(void);
	// TODO: add your methods here.
};

extern CPTNHOOK_API int ncptnhook;

int fncptnhook(void);

LRESULT CALLBACK HookProc ( int code, WPARAM wParam, LPARAM lParam);
