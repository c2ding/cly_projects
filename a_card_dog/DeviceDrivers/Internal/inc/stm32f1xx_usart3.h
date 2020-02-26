/*
#include "stm32f1xx_usart3.h" //����3
	MX_USART2_Init(); //����3��ʼ��
	uint32_t = MX_USART3_input((uint8_t *)&*p_datum, p_length); //��ȡ���� ���س���
	MX_USART3_output((uint8_t *)&*p_datum, p_length); //��������

����:������
  */
#ifndef __STM32F1XX_USART3_H__
#define __STM32F1XX_USART3_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_delay.h"
#include "entrance.h"
/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
extern void MX_USART3_Init(void); //����3��ʼ��
extern uint32_t MX_USART3_input(uint8_t *, uint32_t); //��ȡ����
extern void MX_USART3_output(uint8_t *, uint32_t); //��������
#endif  /* __STM32F1XX_USART3_H__ */


