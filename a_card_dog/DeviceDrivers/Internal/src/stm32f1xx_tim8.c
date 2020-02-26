/**

	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1, 1000); //PWM�������ã�����Ϊ1000���ߵ�ƽ����ʱ��Ϊi
	TIM1->CCR4 = 1000;
����:������
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_tim8.h" 
/* ˽�к궨�� ----------------------------------------------------------------*/
#define ADVANCED_TIM_PWM_FREQ 10000 // PWMƵ��
#define ADVANCED_TIM_PRESCALER 71 // ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��72MHz/(ADVANCED_TIMx_PRESCALER+1)
// ���嶨ʱ�����ڣ�����ʱ����ʼ������ADVANCED_TIMx_PERIODֵ�����ظ������Ĵ���Ϊ0ʱ���¶�ʱ�������ɶ�Ӧ�¼����ж�
#define ADVANCED_TIM_PERIOD (SystemCoreClock/(ADVANCED_TIM_PRESCALER+1)/ADVANCED_TIM_PWM_FREQ)
#define ADVANCED_TIM_REPETITIONCOUNTER 0 // ��jhvcx\��߼���ʱ���ظ������Ĵ���ֵ
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
TIM_HandleTypeDef htim8;
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
void MX_TIM8_Out(uint32_t p_datum)
{
	//__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_4, p_datum*(ADVANCED_TIM_PERIOD/100)); //PWM�������ã�����Ϊ1000���ߵ�ƽ����ʱ��Ϊi
	TIM8->CCR4 = p_datum*(ADVANCED_TIM_PERIOD/100);
}
/*----------------------------------------------------------------------------*/
/**
  * ��������: ������ʱ����ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
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
	__HAL_RCC_TIM8_CLK_DISABLE(); // ������ʱ������ʱ�ӽ���
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8); // ������ʱ�����蹦��GPIO����
	//HAL_NVIC_DisableIRQ(TIM8_CC_IRQn); // ������ʱ���жϽ���
#endif
	__HAL_RCC_TIM8_CLK_ENABLE(); //TIM8ʱ��ʹ��
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
//	HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig); //�����������
//--------------------------------------------------------------------------------
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0; //��ʼռ�ձ�
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1);

	sConfigOC.Pulse = 0; //��ʼռ�ձ�
	HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2);

	sConfigOC.Pulse = 0; //��ʼռ�ձ�
	HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3);

	//sConfigOC.Pulse = 100; //��ʼռ�ձ�
	//HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4);
//--------------------------------------------------------------------------------
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_4);
  
//	HAL_TIMEx_PWMN_Start(&htim8,TIM_CHANNEL_1); //ʹ�ܻ������
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
		TIM8->SR &= ~TIM_SR_UIF;//����жϱ�־λ
	}
}
/*----------------------------------------------------------------------------*/
