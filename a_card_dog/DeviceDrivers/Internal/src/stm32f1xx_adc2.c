/**

����:������
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_adc2.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
ADC_HandleTypeDef hadc2;
/* ��չ���� ------------------------------------------------------------------*/
uint32_t adc2_buffer[ADC2_CHANNEL_NUMBER];
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/* ADC2 init function */
void MX_ADC2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_ChannelConfTypeDef sConfig;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
    /**ADC2 GPIO Configuration    
    PA0-WKUP     ------> ADC2_IN0
    PA1     ------> ADC2_IN1
    PA2     ------> ADC2_IN2
    PA3     ------> ADC2_IN3
    PA4     ------> ADC2_IN4
    PA5     ------> ADC2_IN5
    PA6     ------> ADC2_IN6
    PA7     ------> ADC2_IN7
    PB0     ------> ADC2_IN8
    PB1     ------> ADC2_IN9 
    PC4     ------> ADC2_IN14
    PC5     ------> ADC2_IN15
    PC0     ------> ADC2_IN10
    PC1     ------> ADC2_IN11
    PC2     ------> ADC2_IN12
    PC3     ------> ADC2_IN13
    */
#if 0
    __HAL_RCC_ADC2_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 |GPIO_PIN_4|GPIO_PIN_5);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1);
#endif
	__HAL_RCC_ADC2_CLK_ENABLE(); //ADC2ʱ��ʹ��
//--------------------------------------------------------------------------------
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 |GPIO_PIN_4|GPIO_PIN_5;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//--------------------------------------------------------------------------------
	hadc2.Instance = ADC2;
	hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE; //��ɨ��ģʽ������ȡ��·��ѹ
	hadc2.Init.ContinuousConvMode = DISABLE; //����ת��ģʽ 
	hadc2.Init.DiscontinuousConvMode = ENABLE; //���ģʽ����ͨ����Ҫ����ͨ������Ҫ
	hadc2.Init.NbrOfDiscConversion = 1; //���ģʽת��һ��
	hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START; //�������
	hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT; //ת������Ҷ���
	hadc2.Init.NbrOfConversion = ADC1_CHANNEL_NUMBER; //ת��ͨ������
	s_error |= HAL_ADC_Init(&hadc2);
//--------------------------------------------------------------------------------
	s_error |= HAL_ADCEx_Calibration_Start(&hadc2); //У׼ADC
//--------------------------------------------------------------------------------
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; //����ʱ��

	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	HAL_ADC_ConfigChannel(&hadc2, &sConfig);
	
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	s_error |= HAL_ADC_ConfigChannel(&hadc2, &sConfig);
	
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	s_error |= HAL_ADC_ConfigChannel(&hadc2, &sConfig);
	
	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = ADC_REGULAR_RANK_4;
	s_error |= HAL_ADC_ConfigChannel(&hadc2, &sConfig);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_ADC2_Init(); OK\r\n");
	} else
	{
		printf("MX_ADC2_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
void MX_ADC2_Read(void)
{
	uint32_t i;
	
	for (i=0; i<ADC2_CHANNEL_NUMBER; i++)
	{
		HAL_ADC_Start(&hadc2);
		HAL_ADC_PollForConversion(&hadc2, 50);
		adc2_buffer[i] = HAL_ADC_GetValue(&hadc2);
	}
	HAL_ADC_Stop(&hadc2);
}
/*----------------------------------------------------------------------------*/
