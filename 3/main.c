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
void UART_Init(void);

void PortF_Output(uint32_t leds);
void PortN_Output(uint32_t leds);



// UART
uint8_t busy = 0;
uint32_t data = 0;
void GetUART(void);
void SetUART(uint8_t valor);
	
int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init(); // inicia as portas
	UART_Init(); // inicia o UART

	SysTick_Wait1ms(1000);
	while (1)
	{
		 	//SetUART('1');
			//busy = 1;
		
			GetUART();
	}
}


void GetUART() {
	if (busy) {
		return;
	}

	if ((UART0_FR_R & 0x10) == 0x10) {
		return;
	}
	
	busy = 0;
	data = UART0_DR_R;
	SetUART(data);
}

void SetUART(uint8_t valor) {
	if(busy) {
		return;
	}
	
	while ((UART0_FR_R & 0x20) == 0x20) {
		continue;
	}
	
	UART0_DR_R = valor;
}


void AcendeLeds(int8_t led)
{
	switch(led) {
		case 1:
			PortN_Output(0x02);
			break;
		case 2:
			PortN_Output(0x01);
			break;
		case 3:
			PortF_Output(0x10);
			break;
		case 4:
			PortF_Output(0x03);
			break;
		case 5:
			PortN_Output(0x0);
			PortF_Output(0x0);
			break;
	}
}


