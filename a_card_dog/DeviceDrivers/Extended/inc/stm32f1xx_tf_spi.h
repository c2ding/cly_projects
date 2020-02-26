/*
#include "tf_spi.h" //TF ����
	hspi2.Init.Mode = SPI_MODE_MASTER; //����ģʽ
	hspi2.Init.Direction = SPI_DIRECTION_2LINES_RXONLY; //ѡ��ȫ˫��SPIģʽ
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT; //8λSPI
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW; //ʱ�����ոߵ�ƽ
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE; //�ڵڶ���ʱ�Ӳɼ�����
	hspi2.Init.NSS = SPI_NSS_SOFT; //Nssʹ���������
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB; //�����λ��ʼ����
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 7;

����:������
*/
#ifndef __STM32F1XX_TF_SPI__
#define __STM32F1XX_TF_SPI__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_spi2.h"
/* �궨�� --------------------------------------------------------------------*/
/* ���Ͷ��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
extern int8_t tf_init(void);
extern int8_t tf_read_capacity(uint32_t *capacity);
extern int8_t tf_read_disk(uint8_t *buf, uint32_t sector, uint8_t num);
extern int8_t tf_write_disk(uint8_t *buf, uint32_t sector, uint8_t num);
/*----------------------------------------------------------------------------*/
#endif
