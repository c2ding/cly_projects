/*
#include "stm32f1xx_oled12864_i2c.h" //0.96OLED12864 驱动
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
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_oled12864_i2c.h"
#include "stm32f1xx_i2c1.h"
#include "stm32f1xx_delay.h"
/* 私有宏定义 ----------------------------------------------------------------*/
//#define __OLED12864_RES__ //使用复位引脚

//复位
#if defined(__OLED12864_RES__)
#define OLED12864_RES_PIN	GPIO_PIN_11
#define OLED12864_RES_PORT	GPIOB
#define OLED12864_RES_H()	HAL_GPIO_WritePin(OLED12864_RES_PORT, OLED12864_RES_PIN, GPIO_PIN_SET)
#define OLED12864_RES_L()	HAL_GPIO_WritePin(OLED12864_RES_PORT, OLED12864_RES_PIN, GPIO_PIN_RESET)
#endif

#define ADDRESS 0x78 //OLED地址
#define DATA	 0x40 //数据指令
#define COMMAND 0x00 //命令指令
//使用的I2C端口
#if defined(__STM32F1XX_I2C1_H__)
#define SSD1315_i2c i2c1
#endif
#if defined(__STM32F1XX_I2C2_H__)
#define SSD1315_i2c i2c2
#endif
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
const uint8_t SSD1315_COMMAND[] = {
0xAE,//--turn off oled panel
0x00,//---set low column address
0x10,//---set high column address
0x40,//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
0x81,//--set contrast control register
0xCF,// Set SEG Output Current Brightness
0xA1,//--Set SEG/Column Mapping     0xA0左右反置 0xA1正常
0xC8,//Set COM/Row Scan Direction   0xC0上下反置 0xC8正常
0xA6,//--set normal display
0xA8,//--set multiplex ratio(1 to 64)
0x3F,//--1/64 duty
0xD3,//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
0x00,//-not offset
0xD5,//--set display clock divide ratio/oscillator frequency
0x80,//--set divide ratio, Set Clock as 100 Frames/Sec
0xD9,//--set pre-charge period
0xF1,//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
0xDA,//--set com pins hardware configuration
0x12,
0xDB,//--set vcomh
0x40,//Set VCOM Deselect Level
0x20,//-Set Page Addressing Mode (0x00/0x01/0x02)
0x02,//
0x8D,//--set Charge Pump enable/disable
0x14,//--set(0x10) disable
0xA4,// Disable Entire Display On (0xa4/0xa5)
0xA6,// Disable Inverse Display On (0xa6/a7) 
0xAF,//--turn on oled panel
};
const uint8_t chinese_characters[][32]={
	{0x00,0x3E,0x22,0x96,0x49,0x32,0x4E,0x92,0x22,0x1E,0x00,0xF0,0x00,0x00,0xFF,0x00,
	0x02,0x42,0xD5,0x65,0x45,0x7F,0x25,0x35,0x29,0x22,0x00,0x0F,0x40,0x80,0x7F,0x00},/*"劉",0*/

	{0x04,0x04,0xE4,0xA4,0xA4,0xA4,0xA4,0xFF,0xA4,0xA4,0xA4,0xA4,0xE4,0x04,0x04,0x00,
	0x40,0x40,0x27,0x22,0x12,0x0A,0x06,0xFF,0x06,0x0A,0x12,0x22,0x27,0x40,0x40,0x00},/*"東",1*/

	{0x44,0x58,0xC0,0x7F,0xD0,0x48,0x16,0xD2,0x52,0x52,0xFE,0x52,0x52,0xD2,0x16,0x00,
	0x80,0x60,0x1F,0x00,0x7F,0x20,0x10,0x17,0x15,0x15,0xFF,0x15,0x15,0x17,0x10,0x00},/*"輝",2*/
};
//每个字符占6*8个点阵(除去边 为5*7的编码表)
const uint8_t asciitab_dzk_5x7[][6] =
{
	{0x00,0x00,0x00,0x00,0x00,0x00},//Sp
	{0x00,0x00,0x2F,0x00,0x00,0x00},//!
	{0x00,0x07,0x00,0x07,0x00,0x00},//"
	{0x14,0x7F,0x14,0x7F,0x14,0x00},//#
	{0x24,0x2A,0x7F,0x2A,0x12,0x00},//$
	{0x62,0x64,0x08,0x13,0x23,0x00},//%
	{0x36,0x49,0x55,0x22,0x50,0x00},//&
	{0x00,0x05,0x03,0x00,0x00,0x00},//'
	{0x00,0x1C,0x22,0x41,0x00,0x00},//(
	{0x00,0x41,0x22,0x1C,0x00,0x00},//)
	{0x14,0x08,0x3E,0x08,0x14,0x00},//*
	{0x08,0x08,0x3E,0x08,0x08,0x00},//+
	{0x00,0x00,0xA0,0x60,0x00,0x00},//,
	{0x08,0x08,0x08,0x08,0x08,0x00},//-
	{0x00,0x60,0x60,0x00,0x00,0x00},//.
	{0x20,0x10,0x08,0x04,0x02,0x00},///
	{0x3E,0x51,0x49,0x45,0x3E,0x00},//0
	{0x00,0x42,0x7F,0x40,0x00,0x00},//1
	{0x42,0x61,0x51,0x49,0x46,0x00},//2
	{0x21,0x41,0x45,0x4B,0x31,0x00},//3
	{0x18,0x14,0x12,0x7F,0x10,0x00},//4
	{0x27,0x45,0x45,0x45,0x39,0x00},//5
	{0x3C,0x4A,0x49,0x49,0x30,0x00},//6
	{0x01,0x71,0x09,0x05,0x03,0x00},//7
	{0x36,0x49,0x49,0x49,0x36,0x00},//8
	{0x06,0x49,0x49,0x29,0x1E,0x00},//9
	{0x00,0x36,0x36,0x00,0x00,0x00},//:
	{0x00,0x56,0x36,0x00,0x00,0x00},//;
	{0x08,0x14,0x22,0x41,0x00,0x00},//<
	{0x14,0x14,0x14,0x14,0x14,0x00},//=
	{0x00,0x41,0x22,0x14,0x08,0x00},//>
	{0x02,0x01,0x51,0x09,0x06,0x00},//?
	{0x32,0x49,0x59,0x51,0x3E,0x00},//@
	{0x7C,0x12,0x11,0x12,0x7C,0x00},//A
	{0x7F,0x49,0x49,0x49,0x36,0x00},//B
	{0x3E,0x41,0x41,0x41,0x22,0x00},//C
	{0x7F,0x41,0x41,0x22,0x1C,0x00},//D
	{0x7F,0x49,0x49,0x49,0x41,0x00},//E
	{0x7F,0x09,0x09,0x09,0x01,0x00},//F
	{0x3E,0x41,0x49,0x49,0x7A,0x00},//G
	{0x7F,0x08,0x08,0x08,0x7F,0x00},//H
	{0x00,0x41,0x7F,0x41,0x00,0x00},//I
	{0x20,0x40,0x41,0x3F,0x01,0x00},//J
	{0x7F,0x08,0x14,0x22,0x41,0x00},//K
	{0x7F,0x40,0x40,0x40,0x40,0x00},//L
	{0x7F,0x02,0x0C,0x02,0x7F,0x00},//M
	{0x7F,0x04,0x08,0x10,0x7F,0x00},//N
	{0x3E,0x41,0x41,0x41,0x3E,0x00},//O
	{0x7F,0x09,0x09,0x09,0x06,0x00},//P
	{0x3E,0x41,0x51,0x21,0x5E,0x00},//Q
	{0x7F,0x09,0x19,0x29,0x46,0x00},//R
	{0x46,0x49,0x49,0x49,0x31,0x00},//S
	{0x01,0x01,0x7F,0x01,0x01,0x00},//T
	{0x3F,0x40,0x40,0x40,0x3F,0x00},//U
	{0x1F,0x20,0x40,0x20,0x1F,0x00},//V
	{0x3F,0x40,0x38,0x40,0x3F,0x00},//W
	{0x63,0x14,0x08,0x14,0x63,0x00},//X
	{0x07,0x08,0x70,0x08,0x07,0x00},//Y
	{0x61,0x51,0x49,0x45,0x43,0x00},//Z
	{0x00,0x7F,0x41,0x41,0x00,0x00},//[
	{0x02,0x04,0x08,0x10,0x20,0x00},//'\'
	{0x00,0x41,0x41,0x7F,0x00,0x00},//]
	{0x04,0x02,0x01,0x02,0x04,0x00},//^
	{0x40,0x40,0x40,0x40,0x40,0x00},//_
	{0x00,0x01,0x02,0x04,0x00,0x00},//`
	{0x20,0x54,0x54,0x54,0x78,0x00},//a
	{0x7F,0x48,0x44,0x44,0x38,0x00},//b
	{0x38,0x44,0x44,0x44,0x20,0x00},//c
	{0x38,0x44,0x44,0x48,0x7F,0x00},//d
	{0x38,0x54,0x54,0x54,0x18,0x00},//e
	{0x08,0x7E,0x09,0x01,0x02,0x00},//f
	{0x18,0xA4,0xA4,0xA4,0x7C,0x00},//g
	{0x7F,0x08,0x04,0x04,0x78,0x00},//h
	{0x00,0x44,0x7D,0x40,0x00,0x00},//i
	{0x40,0x80,0x84,0x7D,0x00,0x00},//j
	{0x7F,0x10,0x28,0x44,0x00,0x00},//k
	{0x00,0x41,0x7F,0x40,0x00,0x00},//l
	{0x7C,0x04,0x18,0x04,0x78,0x00},//m
	{0x7C,0x08,0x04,0x04,0x78,0x00},//n
	{0x38,0x44,0x44,0x44,0x38,0x00},//o
	{0xFC,0x24,0x24,0x24,0x18,0x00},//p
	{0x18,0x24,0x24,0x18,0xFC,0x00},//q
	{0x7C,0x08,0x04,0x04,0x08,0x00},//r
	{0x48,0x54,0x54,0x54,0x20,0x00},//s
	{0x04,0x3F,0x44,0x40,0x20,0x00},//t
	{0x3C,0x40,0x40,0x20,0x7C,0x00},//u
	{0x1C,0x20,0x40,0x20,0x1C,0x00},//v
	{0x3C,0x40,0x30,0x40,0x3C,0x00},//w
	{0x44,0x28,0x10,0x28,0x44,0x00},//x
	{0x1C,0xA0,0xA0,0xA0,0x7C,0x00},//y
	{0x44,0x64,0x54,0x4C,0x44,0x00},//z
	{0x08,0x3E,0x41,0x41,0x00,0x00},//{
	{0x00,0x00,0x7F,0x00,0x00,0x00},//|
	{0x00,0x41,0x41,0x3E,0x08,0x00},//}
	{0x02,0x01,0x02,0x04,0x02,0x00} //~
};
//8*16的点阵
const uint8_t asciitab_dzk_8x16[][16]=
{
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//Sp
	{0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00},//!
	{0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//"
	{0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00},//#
	{0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00},//$
	{0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,0x00,0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00},//%
	{0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10},//&
	{0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//'
	{0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00},//(
	{0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00},//)
	{0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00},//*
	{0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00},//+
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00},//,
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01},//-
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00},//.
	{0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,0x00,0x60,0x18,0x06,0x01,0x00,0x00,0x00},///
	{0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00},//0
	{0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},//1
	{0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00},//2
	{0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00},//3
	{0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00},//4
	{0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00},//5
	{0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00},//6
	{0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00},//7
	{0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00},//8
	{0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00},//9
	{0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00},//:
	{0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00},//;
	{0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00},//<
	{0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00},//=
	{0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00},//>
	{0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00},//?
	{0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00},//@
	{0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20},//A
	{0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00},//B
	{0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00},//C
	{0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00},//D
	{0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00},//E
	{0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00},//F
	{0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00},//G
	{0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20},//H
	{0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},//I
	{0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00},//J
	{0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00},//K
	{0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00},//L
	{0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00},//M
	{0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00},//N
	{0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00},//O
	{0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00},//P
	{0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00},//Q
	{0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20},//R
	{0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00},//S
	{0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00},//T
	{0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00},//U
	{0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00},//V
	{0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00},//W
	{0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20},//X
	{0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00},//Y
	{0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00},//Z
	{0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00},//[
	{0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00},//'\'
	{0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00},//]
	{0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//^
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80},//_
	{0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//`
	{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20},//a
	{0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00},//b
	{0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00},//c
	{0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20},//d
	{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00},//e
	{0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},//f
	{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00},//g
	{0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20},//h
	{0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},//i
	{0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00},//j
	{0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00},//k
	{0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},//l
	{0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F},//m
	{0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20},//n
	{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00},//o
	{0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00},//p
	{0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80},//q
	{0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00},//r
	{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00},//s
	{0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00},//t
	{0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20},//u
	{0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00},//v
	{0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00},//w
	{0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00},//x
	{0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00},//y
	{0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00},//z
	{0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40},//{
	{0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00},//|
	{0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00},//}
	{0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//~
};
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
//OLED初始化
void ssd1315_init(void)
{
	HAL_StatusTypeDef status = HAL_OK;

#if defined(__OLED12864_RES__)
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = OLED12864_RES_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(OLED12864_RES_PORT, &GPIO_InitStruct);
	HAL_GPIO_LockPin(OLED12864_RES_PORT, OLED12864_RES_PIN); //锁定配置
	OLED12864_RES_L();
	_delay_ms(100);
	OLED12864_RES_H();
#endif
	
	status |= HAL_I2C_Mem_Write(&SSD1315_i2c, ADDRESS, COMMAND, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&SSD1315_COMMAND, sizeof(SSD1315_COMMAND), 3000);

#ifdef USE_FULL_ASSERT
	if (status == HAL_OK)
	{
		printf("ssd1315_init(); OK\r\n"); 
	} else
	{
		printf("ssd1315_init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
//写坐标
static void ssd1315_yx(uint8_t p_y, uint8_t p_x)
{
	uint8_t s_tx[3];
	s_tx[0] = 0xB0 + p_y;
	s_tx[1] = ((p_x>>4)&0x0F)|0x10;
	s_tx[2] = p_x & 0x0F;
	HAL_I2C_Mem_Write(&SSD1315_i2c, ADDRESS, COMMAND, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&s_tx, 3, 3000);
}
/*----------------------------------------------------------------------------*/
//清屏
void ssd1315_clear(void)
{
	uint8_t s_y, s_tx[132];

	memset((uint8_t*)&s_tx, 0x00, sizeof(s_tx));
	for (s_y=0; s_y<8; s_y++)
	{
		ssd1315_yx(s_y, 0);
		HAL_I2C_Mem_Write(&SSD1315_i2c, ADDRESS, DATA, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&s_tx, sizeof(s_tx), 3000);
	}
}
/*----------------------------------------------------------------------------*/
//按指定位置显示一个字符
void ssd1315_write_word_5X7(uint8_t p_y, uint8_t p_x, uint8_t p_datum)
{
	ssd1315_yx(p_y, p_x);
	HAL_I2C_Mem_Write(&SSD1315_i2c, ADDRESS, DATA, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&asciitab_dzk_5x7[p_datum-32][0], 6, 3000);
}
/*----------------------------------------------------------------------------*/
//按指定位置显示一串5x7字符
void ssd1315_write_string_5x7(uint8_t p_y, uint8_t p_x, uint8_t *p_datum, uint8_t p_length)
{
	uint8_t s_datum;

	ssd1315_yx(p_y, p_x);
	do {
		s_datum = (*p_datum++)-32;
		HAL_I2C_Mem_Write(&SSD1315_i2c, ADDRESS, DATA, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&asciitab_dzk_5x7[s_datum][0], 6, 3000);
	} while (--p_length);
}
/*----------------------------------------------------------------------------*/
//按指定位置显示一串8x16字符
void ssd1315_write_string_8x16(uint8_t p_y, uint8_t p_x, uint8_t *p_datum, uint8_t p_length)
{
	uint8_t s_datum;

	do {
		s_datum = (*p_datum++)-32;
		ssd1315_yx(p_y, p_x);
		HAL_I2C_Mem_Write(&SSD1315_i2c, ADDRESS, DATA, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&asciitab_dzk_8x16[s_datum][0], 8, 3000);
		ssd1315_yx(p_y+1, p_x);
		HAL_I2C_Mem_Write(&SSD1315_i2c, ADDRESS, DATA, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&asciitab_dzk_8x16[s_datum][8], 8, 3000);
		p_x += 8;
	} while (--p_length);
}
/*----------------------------------------------------------------------------*/
//按指定位置显示一个8x16字符
void ssd1315_write_word(uint8_t p_y, uint8_t p_x, uint8_t p_datum)
{
	ssd1315_yx(p_y, p_x);
	HAL_I2C_Mem_Write(&SSD1315_i2c, ADDRESS, DATA, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&chinese_characters[p_datum-32][0], 16, 3000);
	ssd1315_yx(p_y+1, p_x);
	HAL_I2C_Mem_Write(&SSD1315_i2c, ADDRESS, DATA, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&chinese_characters[p_datum-32][16], 16, 3000);
}
/*----------------------------------------------------------------------------*/
/*
// picture用来显示一个图片
void Picture()
{
  unsigned char x,y;
  unsigned int i=0;
  for(y=0;y<8;y++)
    {
      Write_IIC_Command(0xb0+y);
      Write_IIC_Command(0x00);
      Write_IIC_Command(0x10);
      for(x=0;x<132;x++)
        {
          Write_IIC_Data(show[i++]);
        }
    }
}
*/
