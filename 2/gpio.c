// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL
// Inicializa as portas J e N
// Prof. Guilherme Peron


#include <stdint.h>

#include "tm4c1294ncpdt.h"

#define GPIO_PORTA  (0x001) //bit 0 - Leds PAT
#define GPIO_PORTF  (0x020) //bit 6 - Leds placa
#define GPIO_PORTH  (0x080) //bit 7 - Motor unipolar (PH0 a PH3)
#define GPIO_PORTJ  (0x0100) //bit 8 - Chave SW1
#define GPIO_PORTK  (0x0200) //bit 9 - Display LCD (PK1 a PK7)
#define GPIO_PORTL  (0x400) //bit 10 - Teclado matricial (PL0 a PL3)
#define GPIO_PORTM  (0x800) //bit 11 - Teclado matricial (PM4 a PM7), Display LCD (PM0 a PM2)
#define GPIO_PORTN  (0x1000) //bit 12 - Leds placa
#define GPIO_PORTP  (0x2000) //bit 13 - Transistor controle Leds PAT
#define GPIO_PORTQ  (0x4000) //bit 14 - Leds PAT
void SysTick_Wait1ms(uint32_t delay);

// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports F, J e N
// Parâmetro de entrada: Não tem
// Parâmetro de saída: Não tem
void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTA | GPIO_PORTF | GPIO_PORTH | GPIO_PORTJ | GPIO_PORTK  | GPIO_PORTL | GPIO_PORTM | GPIO_PORTN | GPIO_PORTP | GPIO_PORTQ);
	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while((SYSCTL_PRGPIO_R & (GPIO_PORTA | GPIO_PORTF | GPIO_PORTH | GPIO_PORTJ | GPIO_PORTK | GPIO_PORTL | GPIO_PORTM | GPIO_PORTN | GPIO_PORTP | GPIO_PORTQ) ) != (GPIO_PORTA | GPIO_PORTF | GPIO_PORTH | GPIO_PORTJ | GPIO_PORTK | GPIO_PORTL | GPIO_PORTM | GPIO_PORTN | GPIO_PORTP | GPIO_PORTQ) ){};
	
	// 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTA_AHB_AMSEL_R = 0x00;
	GPIO_PORTF_AHB_AMSEL_R = 0x00;
	GPIO_PORTH_AHB_AMSEL_R = 0x00;
	GPIO_PORTJ_AHB_AMSEL_R = 0x00;
	GPIO_PORTK_AMSEL_R = 0x00;
	GPIO_PORTL_AMSEL_R = 0x00;
	GPIO_PORTM_AMSEL_R = 0x00;
	GPIO_PORTN_AMSEL_R = 0x00;
	GPIO_PORTP_AMSEL_R = 0x00;
	GPIO_PORTQ_AMSEL_R = 0x00;
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTA_AHB_PCTL_R = 0x00;
	GPIO_PORTF_AHB_PCTL_R = 0x00;
	GPIO_PORTH_AHB_PCTL_R = 0x00;
	GPIO_PORTJ_AHB_PCTL_R = 0x00;
	GPIO_PORTK_PCTL_R = 0x00;
	GPIO_PORTL_PCTL_R = 0x00;
	GPIO_PORTM_PCTL_R = 0x00;
	GPIO_PORTN_PCTL_R = 0x00;
	GPIO_PORTP_PCTL_R = 0x00;
	GPIO_PORTQ_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTA_AHB_DIR_R = 0xF0;
	GPIO_PORTF_AHB_DIR_R = 0x11;
	GPIO_PORTH_AHB_DIR_R = 0x0F;
	GPIO_PORTJ_AHB_DIR_R = 0x00;
	GPIO_PORTK_DIR_R = 0xFF; //BIT0 | BIT1
	GPIO_PORTL_DIR_R = 0x00;
	GPIO_PORTM_DIR_R = 0x07;
	GPIO_PORTN_DIR_R = 0x03; //BIT0 | BIT1
	GPIO_PORTP_DIR_R = 0x20;
	GPIO_PORTQ_DIR_R = 0x0F;
		
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa
	GPIO_PORTA_AHB_AFSEL_R = 0x00;
	GPIO_PORTF_AHB_AFSEL_R = 0x00;
	GPIO_PORTH_AHB_AFSEL_R = 0x00;
	GPIO_PORTJ_AHB_AFSEL_R = 0x00;
	GPIO_PORTK_AFSEL_R = 0x00; 
	GPIO_PORTL_AFSEL_R = 0x00; 
	GPIO_PORTM_AFSEL_R = 0x00;
	GPIO_PORTN_AFSEL_R = 0x00; 
	GPIO_PORTP_AFSEL_R = 0x00;
	GPIO_PORTQ_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTA_AHB_DEN_R = 0xF0;
	GPIO_PORTF_AHB_DEN_R = 0x11;
	GPIO_PORTH_AHB_DEN_R = 0x0F;
	GPIO_PORTJ_AHB_DEN_R = 0x03;   //Bit0 e bit1
	GPIO_PORTK_DEN_R = 0xFF; 		   //Bit0 e bit1
	GPIO_PORTL_DEN_R = 0x0F;
	GPIO_PORTM_DEN_R = 0xF7;
	GPIO_PORTN_DEN_R = 0x03; 		   //Bit0 e bit1
	GPIO_PORTP_DEN_R = 0x20;
	GPIO_PORTQ_DEN_R = 0x0F;
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTJ_AHB_PUR_R = 0x03;   //Bit0 e bit1	
	GPIO_PORTL_PUR_R = 0x0F;
}	

