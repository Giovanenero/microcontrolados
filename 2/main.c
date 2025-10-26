// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

#include <stdint.h>
#include "tm4c1294ncpdt.h"

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);

void PortA_Output(uint32_t v);
void PortF_Output(uint32_t leds);
void PortH_Output(uint32_t v);
uint32_t PortJ_Input(void);
void GPIOPortJ_Handler(void);
void PortN_Output(uint32_t leds);
void PortP_Output(uint32_t v);
void PortQ_Output(uint32_t v);
void IniciaInterrupcao(void);

// LCD
void InitLCD(void);
void SetLCDInstrucao(uint32_t inst);
void SetLCDCaracter(uint8_t caracter);
void ImprimeTexto(uint8_t* texto);

uint8_t ABERTO_MSG[] = "Cofre aberto, digite nova senha";
uint8_t FECHADO_MSG[] = "Cofre fechado";
uint8_t ABRINDO_MSG[] = "Cofre abrindo";
uint8_t FECHANDO_MSG[] = "Cofre fechando.";
uint8_t TRAVADO_MSG[] = "Cofre Travado";
uint8_t SENHA_MESTRA_MSG[] = "Insira a senha mestra";

// Teclado
int32_t Teclas_Input(volatile uint32_t *data_in, volatile uint32_t *dir_reg, volatile uint32_t *data_out);
void SetSenha(void);

// Leds
void PiscaLedsPAT(void);
volatile uint8_t piscaLeds = 0; // 1 = pisca leds, 0 = para de piscar

// Motor
typedef enum {
  DIR_HORARIO = 1,
  DIR_ANTIHORARIO = 0
} sentido_t;

typedef enum {
  MODO_PASSO_COMPLETO = 0,
  MODO_MEIO_PASSO     = 1
} modo_passo_t;

uint32_t PassoCompleto[4] = { 0x03, 0x06, 0x0C, 0x09 };
uint32_t MeioPasso[8] = { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09 };

void Start_motor(sentido_t sentido, modo_passo_t modo);

// Cofre
typedef enum estCofre
{
	ABRINDO,
	ABERTO,
	FECHANDO,
	FECHADO,
	TRAVADO,
	SENHA_MESTRA
} estadosCofre;

typedef struct cofre
{
	estadosCofre estado;
	uint8_t* mensagem;
} cofreTipo;

cofreTipo cofre;

// Senha
uint8_t senha[4 + 1]; // 4 digitos + \0
uint8_t idx = 0;
uint8_t senha_salva[4]; // Senha salva no estado ABERTO

// Verifica senha
void VerificaSenhaInserida(void);
uint8_t tentativas = 0;

// Senha mestra
uint8_t senha_mestra[4] = {0, 0, 0, 0};
void VerificaSenhaMestra(void);

