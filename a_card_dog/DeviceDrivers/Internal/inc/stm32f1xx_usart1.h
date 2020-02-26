/*
#include "stm32f1xx_usart1.h" //串口1
	MX_USART1_Init(); //串口1初始化
	uint32_t = MX_USART1_input((uint8_t *)&*p_datum, p_length); //读取数据 返回长度
	MX_USART1_output((uint8_t *)&*p_datum, p_length); //发送数据

作者:刘东辉
  */
#ifndef __STM32F1XX_USART1_H__
#define __STM32F1XX_USART1_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_delay.h"
#include "entrance.h"
/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
extern void MX_USART1_Init(void); //串口1初始化
extern uint32_t MX_USART1_input(uint8_t *, uint32_t); //读取数据
extern void MX_USART1_output(uint8_t *, uint32_t); //发送数据
#endif  /* __STM32F1XX_USART1_H__ */

