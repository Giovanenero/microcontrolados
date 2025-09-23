; main.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 24/08/2020
; Este programa espera o usuário apertar a chave USR_SW1.
; Caso o usuário pressione a chave, o LED1 piscará a cada 0,5 segundo.

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
		
; Declarações EQU - Defines
;<NOME>         EQU <VALOR>
; ========================

DIGITO_0    EQU    2_0111111
DIGITO_1	EQU    2_0000110
DIGITO_2    EQU    2_1011011
DIGITO_3	EQU    2_1001111
DIGITO_4    EQU    2_1100110
DIGITO_5	EQU    2_1101101
DIGITO_6    EQU    2_1111101
DIGITO_7	EQU    2_0000111	
DIGITO_8    EQU    2_1111111
DIGITO_9	EQU    2_1101111	
LIMIT_MIM_TEMP_ALVO        EQU 5
LIMIT_MAX_TEMP_ALVO        EQU 50
ADDRESS_ALVO     EQU 0x20000404
ADDRESS_CURRENT     EQU 0x20000400
DISPLAY_DEZENA     EQU 0x20000600
DISPLAY_UNIDADE     EQU 0x20000604


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
		IMPORT  PLL_Init
		IMPORT  SysTick_Init
		IMPORT  SysTick_Wait1ms			
		IMPORT  GPIO_Init
        IMPORT  PortAQ_Output
        IMPORT  PortB_Output	        
		IMPORT  PortJ_Input
        IMPORT  PortN_Output	        
        IMPORT  PortP_Output	


; -------------------------------------------------------------------------------
; Função main()
Start  		
	BL PLL_Init                  ;Chama a subrotina para alterar o clock do microcontrolador para 80MHz
	BL SysTick_Init
	BL GPIO_Init                 ;Chama a subrotina que inicializa os GPIO

	; Temperatura alvo
	MOV R8, #25
	
	LDR R0, =ADDRESS_ALVO
    STR R8, [R0]
	
	MOV R9, #15
	LDR R0, =ADDRESS_CURRENT
    STR R9, [R0]
	
	; Display
	MOV R7, #1
	LDR R0, =DISPLAY_DEZENA
    STR R7, [R0]
	
	MOV R6, #5
	LDR R0, =DISPLAY_UNIDADE
    STR R6, [R0]
	
    ;BL  AtualizaValorDisplay
	
MainLoop
	BL RefreshDisplay
	BL PortJ_Input				 ;Chama a subrotina que lê o estado das chaves e coloca o resultado em R12



Verifica_SW1	
	CMP R12, #2_00000010			 ;Verifica se somente a chave SW1 está pressionada
	BNE Verifica_SW2                 ;Se o teste falhou, volta para o início do laço principal

	BL AcendeLed1
	BL incrementaAlvo
	MOV R0, #500 ; define o tempo de espera
	BL EsperaXms

	
Verifica_SW2	
	CMP R12, #2_00000001			 ;Verifica se somente a chave SW1 está pressionada
	BNE VerificaTemperatura                 ;Se o teste falhou, volta para o início do laço principal

	BL AcendeLed2
	BL decrementaAlvo
		
	MOV R0, #500 ; define o tempo de espera
	BL EsperaXms
	
	                  ;Volta para o laço principal
					  
	
VerificaTemperatura
	MOV R0, #1000 ; define o tempo de espera
	BL EsperaXms

	MOV R7, #0
	MOV R0, #10
	MUL R7, R10, R0
	ADD R7, R7, R11
	
	LDR R0, =ADDRESS_CURRENT
    LDR R1, [R0]

	LDR R0, =ADDRESS_ALVO
    LDR R2, [R0]

	CMP R1, R2
	BGT decrementaTempAtual
	BLT incrementaTempAtual
	BL AcendeTodosLeds
	
	B MainLoop 


incrementaAlvo                                ; ao pressionar o SW1
	BL AcendeLed1
    LDR R0, =ADDRESS_ALVO
    LDR R1, [R0]
    ADD R1, R1, #1
    STR R1, [R0]

    MOV R2, #LIMIT_MAX_TEMP_ALVO             ; a temperatura alvo não pode ser maior que 50
    CMP R1, R2
    BLT MainLoop

    STR R2, [R0]                            ; força a temperatura alvo ficar em 50
    B MainLoop


decrementaAlvo                                 ; ao pressionar o SW1
	BL AcendeLed2
    LDR R0, =ADDRESS_ALVO     
    LDR R1, [R0]
    SUB R1, R1, #1
    STR R1, [R0]

    MOV R2, #LIMIT_MIM_TEMP_ALVO             ; a temperatura alvo não pode ser maior que 5
    CMP R1, R2
    BGT MainLoop

    STR R2, [R0]                            ; força a temperatura alvo ficar em 5
    B MainLoop



