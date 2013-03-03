;org 100h for com files/
org 100h
;params for calc_mod
;mov ax, 7
;mov bx, 9
;call calc_mod
;call print_digit

mov ax , 561
call is_carmichael
call print_digit

mov ax , 1105
call is_carmichael
call print_digit

mov ax , 2821
call is_carmichael
call print_digit

mov ax , 8911
call is_carmichael
call print_digit

mov ax , 7
call is_carmichael
call print_digit

mov ax , 10
call is_carmichael
call print_digit

mov ax , 282
call is_carmichael
call print_digit

mov ax , 7531
call is_carmichael
call print_digit

;exit with al as return code.
mov al,0
mov ah,4Ch
int 21h

print_digit:
	pusha
	add ax, '0'
	mov [msg], al
	mov dx,msg
	mov ah,9
	int 21h
	popa
	ret
	msg db ' ',0dh, 0ah,'$'

; is_carmichael
; if (a is prime )
	; return false
; for (i = 1; i <= a; i++)
	; if (i is prime)*
		; if (a % i == 0)
			; if (a/i % i == 0)
				; return false
			; if ((a - 1) % (p-1) != 0)
				; return false
; return true


is_carmichael:
	; cx is the counter and I, dx is the original input.
	pusha
	
	push ax	
	call is_prime
	cmp ax, 1
	pop ax
	
	je is_carmichael_false
	
	mov dx, ax ; set dx = ax. well need ax as function input. and the loop counter

	mov cx, 0
	is_carmichael_loop:
		inc cx
		cmp cx, dx
		je is_carmichael_end ;ret true??
		
		mov ax, cx
		call is_prime
		cmp ax, 0
		je is_carmichael_loop ; if (i is prime)
			
			mov ax , dx
			mov bx , cx
			call calc_mod
			cmp ax, 0
			jne is_carmichael_loop ;if (a % i == 0)
				
				push dx
				mov ax ,dx
				xor dx, dx
				div cx ; AX = DX:AX / CX
				pop dx
				
				mov bx , cx ;possibly redundant
				call calc_mod
				cmp ax, 0
				je is_carmichael_false ; if (a/i % i == 0) ret false
					
					mov ax, dx
					mov bx, cx
					dec ax
					dec bx
					call calc_mod
					cmp ax,0
					jne is_carmichael_false ;if ((a - 1) % (p-1) != 0) ret false
		jmp is_carmichael_loop

	is_carmichael_end:
	popa
	mov ax, 1
	ret
	is_carmichael_false:
	popa
	mov ax , 0
	ret

calc_mod:
	push bx
	calc_mod_loop:
		cmp ax, bx
		jb calc_mod_end
		sub ax, bx
		jmp calc_mod_loop
	calc_mod_end:
	pop bx
	ret 

is_prime:
	push bx
	push cx
	mov bx, ax ; 
	xor cx, cx ; cx = 0, cx := divisor count
	is_prime_loop: ;(for bx = ax ; bx > 0; bx--)
		cmp bx, 0
		je is_prime_end
		push ax ;save original ax
		call calc_mod
		cmp ax , 0
		jne notDivisor
		inc cx 
		notDivisor:
		pop ax; restore ax
		cmp cx , 2
		jg ret_false
		dec bx
		jmp is_prime_loop

		
	is_prime_end:	
		cmp cx , 2
		jne ret_false

		pop cx
		pop bx
		mov ax, 1
	ret
	ret_false:
		pop cx
		pop bx
		mov ax, 0
	ret