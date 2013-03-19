org 100h

is_carmichael:
	; cx is the counter and I, dx is the original input.
	pusha
	
	push ax	
	call is_prime
	cmp ax, 1
	pop ax
	
	je is_carmichael_false
	
	mov dx, ax ; set dx = ax. well need ax as function input. and the loop counter

	mov cx, 1
	is_carmichael_loop:
		inc cx
		
		;check only up to a/2
		push dx 	
		mov bx, 2
		mov ax, cx
		mul bx
		jo is_carmichael_end
		pop dx
		cmp ax, dx ;(ax = i*2)
		jg is_carmichael_end
		cmp cx, dx
		je is_carmichael_end ;ret true
		
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
				mov ax, cx
				mul cx
				jo is_carmichael_overflow ;if p^2 overflows, it cant devide a.
				mov bx,ax
				pop dx
				mov ax, dx
				call calc_mod
				cmp ax, 0
				je is_carmichael_false ; if (a % p^2 == 0) ret false
					
					is_carmichael_overflow:
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