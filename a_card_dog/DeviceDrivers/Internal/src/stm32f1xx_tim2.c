/**
���A������

����:������
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_tim2.h" 
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
TIM_HandleTypeDef htim2;
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: ������ʱ����ʼ��
  */
void MX_TIM2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_Encoder_InitTypeDef sConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_TIM2_CLK_DISABLE(); // ������ʱ������ʱ�ӽ���
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1); // ������ʱ�����蹦��GPIO����
	HAL_NVIC_DisableIRQ(TIM2_IRQn); // ������ʱ���жϽ���
#endif
	__HAL_RCC_TIM2_CLK_ENABLE(); //TIM2ʱ��ʹ��
//--------------------------------------------------------------------------------
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //����ģʽ
	GPIO_InitStruct.Pull = GPIO_NOPULL; //��ɽ����
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //����
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_LockPin(GPIOA, GPIO_PIN_0|GPIO_PIN_1); //��������
//--------------------------------------------------------------------------------
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 0xFFFF;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	//s_error |= HAL_TIM_Base_Init(&htim2);
 //--------------------------------------------------------------------------------
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;  
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;   
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;  
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1; 
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;   
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;  
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1; 
	sConfig.IC2Filter = 0;
	s_error |= HAL_TIM_Encoder_Init(&htim2, &sConfig); //��ʼ��������
//--------------------------------------------------------------------------------
	s_error |= HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
//--------------------------------------------------------------------------------
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	s_error |= HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
//--------------------------------------------------------------------------------
/*
__HAL_TIM_SET_COUNTER(&htim2, 0); //���㶨ʱ������     
__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE); //����жϱ�־λ
__HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE); //�����ж�
*/
//--------------------------------------------------------------------------------
	/* �����ж����� */
	HAL_NVIC_SetPriority(TIM2_IRQn, 1, 2);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_TIM2_Init(); OK\r\n"); 
	} else
	{
		printf("MX_TIM2_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
void TIM2_IRQHandler(void)
{
	if ((TIM2->SR&TIM_SR_UIF) != RESET)
	{
		TIM2->SR &= ~TIM_SR_UIF;//����жϱ�־λ
	}
}
/*----------------------------------------------------------------------------*/
