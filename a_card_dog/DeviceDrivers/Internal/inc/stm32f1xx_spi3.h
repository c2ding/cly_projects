/**
	HAL_SPI3_Transmit(&hspi2, &p_datum,1, 1000);
	HAL_SPI3_Receive(&hspi3,RxData, 1, 0xffff); 
	HAL_SPI3_TransmitReceive(&hspi1,&d_send,&d_read,1,0xFF);
	
����:������
  */
#ifndef __STM32F1XX_SPI3_H__
#define	__STM32F1XX_SPI3_H__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi3;
/* �������� ------------------------------------------------------------------*/
extern void MX_SPI3_Init(void);
#endif /* __STM32F1XX_SPI3_H__ */
