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
void PortH_Output(uint32_t v);

void PortN_Output(uint32_t leds);
void AcendeSemaforo1(int32_t led);
void AcendeSemaforo2(int32_t led);

void SetLCDInstrucao(uint32_t inst);
void SetLCDCaracter(uint8_t caracter);
void InitLCD(void);
void ImprimeTexto(uint8_t* texto);
int32_t Teclas_Input(volatile uint32_t *data_in, volatile uint32_t *dir_reg, volatile uint32_t *data_out);
void GetTecla(void);

void rotacionaMotor(void);
void passoMotor(void);
		
void motor_horario_completo(void);
void motor_antihorario_completo(void);
void motor_horario_meio(void);
void motor_antihorario_meio(void);
void Motor_Stop_PortH(void);


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


uint32_t sentido = 0;
uint32_t passo = 0;
uint32_t passoAtual = 0;
uint32_t PassoCompleto[4] = { 0x03, 0x06, 0x0C, 0x09 }; //todos os 4 passos do ciclo do passo completo
uint32_t MeioPasso[8] = { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09 }; //todos os 8 passos do ciclo do meio passo
uint32_t limite = 8; // auxilia a funçao de passo
uint32_t contPassos = 0;


uint8_t Flag = 0;

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	InitLCD();
	
		
	cofre.estado = ABERTO;	
	
	while (1)
	{
		/*
		while(1 > 0){
					rotacionaMotor();
				}*/


		switch(cofre.estado)
		{
			case ABERTO:
				if(Flag == 0)
				{
					ImprimeTexto(ABERTO_MSG);
					Flag = 1;
				}
				GetTecla();
				if (flag_ok ==1) {
					flag_ok = 0;
				}
				break;
				
			case FECHANDO:
				if(Flag == 0)
				{
					ImprimeTexto(FECHANDO_MSG);
					
					// ativa motor
					Flag = 1;
				}
				break;
				
			case FECHADO:
				if(Flag == 0)
				{
					ImprimeTexto(FECHADO_MSG);
					
					// chama GetTecla e verifica se a senha inserida e igual a senha, se for muda de estado para abrindo
					Flag = 1;
				}
				break;
				
			case ABRINDO:
				if(Flag == 0)
				{
					ImprimeTexto(ABRINDO_MSG);
					// ativa motor e depois muda para o estado ABERTO
					Flag = 1;
				}
				break;
				
			case TRAVADO:
				if(Flag == 0)
				{
					ImprimeTexto(TRAVADO_MSG);
					// muda no gettecla se foi informada 3 senhas incorretas....
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
		else if(idx == 4 && t == 11){ // Senha de 4 digitos inserida e # pressionada muda o estado
					Flag = 0;
					cofre.estado = FECHANDO;
				}
		
		else if (t == 11) {
        flag_ok = 1;
    }
		
    ImprimeTexto(senha);	
}


void rotacionaMotor(void){
	SysTick_Wait1ms(2);
	passoMotor();
}

void passoMotor(void) {
	
	if(passo == 2){
		PortH_Output(MeioPasso[passoAtual]);
		limite = 8;
	}
	else{
		PortH_Output(PassoCompleto[passoAtual]);
		limite = 4;
	}
	if (sentido == 1) {
		++passoAtual;
		if (passoAtual >= limite) {
			passoAtual = 0;
		}
	} else {
		if (passoAtual == 0) {
			passoAtual = (limite - 1);
		} else {
			--passoAtual;
		}
	}
	contPassos++;
}


void motor_antihorario_completo(void)
{
    GPIO_PORTH_AHB_DATA_R = 0x0E;  // ~0x01 (NOT 0001) - Ativa bobina 1
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0D;  // ~0x02 (NOT 0010) - Ativa bobina 2
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0B;  // ~0x04 (NOT 0100) - Ativa bobina 3
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x07;  // ~0x08 (NOT 1000) - Ativa bobina 4
    SysTick_Wait1ms(20);  // Delay

}

void motor_horario_completo(void)
{
    GPIO_PORTH_AHB_DATA_R = 0x07;  // ~0x08 - Ativa bobina 4
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0B;  // ~0x04 - Ativa bobina 3
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0D;  // ~0x02 - Ativa bobina 2
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0E;  // ~0x01 - Ativa bobina 1
    SysTick_Wait1ms(20);  // Delay
}

void motor_antihorario_meio(void)
{
    GPIO_PORTH_AHB_DATA_R = 0x0E;  // ~0x01 - Ativa bobina 1
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0C;  // ~0x03 - Ativa bobinas 1+2
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0D;  // ~0x02 - Ativa bobina 2
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x09;  // ~0x06 - Ativa bobinas 2+3
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0B;  // ~0x04 - Ativa bobina 3
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x03;  // ~0x0C - Ativa bobinas 3+4
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x07;  // ~0x08 - Ativa bobina 4
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0E;  // ~0x01 - Volta para bobina 1
    SysTick_Wait1ms(20);  // Delay
}

void motor_horario_meio(void)
{
    GPIO_PORTH_AHB_DATA_R = 0x0E;  // ~0x01 - Ativa bobina 1
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x07;  // ~0x08 - Ativa bobina 4
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x03;  // ~0x0C - Ativa bobinas 3+4
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0B;  // ~0x04 - Ativa bobina 3
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x09;  // ~0x06 - Ativa bobinas 2+3
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0D;  // ~0x02 - Ativa bobina 2
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0C;  // ~0x03 - Ativa bobinas 1+2
    SysTick_Wait1ms(20);  // Delay
    GPIO_PORTH_AHB_DATA_R = 0x0E;  // ~0x01 - Volta para bobina 1
    SysTick_Wait1ms(20);  // Delay
}







void AcendeSemaforo1(int32_t led)
{
		PortN_Output(led);
}

void AcendeSemaforo2(int32_t led)
{
		PortF_Output(led);
}
