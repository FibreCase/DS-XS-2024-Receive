//
// Created by fibre on 2024/5/6.
//

#include "USER.h"

uint8_t pwm_flag = 0;
uint16_t pwm_value = 0;
uint32_t pwm_sum = 0;
uint32_t Freq = 0;

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