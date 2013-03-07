; extern _printf
extern _fopen
extern _fread
extern _fclose
extern _printf

SECTION .data	 	 
readMode db "r", 0x00
filePath: db	"login.txt", 0x00
loginTxt db "00000000", 0x00
passwordz db "13371337", 0x00
passwordzLength dd 8

welcomeMsg db "Welcome!", 0x0D, 0x0A, 0x00
failedOpenMsg db "Cannot open password file", 0x0D, 0x0A, 0x00
failedOpenErr dd 1
failedReadMsg db "Error reading password file", 0x0D, 0x0A, 0x00
failedReadErr dd 2
mismatchMsg db "No match.", 0x0D, 0x0A, 0x00
mismatchErr dd 3

SECTION .text	 	 
global _main	 	 
_main:
	push ebp		 
	mov ebp,esp

	; open file
	push dword readMode
	push dword filePath
	call _fopen
	add esp, 8
	cmp eax, 0
	je FAILED_OPEN
	mov ebx, eax

	; read file
	push dword ebx
	push dword 8
	push dword 1
	push dword loginTxt
	call _fread
	add esp, 16
	cmp eax, [passwordzLength]
	jne FAILED_READ

	; compare password
	mov esi, passwordz
	mov edi, loginTxt
	mov ecx, [passwordzLength]
	rep cmpsb
	jne FAILED_PASSWORD

	; greet user
	mov eax, welcomeMsg
	call print
	mov eax, 0x00000000
	jmp EXIT

	EXIT:
	mov esp, ebp
	pop ebp
	ret

FAILED_PASSWORD:
	mov eax, mismatchMsg
	call print
	mov eax, [mismatchErr]
	jmp EXIT

FAILED_OPEN:
	mov eax, failedOpenMsg
	call print
	mov eax, [failedOpenErr]
	jmp EXIT

FAILED_READ:
	mov eax, failedReadMsg
	call print
	mov eax, [failedReadErr]
	jmp EXIT

print:
	push dword eax
	call _printf
	add esp, 4
	ret
