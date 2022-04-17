.386
			   .model flat, C
			   option casemap :none

			   include \masm32\include\msvcrt.inc
			   includelib \masm32\lib\msvcrt.lib
			   
				
	.data
	             FormatInteger db '%d', 0
	             FormatDecimal db '%f', 0
				 FALSE equ 0
				 TRUE  equ
			   a dd ?
b dd ?
d dd ?
flag dd FALSE
const1 dd 3
const2 dd 5

	.code
				start:
			   jmp L1
L2:
fild const1
fild const2
fadd 
fstp b
L1:
fild b
fild a
fcompp
je L2

	invoke crt__exit, 0
end start
			   
