/**

作者:刘东辉
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_tim6.h" 
/* 私有宏定义 ----------------------------------------------------------------*/
// 定义定时器预分频，定时器实际时钟频率为：72MHz/（GENERAL_TIMx_PRESCALER+1）
#define GENERAL_TIM_PRESCALER            72 - 1  // 实际时钟频率为：1MHz

// 定义定时器周期，当定时器开始计数到GENERAL_TIMx_PERIOD值是更新定时器并生成对应事件和中断
#define GENERAL_TIM_PERIOD               10000 - 1  // 定时器产生中断频率为：1MHz/100000=1KHz，即10ms定时周期

// 最终定时器频率计算为： 72MHz/（GENERAL_TIMx_PRESCALER+1）/GENERAL_TIMx_PERIOD
// 比如需要产生20ms周期定时，可以设置为： 72MHz/（359+1）/4000=50Hz，即20ms周期
// 这里设置 GENERAL_TIMx_PRESCALER=359；GENERAL_TIMx_PERIOD=4000；
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
TIM_HandleTypeDef htim6;
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 基本定时器初始化
  */
void MX_TIM6_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_TIM6_CLK_DISABLE(); // 基本定时器外设时钟禁用
	//HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1); // 基本定时器外设功能GPIO配置
	HAL_NVIC_DisableIRQ(TIM6_IRQn); // 基本定时器中断禁用
#endif
	__HAL_RCC_TIM6_CLK_ENABLE(); //TIM6时钟使能
//--------------------------------------------------------------------------------
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = GENERAL_TIM_PRESCALER;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = GENERAL_TIM_PERIOD;
	htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	s_error |= HAL_TIM_Base_Init(&htim6);
//--------------------------------------------------------------------------------
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	s_error |= HAL_TIM_ConfigClockSource(&htim6, &sClockSourceConfig);
//--------------------------------------------------------------------------------
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	s_error |= HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig);
//--------------------------------------------------------------------------------
 	s_error = HAL_TIM_Base_Start_IT(&htim6);
//--------------------------------------------------------------------------------
	/* 外设中断配置 */
	HAL_NVIC_SetPriority(TIM6_IRQn, 1, 6);
	HAL_NVIC_EnableIRQ(TIM6_IRQn);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_TIM6_Init(); OK\r\n"); 
	} else
	{
		printf("MX_TIM6_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
void TIM6_IRQHandler(void)
{
	if ((TIM6->SR&TIM_SR_UIF) != RESET)
	{
		TIM2->SR &= ~TIM_SR_UIF;//清除中断标志位
	}
}
/*----------------------------------------------------------------------------*/
