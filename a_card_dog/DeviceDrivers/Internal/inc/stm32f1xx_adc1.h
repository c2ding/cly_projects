/**

����:������
  */
#ifndef __STM32F1XX_ADC1_H__
#define	__STM32F1XX_ADC1_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* �궨�� --------------------------------------------------------------------*/
#define ADC1_CHANNEL_NUMBER 4  //����ת��ͨ������
/* ���Ͷ��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern uint32_t adc1_buffer[ADC1_CHANNEL_NUMBER];
/* �������� ------------------------------------------------------------------*/
extern void MX_ADC1_Init(void);
extern void MX_ADC1_Read(void);
/*----------------------------------------------------------------------------*/
#endif /* __STM32F1XX_ADC1_H__ */
