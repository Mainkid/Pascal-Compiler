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
r real8 ?
flag dd FALSE
const1 dd 155
const2 dd 35

	.code
				start:
			   fild const1
fistp a
fild const2
fistp b
jmp L1
L2:
fild a
fild b
fcompp
fstsw ax
sahf
jae L3
fild a
fild b
fsub 
fistp a
jmp L4
L3:
fild b
fild a
fsub 
fistp b
L4:
L1:
fild a
fild b
fcompp
fstsw ax
sahf
jne L2
invoke crt_printf, addr FormatInteger, a


	invoke crt__exit, 0
end start
			   
