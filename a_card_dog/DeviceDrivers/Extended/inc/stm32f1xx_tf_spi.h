/*
#include "tf_spi.h" //TF 驱动
	hspi2.Init.Mode = SPI_MODE_MASTER; //主机模式
	hspi2.Init.Direction = SPI_DIRECTION_2LINES_RXONLY; //选择全双工SPI模式
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT; //8位SPI
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW; //时钟悬空高电平
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE; //在第二个时钟采集数据
	hspi2.Init.NSS = SPI_NSS_SOFT; //Nss使用软件控制
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB; //从最高位开始传输
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 7;

作者:刘东辉
*/
#ifndef __STM32F1XX_TF_SPI__
#define __STM32F1XX_TF_SPI__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_spi2.h"
/* 宏定义 --------------------------------------------------------------------*/
/* 类型定义 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
extern int8_t tf_init(void);
extern int8_t tf_read_capacity(uint32_t *capacity);
extern int8_t tf_read_disk(uint8_t *buf, uint32_t sector, uint8_t num);
extern int8_t tf_write_disk(uint8_t *buf, uint32_t sector, uint8_t num);
/*----------------------------------------------------------------------------*/
#endif
