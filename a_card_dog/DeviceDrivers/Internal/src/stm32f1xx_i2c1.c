/**

作者:刘东辉
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_i2c1.h"
/* 私有类型定义 --------------------------------------------------------------*/
#define I2C1_USE_PB6_PB7 //使用PB6/PB7
//#define I2C1_USE_PB8_PB9 //使用PB8/PB9
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
I2C_HandleTypeDef i2c1;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: I2C外设初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void MX_I2C1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_I2C1_CLK_DISABLE(); // I2C1外设时钟禁用
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7); // I2C1外设功能GPIO配置
	//HAL_NVIC_DisableIRQ(I2C1_EV_IRQn); // I2C1中断禁用
	//HAL_NVIC_DisableIRQ(I2C1_ER_IRQn); // I2C1中断禁用
#endif
	__HAL_RCC_I2C1_CLK_ENABLE(); //I2C1时钟使能
//--------------------------------------------------------------------------------
#ifdef I2C1_USE_PB6_PB7
	//PB6     ------> I2C1_SCL
	//PB7     ------> I2C1_SDA 
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	//__HAL_AFIO_REMAP_I2C1_DISABLE()
#endif
#ifdef I2C1_USE_PB8_PB9
	//PB8     ------> I2C1_SCL
	//PB9     ------> I2C1_SDA 
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	__HAL_AFIO_REMAP_I2C1_ENABLE()
#endif
//--------------------------------------------------------------------------------
	i2c1.Instance             = I2C1;
	i2c1.Init.ClockSpeed      = I2C_SPEEDCLOCK;
	i2c1.Init.DutyCycle       = I2C_DUTYCYCLE;
	i2c1.Init.OwnAddress1     = 0;
	i2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	i2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c1.Init.OwnAddress2     = 0;
	i2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
	s_error |= HAL_I2C_Init(&i2c1);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_I2C1_Init(); OK\r\n");
	} else
	{
		printf("MX_I2C1_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
