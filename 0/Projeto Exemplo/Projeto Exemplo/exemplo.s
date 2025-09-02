; Exemplo.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 12/03/2018

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declarações EQU - Defines
;<NOME>         EQU <VALOR>
; -------------------------------------------------------------------------------
; Área de Dados - Declarações de variáveis
		AREA  DATA, ALIGN=2
		; Se alguma variável for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a variável <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma variável de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posição da RAM		

; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma função do arquivo for chamada em outro arquivo	
        EXPORT Start                ; Permite chamar a função Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma função externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; função <func>

; -------------------------------------------------------------------------------
; Função main()

memInicio EQU 0x20000400
memFim EQU 0x20000600

Start  
; Comece o código aqui <======================================================

	LDR R10, =memInicio	; ponteiro do loop externo
	LDR R12, =memFim
	LDRH R2, [R12]
	SUB R10, R10, #2
	B loopExterno

atualiza
	LDR R4, [R10]
	LDR R5, [R11]
	STRH R4, [R11]
	STRH R5, [R10]
	B loopInternoNext	
	
loopExterno
	ADD R10, R10, #2
	LDRH R0, [R10]
	CMP R0, R2
	BEQ fim

	ADD R11, R10, #2
	
loopInterno
	LDRH R1, [R11]
	CMP R1, R2
	BEQ loopExterno

	LDRH R0, [R10]
	LDRH R1, [R11]
	CMP R0, R1
	BGT atualiza

loopInternoNext
	ADD R11, R11, #2
	B loopInterno



; ============================================

fim

	LDR R10, =memInicio   ; ponteiro para o início do vetor
    MOV R6, #0            ; maior tamanho de PG encontrado
    LDRH R7, [R10]           ; primeiro elemento da maior PG
    MOV R14, #0           ; razão da maior PG
	
loop_inicio
	LDRH R0, [R10]
    CMP R0, #0
    BGE fimPrograma      ; fim do vetor
    MOV R11, R10          ; ponteiro para o próximo elemento
   
proximoDivisor
	ADD R11, R11, #2
	LDRH R1, [R11]
    CMP R1, #0
	BGE testaPG      ; não há próximo elemento
    B encontraDivisores
	
encontraDivisores
	; R10 = elemento atual (a[i])
    ; R11 = elemento seguinte (a[i+1])
        
    LDRH R0, [R11]        ; b = a[i+1]
    LDRH R1, [R10]        ; a = a[i]
    UDIV R3, R0, R1       ; r = b / a
    MLS  R4, R3, R1, R0   ; resto = b - r*a
    CMP  R4, #0
    BNE proximoDivisor    ; se resto != 0, não é divisor válido

    ; temos razão válida
    PUSH {R3}             ; salva razão na pilha
    B proximoDivisor


testaPG
	LDRH R0, [R10]
	CMP R0, #0
	BEQ fimPrograma
	
	POP {R9}
	
loopTestaPG

	MUL R0, R7, R9		; a2 = a1 * q
	UDIV R1, R1,         ; atual / anterior
    MLS  R4, R9, R1, R0   ; resto
	CMP  R4, #0
	



testaPg
	POP {R3}              ; pega a razão candidata
    MOV R8, #2            ; contador da sequência (2 elementos já considerados)
	MOV R12, R10          ; ponteiro para o início da sequência
	ADD R13, R12, #2      		; próximo elemento a verificar
	
loopPg
	CMP R13, R12          ; verifica limite do vetor
    BGE verificaMaior
	CMP R13, R12
    BGE verificaMaior

	LDRH R0, [R13]        ; atual
    LDRH R1, [R12]        ; anterior
	UDIV R9, R0, R1       ; atual / anterior
    MLS  R4, R9, R1, R0   ; resto
	CMP  R4, #0
	BNE verificaMaior
	CMP  R9, R3            ; quociente == razão?
	BNE verificaMaior

	ADD R8, R8, #1
    ADD R12, R12, #2
	ADD R13, R13, #2
	B loopPg
	
verificaMaior
	CMP R8, R6
    BLE continuaLoop
	MOV R6, R8            ; atualiza tamanho da maior sequência
	LDRH R7, [R10]        ; salva primeiro elemento
	MOV R14, R3           ; salva a razão
	
continua_loop
	ADD R10, R10, #2      ; avança para o próximo elemento do vetor
    CMP R10, R12
	BLT loopInicio
	
fimPrograma

	NOP
    ALIGN                           ; garante que o fim da seção está alinhada 
    END                             ; fim do arquivo
