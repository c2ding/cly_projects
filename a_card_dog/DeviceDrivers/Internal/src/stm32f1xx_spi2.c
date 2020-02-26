/**
	HAL_SPI2_Transmit(&hspi2, &p_datum,1, 1000);
	HAL_SPI2_Receive(&hspi3,RxData, 1, 0xffff); 
	HAL_SPI2_TransmitReceive(&hspi1,&d_send,&d_read,1,0xFF);
	
作者:刘东辉
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_spi2.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
SPI_HandleTypeDef hspi2;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: SPI2外设初始化
  */
void MX_SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_SPI2_CLK_DISABLE(); // SPI2外设时钟禁用
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15); // SPI2外设功能GPIO配置
	//HAL_NVIC_DisableIRQ(SPI2_IRQn); // SPI2中断禁用
#endif
	__HAL_RCC_SPI2_CLK_ENABLE(); //SPI2时钟使能
//--------------------------------------------------------------------------------
	//PB13     ------> SPI2_SCK     ;SCL
	//PB14     ------> SPI2_MISO 入 ;xx
	//PB15     ------> SPI2_MOSI 出 ;SDA
	GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//--------------------------------------------------------------------------------
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER; //主机模式
	hspi2.Init.Direction = SPI_DIRECTION_2LINES; //选择全双工SPI模式
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT; //8位SPI
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW; //时钟悬空高电平
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE; //在第二个时钟采集数据
	hspi2.Init.NSS = SPI_NSS_SOFT; //Nss使用软件控制
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB; //从最高位开始传输
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 7;
	s_error |= HAL_SPI_Init(&hspi2);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_SPI2_Init(); OK\r\n");
	} else
	{
		printf("MX_SPI2_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
