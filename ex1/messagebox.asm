EXTERN _LoadLibraryA@4
EXTERN _GetProcAddress@8
EXTERN _GetLastError@0
EXTERN _printf

SECTION .data
digitFormat db "%d",0x0D,0x0A,0x00

user32dll db "User32.dll",0x00
msgBoxA db "MessageBoxA",0x00
MB_OK dd 0
MB_CAPTION db "Success!",0x00
MB_TEXT db "I know win32 assembly!",0x00

MSG_LOAD_LIB_FAIL db "Failed to load library User32.dll",0x00
MSG_GET_PROC_FAIL db "Failed to find function MessageBoxA", 0x00
MSG_MB_FAIL db "Failed to show MessageBox", 0x00

ERR_LOAD_LIB_FAIL dd 1
ERR_GET_PROC_FAIL dd 2
ERR_MB_FAIL dd 3

SECTION .text
global _main
_main:
	push ebp
	mov ebp, esp

	push dword user32dll
	call _LoadLibraryA@4
	cmp eax, 0
	je FAIL_LIB

	push dword msgBoxA
	push dword eax
	call _GetProcAddress@8
	cmp eax, 0
	je FAIL_PROC

	push dword [MB_OK]
	push dword MB_CAPTION
	push dword MB_TEXT
	push dword 0

	call eax
	cmp eax, 0
	je FAIL_MB

	mov eax, 0;ret value

	EXIT:
	mov esp, ebp
	pop ebp
	ret

FAIL_LIB:
	mov eax, MSG_LOAD_LIB_FAIL
	call print
	mov eax, [ERR_LOAD_LIB_FAIL]
	jmp EXIT
FAIL_PROC:
	mov eax, MSG_GET_PROC_FAIL
	call print
	mov eax, [ERR_GET_PROC_FAIL]
	jmp EXIT

FAIL_MB:
	mov eax, MSG_MB_FAIL
	call print
	call _GetLastError@0

	call printdigt

	mov eax, [ERR_MB_FAIL]
	jmp EXIT

print:
	push dword eax
	call _printf
	add esp, 4
	ret

printdigt:
	push dword eax
	push dword digitFormat
	call _printf
	add esp, 8
	ret
