/**
	HAL_SPI1_Transmit(&hspi2, &p_datum,1, 1000);
	HAL_SPI1_Receive(&hspi3,RxData, 1, 0xffff); 
	HAL_SPI1_TransmitReceive(&hspi1,&d_send,&d_read,1,0xFF);
	
作者:刘东辉
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_spi1.h"

/* 私有类型定义 --------------------------------------------------------------*/
//#define SPI1_USE_PA5_PA6_PA7 //使用PA5/PA6/PA7
#define SPI1_USE_PB3_PB4_PB5 //使用PB3/PB4/PB5
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: SPI1外设初始化
  */
void MX_SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_SPI1_CLK_DISABLE(); // SPI1外设时钟禁用
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7); // SPI1外设功能GPIO配置
	//HAL_NVIC_DisableIRQ(SPI1_IRQn); // SPI1中断禁用
#endif
	__HAL_RCC_SPI1_CLK_ENABLE(); //SPI1时钟使能
//--------------------------------------------------------------------------------
#ifdef SPI1_USE_PA5_PA6_PA7
	//PA5     ------> SPI1_SCK
	//PA6     ------> SPI1_MISO 入
	//PA7     ------> SPI1_MOSI 出
	GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//__HAL_AFIO_REMAP_SPI1_DISABLE();
#endif
#ifdef SPI1_USE_PB3_PB4_PB5
	//PB3     ------> SPI1_SCK   ;SCL
	//PB4     ------> SPI1_MISO  ;xx
	//PB5     ------> SPI1_MOSI  ;SDA
	GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	__HAL_AFIO_REMAP_SPI1_ENABLE();
#endif
//--------------------------------------------------------------------------------
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	s_error |= HAL_SPI_Init(&hspi1);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_SPI1_Init(); OK\r\n");
	} else
	{
		printf("MX_SPI1_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
