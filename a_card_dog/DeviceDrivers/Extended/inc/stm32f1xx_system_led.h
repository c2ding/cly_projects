/*

作者:刘东辉
*/
#ifndef __STM32F1XX_SYSTEM_LED_H__
#define __STM32F1XX_SYSTEM_LED_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
//运行 LED
#define system_led_port			GPIOD
#define system_led_pin			GPIO_PIN_1
#define SYSTEM_LED_ON()			HAL_GPIO_WritePin(system_led_port, system_led_pin, GPIO_PIN_RESET)
#define SYSTEM_LED_OFF()		HAL_GPIO_WritePin(system_led_port, system_led_pin, GPIO_PIN_SET)
//#define SYSTEM_LED_REVERSAL()	HAL_GPIO_TogglePin(system_led_port, system_led_pin); //反转
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
extern void system_led_init(void);
extern void system_led(void);
/*----------------------------------------------------------------------------*/
#endif


