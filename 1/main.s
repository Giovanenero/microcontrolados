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


	MOV R6, #0 ; valor inicial do display
	
    MOV R5, R6
    BL  AtualizaDisplay
	
	BL LigaDezenas
	
	
MainLoop
	BL PortJ_Input				 ;Chama a subrotina que lê o estado das chaves e coloca o resultado em R12

	MOV R5, R6
	BL  AtualizaDisplay
	
	;MOV R0, #1000
    ;BL  EsperaXms
	
Verifica_SW1	
	CMP R12, #2_00000010			 ;Verifica se somente a chave SW1 está pressionada
	BNE Verifica_SW2                 ;Se o teste falhou, volta para o início do laço principal

	BL AcendeLed1
	BL IncrementaValor
	
	MOV R0, #500 ; define o tempo de espera
	BL EsperaXms

	
Verifica_SW2	
	CMP R12, #2_00000001			 ;Verifica se somente a chave SW1 está pressionada
	BNE MainLoop                 ;Se o teste falhou, volta para o início do laço principal

	BL AcendeLed1
	
	;ADD R6, #1
    ;CMP R6, #10
    ;IT GE
    ;    MOVGE R6, #0
	BL DecrementaValor
		
	MOV R0, #500 ; define o tempo de espera
	BL EsperaXms
	
	B MainLoop                   ;Volta para o laço principal






LigaUnidades
    PUSH {LR}
    MOV  R5, #2_100000      ; PB5 = 1, PB4 = 0
    BL   PortB_Output       ; escreve em PB4/PB5
    POP  {LR}
    BX   LR	
	
LigaDezenas
	PUSH {LR}
    MOV  R5, #2_10000       ; PB5 = 1, PB4 = 0
    BL   PortB_Output       ; escreve em PB4/PB5
    POP  {LR}
    BX   LR	
	
	
IncrementaValor
    ADD R6, #1
    CMP R6, #10
    IT  GE
        MOVGE R6, #0
    BX  LR


DecrementaValor
    SUB R6, #1
    CMP R6, #0
    IT  LT
        MOVLT R6, #9
    BX  LR

AumentaDezena


DiminuiDezena


	
AtualizaDisplay
    MOV  R7, #0
    PUSH {LR}

    CMP  R5, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_0
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R5, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_1
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R5, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_2
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R5, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_3
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R5, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_4
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R5, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_5
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R5, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_6
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R5, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_7
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R5, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_8  
        BLEQ  PortAQ_Output

    ADD  R7, #1
    CMP  R5, R7
    ITT  EQ
        MOVEQ R4, #DIGITO_9  
        BLEQ  PortAQ_Output

    POP  {LR}
    BX   LR	
	
	
	
	
	
	
	
	
	
	
	
Verifica_Nenhuma
	CMP	R0, #2_00000011			 ;Verifica se nenhuma chave está pressionada
	BNE Verifica_SW1			 ;Se o teste viu que tem pelo menos alguma chave pressionada pula
	BL ApagaLeds
	;MOV R0, #0                   ;Não acender nenhum LED
	;BL PortN_Output			 	 ;Chamar a função para não acender nenhum LED
	B MainLoop					 ;Se o teste viu que nenhuma chave está pressionada, volta para o laço principal
	
;Verifica_SW1	
	;CMP R0, #2_00000010			 ;Verifica se somente a chave SW1 está pressionada
	;BNE MainLoop                 ;Se o teste falhou, volta para o início do laço principal

	;BL AcendeLed1
	
	;MOV R0, #1000 ; define o tempo de espera
	;BL EsperaXms
	
	;B MainLoop                   ;Volta para o laço principal






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
	
ApagaLeds
	PUSH {LR}
	MOV R0, #0                   ;Não acende nenhum LED
	BL PortN_Output
	POP {LR}
	
	BX LR

Espera1Segundo
	PUSH {LR}
	MOV R0, #1000                ;Chamar a rotina para esperar 0,5s
	BL SysTick_Wait1ms
	POP {LR}
	
	BX LR

EsperaXms ; usa o R0 como valor de espera
	PUSH {LR}
	BL SysTick_Wait1ms
	POP {LR}
	
	BX LR

; Funcao para ativar o transistor de controle do ativamento dos displays
AtivaTransistorB 
	PUSH {LR}
	MOV R0, #10    ; define o tempo de espera
	BL EsperaXms
	
	BL PortB_Output
	
	MOV R0, #10
	BL EsperaXms
	
	MOV R5, #0
	BL PortB_Output
	
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
