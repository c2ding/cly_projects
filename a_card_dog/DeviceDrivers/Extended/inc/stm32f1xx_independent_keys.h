/*
#include "independent_keys.h" //独立按键
	independent_keys_init(); //按键初始化
	independent_keys_scan(); //键盘扫描,并产生相应的事件
	
作者:刘东辉
*/
#ifndef __STM32F1xx_INDEPENDENT_KEYS__
#define __STM32F1xx_INDEPENDENT_KEYS__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* 宏定义 --------------------------------------------------------------------*/
#define KEY_SETUP 0
#define KEY_UP    1
#define KEY_DOWN  2
#define KEY_ENTER 3

#define SINGLE_CLICK		0x00 //单击
#define DOUBLE_CLICK		0x40 //双击
#define LONG_PRESS			0x80 //长按
#define CONTINGOUS_TRIGGER	0xC0 //连续触发
/* 类型定义 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
extern void independent_keys_init(void); //按键初始化
extern void independent_keys_scan(void); //键盘扫描,并产生相应的事件
#endif
