/*

����:������
*/
#ifndef __STM32F1XX_SYSTEM_LED_H__
#define __STM32F1XX_SYSTEM_LED_H__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
//���� LED
#define system_led_port			GPIOD
#define system_led_pin			GPIO_PIN_1
#define SYSTEM_LED_ON()			HAL_GPIO_WritePin(system_led_port, system_led_pin, GPIO_PIN_RESET)
#define SYSTEM_LED_OFF()		HAL_GPIO_WritePin(system_led_port, system_led_pin, GPIO_PIN_SET)
//#define SYSTEM_LED_REVERSAL()	HAL_GPIO_TogglePin(system_led_port, system_led_pin); //��ת
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
extern void system_led_init(void);
extern void system_led(void);
/*----------------------------------------------------------------------------*/
#endif


