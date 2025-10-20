// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

#include <stdint.h>

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

typedef enum estSinal
{
	VERDE,
	AMARELO,
	VERMELHO
} estadosSinal;

typedef struct sinal
{
	estadosSinal estado;
} sinalTipo;


int32_t tempo = 2;
int32_t i = 0;
sinalTipo sinal[1];

int main(void)
{
	sinal[0].estado = VERMELHO;
	sinal[1].estado = VERMELHO;
	
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	
	// LED 1 = 0x2
	// LED 2 = 0x1
	// LED 3 = 0x10
	// LED 4 = 0x3
	
	// LED 1 e 2 = 0x3
	// LED 3 e 4 = 0x11
	
	// LED 1 = VERDE
	// LED 2 = VERMELHO
	// LED 1 e 2 = AMARELO
	
	// LED 3 = VERDE
	// LED 4 = VERMELHO
	// LED 3 e 4 = AMARELO
	
	while (1)
	{
		switch(i)
		{
			case 0:
				tempo = 200;
				sinal[0].estado = VERMELHO;
				sinal[1].estado = VERMELHO;
				break;
			case 1: 
				tempo = 2000;
				sinal[0].estado = VERMELHO;
				sinal[1].estado = VERDE;
				break;
			case 2:
				tempo = 4000;
			sinal[0].estado = VERMELHO;
			sinal[1].estado = AMARELO;
				break;
			case 3:
				tempo = 2000;
			sinal[0].estado = VERMELHO;
			sinal[1].estado = VERMELHO;
				break;
			case 4:
				tempo = 20000;
			sinal[0].estado = VERDE;
			sinal[1].estado = VERMELHO;
				break;
			case 5:
				tempo = 4000;
			sinal[0].estado = AMARELO;
			sinal[1].estado = VERMELHO;
				break;
			default:
				tempo = 2000;
				i = 0;
				sinal[0].estado = VERMELHO;
				sinal[1].estado = VERMELHO;			
		}			
	

		estadosSinal sinal1 = sinal[0].estado;
		switch(sinal1)
		{
			case VERDE:
				AcendeSemaforo1(0x2);
				break;
			
			case AMARELO:
				AcendeSemaforo1(0x3);
			break;
			
			case VERMELHO:
				AcendeSemaforo1(0x1);
			break;
			
			default:
				AcendeSemaforo1(0x00);
		}  

		estadosSinal sinal2 = sinal[1].estado;
		switch(sinal2)
		{
			case VERDE:
				AcendeSemaforo2(0x11);
				break;
			
			case AMARELO:
				AcendeSemaforo2(0x10);
			break;
			
			case VERMELHO:
				AcendeSemaforo2(0x3);
			break;
			
			default:
				AcendeSemaforo2(0x00);
		} 
		SysTick_Wait1ms(tempo);
		i++;
	}
}

void AcendeSemaforo1(int32_t led)
{
		PortN_Output(led);
}

void AcendeSemaforo2(int32_t led)
{
		PortF_Output(led);
}