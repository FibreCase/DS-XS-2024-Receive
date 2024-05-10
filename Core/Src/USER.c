//
// Created by fibre on 2024/5/6.
//

#define UNIT_TIME 200 //100ms

#include "USER.h"

uint8_t pwm_flag = 0;

uint16_t pwm_value = 0;

uint32_t pwm_sum = 0;

uint32_t Freq = 0;

uint8_t Manual_State = 0;

uint16_t Manual_Rcd = 0;
uint16_t Manual_Rcd_Last = 0;
uint16_t Manual_Rcd_Last_En = 0;
uint16_t Manual_Rcd_Diff = 0;

uint16_t Cap_Rcd[2] = {0};

uint32_t Cap_Flag = 0;

uint8_t Morse_Flag = 0;

uint8_t Morse_Code[6] = {0};

uint8_t TxBuffer[4][16];

void Show_OLED(uint8_t state) {
	Show_Freq_OLED();
	Show_State_OLED(state);
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

void Manual_Morse(void) {
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET && Manual_State == 0) {
		HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, LED_G_Pin, GPIO_PIN_SET);
	}
	else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET && Manual_State == 0) {
		HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_RESET);
		Manual_State = 1;
		delayMicroseconds(UNIT_TIME * 1000);
		return;
	}
	else if (Manual_State == 1) {
		for (int i = 0; i < 10; ++i) {
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
		for (int i = 0; i < 10; ++i) {
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_SET) {
				delayMicroseconds(30);
			}
			else {
				HAL_Delay(10);
				return;
			}
		}
		Manual_State = 0;
	}
}

//status 3
void Morse_Decode_Init(void) {
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_3);
}

void Morse_Decode_Close(void) {
	HAL_TIM_IC_Stop_IT(&htim4, TIM_CHANNEL_3);
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
}

//Moris Code
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	uint16_t Cap_Diff = 0;
	//1:dot 2:dash

	if (htim == &htim4 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
		Cap_Rcd[0] = Cap_Rcd[1];
		Cap_Rcd[1] = HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_3);
	}
	else {
		return;
	}

	Cap_Diff = Cap_Rcd[1] - Cap_Rcd[0];

	if (Cap_Diff >= 0 && Cap_Diff < 10) {
		//continuous signal
		Cap_Flag++;
		return;
	}
	else if (Cap_Diff >= 500 && Cap_Diff <= 2000) {
		//break
		Morse_Decode_Break();
	}
	else if (Cap_Diff >= 2000 && Cap_Diff <= 4000) {
		//space
		Morse_Decode_Space();
	}
	else {
		return;
	}

	//dot: 100ms dash: 300ms tim: 1/10000s
}

void Morse_Decode_Break(void) {
	if (Cap_Flag == 0) {
		return;
	}
	else if (Cap_Flag >= 500 && Cap_Flag <= 1500) {
		//dot
		Morse_Code[Morse_Flag] = 1;
	}
	else if (Cap_Flag >= 2500 && Cap_Flag <= 3500) {
		//dash
		Morse_Code[Morse_Flag] = 2;
	}
	else {
		return;
	}

	Morse_Flag++;
	Cap_Flag = 0;

}

void Morse_Decode_Space(void) {
		__HAL_TIM_SetCounter(&htim4, 0);
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