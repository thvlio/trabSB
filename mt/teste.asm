L1: EQU 1
L2: EQU 0

SECTION TEXT
MUL_N:
	MACRO
	MULT N
	STORE N
	END
DIV_N:
	MACRO
	DIV N
	STORE N
	END
IF L1
LOAD SPACE
IF L2
INPUT SPACE
IF L1
MUL_N
IF L2
DIV_N

SECTION DATA
N: SPACE

;arquivo de saida depois de passar as macros:
;SECTION TEXT
;LOAD SPACE
;MULT N
;STORE N
;SECTION DATA
;N: SPACE