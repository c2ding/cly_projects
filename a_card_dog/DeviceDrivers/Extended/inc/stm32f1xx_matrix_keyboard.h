/*
#include "matrix_keyboard.h"//按键识别与按键事件发起
	matrix_keyboard_init(); //按键初始化
	matrix_keyboard_scan(); //键盘扫描,并产生相应的事件
	
作者:刘东辉 
*/
#ifndef __STM32F1xx_MATRIX_KEYBOARD__
#define __STM32F1xx_MATRIX_KEYBOARD__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* 宏定义 --------------------------------------------------------------------*/
/* 类型定义 ------------------------------------------------------------------*/

#define SINGLE_CLICK		0x00 //单击
#define DOUBLE_CLICK		0x40 //双击
#define LONG_PRESS			0x80 //长按
#define CONTINGOUS_TRIGGER	0xC0 //连续触发
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
extern void matrix_keyboard_init(void); //按键初始化
extern void matrix_keyboard_scan(void); //键盘扫描,并产生相应的事件
#endif
