;org 100h for com files
org 100h

call get_rand
mov bl, al; the random number selected
mov bh, 0
; call print_num
; call print_CRLF

xor cx, cx; try counter
call print_opening_msg
LOOP_START:
	call print_gess_msg
	call readNum
	inc cx
	cmp al, bl
	jz WIN
	cmp bl, al
	jb HIGH_NUM
	LOW_NUM:
		call print_low_msg
		JMP END_OF_COND_1
	HIGH_NUM:
		call print_high_msg
		JMP END_OF_COND_1
	END_OF_COND_1:
	JMP LOOP_START
WIN:
mov ax, cx
call print_victory

;exit in an orderly fashion
mov al, 0
mov ah, 0x4C
int 0x21

readNum:
	push bx
	call readKey
	sub al, '0'
	mov bl, 10
	mul bl
	mov bl, al
	call readKey
	sub al, '0'
	add al, bl
	pop bx
	ret

readKey:
	mov ah, 0x01
	int 0x21
	ret

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

print_high_msg:
	push dx
	mov dx, highMsg
	call print
	pop dx
	ret

print_low_msg:
	push dx
	mov dx, lowMsg
	call print
	pop dx
	ret

print:
	pusha
	mov ah, 9
	int 0x21
	popa
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
	cmp al, 0
	jz print_num_lbl_2
	call print_digit
	print_num_lbl_2:
	mov al, ah
	mov ah, 0
	mov bl, 10
	div bl
	cmp al, 0
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

print_victory:
	pusha
	mov dx, victoryMsg_partA
	call print
	call print_num
	mov dx, victoryMsg_partB
	call print
	popa
	ret

CRLF db 0x0D, 0x0A, "$"
openingMsg db "I selected a random number between 0 and 99.", 0x0D, 0x0A, "$"
gessMsg db "Please enter your guess: $"
lowMsg db 0x0D, 0x0A, "Your number is lower than mine.", 0x0D, 0x0A, "$"
highMsg db 0x0D, 0x0A, "Your number is higher than mine.", 0x0D, 0x0A, "$"
victoryMsg_partA db 0x0D, 0x0A, "You win! Try count: $"
victoryMsg_partB db ".", 0x0D, 0x0A, "$"
