/**

����:������
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_tim6.h" 
/* ˽�к궨�� ----------------------------------------------------------------*/
// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��72MHz/��GENERAL_TIMx_PRESCALER+1��
#define GENERAL_TIM_PRESCALER            72 - 1  // ʵ��ʱ��Ƶ��Ϊ��1MHz

// ���嶨ʱ�����ڣ�����ʱ����ʼ������GENERAL_TIMx_PERIODֵ�Ǹ��¶�ʱ�������ɶ�Ӧ�¼����ж�
#define GENERAL_TIM_PERIOD               10000 - 1  // ��ʱ�������ж�Ƶ��Ϊ��1MHz/100000=1KHz����10ms��ʱ����

// ���ն�ʱ��Ƶ�ʼ���Ϊ�� 72MHz/��GENERAL_TIMx_PRESCALER+1��/GENERAL_TIMx_PERIOD
// ������Ҫ����20ms���ڶ�ʱ����������Ϊ�� 72MHz/��359+1��/4000=50Hz����20ms����
// �������� GENERAL_TIMx_PRESCALER=359��GENERAL_TIMx_PERIOD=4000��
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
TIM_HandleTypeDef htim6;
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: ������ʱ����ʼ��
  */
void MX_TIM6_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_TIM6_CLK_DISABLE(); // ������ʱ������ʱ�ӽ���
	//HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1); // ������ʱ�����蹦��GPIO����
	HAL_NVIC_DisableIRQ(TIM6_IRQn); // ������ʱ���жϽ���
#endif
	__HAL_RCC_TIM6_CLK_ENABLE(); //TIM6ʱ��ʹ��
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
	/* �����ж����� */
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
		TIM2->SR &= ~TIM_SR_UIF;//����жϱ�־λ
	}
}
/*----------------------------------------------------------------------------*/