uint8_t Flag = 0;

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init(); // inicia as portas
	IniciaInterrupcao(); // inicia a interrupcao na port J
	InitLCD(); // inicia o display
	PortP_Output(0x20); // inicia o transistor para controlar os leds da PAT
	cofre.estado = ABERTO;	
	
	while (1)
	{
		switch(cofre.estado)
		{
			case ABERTO:
				if(Flag == 0)
				{
					ImprimeTexto(ABERTO_MSG);
					Flag = 1;
				}
				SetSenha();
				break;
				
			case FECHANDO:
				if(Flag == 0)
				{
					ImprimeTexto(FECHANDO_MSG);
					SysTick_Wait1ms(1000);
					
					for(int i=0; i <= 3; i++){
							Start_motor(DIR_ANTIHORARIO, MODO_MEIO_PASSO);
					}
					Flag = 0;
					cofre.estado = FECHADO;
				}
				break;
				
			case FECHADO:
				if(Flag == 0)
				{
					ImprimeTexto(FECHADO_MSG);
					Flag = 1;
				}
				VerificaSenhaInserida();
				break;
				
			case ABRINDO:
				if(Flag == 0)
				{
					ImprimeTexto(ABRINDO_MSG);
					SysTick_Wait1ms(1000);
					
					for(int i=0; i <= 3; i++){
							Start_motor(DIR_HORARIO, MODO_PASSO_COMPLETO);
					}
					Flag = 0;
					cofre.estado = ABERTO;
				}
				break;
				
			case TRAVADO:
				if(Flag == 0)
				{
					ImprimeTexto(TRAVADO_MSG);
					PortP_Output(0x20); // Ativa o transistor para ativar os leds
					piscaLeds = 1;	
					GPIO_PORTJ_AHB_IM_R = 0x1; // Ativa a interrupcao na port J
					
					while (piscaLeds && cofre.estado == TRAVADO)
					{
						PiscaLedsPAT();
					}
					if (cofre.estado == TRAVADO) {
            Flag = 1;
					}
				}
				break;
				
			case SENHA_MESTRA:
				GPIO_PORTJ_AHB_IM_R = 0x0; // desativa a interrupcao na port J
				if(Flag == 0)
				{
					ImprimeTexto(SENHA_MESTRA_MSG);
					Flag = 1;
				}
				VerificaSenhaMestra();
				PortP_Output(0x0);
				break;
		}  	
	}
}


static void SetSenha(void) {
    int32_t tecla = Teclas_Input(&GPIO_PORTL_DATA_R, &GPIO_PORTM_DIR_R, &GPIO_PORTM_DATA_R);
    if (tecla < 0) {
        return; // Nenhuma tecla pressionada
    }

    // Debounce no botao
    while (Teclas_Input(&GPIO_PORTL_DATA_R, &GPIO_PORTM_DIR_R, &GPIO_PORTM_DATA_R) >= 0) {
        SysTick_Wait1ms(10);
    }

    if (tecla >= 0 && tecla <= 9) {
        if (idx < 4) {
            senha[idx++] = (uint8_t)('0' + tecla); // Salva em ASCII para imprimir, '0' = 48
            senha[idx] = '\0';
        } 

				else { // Se inserido mais de 4 caracteres apaga tudo e limpa o display
					idx = 0;
          senha[0] = '\0';
					SetLCDInstrucao(0x01);
        }
    } 
		else if(idx == 4 && tecla == 11){ // Senha de 4 digitos inserida e # pressionada muda o estado
			for (int k = 0; k < 4; k++) { // Converte a senha digitada para int e salva em senha_salva
				uint8_t caracter = senha[k];
				senha_salva[k] = (uint8_t)(caracter - '0'); // Salva como int de 0 a 9, para converter subtrai '0' = 48
			}
			Flag = 0;
			idx = 0;
			senha[0] = '\0'; // limpa a senha para a proxima vez que voltar para este estado
			cofre.estado = FECHANDO;
		}
		
    ImprimeTexto(senha);	
}


void Start_motor(sentido_t sentido, modo_passo_t modo)
{
  const uint32_t limite = (modo == MODO_MEIO_PASSO) ? 8 : 4; // identifica o numero de estados
  const uint32_t total_passos = (modo == MODO_MEIO_PASSO) ? 2048 : 1024;

  uint32_t idx = 0;

  for (uint32_t j = 0; j < total_passos; j++) {

    if (modo == MODO_MEIO_PASSO) {
      PortH_Output(MeioPasso[idx]);
    } 
		else {
      PortH_Output(PassoCompleto[idx]);
    }

    SysTick_Wait1ms(5);

    if (sentido == DIR_ANTIHORARIO) {
      idx++;
      if (idx >= limite) {
				idx = 0;
			}
    } 
		else {
      if (idx == 0) {
				idx = (limite - 1);
			}
      else {
				idx--;
			}
    }
  }
}


