/*
#include "stm32f1xx_usart2.h" //����2
	MX_USART2_Init(); //����2��ʼ��
	uint32_t = MX_USART2_input((uint8_t *)&*p_datum, p_length); //��ȡ���� ���س���
	MX_USART2_output((uint8_t *)&*p_datum, p_length); //��������
	
����:������
  */

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_usart2.h"
/* ˽�к궨�� ----------------------------------------------------------------*/
//	#define USE_PRINTING //ʹ�õ���ģʽ
//	#define USART2_USE_485 //ʹ��485��ʽ

	#define USART2_USE_PA2_PA3 //ʹ��PA2/PA3
//	#define USART2_USE_PD5_PD6 //ʹ��PD5/PD6
#ifndef USE_PRINTING
#define USART2_RX_BUFFER_SIZE 256 //���ջ��峤��
#define USART2_TX_BUFFER_SIZE 256 //���ͻ��峤��
#ifdef USART2_USE_485 //485
#define usart2_use_485_gpio		GPIOC
#define usart2_use_485_pin		GPIO_Pin_13
#define usart2_use_485_off()	HAL_GPIO_WritePin(usart2_use_485_gpio, usart2_use_485_pin, GPIO_PIN_RESET) //��
#define usart2_use_485_on()		HAL_GPIO_WritePin(usart2_use_485_gpio, usart2_use_485_pin, GPIO_PIN_SET) //��
#endif
/* ˽�����Ͷ��� --------------------------------------------------------------*/
typedef struct
{
	uint32_t rx_head; //����ͷ
	uint32_t rx_tail; //����β
	uint32_t rx_count; //���ݳ���
	uint8_t rx_buf[USART2_RX_BUFFER_SIZE]; //����

	uint32_t tx_head;
	uint32_t tx_tail;
	uint32_t tx_count;
	uint8_t tx_buf[USART2_TX_BUFFER_SIZE];
} USART2_BUFFER; //�������ݽṹ
/* ˽�б��� ------------------------------------------------------------------*/
static USART2_BUFFER g_usart2_buffer; //����
#endif
//UART_HandleTypeDef huart2;
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
void MX_USART2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	UART_HandleTypeDef huart2;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_USART2_CLK_DISABLE(); // ��������ʱ�ӽ���
#ifdef USART2_USE_PA2_PA3
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3); // �������蹦��GPIO����
#endif
#ifdef USART2_USE_PD5_PD6
	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5|GPIO_PIN_6); // �������蹦��GPIO����
#endif
	HAL_NVIC_DisableIRQ(USART2_IRQn); // �����жϽ���
#endif
	__HAL_RCC_USART2_CLK_ENABLE();  //USART2ʱ��ʹ��
//--------------------------------------------------------------------------------
#ifdef USART2_USE_PA2_PA3
    //PA2     ------> USART2_TX
    //PA3     ------> USART2_RX 
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	//__HAL_AFIO_REMAP_USART2_DISABLE();
#endif
#ifdef USART2_USE_PD5_PD6
	//PD5     ------> USART2_TX
	//PD6     ------> USART2_RX 
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	__HAL_AFIO_REMAP_USART2_ENABLE();
#endif

//485ʹ�ܽ�
#ifdef USART2_USE_485 //485ʹ�ܽ�
	GPIO_InitStructure.GPIO_Pin = usart2_use_485_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init(usart2_use_485_gpio, &GPIO_InitStructure); //
	usart2_use_485_off(); //485��ȡ
