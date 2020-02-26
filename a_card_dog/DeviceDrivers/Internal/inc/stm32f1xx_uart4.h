/*
#include "stm32f1xx_uart4.h" //����4
	MX_USART2_Init(); //����4��ʼ��
	uint32_t = MX_UART4_input((uint8_t *)&*p_datum, p_length); //��ȡ���� ���س���
	MX_UART4_output((uint8_t *)&*p_datum, p_length); //��������

����:������
  */
#ifndef __STM32F1XX_UART4_H__
#define __STM32F1XX_UART4_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_delay.h"
#include "entrance.h"
/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
extern void MX_UART4_Init(void); //����4��ʼ��
extern uint32_t MX_UART4_input(uint8_t *, uint32_t); //��ȡ����
extern void MX_UART4_output(uint8_t *, uint32_t); //��������

#endif  /* __STM32F1XX_UART4_H__ */


