//
// Created by fibre on 2024/5/6.
//

#ifndef USER_H
#define USER_H

#endif //USER_H

#include "main.h"

void ALL_Response_SET(void);
void ALL_Response_RESET(void);
void F10k_Response_SET(void);
void F10k_Response_RESET(void);
void Morse_Decode_Init(void);
void Morse_Decode_Close(void);
void Morse_Decode_DD(void);
void Morse_Decode_W(void);

void Show_Freq_OLED(void);
void Show_State_OLED(uint8_t state);