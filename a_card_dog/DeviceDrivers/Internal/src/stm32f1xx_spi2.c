/**
	HAL_SPI2_Transmit(&hspi2, &p_datum,1, 1000);
	HAL_SPI2_Receive(&hspi3,RxData, 1, 0xffff); 
	HAL_SPI2_TransmitReceive(&hspi1,&d_send,&d_read,1,0xFF);
	
����:������
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_spi2.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
SPI_HandleTypeDef hspi2;
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: SPI2�����ʼ��
  */
void MX_SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_SPI2_CLK_DISABLE(); // SPI2����ʱ�ӽ���
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15); // SPI2���蹦��GPIO����
	//HAL_NVIC_DisableIRQ(SPI2_IRQn); // SPI2�жϽ���
#endif
	__HAL_RCC_SPI2_CLK_ENABLE(); //SPI2ʱ��ʹ��
//--------------------------------------------------------------------------------
	//PB13     ------> SPI2_SCK     ;SCL
	//PB14     ------> SPI2_MISO �� ;xx
	//PB15     ------> SPI2_MOSI �� ;SDA
	GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//--------------------------------------------------------------------------------
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER; //����ģʽ
	hspi2.Init.Direction = SPI_DIRECTION_2LINES; //ѡ��ȫ˫��SPIģʽ
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT; //8λSPI
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW; //ʱ�����ոߵ�ƽ
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE; //�ڵڶ���ʱ�Ӳɼ�����
	hspi2.Init.NSS = SPI_NSS_SOFT; //Nssʹ���������
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB; //�����λ��ʼ����
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 7;
	s_error |= HAL_SPI_Init(&hspi2);
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("MX_SPI2_Init(); OK\r\n");
	} else
	{
		printf("MX_SPI2_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
