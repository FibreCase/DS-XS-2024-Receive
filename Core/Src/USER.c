//
// Created by fibre on 2024/5/6.
//

#define UNIT_TIME 200

#include "USER.h"

//freq
uint8_t pwm_flag = 0;

uint16_t pwm_value = 0;

uint32_t pwm_sum = 0;

uint32_t Freq = 0;

//manual_morse
uint8_t Manual_State = 0;

//morse translate
uint8_t Auto_Bit_State = 0;
// 0: ready 1: dot 2: dash

uint8_t Auto_Char_State = 0;
// 0: ready 1: receive

uint8_t Auto_Word_State = 0;
// 0: ready 1: receive 2: request translate

uint8_t Morse_Bit_Flag = 0;

uint8_t Morse_Char_Flag = 0;

uint8_t Morse_Code[256][6];

uint16_t Morse_Word[256];

char Translate_Word[256];

uint16_t Morse_Sheet[36] = {
	22222, 12222, 11222, 11122, 11112, 11111,
	21111, 22111, 22211, 22221, 12, 2111, 2121, 211, 1,//E
	1121, 221, 1111, 11, 1222, 212, 1211, 22, 21, 222,//O
	1221, 2212, 121, 111, 2, 112, 1112, 122, 2112, 2122, 2211//Z
};

const unsigned char Morse_Char[36] = {
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, //0-9
	65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, //A-K
	76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, //L-U
	87, 88, 89, 90 //V-Z
};

//OLED show
uint8_t TxBuffer[4][16];

void Show_OLED(uint8_t state) {
	Show_Freq_OLED();
	Show_State_OLED(state);
}

void Show_OLED_STATUS3(uint8_t state) {
	Show_Freq_OLED();
	Show_State_OLED(state);
	sprintf((char *)TxBuffer[2], "%d", Auto_Bit_State);
	OLED_Show_String(0, 2, (uint8_t *)TxBuffer[2]);
}

void Show_State_OLED(uint8_t state) {
	sprintf((char *)TxBuffer[0], "State= %2d", state);
	OLED_Show_String(0, 0, TxBuffer[0]);
}

void Show_Freq_OLED(void) {
	sprintf((char *)TxBuffer[3], "Freq= %7.1fkHz", (float)Freq / 10);
	OLED_Show_String(0, 6, TxBuffer[3]);
}

//status 0
void ALL_Response_SET(void) {
	HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
}

void ALL_Response_RESET(void) {
	HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
}

//status 1
void F10k_Response_SET(void) {
	if (Freq >= 99 && Freq <= 101) {
		//twinkle LED_R
		HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_RESET);
	}
	HAL_Delay(100);
}

void F10k_Response_RESET(void) {
	HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
}

////status 2
//void Manual_Morse(void) {
//	//init
//	Manual_Rcd_Last_En = Manual_Rcd;
//	Manual_Rcd = __HAL_TIM_GetCounter(&htim2);
//	Manual_Rcd_Diff = Manual_Rcd - Manual_Rcd_Last;
//
//	//start
//	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET && Manual_State == 0) {
//		__HAL_TIM_SetCounter(&htim2, 0);
//		HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_RESET);
//		Manual_State = 1;
//		Manual_Rcd_Diff = 0;
//		HAL_Delay(UNIT_TIME);
//		return;
//	}
//	else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET && Manual_State != 0) {
//		Morse_Switch();
//		Manual_Rcd_Last = Manual_Rcd_Last_En;
//		HAL_Delay(UNIT_TIME);
//		return;
//	}
//	else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_SET && Manual_State != 0) {
//		if (Manual_State == 1 && Manual_Rcd_Diff <= 2.5 * UNIT_TIME) {
//			Morse_Switch();
//			return;
//		}
//		else if (Manual_State == 2 && Manual_Rcd_Diff <= 2 * UNIT_TIME) {
//			Morse_Switch();
//			return;
//		}
//		else {
//			Manual_State = 0;
//			//R off G off
//			HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_SET);
//		}
//	}
//	else {
//		//R off G off
//		HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
//		HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_SET);
//		return;
//	}
//}
//
//void Morse_Switch(void) {
//	//300ms + 200ms
//	if (Manual_Rcd > 3 * UNIT_TIME) {
//		//R off G on
//		HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
//		HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_RESET);
//		Manual_State = 2;
//		return;
//	}
//}

//status 2
void Manual_Morse(void) {
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_SET && Manual_State == 0) {
		HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_SET);
	}
	else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET && Manual_State == 0) {
		HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_RESET);
		Manual_State = 1;
		HAL_Delay(UNIT_TIME);
		return;
	}
	else if (Manual_State == 1) {
		for (int i = 0; i < 100; ++i) {
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_SET) {
				delayMicroseconds(30);
			}
			else {
				Manual_State = 2;
				HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_RESET);
				return;
			}
		}
		Manual_State = 0;
	}
	else if (Manual_State == 2) {
		for (int i = 0; i < 50; ++i) {
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_SET) {
				delayMicroseconds(30);
			}
			else {
				HAL_Delay(10);
				return;
			}
		}
		Manual_State = 0;
		HAL_Delay(100);
	}
}

