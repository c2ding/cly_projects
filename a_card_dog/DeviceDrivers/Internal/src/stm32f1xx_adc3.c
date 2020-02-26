/**

����:������
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_adc3.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
ADC_HandleTypeDef hadc3;
/* ��չ���� ------------------------------------------------------------------*/
uint32_t adc3_buffer[ADC3_CHANNEL_NUMBER];
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/* ADC1 init function */
void MX_ADC3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_ChannelConfTypeDef sConfig;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
    /**ADC3 GPIO Configuration    
    PF6     ------> ADC3_IN4
    PF7     ------> ADC3_IN5
    PF8     ------> ADC3_IN6
    PF9     ------> ADC3_IN7
    PF10     ------> ADC3_IN8
    PC0     ------> ADC3_IN10
    PC1     ------> ADC3_IN11
    PC2     ------> ADC3_IN12
    PC3     ------> ADC3_IN13
    PA0-WKUP     ------> ADC3_IN0
    PA1     ------> ADC3_IN1
    PA2     ------> ADC3_IN2
    PA3     ------> ADC3_IN3 
    */
#if 0
    __HAL_RCC_ADC3_CLK_DISABLE();
  
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9 |GPIO_PIN_10);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
#endif
	__HAL_RCC_ADC3_CLK_ENABLE(); //ADC3ʱ��ʹ��
//--------------------------------------------------------------------------------
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9 |GPIO_PIN_10;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//--------------------------------------------------------------------------------
	hadc3.Instance = ADC3;
	hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE; //��ɨ��ģʽ������ȡ��·��ѹ
	hadc3.Init.ContinuousConvMode = DISABLE; //����ת��ģʽ 
	hadc3.Init.DiscontinuousConvMode = ENABLE; //���ģʽ����ͨ����Ҫ����ͨ������Ҫ
	hadc3.Init.NbrOfDiscConversion = 1; //���ģʽת��һ��
	hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START; //�������
	hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT; //ת������Ҷ���
	hadc3.Init.NbrOfConversion = ADC1_CHANNEL_NUMBER; //ת��ͨ������
	s_error |= HAL_ADC_Init(&hadc3);
//--------------------------------------------------------------------------------
	s_error |= HAL_ADCEx_Calibration_Start(&hadc3); //У׼ADC
//--------------------------------------------------------------------------------
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; //����ʱ��

	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	HAL_ADC_ConfigChannel(&hadc3, &sConfig);
	
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	s_error |= HAL_ADC_ConfigChannel(&hadc3, &sConfig);
	
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	s_error |= HAL_ADC_ConfigChannel(&hadc3, &sConfig);
	
	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = ADC_REGULAR_RANK_4;
	s_error |= HAL_ADC_ConfigChannel(&hadc3, &sConfig);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_ADC3_Init(); OK\r\n");
	} else
	{
		printf("MX_ADC3_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
void MX_ADC2_Read(void)
{
	uint32_t i;
	
	for (i=0; i<ADC3_CHANNEL_NUMBER; i++)
	{
		HAL_ADC_Start(&hadc3);
		HAL_ADC_PollForConversion(&hadc3, 50);
		adc3_buffer[i] = HAL_ADC_GetValue(&hadc3);
	}
	HAL_ADC_Stop(&hadc3);
}
/*----------------------------------------------------------------------------*/
