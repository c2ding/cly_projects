/*
#include "stm32f1xx_uart5.h" //����5
	MX_USART2_Init(); //����5��ʼ��
	uint32_t = MX_UART5_input((uint8_t *)&*p_datum, p_length); //��ȡ���� ���س���
	MX_UART5_output((uint8_t *)&*p_datum, p_length); //��������

����:������
  */
#ifndef __STM32F1XX_UART5_H__
#define __STM32F1XX_UART5_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_delay.h"
#include "entrance.h"
/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
extern void MX_UART5_Init(void); //����5��ʼ��
extern uint32_t MX_UART5_input(uint8_t *, uint32_t); //��ȡ����
extern void MX_UART5_output(uint8_t *, uint32_t); //��������

#endif  /* __STM32F1XX_UART5_H__ */


