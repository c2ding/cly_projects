/**

作者:刘东辉
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_dac.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
DAC_HandleTypeDef hdac;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/* ADC1 init function */
void MX_DAC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	DAC_ChannelConfTypeDef sConfig = {0};
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_DAC1_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5);
#endif
	__HAL_RCC_DAC_CLK_ENABLE();
//--------------------------------------------------------------------------------
	//PA4     ------> DAC1_OUT1
	//PA5     ------> DAC1_OUT2 
	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//--------------------------------------------------------------------------------
	hdac.Instance = DAC1;
	s_error |= HAL_DAC_Init(&hdac);
	// DAC channel OUT1 config 
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	//sConfig.DAC_Trigger = DAC_TRIGGER_T4_TRGO; //由定时器4触发
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	s_error |= HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1); //DAC通道输出配置
	s_error |= HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); //设置DAC通道值
	s_error |= HAL_DAC_Start(&hdac, DAC_CHANNEL_1); //启动DAC
	// DAC channel OUT2 config 
	//sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO; //由定时器2触发
	s_error |= HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2); //DAC通道输出配置
	s_error |= HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 0); //设置DAC通道值
	s_error |= HAL_DAC_Start(&hdac, DAC_CHANNEL_2); //启动DAC
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_DAC_Init(); OK\r\n");
	} else
	{
		printf("MX_DAC_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
