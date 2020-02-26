/*
#include "ds3231.h"
	ds1302_init();//时钟芯片初始化
	ds1302_switch(OFF/ON);//晶振开关
	ds1302_write_clock((uint8_t *)&datum);//写入DS1302时钟数据7位
	ds1302_read_clock((uint8_t *)&datum);//读取DS1302时钟数据


Vcc2 SCLK IO RST
  |   |   |   |
 --------------
| 8   7   6   5|
|    DS1302    |
|              |
| 1   2   3   4|
 --------------
  |   |   |   |
Vcc1  X1  X2 GND
2,3脚32.768HZ晶振 8脚接后备电源

作者:刘东辉 
*/
#ifndef __STM32F1XX_DS3231__
#define __STM32F1XX_DS3231__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h" 
/* 宏定义 --------------------------------------------------------------------*/
/* 类型定义 ------------------------------------------------------------------*/
typedef struct {
	uint16_t	year;	//年
	uint8_t		month;	//月
	uint8_t		mday;	//日
	uint8_t		hour;	//时
	uint8_t		min;	//分
	uint8_t		sec;	//秒
	uint8_t		wday;	//星期
	int8_t temperature; //温度
} RTCTIME;
/* 扩展变量 ------------------------------------------------------------------*/
extern RTCTIME g_ds3231;
/* 函数声明 ------------------------------------------------------------------*/
extern HAL_StatusTypeDef ds3231_write_clock(void); //写入DS3231时钟
extern HAL_StatusTypeDef ds3231_read_clock(void); //读取DS3231时钟
#endif
