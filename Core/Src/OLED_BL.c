//
// Created by fibre on 2024/1/22.
//

//本驱动用于OLED显示屏显示文字
#include "OLED_BL.h"
#include "OLED_BL_Font.h"

/**
  * @brief 向 oled 写入命令
  */
void OLED_Write_Cmd(uint8_t cmd) {
	uint8_t temp = cmd;
	HAL_I2C_Mem_Write(&I2C_X, OLED_ADDR, CMD_CTRL, I2C_MEMADD_SIZE_8BIT, &temp, sizeof temp,100);
}

/**
  * @brief 向 oled 写入数据
  */
void OLED_Write_Data(uint8_t data) {
	uint8_t temp = data;
	HAL_I2C_Mem_Write(&I2C_X, OLED_ADDR, DATA_CTRL, I2C_MEMADD_SIZE_8BIT, &temp, sizeof temp,100);
}

/**
  * @brief 设置 oled 写入坐标
  * @param x    列坐标：0 ~ 127
  * @param y    行坐标：0 ~ 63
  */
void OLED_Set_Pos(uint8_t x, uint8_t y) {
	OLED_Write_Cmd(0xb0 + y);                 // 写入页地址
	OLED_Write_Cmd((x & 0x0f));               // 写入列的地址 (低半字节)
	OLED_Write_Cmd(((x & 0xf0) >> 4) | 0x10); // 写入列的地址(高半字节)
}

/**
  * @brief oled 清屏
  */
void OLED_Clear(void) {
	unsigned char page, row;
	for (page = 0; page < 8; page++) {
		OLED_Write_Cmd(0xb0 + page); // 从0 ~ 7页依次写入
		OLED_Write_Cmd(0x00);        // 低位列地址
		OLED_Write_Cmd(0x10);        // 高位列地址
		for (row = 0; row < 128; row++) {
			OLED_Write_Data(0X00);
		}
	}
}

void OLED_Init(void) {
	HAL_Delay(100); // 延迟(必须要有)

	OLED_Write_Cmd(0xAE); // 关闭显示

	OLED_Write_Cmd(0x00); // 设置低列地址
	OLED_Write_Cmd(0x10); // 设置高列地址
	OLED_Write_Cmd(0x40); // 设置起始行地址
	OLED_Write_Cmd(0xB0); // 设置页地址

	OLED_Write_Cmd(0x81); // 对比度设置，可设置亮度
	OLED_Write_Cmd(0xFF); // 265

	OLED_Write_Cmd(0xA1); // 设置段(SEG)的起始映射地址
	OLED_Write_Cmd(0xA6); // 正常显示；0xa7逆显示

	OLED_Write_Cmd(0xA8); // 设置驱动路数（16~64）
	OLED_Write_Cmd(0x3F); // 64duty

	OLED_Write_Cmd(0xC8); // 重映射模式，COM[N-1]~COM0扫描

	OLED_Write_Cmd(0xD3); // 设置显示偏移
	OLED_Write_Cmd(0x00); // 无偏移

	OLED_Write_Cmd(0xD5); // 设置震荡器分频
	OLED_Write_Cmd(0x80); // 使用默认值

	OLED_Write_Cmd(0xD9); // 设置 Pre-Charge Period
	OLED_Write_Cmd(0xF1); // 使用官方推荐值

	OLED_Write_Cmd(0xDA); // 设置 com pin configuartion
	OLED_Write_Cmd(0x12); // 使用默认值

	OLED_Write_Cmd(0xDB); // 设置 Vcomh，可调节亮度（默认）
	OLED_Write_Cmd(0x40); // 使用官方推荐值

	OLED_Write_Cmd(0x8D); // 设置OLED电荷泵
	OLED_Write_Cmd(0x14); // 开显示

	OLED_Write_Cmd(0xAF); // 开启OLED面板显示
}

/**
  * @brief 将 OLED 从休眠中唤醒
  */
void OLED_ON(void) {
	OLED_Write_Cmd(0X8D); // 设置电荷泵
	OLED_Write_Cmd(0X14); // 开启电荷泵
	OLED_Write_Cmd(0XAF); // OLED 唤醒
}

/**
  * @brief 让OLED休眠
  */
void OLED_OFF(void) {
	OLED_Write_Cmd(0X8D); // 设置电荷泵
	OLED_Write_Cmd(0X10); // 关闭电荷泵
	OLED_Write_Cmd(0XAE); // OLED 休眠
}

/**
  * @brief 显示 bsp_font.h 中的ASCII字符 字体为 Consolas
  * @param x：  起始点列坐标：0 ~ 127
  *        y:   起始点页坐标：0 ~ 7
  * @param *ch：要显示的字符串
  */
void OLED_Show_String(uint8_t x, uint8_t y, uint8_t *ch) {
	uint8_t i = 0, c = 0, k = 0;

	while (ch[k] != '\0') {
		c = ch[k] - ' ';
		if (x > 120) {
			x = 0;
			y = y + 2;
		}
		OLED_Set_Pos(x, y);
		for (i = 0; i < 8; i++) {
			OLED_Write_Data(Consolas_8X16[c * 16 + i]);
		}

		OLED_Set_Pos(x, y + 1);
		for (i = 0; i < 8; i++) {
			OLED_Write_Data(Consolas_8X16[c * 16 + i + 8]);
		}

		x += 8;
		k++;
	}
}

