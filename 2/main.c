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
void PortF_Output(uint32_t leds);
uint32_t PortJ_Input(void);
void PortN_Output(uint32_t leds);
void AcendeSemaforo1(int32_t led);
void AcendeSemaforo2(int32_t led);

void SetLCDInstrucao(uint32_t inst);
void SetLCDCaracter(uint8_t caracter);
void InitLCD(void);
void ImprimeTexto(uint8_t* texto);
int32_t Teclas_Input(volatile uint32_t *data_in, volatile uint32_t *dir_reg, volatile uint32_t *data_out);
void GetTecla(void);
	

uint8_t ABERTO_MSG[] = "Cofre aberto, digite nova senha";
uint8_t FECHADO_MSG[] = "Cofre fechado";
uint8_t ABRINDO_MSG[] = "Cofre abrindo";
uint8_t FECHANDO_MSG[] = "Cofre fechando.";
uint8_t TRAVADO_MSG[] = "Cofre Travado";

typedef enum estCofre
{
	ABRINDO,
	ABERTO,
	FECHANDO,
	FECHADO,
	TRAVADO
} estadosCofre;

typedef struct cofre
{
	estadosCofre estado;
	uint8_t* mensagem;
} cofreTipo;


int32_t tempo = 2;
int32_t i = 0;
cofreTipo cofre;

int32_t tecla = -1;
char buf[12];
static uint8_t senha[16 + 1]; // 16 senha (4 caracteres) + \0
static uint8_t idx = 0;
static uint8_t flag_ok = 0;


int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	InitLCD();
	uint8_t Flag = 0;
		
	cofre.estado = ABERTO;	
	
	while (1)
	{
//		switch(i)
//		{
//			case 0:
//				cofre.estado = ABERTO;
//				cofre.mensagem = ABERTO_MSG;
//				
//				break;
//			case 1: 
//				tempo = 2000;
//				
//				break;
//			case 2:
//				tempo = 4000;
//			
//				break;
//			case 3:
//				tempo = 2000;
//			
//				break;
//			case 4:
//				tempo = 20000;
//			
//				break;
//			case 5:
//				tempo = 4000;
//			
//				break;
//			default:
//				tempo = 2000;
//				i = 0;
//				
//		}
/*
		tecla = Teclas_Input(&GPIO_PORTL_DATA_R, &GPIO_PORTM_DIR_R, &GPIO_PORTM_DATA_R);
		if(tecla != -1)
		{
			ImprimeTexto((uint8_t*)buf);
		}*/
		GetTecla();
		if (flag_ok ==1) {
            flag_ok = 0;
        }
				

		switch(cofre.estado)
		{
			case ABERTO:
				if(Flag == 0)
				{
					
					//ImprimeTexto(ABERTO_MSG);
					Flag = 1;
				}
				break;
		}  

		
	}
}


static void GetTecla(void) {
    int32_t t = Teclas_Input(&GPIO_PORTL_DATA_R, &GPIO_PORTM_DIR_R, &GPIO_PORTM_DATA_R);
    if (t < 0) {
        return; // Nenhuma pressionada
    }

    // Debounce no botao
    while (Teclas_Input(&GPIO_PORTL_DATA_R, &GPIO_PORTM_DIR_R, &GPIO_PORTM_DATA_R) >= 0) {
        SysTick_Wait1ms(10);
    }

    if (t >= 0 && t <= 9) {
        if (idx < 4) {
            senha[idx++] = (char)('0' + t);
            senha[idx] = '\0';
        } 
				else {
					idx = 0;
          senha[0] = '\0';
					SetLCDInstrucao(0x01);
        }
    } 
		else if (t == 11) {
        flag_ok = 1;
    }

    ImprimeTexto(senha);
}



void AcendeSemaforo1(int32_t led)
{
		PortN_Output(led);
}

void AcendeSemaforo2(int32_t led)
{
		PortF_Output(led);
}
