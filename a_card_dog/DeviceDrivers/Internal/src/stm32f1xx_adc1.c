/**

作者:刘东辉
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_adc1.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
/* 扩展变量 ------------------------------------------------------------------*/
uint32_t adc1_buffer[ADC1_CHANNEL_NUMBER];
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/* ADC1 init function */
void MX_ADC1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_ChannelConfTypeDef sConfig;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
    /**ADC1 GPIO Configuration    
    PA0-WKUP     ------> ADC1_IN0
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA3     ------> ADC1_IN3
    PA4     ------> ADC1_IN4
    PA5     ------> ADC1_IN5
    PA6     ------> ADC1_IN6
    PA7     ------> ADC1_IN7
    PB0     ------> ADC1_IN8
    PB1     ------> ADC1_IN9
    PC0     ------> ADC1_IN10
    PC1     ------> ADC1_IN11
    PC2     ------> ADC1_IN12
    PC3     ------> ADC1_IN13
    PC4     ------> ADC1_IN14
    PC5     ------> ADC1_IN15
    */
#if 0
	__HAL_RCC_ADC1_CLK_DISABLE();

	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1);
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 |GPIO_PIN_4|GPIO_PIN_5);
#endif
	__HAL_RCC_ADC1_CLK_ENABLE(); //ADC1时钟使能
//--------------------------------------------------------------------------------
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 |GPIO_PIN_4|GPIO_PIN_5;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//--------------------------------------------------------------------------------
	hadc1.Instance = ADC1;
	hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE; //打开扫描模式，来读取多路电压
	hadc1.Init.ContinuousConvMode = DISABLE; //连续转换模式 
	hadc1.Init.DiscontinuousConvMode = ENABLE; //间断模式，多通道才要，单通道不需要
	hadc1.Init.NbrOfDiscConversion = 1; //间断模式转换一次
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; //软件触发
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT; //转换结果右对齐
	hadc1.Init.NbrOfConversion = ADC1_CHANNEL_NUMBER; //转换通道个数
	s_error |= HAL_ADC_Init(&hadc1);
//--------------------------------------------------------------------------------
	s_error |= HAL_ADCEx_Calibration_Start(&hadc1); //校准ADC
//--------------------------------------------------------------------------------
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; //采样时间

	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	s_error |= HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	s_error |= HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	
	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = ADC_REGULAR_RANK_4;
	s_error |= HAL_ADC_ConfigChannel(&hadc1, &sConfig);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_ADC1_Init(); OK\r\n");
	} else
	{
		printf("MX_ADC1_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
void MX_ADC1_Read(void)
{
	uint32_t i;
	
	for (i=0; i<ADC1_CHANNEL_NUMBER; i++)
	{
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 50);
		adc1_buffer[i] = HAL_ADC_GetValue(&hadc1);
	}
	HAL_ADC_Stop(&hadc1);
}
/*----------------------------------------------------------------------------*/
