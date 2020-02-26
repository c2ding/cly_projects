/*
#include "oled12864.h" //0.96OLED12864 驱动
	ssd1315_init(); //OLED初始化
	ssd1315_clear(); //清屏
	//按指定位置显示一串5x7字符
	//y:0~7 x:(0~122) 每行最多21个字符
	ssd1315_write_string_5x7(y, x, &"123456", length);
	//按指定位置显示一串8x16字符
	//y:0~6 x:(0~120) 每行最多16个字符
	ssd1315_write_string_8x16(y, x, &"123456", length);
	//按指定位置显示一个16*16字符(汉字)
	//y:0~6 x:(0~112) 每行最多8个字符 ?:chinese_characters中的第几个字符表
	ssd1315_write_word(y, x, ?);

	GND:地&VCC:3.3V&RST(RES):接高&DC(DC):接地&CS:接地&D0:SCL&D1:SDA

作者:刘东辉
*/
#ifndef __STM32F1XX_SSD1315__
#define __STM32F1XX_SSD1315__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* 宏定义 --------------------------------------------------------------------*/
/* 类型定义 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
extern void ssd1315_init(void); //OLED初始化
extern void ssd1315_clear(void); //清屏
extern void ssd1315_write_word_5X7(uint8_t, uint8_t, uint8_t); //按指定位置显示一个字符
extern void ssd1315_write_string_5x7(uint8_t, uint8_t, uint8_t *, uint8_t); //按指定位置显示一串5x7字符
extern void ssd1315_write_word_8x16(uint8_t, uint8_t, uint8_t); //按指定位置显示一个字符
extern void ssd1315_write_string_8x16(uint8_t, uint8_t, uint8_t *, uint8_t); //按指定位置显示一串8x16字符
extern void ssd1315_write_word(uint8_t, uint8_t, uint8_t); //按指定位置显示一个16*16字符(汉字)

#endif
