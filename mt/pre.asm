L1: EQU 1
L2: 
    EQU 0
L3: EQU 1
L4:
    equ       l3
SECTION TEXT
IF L1
LOAD SPACE ;faz esta operação se L1 for verdadeiro
IF L2
    INPUT SPACE ;faz esta operação se L2 for verdadeiro
IF L3
ADD L4

SECTION DATA
N: SPACE

;arquivo do saida pre-processado:
;SECTION TEXT
;LOAD SPACE
;ADD 1
;SECTION DATA
;N: SPACE