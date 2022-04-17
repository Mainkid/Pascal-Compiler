.386
			   .model flat, C
			   option casemap :none

			   include \masm32\include\msvcrt.inc
			   includelib \masm32\lib\msvcrt.lib
			   
				
	.data
	             FormatInteger db '%d ', 0
	             FormatDecimal db '%f ', 0
				 FALSE equ 0
				 TRUE  equ
			   a dd ?
b dd ?
d dd ?
flag dd FALSE
const1 dd 100
const2 dd 50
const3 dd 10
const4 dd 20
const5 dd 100
const6 dd 5

	.code
				start:
			   fild const1
fistp a
jmp L1
L2:
fild a
fild const3
fsub 
fistp a
invoke crt_printf, addr FormatInteger, a
fild const4
fistp b
jmp L3
L4:
fild b
fild const6
fmul 
fistp b
invoke crt_printf, addr FormatInteger, b
L3:
fild b
fild const5
fcompp
fstsw ax
sahf
je L4
L1:
fild a
fild const2
fcompp
fstsw ax
sahf
jb L2


	invoke crt__exit, 0
end start
			   
