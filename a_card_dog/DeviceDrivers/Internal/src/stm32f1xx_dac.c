/**

����:������
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_dac.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
DAC_HandleTypeDef hdac;
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
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
	//sConfig.DAC_Trigger = DAC_TRIGGER_T4_TRGO; //�ɶ�ʱ��4����
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	s_error |= HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1); //DACͨ���������
	s_error |= HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0); //����DACͨ��ֵ
	s_error |= HAL_DAC_Start(&hdac, DAC_CHANNEL_1); //����DAC
	// DAC channel OUT2 config 
	//sConfig.DAC_Trigger = DAC_TRIGGER_T2_TRGO; //�ɶ�ʱ��2����
	s_error |= HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2); //DACͨ���������
	s_error |= HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 0); //����DACͨ��ֵ
	s_error |= HAL_DAC_Start(&hdac, DAC_CHANNEL_2); //����DAC
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
