/**

	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1, 1000); //PWM重新配置，周期为1000，高电平持续时间为i
	TIM1->CCR4 = 1000;
作者:刘东辉
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_tim8.h" 
/* 私有宏定义 ----------------------------------------------------------------*/
#define ADVANCED_TIM_PWM_FREQ 10000 // PWM频率
#define ADVANCED_TIM_PRESCALER 71 // 定义定时器预分频，定时器实际时钟频率为：72MHz/(ADVANCED_TIMx_PRESCALER+1)
// 定义定时器周期，当定时器开始计数到ADVANCED_TIMx_PERIOD值并且重复计数寄存器为0时更新定时器并生成对应事件和中断
#define ADVANCED_TIM_PERIOD (SystemCoreClock/(ADVANCED_TIM_PRESCALER+1)/ADVANCED_TIM_PWM_FREQ)
#define ADVANCED_TIM_REPETITIONCOUNTER 0 // 定jhvcx\义高级定时器重复计数寄存器值
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
TIM_HandleTypeDef htim8;
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
void MX_TIM8_Out(uint32_t p_datum)
{
	//__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, p_datum*(ADVANCED_TIM_PERIOD/100)); //PWM重新配置，周期为1000，高电平持续时间为i
	TIM8->CCR4 = p_datum*(ADVANCED_TIM_PERIOD/100);
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 基本定时器初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void MX_TIM8_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_ClockConfigTypeDef sClockSourceConfig;
//	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_TIM8_CLK_DISABLE(); // 基本定时器外设时钟禁用
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8); // 基本定时器外设功能GPIO配置
	//HAL_NVIC_DisableIRQ(TIM8_CC_IRQn); // 基本定时器中断禁用
#endif
	__HAL_RCC_TIM8_CLK_ENABLE(); //TIM8时钟使能
//--------------------------------------------------------------------------------
    //PC6     ------> TIM8_CH1
    //PC7     ------> TIM8_CH2
    //PC8     ------> TIM8_CH3 
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//--------------------------------------------------------------------------------
	htim8.Instance = TIM8;
	htim8.Init.Prescaler = ADVANCED_TIM_PRESCALER;
	htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim8.Init.Period = ADVANCED_TIM_PERIOD;
	htim8.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	htim8.Init.RepetitionCounter = ADVANCED_TIM_REPETITIONCOUNTER;
	HAL_TIM_Base_Init(&htim8);
//--------------------------------------------------------------------------------
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig);
//--------------------------------------------------------------------------------
	HAL_TIM_PWM_Init(&htim8);
//--------------------------------------------------------------------------------
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig);
//--------------------------------------------------------------------------------
//	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
//	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
//	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
//	sBreakDeadTimeConfig.DeadTime = 0;
//	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
//	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
//	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
//	HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig); //互补输出设置
//--------------------------------------------------------------------------------
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0; //初始占空比
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1);

	sConfigOC.Pulse = 0; //初始占空比
	HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2);

	sConfigOC.Pulse = 0; //初始占空比
	HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3);

	//sConfigOC.Pulse = 100; //初始占空比
	//HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4);
//--------------------------------------------------------------------------------
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_4);
  
//	HAL_TIMEx_PWMN_Start(&htim8,TIM_CHANNEL_1); //使能互补输出
//	HAL_TIMEx_PWMN_Start(&htim8,TIM_CHANNEL_2);
//	HAL_TIMEx_PWMN_Start(&htim8,TIM_CHANNEL_3);
//	HAL_TIMEx_PWMN_Start(&htim8,TIM_CHANNEL_4);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("GENERAL_TIM8_Init(); OK\r\n"); 
	} else
	{
		printf("GENERAL_TIM8_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
void TIM8_IRQHandler(void)
{
	if ((TIM8->SR&TIM_SR_UIF) != RESET)
	{
		TIM8->SR &= ~TIM_SR_UIF;//清除中断标志位
	}
}
/*----------------------------------------------------------------------------*/
