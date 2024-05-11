//
// Created by fibre on 2024/5/6.
//

#ifndef USER_H
#define USER_H

#endif //USER_H

#include "main.h"

void Show_OLED(uint8_t state);

void ALL_Response_SET(void);
void ALL_Response_RESET(void);
void F10k_Response_SET(void);
void F10k_Response_RESET(void);

void Manual_Morse(void);

void Auto_Morse(void);
void Auto_Morse_One_Bit(void);
void Auto_Morse_One_Char(void);
void Auto_Morse_One_Word(void);
void Auto_Morse_Translate(void);

void Show_Freq_OLED(void);
void Show_State_OLED(uint8_t state);

void Show_OLED_STATUS3(uint8_t state);

static inline void delayMicroseconds(uint32_t us);