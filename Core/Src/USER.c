//
// Created by fibre on 2024/5/6.
//

#include "USER.h"

uint8_t pwm_flag = 0;
uint16_t pwm_value = 0;
uint32_t pwm_sum = 0;
uint32_t Freq = 0;

uint16_t Cap_Rcd[2] = {0};
uint32_t Cap_Flag = 0;
uint8_t Morse_Flag = 0;
uint8_t Morse_Code[6] = {0};

uint8_t TxBuffer[16];

void Show_State_OLED(uint8_t state) {
	sprintf((char *)TxBuffer, "State= %2d", state);
	OLED_Show_String(0,0, TxBuffer);
}

void Show_Freq_OLED(void) {
	sprintf((char *)TxBuffer, "Freq= %7dHz", Freq*100);
	OLED_Show_String(0,6, TxBuffer);
}

//status 0
void ALL_Response_SET(void) {
	HAL_GPIO_WritePin(GPIOA,LED_R_Pin,GPIO_PIN_SET);
}
void ALL_Response_RESET(void) {
	HAL_GPIO_WritePin(GPIOA,LED_R_Pin,GPIO_PIN_RESET);
}
//status 1
void F10k_Response_SET(void) {
	OLED_Show_String(0,0, TxBuffer);
	if (Freq >= 99 && Freq <= 101) {
		//twinkle LED_R
		HAL_GPIO_WritePin(GPIOA,LED_R_Pin,GPIO_PIN_SET);
	}
}
void F10k_Response_RESET(void) {
	HAL_GPIO_WritePin(GPIOA,LED_R_Pin,GPIO_PIN_RESET);
}
//status 2
void Morse_Decode_Init(void) {
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
}
void Morse_Decode_Close(void) {
	HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_1);
}


//Frequncy Calaulate
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == htim4.Instance) {
		pwm_value = __HAL_TIM_GetCounter(&htim2);
		__HAL_TIM_SetCounter(&htim2, 0);
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

	if (htim == &htim3 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
		Cap_Rcd[0] = Cap_Rcd[1];
		Cap_Rcd[1] = HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_1);
	}
	else {
		return;
	}

	Cap_Diff = Cap_Rcd[1] - Cap_Rcd[0];

	if (Cap_Diff >= 0 && Cap_Diff < 4) {
		//continuous signal
		Cap_Flag++;
		return;
	}
	else if (Cap_Diff >= 1500 && Cap_Diff <= 2500) {
		//break
		Morse_Decode_DD();
	}
	else if (Cap_Diff >= 4000 && Cap_Diff <= 6000) {
		//space
		Morse_Decode_W();
	}
	else {
		return;
	}



	//dot: 100ms dash: 300ms tim: 1/10000s
}

void Morse_Decode_DD(void) {
	if (Cap_Flag == 0) {
		return;
	}
	else if (Cap_Flag >= 50 && Cap_Flag <= 150) {
		//dot
		Morse_Code[Morse_Flag] = 1;
	}
	else if (Cap_Flag >= 250 && Cap_Flag <= 350) {
		//dash
		Morse_Code[Morse_Flag] = 2;
	}
	else {
		return;
	}

	Morse_Flag++;
	Cap_Flag = 0;
}

void Morse_Decode_W(void) {

}