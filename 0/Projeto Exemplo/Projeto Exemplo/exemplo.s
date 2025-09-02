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
	
	; inicializa as variáveis
	LDR R10, =memInicio
	LDRH R0, [R10]
	MOV R3, #1
	MOV R4, #4					; razao
	LDR	R11, =0x20000608
	STRH R0, [R11], #20			; primeiro termo
	STRH R0, [R11], #20			; primeiro termo auxiliar
	STRH R3, [R11], #20			; tamanho da maior sequencia
	STRH R3, [R11], #20			; tamanho da maior sequencia auxiliar
	STRH R4, [R11], #20			; armazena a razao da maior sequencia
	LDR R0, =memInicio			; ponteiro do inicio da lista
	MOV R1, R0					; ponteiro do proximo elemento da lista
	B inicio

atualizaPilha
	PUSH {R11}
	B inicio
	;B atualizarSeqAux
	
	

inicio
	LDRH R10, [R0]				; conteudo de R10
	ADD R1, R1, #2				; atualiza o ponteiro de R1 
	LDRH R11, [R1]
	CMP R11, #0
	BEQ atualizaPonteiroIncio
	
	
	UDIV R3, R11, R10		
	MLS	R4, R3, R10, R11
	CMP R4, #0					; se R4 == 0, ent é inteiro
	BEQ verificaRazao
	B inicio
	;CMP R4, #4

verificaRazao
	CMP R3, #4
	BEQ atualizaPilha
	B inicio
	
atualizaPonteiroIncio
	ADD R0, R0, #2
	MOV R1, R0
	LDRH R10, [R0]
	CMP R10, #0
	BEQ fimPG
	B inicio

fimPG

	LDR R0, =memFim
	LDR R7, =memInicio
	ADD R7, R7, #4
	SUB R0, R0, #2
	
salvarPG

	POP {R1}
	
	CMP R13, R7
	BEQ acabou
	
	ADD R0, R0, #2
	STRH R1, [R0]
	B salvarPG
	
acabou
	
	
	

	
	

	; R10 = elemento atual (a[i])
    ; R11 = elemento seguinte (a[i+1])
        
    ;LDRH R0, [R11]        ; b = a[i+1]
    ;LDRH R1, [R10]        ; a = a[i]
    ;UDIV R3, R0, R1       ; r = b / a
    ;MLS  R4, R3, R1, R0   ; resto = b - r*a
    ;CMP  R4, #0
    ;BNE proximoDivisor    ; se resto != 0, não é divisor válido

	NOP
    ALIGN                           ; garante que o fim da seção está alinhada 
    END                             ; fim do arquivo
