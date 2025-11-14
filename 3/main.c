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

// Estados
typedef enum estMotor
{
	INICIAL,
	MODO_CONTROLE,
	MODO_SENTIDO,
	MODO_VELOCIDADE,
	MOTOR_LIGADO,
	CONTROLE_POTENCIOMETRO,
	CONTROLE_TERMINAL,
} estadosMotor;

typedef struct motor
{
	estadosMotor estado;
	uint8_t modo; // 0 = terminal, 1 = potenciometro
	uint8_t sentido; // 0 = horario, 1 = anti-horario
	uint8_t velocidade;
} motorTipo;

motorTipo motor;

// UART
uint8_t busy = 0;
uint32_t data = 0;
uint32_t value = 0;

uint32_t GetUART(void);
void SetUART(uint8_t valor);
void ImprimeFraseUART(uint8_t* texto);
void SetEstado(void);
void ClearUART(void);
uint8_t INICIAL_MSG[] = "Motor parado, pressione * para iniciar";
uint8_t MODO_CONTROLE_MSG[] = "Defina o modo de controle do motor: p = potenciometro, t = terminal";
uint8_t MODO_SENTIDO_MSG[] = "Defina o sentido de rotacao: h = horario, a = anti-horario";
uint8_t MODO_VELOCIDADE_MSG[] = "Defina a velocidade do motor: 5 = 50%, 6 = 60%, 7 = 70%, 8 = 80%, 9 = 90%, 0 = 100%";


uint8_t Flag = 0;

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init(); // inicia as portas
	UART_Init(); // inicia o UART

	SysTick_Wait1ms(1000);
	
	motor.estado = INICIAL;
	while (1)
	{
		switch(motor.estado)
		{
			case INICIAL:
			if(Flag == 0)
				{
					ImprimeFraseUART(INICIAL_MSG);
					Flag = 1;
				}
				SetEstado();
				break;
				
			case MODO_CONTROLE:
				if(Flag == 0)
				{
					ImprimeFraseUART(MODO_CONTROLE_MSG);
					Flag = 1;
				}
				SetEstado();
				break;
			
			case CONTROLE_TERMINAL:
				if(Flag == 0)
				{
					//ImprimeFraseUART();
					Flag = 1;
				}
				SetEstado();
				break;
			
			case MODO_SENTIDO:
				if(Flag == 0)
				{
					ImprimeFraseUART(MODO_SENTIDO_MSG);
					Flag = 1;
				}
				SetEstado();
				break;
			
			case MODO_VELOCIDADE:
				if(Flag == 0)
				{
					ImprimeFraseUART(MODO_VELOCIDADE_MSG);
					Flag = 1;
				}
				SetEstado();
				break;
			
			case CONTROLE_POTENCIOMETRO:
				ImprimeFraseUART(INICIAL_MSG);
				break;
			
			case MOTOR_LIGADO:
				//LigaMotor
				break;
		}
	}
}


uint32_t GetUART() {
	if ((UART0_FR_R & 0x10) == 0x10) {
		return '-';
	}

	data = UART0_DR_R;
	//SetUART(data);
	
	if (data >= '0' && data <= '9') {
		uint32_t numero = data - '0';
		return numero;
	}
	else {
		return data;
	}
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

void ImprimeFraseUART(uint8_t* texto)
{
	ClearUART();
	uint8_t i= 0;
	while(texto[i] != '\0'){
		SetUART(texto[i]);
		i++;
	}
}

void ClearUART(void)
{
	uint8_t i = 0;
	uint8_t clearSeq[] = "\033[2J\033[H"; //  \033[2J = ESC e limpa a tela, \033[2H = move o cursor para o inicio da linha 
	
	while (clearSeq[i] != '\0') {
			SetUART(clearSeq[i]);
			i++;
	}
}

void SetEstado()
{
	value = GetUART();
	if(motor.estado == INICIAL)
	{
		if(value == '*')
		{
			motor.estado = MODO_CONTROLE;
			motor.velocidade = 0;
			Flag = 0;
		}
	}
	
	else if(motor.estado == MODO_CONTROLE)
	{
		if(value == 't') // modo terminal (motor.modo = 0)
		{
			motor.modo = 0;
			motor.velocidade = 0;
			motor.estado = MODO_SENTIDO;
			Flag = 0;
		}
		else if(value == 'p') // modo potenciometro (motor.modo = 1)
		{
			motor.modo = 1;
			motor.velocidade = 0;
			//motor.estado = MODO_SENTIDO;
			Flag = 0;
		}
	}
	
	else if(motor.estado == MODO_SENTIDO)
	{
		if(value == 'h') // sentido horario (motor.sentido = 0)
		{
			motor.sentido = 0;
			motor.velocidade = 0;
			motor.estado = MODO_VELOCIDADE;
			Flag = 0;
		}
		else if(value == 'a') // sentido anti-horario (motor.sentido = 1)
		{
			motor.sentido = 1;
			motor.velocidade = 0;
			motor.estado = MODO_VELOCIDADE;
			Flag = 0;
		}
	}
	
	else if(motor.estado == MODO_VELOCIDADE)
	{
		if(value == '5') // velocidade = 50%
		{
			motor.velocidade = 50;
			motor.estado = MOTOR_LIGADO;
			Flag = 0;
		}
		else if(value == '6') // velocidade = 60%
		{
			motor.velocidade = 60;
			motor.estado = MOTOR_LIGADO;
			Flag = 0;
		}
		else if(value == '7') // velocidade = 70%
		{
			motor.velocidade = 60;
			motor.estado = MOTOR_LIGADO;
			Flag = 0;
		}
		else if(value == '8') // velocidade = 80%
		{
			motor.velocidade = 60;
			motor.estado = MOTOR_LIGADO;
			Flag = 0;
		}
		else if(value == '9') // velocidade = 90%
		{
			motor.velocidade = 60;
			motor.estado = MOTOR_LIGADO;
			Flag = 0;
		}
		else if(value == '0') // velocidade = 100%
		{
			motor.velocidade = 60;
			motor.estado = MOTOR_LIGADO;
			Flag = 0;
		}
	}
	
	else if(motor.estado == MOTOR_LIGADO)
	{
		// Altera o sentido
		if(value == 'h') // sentido horario (motor.sentido = 0)
		{
			motor.sentido = 0;
		}
		else if(value == 'a') // sentido anti-horario (motor.sentido = 1)
		{
			motor.sentido = 1;
		}
		
		//Altera a velocidade
		if(value == '5') // velocidade = 50%
		{
			motor.velocidade = 50;
		}
		else if(value == '6') // velocidade = 60%
		{
			motor.velocidade = 60;
		}
		else if(value == '7') // velocidade = 70%
		{
			motor.velocidade = 60;
		}
		else if(value == '8') // velocidade = 80%
		{
			motor.velocidade = 60;
		}
		else if(value == '9') // velocidade = 90%
		{
			motor.velocidade = 60;
		}
		else if(value == '0') // velocidade = 100%
		{
			motor.velocidade = 60;
		}
		
		//Para o motor
		else if(value == 's')
		{
			motor.velocidade = 0;
			motor.estado = INICIAL;
			Flag = 0;
		}
	}
	
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


