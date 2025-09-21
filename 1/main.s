; main.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 24/08/2020
; Este programa espera o usu�rio apertar a chave USR_SW1.
; Caso o usu�rio pressione a chave, o LED1 piscar� a cada 0,5 segundo.

; -------------------------------------------------------------------------------
        THUMB                        ; Instru��es do tipo Thumb-2
; -------------------------------------------------------------------------------
		
; Declara��es EQU - Defines
;<NOME>         EQU <VALOR>
; ========================

; -------------------------------------------------------------------------------
; �rea de Dados - Declara��es de vari�veis
		AREA  DATA, ALIGN=2
		; Se alguma vari�vel for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a vari�vel <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma vari�vel de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posi��o da RAM		

; -------------------------------------------------------------------------------
; �rea de C�digo - Tudo abaixo da diretiva a seguir ser� armazenado na mem�ria de 
;                  c�digo
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma fun��o do arquivo for chamada em outro arquivo	
        EXPORT Start                ; Permite chamar a fun��o Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma fun��o externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; fun��o <func>
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
; Fun��o main()
Start  		
	BL PLL_Init                  ;Chama a subrotina para alterar o clock do microcontrolador para 80MHz
	BL SysTick_Init
	BL GPIO_Init                 ;Chama a subrotina que inicializa os GPIO


	MOV R6, #0 ; valor inicial do display
	
MainLoop
	BL PortJ_Input				 ;Chama a subrotina que l� o estado das chaves e coloca o resultado em R0
	; SW1 PRESSIONADA = 10 == 0x02
	; SW2 PRESSIODADA = 01 = 0x01
	; AMBAS PRESSIONADAS = 00 == 0x00
	; NENHUMA PRESSIONADA = 11 == 0x03
	
Verifica_Nenhuma
	CMP	R0, #2_00000011			 ;Verifica se nenhuma chave est� pressionada
	BNE Verifica_SW1			 ;Se o teste viu que tem pelo menos alguma chave pressionada pula
	BL ApagaLeds
	;MOV R0, #0                   ;N�o acender nenhum LED
	;BL PortN_Output			 	 ;Chamar a fun��o para n�o acender nenhum LED
	B MainLoop					 ;Se o teste viu que nenhuma chave est� pressionada, volta para o la�o principal
	
Verifica_SW1	
	CMP R0, #2_00000010			 ;Verifica se somente a chave SW1 est� pressionada
	BNE MainLoop                 ;Se o teste falhou, volta para o in�cio do la�o principal

	BL AcendeLed1
	
	MOV R0, #1000 ; define o tempo de espera
	BL EsperaXms
	
	B MainLoop                   ;Volta para o la�o principal






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
	MOV R0, #0                   ;N�o acende nenhum LED
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
    ALIGN                        ;Garante que o fim da se��o est� alinhada 
    END                          ;Fim do arquivo
