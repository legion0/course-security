;org 100h for com files
org 100h

call get_rand
; XXX: remove display of number
call print_num
call print_CRLF
call print_num2
call print_CRLF

call print_opening_msg
call print_gess_msg

;exit in an orderly fashion
mov al, 0
mov ah, 4Ch
int 21h

print_opening_msg:
	push dx
	mov dx, openingMsg
	call print
	pop dx
	ret

print_gess_msg:
	push dx
	mov dx, gessMsg
	call print
	pop dx
	ret

print:
	mov ah, 9
	int 21h
	ret

print_digit:
	pusha
	add al, '0'
	mov [print_digit_msg], al
	mov dx,print_digit_msg
	call print
	popa
	ret
	print_digit_msg db ' $'

print_num:
	pusha
	mov ah, 0
	mov bl, 100
	div bl
	call print_digit
	mov al, ah
	mov ah, 0
	mov bl, 10
	div bl
	call print_digit
	mov al, ah
	mov ah, 0
	call print_digit
	popa
	ret

print_num2:
	pusha
	mov ah, 0
	mov bl, 100
	div bl
	jz print_num_lbl_2
	call print_digit
	print_num_lbl_2:
	mov al, ah
	mov ah, 0
	mov bl, 10
	div bl
	jz print_num_lbl_3
	call print_digit
	print_num_lbl_3:
	mov al, ah
	mov ah, 0
	call print_digit
	popa
	ret

print_CRLF:
	pusha
	mov dx, CRLF
	call print
	popa
	ret

get_rand:
	push bx
	push cx
	push dx

	mov ah, 0x2C
	int 0x21
	mov al, dl
	mov ah, 0
	mov bl, 100
	div bl
	mov al, ah

	pop dx
	pop cx
	pop bx
	ret

CRLF db 0x0D, 0x0A, "$"
openingMsg db "I selected a random number between 0 and 99.", 0x0D, 0x0A, "$"
gessMsg db "Please enter your guess: $"
lowMsg db 0x0D, 0x0A, "Your number is lower than mine.", 0x0D, 0x0A, "$"
highMsg db 0x0D, 0x0A, "Your number is higher than mine.", 0x0D, 0x0A, "$"
victoryMsg db 0x0D, 0x0A, "You win! Try count: ???.", 0x0D, 0x0A, "$"
victoryMsgOffset db 20