void VerificaSenhaInserida(void) {
    int32_t tecla = Teclas_Input(&GPIO_PORTL_DATA_R, &GPIO_PORTM_DIR_R, &GPIO_PORTM_DATA_R);
    if (tecla < 0) {
        return; // Nenhuma tecla pressionada
    }

    // Debounce no botao
    while (Teclas_Input(&GPIO_PORTL_DATA_R, &GPIO_PORTM_DIR_R, &GPIO_PORTM_DATA_R) >= 0) {
        SysTick_Wait1ms(10);
    }

    if (tecla >= 0 && tecla <= 9) {
        if (idx < 4) {
            senha[idx++] = (uint8_t)('0' + tecla);  // Salva em ASCII para imprimir, '0' = 48
            senha[idx] = '\0';
        } 
				else { // Se inserido mais de 4 caracteres apaga tudo e limpa o display
            idx = 0;
            senha[0] = '\0';
						SetLCDInstrucao(0x01);
            return;
        }
        ImprimeTexto(senha);
        return;
    }

		// Verifica a senha se # for pressionado
    if (tecla == 11) {
        if (idx != 4) {
            return;
        }

				// Compara a senha inserida com a senha_salva[]
        for (int k = 0; k < 4; k++) {
            uint8_t digito = (uint8_t)(senha[k] - '0');
            if (digito != senha_salva[k]) {
                tentativas++;
                idx = 0;
                senha[0] = '\0';
                if (tentativas >= 3) {
                    Flag = 0;
                    cofre.estado = TRAVADO;
                }
                return;
            }
        }

				// Senha certa muda de estado
        tentativas = 0;
        Flag = 0;
        cofre.estado = ABRINDO;
        idx = 0;
        senha[0] = '\0';
        return;
    }

    return;
}

void VerificaSenhaMestra(void) {
    int32_t tecla = Teclas_Input(&GPIO_PORTL_DATA_R, &GPIO_PORTM_DIR_R, &GPIO_PORTM_DATA_R);
    if (tecla < 0) {
			return; // Nenhuma tecla pressionada
		}

    // Debounce no botao
    while (Teclas_Input(&GPIO_PORTL_DATA_R, &GPIO_PORTM_DIR_R, &GPIO_PORTM_DATA_R) >= 0) {
        SysTick_Wait1ms(10);
    }

    if (tecla >= 0 && tecla <= 9) {
        if (idx < 4) {
            senha[idx++] = (uint8_t)('0' + tecla);
            senha[idx] = '\0';
        } 
				else {
            idx = 0;
            senha[0] = '\0';
            return;
        }
        ImprimeTexto(senha);
        return;
    }

		// Verifica a senha se # for pressionado
    if (tecla == 11) {
        if (idx != 4) {
            return;
        }

				// Verifica a senha digitada com a senha mestra
        for (int k = 0; k < 4; k++) {
            uint8_t digito = (uint8_t)(senha[k] - '0');
            if (digito != senha_mestra[k]) {
                idx = 0;
                senha[0] = '\0';
                return;
            }
        }

        // Se a senha for igual muda de estado
        Flag = 0;
        cofre.estado = ABRINDO;
        idx = 0;
        senha[0] = '\0';
        return;
    }

    return;
}

void PiscaLedsPAT(void) 
{
		PortA_Output(0xFF); // LEDS 1 A 4 (1111 1111 = 0XFF)
		PortQ_Output(0xF); // LEDS 5 A 8 (1111 = 0XF)
	
		SysTick_Wait1ms(1000);
	
		PortA_Output(0x0);
		PortQ_Output(0x0);
	
		SysTick_Wait1ms(1000);
}

void GPIOPortJ_Handler(void){
	GPIO_PORTJ_AHB_ICR_R = 0x1; // limpa a interrupcao
	Flag = 0;
	piscaLeds = 0;
	PortP_Output(0x00);
  idx = 0;
	senha[0] = '\0';
	cofre.estado = SENHA_MESTRA;
}
