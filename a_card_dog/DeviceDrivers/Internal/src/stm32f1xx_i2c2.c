/**

作者:刘东辉
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_i2c2.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
I2C_HandleTypeDef i2c2;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: I2C外设初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void MX_I2C2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_I2C2_CLK_DISABLE(); // I2C2外设时钟禁用
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11); // I2C2外设功能GPIO配置
	//HAL_NVIC_DisableIRQ(I2C2_EV_IRQn); // I2C2中断禁用
	//HAL_NVIC_DisableIRQ(I2C2_ER_IRQn); // I2C2中断禁用
#endif
	__HAL_RCC_I2C2_CLK_ENABLE(); //I2C2时钟使能
//--------------------------------------------------------------------------------
	//PB10     ------> I2C2_SCL
	//PB11     ------> I2C2_SDA 
	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//--------------------------------------------------------------------------------
	i2c2.Instance             = I2C2;
	i2c2.Init.ClockSpeed      = I2C_SPEEDCLOCK;
	i2c2.Init.DutyCycle       = I2C_DUTYCYCLE;
	i2c2.Init.OwnAddress1     = 0;
	i2c2.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	i2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c2.Init.OwnAddress2     = 0;
	i2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c2.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
	s_error |= HAL_I2C_Init(&i2c2);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_I2C2_Init(); OK\r\n");
	} else
	{
		printf("MX_I2C2_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
