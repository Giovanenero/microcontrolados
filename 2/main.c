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
static uint8_t senha[4 + 1]; // 16 senha (4 caracteres) + \0
static uint8_t idx = 0;
static uint8_t flag_ok = 0;

// Motor
typedef enum {
  DIR_HORARIO = 1,
  DIR_ANTIHORARIO = 0
} sentido_t;

typedef enum {
  MODO_PASSO_COMPLETO = 0,
  MODO_MEIO_PASSO     = 1
} modo_passo_t;

// 28BYJ-48: 2048 passos/volta em MEIO PASSO
#define STEPS_PER_REV_HALF   2048U
#define STEPS_PER_REV_FULL   (STEPS_PER_REV_HALF / 2U)  // 1024 em passo completo
#define STEP_DELAY_MS        5U 

void start_motor(sentido_t sentido, modo_passo_t modo);


// Insere senha
uint8_t senha_salva[4];
uint8_t idx_salva = 0;

// Verifica senha
int32_t VerificaSenhaInserida(void);

static uint8_t senha_inserida[4 + 1] = {0}; // buffer para o que o usuário digita agora
uint8_t tentativas = 0;              // conta erros consecutivos
static uint8_t idx_inserida = 0;            // tamanho da senha_inserida






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
					SysTick_Wait1ms(1000);
					/*
					for(int i=0; i <= 3; i++){
							start_motor(DIR_ANTIHORARIO, MODO_MEIO_PASSO);
					}*/
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
					/*
					for(int i=0; i <= 3; i++){
							start_motor(DIR_HORARIO, MODO_PASSO_COMPLETO);
					}*/
					Flag = 0;
					cofre.estado = ABERTO;
				}
				break;
				
			case TRAVADO:
				if(Flag == 0)
				{
					ImprimeTexto(TRAVADO_MSG);
					//chama interrupcao e pisca leds
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
					// converte a senha digitada para int e salva em senha_salva
					for (int k = 0; k < 4; k++) {
								uint8_t caracter = senha[k];
								senha_salva[k] = (uint8_t)(caracter - '0'); // salva como 0 a 9
					}
					idx = 0;
					senha[0] = '\0';
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




void start_motor(sentido_t sentido, modo_passo_t modo)
{
  const uint32_t limite = (modo == MODO_MEIO_PASSO) ? 8U : 4U;
  const uint32_t total_passos = (modo == MODO_MEIO_PASSO) ? STEPS_PER_REV_HALF : STEPS_PER_REV_FULL;

  uint32_t idx = 0U;  // começa no primeiro estado da sequência

  for (uint32_t s = 0; s < total_passos; s++) {

    if (modo == MODO_MEIO_PASSO) {
      PortH_Output(MeioPasso[idx]);      // usa seu padrão de meio passo
    } else {
      PortH_Output(PassoCompleto[idx]);  // usa seu padrão de passo completo
    }

    SysTick_Wait1ms(STEP_DELAY_MS);

    // avança ou retrocede o índice conforme o sentido
    if (sentido == DIR_ANTIHORARIO) {
      idx++;
      if (idx >= limite) idx = 0U;
    } else {
      if (idx == 0U) idx = (limite - 1U);
      else idx--;
    }
  }

  // Opcional: se quiser desenergizar após a volta, chame aqui sua função de stop.
  // Motor_Stop_PortH();
}


int32_t VerificaSenhaInserida(void) {   // -1=aguardando; 0=errada; 1=ok; -2=overflow limpado
    // 1) Lê uma tecla
    int32_t t = Teclas_Input(&GPIO_PORTL_DATA_R, &GPIO_PORTM_DIR_R, &GPIO_PORTM_DATA_R);
    if (t < 0) {
        return -1; // nenhuma tecla
    }

    // Debounce
    while (Teclas_Input(&GPIO_PORTL_DATA_R, &GPIO_PORTM_DIR_R, &GPIO_PORTM_DATA_R) >= 0) {
        SysTick_Wait1ms(10);
    }

    // 2) Dígitos 0..9: acumula em senha[] (máx 4)
    if (t >= 0 && t <= 9) {
        if (idx < 4) {
            senha[idx++] = (uint8_t)('0' + t);  // guarda ASCII
            senha[idx] = '\0';
        } else {
            // overflow: limpa como no seu GetTecla
            idx = 0;
            senha[0] = '\0';
            return -2;
        }
        ImprimeTexto(senha);
        return -1; // ainda aguardando '#'
    }

    // 3) Tecla '#': validar
    if (t == 11) {
        if (idx != 4) {
            return -1; // incompleta
        }

        // compara senha[] (ASCII) com senha_salva[] (0..9)
        for (int k = 0; k < 4; k++) {
            uint8_t dig = (uint8_t)(senha[k] - '0'); // '7' -> 7
            if (dig != senha_salva[k]) {
                // senha errada
                tentativas++;
                idx = 0;
                senha[0] = '\0';
                if (tentativas >= 3) {
                    Flag = 0;
                    cofre.estado = TRAVADO;
                }
                return 0;
            }
        }

        // senha correta
        tentativas = 0;
        Flag = 0;
        cofre.estado = ABRINDO;
        idx = 0;
        senha[0] = '\0';
        return 1;
    }

    // 4) Outras teclas: ignora
    return -1;
}







void AcendeSemaforo1(int32_t led)
{
		PortN_Output(led);
}

void AcendeSemaforo2(int32_t led)
{
		PortF_Output(led);
}
