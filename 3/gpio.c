// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas J e N
// Prof. Guilherme Peron


#include <stdint.h>

#include "tm4c1294ncpdt.h"

#define GPIO_PORTA  (0x001) // UART
#define GPIO_PORTE  (0x010) // Potenciometro e ADC
#define GPIO_PORTF  (0x020) // 

void SysTick_Wait1ms(uint32_t delay);

// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports F, J e N
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTA | GPIO_PORTE | GPIO_PORTF);
	
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTA | GPIO_PORTE | GPIO_PORTF) ) != (GPIO_PORTA | GPIO_PORTE | GPIO_PORTF) ){};
		
	// 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTE_AHB_AMSEL_R = 0x00;
	GPIO_PORTF_AHB_AMSEL_R = 0x00;
	
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTA_AHB_PCTL_R = 0x11;
	GPIO_PORTE_AHB_PCTL_R = 0x00;
	GPIO_PORTF_AHB_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTA_AHB_DIR_R = 0xF0;
	GPIO_PORTE_AHB_DIR_R = 0x00;
	GPIO_PORTF_AHB_DIR_R = 0x11;

	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa
	GPIO_PORTA_AHB_AFSEL_R = 0x03;
	GPIO_PORTE_AHB_AFSEL_R = 0x00;
	GPIO_PORTF_AHB_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTA_AHB_DEN_R = 0x03;
	GPIO_PORTE_AHB_DEN_R = 0x0F;
	GPIO_PORTF_AHB_DEN_R = 0x11;

	// 7. Habilitar resistor de pull-up interno, setar PUR para 1

}	

void UART_Init(void) {
	uint32_t UART0 = SYSCTL_RCGCUART_R0;
	SYSCTL_RCGCUART_R = UART0;
	while((SYSCTL_RCGCUART_R & UART0) != UART0) {}
	
	UART0_CTL_R = 0x00;
	
	// Bound-rate: 9.600 bit/s
	// BRD = 80.000.000 / 8 * 9.600 =  1041,6666
	UART0_IBRD_R = 1041;
	UART0_FBRD_R = 43; // 64*0,6666

	UART0_LCRH_R = 0x7A; // 0111 1010 => 8 bits como tamanho palavra, habilita filas, 2 stop bit, habilita paridade e define como impar
	UART0_CC_R = 0x00;
	UART0_CTL_R = 0x321; // 0011 0010 0001 => habilita TXE, RXE, HSE (8bits) e UARTEN
}


// -------------------------------------------------------------------------------
// Função PortF_Output
// Escreve os valores no port F
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: não tem
void PortF_Output(uint32_t valor)
{
    uint32_t temp;
    //vamos zerar somente os bits menos significativos
    //para uma escrita amigável nos bits 0 e 1
    temp = GPIO_PORTF_AHB_DATA_R & 0xEE;
    //agora vamos fazer o OR com o valor recebido na função
    temp = temp | valor;
    GPIO_PORTF_AHB_DATA_R = temp; 
}


// -------------------------------------------------------------------------------
// Função PortN_Output
// Escreve os valores no port N
// Parâmetro de entrada: Valor a ser escrito
// Parâmetro de saída: não tem
void PortN_Output(uint32_t valor)
{
    uint32_t temp;
    //vamos zerar somente os bits menos significativos
    //para uma escrita amigável nos bits 0 e 1
    temp = GPIO_PORTN_DATA_R & 0xFC;
    //agora vamos fazer o OR com o valor recebido na função
    temp = temp | valor;
    GPIO_PORTN_DATA_R = temp; 
}
