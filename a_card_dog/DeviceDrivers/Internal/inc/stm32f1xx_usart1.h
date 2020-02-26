/*
#include "stm32f1xx_usart1.h" //����1
	MX_USART1_Init(); //����1��ʼ��
	uint32_t = MX_USART1_input((uint8_t *)&*p_datum, p_length); //��ȡ���� ���س���
	MX_USART1_output((uint8_t *)&*p_datum, p_length); //��������

����:������
  */
#ifndef __STM32F1XX_USART1_H__
#define __STM32F1XX_USART1_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_delay.h"
#include "entrance.h"
/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
extern void MX_USART1_Init(void); //����1��ʼ��
extern uint32_t MX_USART1_input(uint8_t *, uint32_t); //��ȡ����
extern void MX_USART1_output(uint8_t *, uint32_t); //��������
#endif  /* __STM32F1XX_USART1_H__ */