#endif
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
	HAL_NVIC_SetPriority(USART2_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
#endif
//--------------------------------------------------------------------------------
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	s_error |= HAL_UART_Init(&huart2);
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
	//__HAL_UART_ENABLE_IT(&huart2, USART_IT_TXE); //�����ж�
	__HAL_UART_ENABLE_IT(&huart2, USART_IT_RXNE); //�����ж�
	__HAL_UART_ENABLE_IT(&huart2, USART_IT_IDLE); //�����ж�
#endif
//	__HAL_UART_ENABLE_IT(&huart2, USART_IT_ERR);
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
//���ݻ�����
	g_usart2_buffer.rx_head = 0;
	g_usart2_buffer.rx_tail = 0;
	g_usart2_buffer.rx_count = 0;
	g_usart2_buffer.tx_head = 0;
	g_usart2_buffer.tx_tail = 0;
	g_usart2_buffer.tx_count = 0;
#endif
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		#ifdef USE_PRINTING
		printf("\r\n \\ | / \r\n-  L  -\r\n / | \\ \r\n");
		printf("< %s %s >\r\n", __DATE__, __TIME__); //��ʾ ����  ʱ��
		#endif
		printf("MX_USART2_Init(); OK\r\n");
	} else
	{
		printf("MX_USART2_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
#if defined(USE_FULL_ASSERT) && defined(USE_PRINTING)
//void _sys_exit(int x) {x = x;}
/**
  * ��������: �ض���c�⺯��printf��DEBUG_USART2
  */
int fputc(int ch, FILE *f)
{
	//HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
	while ((USART2->SR & UART_FLAG_TC) == RESET) {;}
	USART2->DR = (uint8_t)ch;
	return ch;
}
/*----------------------------------------------------------------------------*/
/**
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_USART2
  */
int fgetc(FILE * f)
{
	//uint8_t ch = 0;
	//HAL_UART_Receive(&huart2,&ch, 1, 0xFFFF);
	//return ch;
	while ((USART2->SR & UART_FLAG_RXNE) == RESET) {;}
	return ((int)(USART2->DR&0xFF));
}
#endif
#ifndef USE_PRINTING
/*----------------------------------------------------------------------------*/
//�ӽ��ջ���������ȡ�ַ�
uint32_t MX_USART2_input(uint8_t *p_datum, uint32_t p_length)
{
	uint32_t s_i;
	for (s_i=0; s_i<p_length; s_i++)
	{
		if (g_usart2_buffer.rx_count > 0)
		{
			*p_datum++ = g_usart2_buffer.rx_buf[g_usart2_buffer.rx_tail];
			if (USART2_RX_BUFFER_SIZE == ++g_usart2_buffer.rx_tail) {g_usart2_buffer.rx_tail = 0;}
			g_usart2_buffer.rx_count--;
		} else {break;} //���ݳ���Ϊ0ʱ����
	}
	return s_i;
}
/*----------------------------------------------------------------------------*/
//�����ݷ��뷢�ͻ�����
void MX_USART2_output(uint8_t *p_datum, uint32_t p_length)
{
	do {
		if (USART2_TX_BUFFER_SIZE > g_usart2_buffer.tx_count)
		{
			g_usart2_buffer.tx_buf[g_usart2_buffer.tx_head] = *p_datum++;
			if (USART2_TX_BUFFER_SIZE == ++g_usart2_buffer.tx_head) {g_usart2_buffer.tx_head = 0;}
			g_usart2_buffer.tx_count++;
		} else {break;}
	} while (--p_length);
	#ifdef USART2_USE_485 //485ʹ�ܽ�
	usart2_use_485_on(); //485����
	#endif
	//__HAL_UART_ENABLE_IT(&usart2, USART_IT_TXE); //������2�����ж�
	USART2->CR1 |= USART_CR1_TXEIE;
}
/*----------------------------------------------------------------------------*/
void USART2_IRQHandler(void) //����2�жϷ������
{
	uint8_t s_datum;
	//if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE)!=RESET) //USART2�����ж�
	if ((USART2->SR&UART_FLAG_RXNE) == UART_FLAG_RXNE) //USART2�����ж�
	{
		//s_datum = USART_ReceiveData(USART2); //��ȡ���յ�������
		s_datum = USART2->DR&0xFF;
		if (USART2_RX_BUFFER_SIZE > g_usart2_buffer.rx_count)
		{
			g_usart2_buffer.rx_buf[g_usart2_buffer.rx_head] = s_datum;
			if (USART2_RX_BUFFER_SIZE == ++g_usart2_buffer.rx_head) {g_usart2_buffer.rx_head = 0;}
			g_usart2_buffer.rx_count++;
		}
    }
//--------------------------------------------------------------------------------
	//if (__HAL_UART_GET_FLAG(&usart2, UART_FLAG_TXE)!=RESET) //USART2�����ж�
	//if ((USART2->SR&UART_FLAG_TXE) == UART_FLAG_TXE) //USART2�����ж�
	if ((USART2->SR&UART_FLAG_TC) == UART_FLAG_TC) //USART2�����ж�
	{
		if (g_usart2_buffer.tx_count > 0)
		{
			//USART_SendData(USART2, g_usart2_buffer->tx_buf[g_usart2_buffer->tx_tail])
			USART2->DR = g_usart2_buffer.tx_buf[g_usart2_buffer.tx_tail];
			if (USART2_TX_BUFFER_SIZE == ++g_usart2_buffer.tx_tail) {g_usart2_buffer.tx_tail = 0;}
			g_usart2_buffer.tx_count--;
		} else 
		{
			//__HAL_UART_DISABLE_IT(&usart2, USART_IT_TXE); //�رշ����ж�
			USART2->CR1 &= ~USART_CR1_TXEIE; //�رշ����ж�
			#ifdef USART2_USE_485 //485ʹ�ܽ�
			_delay_us(11000000/115200);
			usart2_use_485_off(); //485��ȡ
			#endif
		}
		//printf("\r\nOK\r\n");
	}
//--------------------------------------------------------------------------------
	//if (__HAL_UART_GET_FLAG(&usart2,UART_FLAG_IDLE)!=RESET) //USART2�����ж�
	if ((USART2->SR&UART_FLAG_IDLE) == UART_FLAG_IDLE) //USART2�����ж�
	{
		usart2_event = 1;
		s_datum = USART2->SR;
		s_datum = USART2->DR;
	}
//--------------------------------------------------------------------------------
//	if (__HAL_UART_GET_FLAG(USART2, USART_IT_PE|USART_IT_FE|USART_IT_NE) != RESET) //����
//	if ((USART2->SR&(USART_FLAG_ORE|USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) != NULL)
//	{
//		MX_USART2_Init();
//	}
}
/*----------------------------------------------------------------------------*/
#endif