//status 3
void Auto_Morse(void) {
	if (Auto_Word_State == 2) {
		Auto_Morse_Translate();

		for (int i = 0; i < 256; ++i) {
			for (int j = 0; j < 6; ++j) {
				Morse_Code[i][j] = 0;
			}
		}

		Auto_Word_State = 0;

		Morse_Char_Flag = 0;
		Morse_Bit_Flag = 0;
	}
	else {
		Auto_Morse_One_Word();
	}
}

void Auto_Morse_One_Word(void) {
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_SET && Auto_Char_State == 0) {
		if (__HAL_TIM_GetCounter(&htim2) > 6000) { //3000ms
			Auto_Word_State = 2;
			HAL_TIM_Base_Stop(&htim2);
				__HAL_TIM_SetCounter(&htim2, 0);
			return;
		}
	}

		//one word start
	else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET && Auto_Char_State == 0) {
		Auto_Word_State = 1;
		HAL_TIM_Base_Start(&htim2);
			__HAL_TIM_SetCounter(&htim2, 0);
		Auto_Morse_One_Char();
	}
	else if (Auto_Char_State != 0) {
			__HAL_TIM_SetCounter(&htim2, 0);
		Auto_Morse_One_Char();
	}
}

void Auto_Morse_One_Char(void) {
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_SET && Auto_Bit_State == 0) {
		Auto_Char_State = 0;
		HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_SET);
		return;
	}
		//one word start
	else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET && Auto_Bit_State == 0) {
		Auto_Char_State = 1;

		Morse_Bit_Flag = 0;
		Morse_Char_Flag++;
		Auto_Morse_One_Bit();
	}
	else if (Auto_Bit_State != 0) {
		Auto_Morse_One_Bit();
	}
}

void Auto_Morse_One_Bit(void) {
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_SET && Auto_Bit_State == 0) {
		HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_SET);
	}
		//one bit start
	else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET && Auto_Bit_State == 0) {
		HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_RESET);
		Auto_Bit_State = 1;
		HAL_Delay(UNIT_TIME);
		return;
	}
		//bit switch
	else if (Auto_Bit_State == 1) {
		for (int i = 0; i < 100; ++i) {
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_SET) {
				delayMicroseconds(30);
			}
			else {
				Auto_Bit_State = 2;
				HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_RESET);
				return;
			}
		}

		//end with dot
		Auto_Bit_State = 0;
		Morse_Code[Morse_Char_Flag][Morse_Bit_Flag++] = 1;
	}
		//bit end
	else if (Auto_Bit_State == 2) {
		for (int i = 0; i < 50; ++i) {
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_SET) {
				delayMicroseconds(30);
			}
			else {
				HAL_Delay(50);
				return;
			}
		}

		//end with dash
		Auto_Bit_State = 0;
		Morse_Code[Morse_Char_Flag][Morse_Bit_Flag++] = 2;
	}
}

uint16_t Square_10(uint8_t num) {
	uint16_t result = 1;
	for (int i = 0; i < num; ++i) {
		result *= 10;
	}
	return result;
}

void Auto_Morse_Translate(void) {
	OLED_Show_String(0, 0, (uint8_t *)"Translating...");

	for (int i = 0; i < 256; ++i) {
		for (int j = 0; j < 6; ++j) {
			if (Morse_Code[i][j] == 1) {
				Morse_Word[i] = Morse_Word[i] * 10 + 1;
			}
			else if (Morse_Code[i][j] == 2) {
				Morse_Word[i] = Morse_Word[i] * 10 + 2;
			}
			else if (Morse_Code[i][j] == 0) {
				break;
			}
		}
	}

	for (int i = 0; i < 256; ++i) {
		if (Morse_Word[i] == 0) {
			Translate_Word[i] = '\0';
			break;
		}
		else {
			for (int j = 0; j < 36; ++j) {
				if (Morse_Word[i] == Morse_Sheet[j]) {
					Translate_Word[i] = Morse_Char[j];
					break;
				}
			}
		}
	}

	OLED_Show_String(0, 4, (uint8_t *)Translate_Word);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	//Frequncy Calaulate
	if (htim->Instance == htim3.Instance) {
		pwm_value = __HAL_TIM_GetCounter(&htim1);
			__HAL_TIM_SetCounter(&htim1, 0);
		pwm_sum += pwm_value;
		pwm_value = 0;
		pwm_flag++;
		if (pwm_flag == 100) {
			Freq = pwm_sum;
			pwm_flag = 0;
			pwm_sum = 0;
		}
	}

	//auto morse end
	if (htim->Instance == htim3.Instance) {

	}
}

static inline void delayMicroseconds(uint32_t us) {
	__IO uint32_t currentTicks = SysTick->VAL;
	/* Number of ticks per millisecond */
	const uint32_t tickPerMs = SysTick->LOAD + 1;
	/* Number of ticks to count */
	const uint32_t nbTicks = ((us - ((us > 0) ? 1 : 0)) * tickPerMs) / 1000;
	/* Number of elapsed ticks */
	uint32_t elapsedTicks = 0;
	__IO uint32_t oldTicks = currentTicks;
	do {
		currentTicks = SysTick->VAL;
		elapsedTicks += (oldTicks < currentTicks) ? tickPerMs + oldTicks - currentTicks :
		                oldTicks - currentTicks;
		oldTicks = currentTicks;
	}
	while (nbTicks > elapsedTicks);
}