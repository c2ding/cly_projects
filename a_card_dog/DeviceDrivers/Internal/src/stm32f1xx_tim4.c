/**
���B������

����:������
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_tim4.h" 
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
TIM_HandleTypeDef htim4;
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ������ʱ����ʼ��
  */
void MX_TIM4_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_Encoder_InitTypeDef sConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_TIM4_CLK_DISABLE(); // ������ʱ������ʱ�ӽ���
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7); // ������ʱ�����蹦��GPIO����
	HAL_NVIC_DisableIRQ(TIM4_IRQn); // ������ʱ���жϽ���
#endif
	__HAL_RCC_TIM4_CLK_ENABLE(); //TIM4ʱ��ʹ��
//--------------------------------------------------------------------------------
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //����ģʽ
	GPIO_InitStruct.Pull = GPIO_NOPULL; //��ɽ����
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //����
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_LockPin(GPIOB, GPIO_PIN_6|GPIO_PIN_7); //��������
//--------------------------------------------------------------------------------
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 0;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 0xFFFF;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	//s_error |= HAL_TIM_Base_Init(&htim4);
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
	s_error |= HAL_TIM_Encoder_Init(&htim4, &sConfig);
//--------------------------------------------------------------------------------
	s_error |= HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
//--------------------------------------------------------------------------------
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	s_error |= HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig);
//--------------------------------------------------------------------------------
/*
__HAL_TIM_SET_COUNTER(&htim4, 0); //���㶨ʱ������     
__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE); //����жϱ�־λ
__HAL_TIM_ENABLE_IT(&htim4,TIM_IT_UPDATE); //�����ж�
*/
//--------------------------------------------------------------------------------
	/* �����ж����� */
	HAL_NVIC_SetPriority(TIM4_IRQn, 1, 4);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_TIM4_Init(); OK\r\n"); 
	} else
	{
		printf("MX_TIM4_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
void TIM4_IRQHandler(void)
{
	if ((TIM4->SR&TIM_SR_UIF) != RESET)
	{
		TIM4->SR &= ~TIM_SR_UIF;//����жϱ�־λ
	}
}
/*----------------------------------------------------------------------------*/