incrementaTempAtual                                ; ao pressionar o SW1
	BL AcendeLed1
    LDR R0, =ADDRESS_CURRENT
    LDR R1, [R0]
    ADD R1, R1, #1
    STR R1, [R0]

    MOV R2, #LIMIT_MAX_TEMP_ALVO             ; a temperatura alvo não pode ser maior que 50
    CMP R1, R2
    BLT MainLoop

    STR R2, [R0]                            ; força a temperatura alvo ficar em 50
    B MainLoop


decrementaTempAtual                                 ; ao pressionar o SW1
	BL AcendeLed2
    LDR R0, =ADDRESS_CURRENT     
    LDR R1, [R0]
    SUB R1, R1, #1
    STR R1, [R0]

    MOV R2, #LIMIT_MIM_TEMP_ALVO             ; a temperatura alvo não pode ser maior que 5
    CMP R1, R2
    BGT MainLoop

    STR R2, [R0]                            ; força a temperatura alvo ficar em 5
    B MainLoop


LigaUnidades
    PUSH {LR}
	MOV  R5, #0      ; PB5 = 1, PB4 = 0
    BL   PortP_Output
	
    MOV  R5, #2_100000      ; PB5 = 1, PB4 = 0
    BL   PortB_Output       ; escreve em PB4/PB5
    POP  {LR}
    BX   LR	
	
LigaDezenas
	PUSH {LR}
	
	MOV  R5, #0      ; PB5 = 1, PB4 = 0
    BL   PortP_Output
	
    MOV  R5, #2_10000       ; PB5 = 1, PB4 = 0
    BL   PortB_Output       ; escreve em PB4/PB5
    POP  {LR}
    BX   LR	
	
LigaLeds
	PUSH {LR}
	MOV  R5, #0
	BL   PortB_Output
	
    MOV  R5, #2_100000       ; PB5 = 1, PB4 = 0
    BL   PortP_Output       ; escreve em PB4/PB5
    POP  {LR}
    BX   LR	
	
	
RefreshDisplay
    PUSH {LR}
	
	LDR R0, =ADDRESS_CURRENT     
    LDR R1, [R0]
	
	MOV R0, #10
	UDIV R2, R1, R0
	
	MLS R3, R2, R0, R1
	MOV R5, R11
	
	LDR R0, =DISPLAY_DEZENA
	STR R2, [R0]
	
	LDR R0, =DISPLAY_UNIDADE
	STR R3, [R0]
	
    ; Unidades (PB5) 
	LDR R0, =DISPLAY_UNIDADE
    BL  AtualizaValorDisplay
    BL  LigaUnidades
    MOV R0, #1
    BL  EsperaXms

    ; Dezenas (PB4) 
	LDR R0, =DISPLAY_DEZENA
    BL  AtualizaValorDisplay
    BL  LigaDezenas
    MOV R0, #1
    BL  EsperaXms
	
	; Leds
	LDR R0, =ADDRESS_ALVO
	LDR R4, [R0]
    BL  PortAQ_Output
    BL  LigaLeds
    MOV R0, #1
    BL  EsperaXms

    POP {LR}
    BX  LR	

AtualizaLeds


AtualizaValorDisplay
	LDR R1, [R0]
	
    MOV  R7, #0
    PUSH {LR}

    CMP  R1, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_0
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R1, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_1
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R1, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_2
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R1, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_3
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R1, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_4
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R1, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_5
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R1, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_6
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R1, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_7
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R1, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_8  
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R1, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_9  
        BLEQ  PortAQ_Output

    POP  {LR}
    BX   LR	
	
	
EsperaXms ; usa o R0 como valor de espera
	PUSH {LR}
	BL SysTick_Wait1ms
	POP {LR}
	
	BX LR	
	
	
AcendeLed1
	PUSH {LR}
	MOV R0, #2_00000010          ; Acende o primeiro LED apenas
	BL PortN_Output
	POP {LR}
	
	BX LR
	
AcendeLed2
	PUSH {LR}
	MOV R0, #2_00000001          ; Acende o segundo LED apenas
	BL PortN_Output
	POP {LR}
	
	BX LR
	
AcendeTodosLeds
	PUSH {LR}
	MOV R0, #2_00000011          ; Acende os dois leds
	BL PortN_Output
	POP {LR}
	
	BX LR	


; Funcao para ativar o transistor de controle dos leds da PAT
AtivaTransistorP
	PUSH {LR}
	MOV R0, #10
	BL EsperaXms
	
	BL PortP_Output
	
	MOV R0, #10
	BL EsperaXms
	
	MOV R5, #0
	BL PortP_Output
	
	POP {LR}
	BX LR						 ;return



; -------------------------------------------------------------------------------------------------------------------------
; Fim do Arquivo
; -------------------------------------------------------------------------------------------------------------------------	
    ALIGN                        ;Garante que o fim da seção está alinhada 
    END                          ;Fim do arquivo