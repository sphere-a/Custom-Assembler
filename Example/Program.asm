ORG FFFEh 						; Set origin to 0xFFFF
:RESET_VECTOR: DW :PROGRAM:		; Set reset vector to 0x7FFF

ORG 9000h 						; Create variable
:VARIABLE: DB FFh 				; Init to 0

ORG 8000h 						; Set origin to 0x7FFF
:PROGRAM:
LDA 00 							; Load 0 to the A register
SAB 							; Switch registers
LDA 01 							; Load 1 to the A register
ADD 							; Add the two registers
sta :VARIABLE:					; End of programme