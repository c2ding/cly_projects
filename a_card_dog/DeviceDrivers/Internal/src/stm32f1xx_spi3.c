/**
	HAL_SPI3_Transmit(&hspi2, &p_datum,1, 1000);
	HAL_SPI3_Receive(&hspi3,RxData, 1, 0xffff); 
	HAL_SPI3_TransmitReceive(&hspi1,&d_send,&d_read,1,0xFF);
	
作者:刘东辉
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_spi3.h"
/* 私有类型定义 --------------------------------------------------------------*/
#define SPI3_USE_PB3_PB4_PB5 //使用PB3/PB4/PB5
//#define SPI3_USE_PC10_PC11_PC12 //使用PC10/PC11/PC12
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
SPI_HandleTypeDef hspi3;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: SPI3外设初始化
  */
void MX_SPI3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_SPI3_CLK_DISABLE(); // SPI3外设时钟禁用
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5); // SPI3外设功能GPIO配置
	//HAL_NVIC_DisableIRQ(SPI3_IRQn); // SPI3中断禁用
#endif
	__HAL_RCC_SPI3_CLK_ENABLE(); //SPI3时钟使能
//--------------------------------------------------------------------------------
#ifdef SPI3_USE_PB3_PB4_PB5
	//PB3     ------> SPI3_SCK
	//PB4     ------> SPI3_MISO 入
	//PB5     ------> SPI3_MOSI 出
	GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	//__HAL_AFIO_REMAP_SPI3_DISABLE();
#endif
#ifdef SPI3_USE_PC10_PC11_PC12
	//PC10     ------> SPI1_SCK   ;SCL
	//PC11     ------> SPI1_MISO  ;xx
	//PC12     ------> SPI1_MOSI  ;SDA
	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	__HAL_AFIO_REMAP_SPI3_ENABLE();
#endif
//--------------------------------------------------------------------------------
	hspi3.Instance = SPI3;
	hspi3.Init.Mode = SPI_MODE_MASTER;
	hspi3.Init.Direction = SPI_DIRECTION_2LINES;
	hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi3.Init.NSS = SPI_NSS_SOFT;
	hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi3.Init.CRCPolynomial = 10;
	s_error |= HAL_SPI_Init(&hspi3);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_SPI3_Init(); OK\r\n");
	} else
	{
		printf("MX_SPI3_Init(); ERROR\r\n"); 
	}
#endif

}
/*----------------------------------------------------------------------------*/
