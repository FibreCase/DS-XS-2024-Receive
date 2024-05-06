//
// Created by fibre on 2024/1/22.
//

#ifndef OLED_BL_H
#define OLED_BL_H

#include "main.h"
#include "i2c.h"

#define I2C_X           hi2c1
#define OLED_ADDR       0x78
#define CMD_CTRL        0X00
#define DATA_CTRL       0X40

#endif //OLED_BL_H

void OLED_Write_Cmd(uint8_t cmd);
void OLED_Set_Pos(uint8_t x, uint8_t y);

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ON(void);
void OLED_OFF(void);

void OLED_Show_String(uint8_t x, uint8_t y, uint8_t *ch);
/**
  * @brief 显示ASCII字符 字体为 Consolas
  * @param x：  起始点列坐标：0 ~ 127
  *        y:   起始点页坐标：0 ~ 7
  * @param *ch：要显示的字符串
  */