void PortA_Output(uint32_t v) {
    uint32_t valor = v & 0xF0;
	
    GPIO_PORTA_AHB_DATA_R = (GPIO_PORTA_AHB_DATA_R & ~(0xF0u)) | valor; 
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

// PortH_Output = controla o motor
void PortH_Output(uint32_t v) {
    uint32_t valor = (v) & 0x0F;
	
    GPIO_PORTH_AHB_DATA_R = (GPIO_PORTH_AHB_DATA_R & ~(0x0Fu)) | valor; 
}


void SetLCDInstrucao(uint32_t inst){
	uint32_t mascara = GPIO_PORTK_DIR_R & 0xFF;
	uint32_t valor = inst & mascara;

	GPIO_PORTK_DATA_R = (GPIO_PORTK_DATA_R & ~(mascara)) | valor; 
	GPIO_PORTM_DATA_R = ((GPIO_PORTM_DATA_R | 0x04) & ~(0x01));

	SysTick_Wait1ms(1);
	
	GPIO_PORTM_DATA_R = GPIO_PORTM_DATA_R & ~(0x04);
	SysTick_Wait1ms(2);
}

void SetLCDCaracter(uint8_t caracter){
	GPIO_PORTK_DATA_R = caracter;
	GPIO_PORTM_DATA_R = GPIO_PORTM_DATA_R | 0x05;

	SysTick_Wait1ms(1);
	
	GPIO_PORTM_DATA_R = GPIO_PORTM_DATA_R & 0xFB;
	SysTick_Wait1ms(2);
}

void InitLCD (void){
	SetLCDInstrucao(0x38); // Configuracao do LCD, inicia no modo de 2 linhas
	SetLCDInstrucao(0x06); // Habilita autoincremento para a direita no cursor
	SetLCDInstrucao(0x0E); // Configuracao do cursor (habilita display, cursor e nao pisca)
	SetLCDInstrucao(0x01); // Reset no display (limpa e coloca o cursor na home)
}

void ImprimeTexto(uint8_t* texto){
	SetLCDInstrucao(0x01);
	int i = 0;

	while(texto[i] != '\0'){
		SetLCDCaracter(texto[i]);
		i++;
		if(i == 16) // Se i==16 esta na ultima posicao da primeira linha, passa para a primeira posicao da segunda linha
			SetLCDInstrucao(0xC0); // Move o cursor para a linha 2 coluna 1
	}
	return;
}


// -------------------------------------------------------------------------------
// Função PortJ_Input
// Lê os valores de entrada do port J
// Parâmetro de entrada: Não tem
// Parâmetro de saída: o valor da leitura do port
uint32_t PortJ_Input(void)
{
	return GPIO_PORTJ_AHB_DATA_R;
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

void PortP_Output(uint32_t v) {
    uint32_t valor = v & 0x20;

    GPIO_PORTP_DATA_R = (GPIO_PORTP_DATA_R & ~(0x0Fu)) | valor; 
}

void PortQ_Output(uint32_t v) {
    uint32_t valor = v & 0x0F;

    GPIO_PORTQ_DATA_R = (GPIO_PORTQ_DATA_R & ~(0x0Fu)) | valor; 
}

int32_t Teclas_Input(volatile uint32_t *data_in, volatile uint32_t *dir_reg, volatile uint32_t *data_out)
{
		// Valor fica 1 se a coluna nao for pressionada
    uint32_t r;
		// Coluna 1
    *dir_reg = 0x47; // 0x07 = PL0 a PL2 + 0x47 = PM6
    *data_out &= ~0x40; // limpa o bit 6 referente a coluna 1

    SysTick_Wait1ms(1);
    r = *data_in;

    if (r == 0xE) return 3; // 1110
    if (r == 0xD) return 6; // 1101
    if (r == 0xB) return 9; // 1011
    if (r == 0x7) return 11; // 0111

		// Coluna 2
    *dir_reg = 0x17; // 0x10 = PM4 + 0x07 = PL0 a PL2
    *data_out &= ~0x10; // limpa o bit 4 referente a coluna 2

    SysTick_Wait1ms(1);
    r = *data_in;

		if (r == 0xE) return 1;   // 1110
		if (r == 0xD) return 4;   // 1101
		if (r == 0xB) return 7;   // 1011
		if (r == 0x7) return 10;  // 0111
		
		// Coluna 3
    *dir_reg = 0x27; // 0x20 = PM5 + 0x07 = PL0 a PL2
    *data_out &= ~0x20; // limpa o bit 5 referente a coluna 3

    SysTick_Wait1ms(1);
    r = *data_in;

    if (r == 0xE) return 2;   // 1110
		if (r == 0xD) return 5;   // 1101
		if (r == 0xB) return 8;   // 1011
		if (r == 0x7) return 0;   // 0111

		// Nenhuma tecla identificada
    return -1;
}

void IniciaInterrupcao(void) {
	GPIO_PORTJ_AHB_IM_R = 0x0;
	GPIO_PORTJ_AHB_IS_R = 0x0;
	GPIO_PORTJ_AHB_IBE_R = 0x0;
	GPIO_PORTJ_AHB_IEV_R = 0x0;
	GPIO_PORTJ_AHB_ICR_R = 0x1;
	
	NVIC_EN1_R = 0x80000;
	NVIC_PRI12_R = (5<<29);
}
