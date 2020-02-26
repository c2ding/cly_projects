/*
#include "stm32f1xx_uart4.h" //串口4
	MX_USART2_Init(); //串口4初始化
	uint32_t = MX_UART4_input((uint8_t *)&*p_datum, p_length); //读取数据 返回长度
	MX_UART4_output((uint8_t *)&*p_datum, p_length); //发送数据

作者:刘东辉
  */
#ifndef __STM32F1XX_UART4_H__
#define __STM32F1XX_UART4_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_delay.h"
#include "entrance.h"
/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
extern void MX_UART4_Init(void); //串口4初始化
extern uint32_t MX_UART4_input(uint8_t *, uint32_t); //读取数据
extern void MX_UART4_output(uint8_t *, uint32_t); //发送数据

#endif  /* __STM32F1XX_UART4_H__ */


