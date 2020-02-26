/*
#include "stm32f1xx_uart5.h" //����5
	MX_USART2_Init(); //����5��ʼ��
	uint32_t = MX_UART5_input((uint8_t *)&*p_datum, p_length); //��ȡ���� ���س���
	MX_UART5_output((uint8_t *)&*p_datum, p_length); //��������

����:������
  */

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_uart5.h"
/* ˽�к궨�� ----------------------------------------------------------------*/
//	#define USE_PRINTING //ʹ�õ���ģʽ
//	#define UART5_USE_485 //ʹ��485��ʽ

#ifndef USE_PRINTING
#define UART5_RX_BUFFER_SIZE 256 //���ջ��峤��
#define UART5_TX_BUFFER_SIZE 256 //���ͻ��峤��
#ifdef UART5_USE_485 //485
#define uart5_use_485_gpio		GPIOD
#define uart5_use_485_pin		GPIO_PIN_1
#define uart5_use_485_off()		HAL_GPIO_WritePin(uart5_use_485_gpio, uart5_use_485_pin, GPIO_PIN_RESET) //��
#define uart5_use_485_on()		HAL_GPIO_WritePin(uart5_use_485_gpio, uart5_use_485_pin, GPIO_PIN_SET) //��
#endif
/* ˽�����Ͷ��� --------------------------------------------------------------*/
typedef struct
{
	uint32_t rx_head; //����ͷ
	uint32_t rx_tail; //����β
	uint32_t rx_count; //���ݳ���
	uint8_t rx_buf[UART5_RX_BUFFER_SIZE]; //����

	uint32_t tx_head;
	uint32_t tx_tail;
	uint32_t tx_count;
	uint8_t tx_buf[UART5_TX_BUFFER_SIZE];
} UART5_BUFFER; //�������ݽṹ
	/* ˽�б��� ------------------------------------------------------------------*/
static UART5_BUFFER g_uart5_buffer; //����
	#endif
//UART_HandleTypeDef huart5;
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
void MX_UART5_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	UART_HandleTypeDef huart5;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_UART5_CLK_DISABLE(); // ��������ʱ�ӽ���
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_12); // �������蹦��GPIO����
	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2); // �������蹦��GPIO����
	HAL_NVIC_DisableIRQ(UART5_IRQn); // �����жϽ���
#endif
	__HAL_RCC_UART5_CLK_ENABLE(); //UART5ʱ��ʹ��
//--------------------------------------------------------------------------------
    //PC12     ------> UART5_TX
    //PD2      ------> UART5_RX 
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

//485ʹ�ܽ�
#ifdef UART5_USE_485 //485ʹ�ܽ�
	GPIO_InitStruct.Pin = uart5_use_485_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(uart5_use_485_gpio, &GPIO_InitStruct);
	HAL_GPIO_LockPin(uart5_use_485_gpio, uart5_use_485_pin); //��������
	uart5_use_485_off(); //485��ȡ
#endif
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
	HAL_NVIC_SetPriority(UART5_IRQn, 1, 4);
	HAL_NVIC_EnableIRQ(UART5_IRQn);
#endif
//--------------------------------------------------------------------------------
	huart5.Instance = UART5;
	huart5.Init.BaudRate = 115200;
	huart5.Init.WordLength = UART_WORDLENGTH_8B;
	huart5.Init.StopBits = UART_STOPBITS_1;
	huart5.Init.Parity = UART_PARITY_NONE;
	huart5.Init.Mode = UART_MODE_TX_RX;
	huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart5.Init.OverSampling = UART_OVERSAMPLING_16;
	s_error |= HAL_UART_Init(&huart5);
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
	//__HAL_UART_ENABLE_IT(&huart5,USART_IT_TXE); //�����ж�
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_RXNE); //�����ж�
	__HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE); //�����ж�
#endif
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
//���ݻ�����
	g_uart5_buffer.rx_head = 0;
	g_uart5_buffer.rx_tail = 0;
	g_uart5_buffer.rx_count = 0;
	g_uart5_buffer.tx_head = 0;
	g_uart5_buffer.tx_tail = 0;
	g_uart5_buffer.tx_count = 0;
#endif
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		#ifdef USE_PRINTING
		printf("\r\n \\ | / \r\n-  L  -\r\n / | \\ \r\n");
		printf("< %s %s >\r\n", __DATE__, __TIME__); //��ʾ ����  ʱ��
		#endif
		printf("MX_UART5_Init(); OK\r\n");
	} else
	{
		printf("MX_UART5_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
#if defined(USE_FULL_ASSERT) && defined(USE_PRINTING)
//void _sys_exit(int x) {x = x;}
/**
  * ��������: �ض���c�⺯��printf��DEBUG_UART5
  */
int fputc(int ch, FILE *f)
{
	//HAL_UART_Transmit(&huart5, (uint8_t *)&ch, 1, 0xFFFF);
	while ((UART5->SR & UART_FLAG_TC) == RESET) {;}
	UART5->DR = (uint8_t)ch;
	return ch;
}
/*----------------------------------------------------------------------------*/
/**
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_UART5
  */
int fgetc(FILE * f)
{
	//uint8_t ch = 0;
	//HAL_UART_Receive(&huart5,&ch, 1, 0xFFFF);
	//return ch;
	while ((UART5->SR & UART_FLAG_RXNE) == RESET) {;}
	return ((int)(UART5->DR&0xFF));
}
#endif
#ifndef USE_PRINTING
/*----------------------------------------------------------------------------*/
//�ӽ��ջ���������ȡ�ַ�
uint32_t MX_UART5_input(uint8_t *p_datum, uint32_t p_length)
{
	uint32_t s_i;
	for (s_i=0; s_i<p_length; s_i++) //������󳤶�
	{
		if (g_uart5_buffer.rx_count > 0)
		{
			*p_datum++ = g_uart5_buffer.rx_buf[g_uart5_buffer.rx_tail];
			if (UART5_RX_BUFFER_SIZE == ++g_uart5_buffer.rx_tail) {g_uart5_buffer.rx_tail = 0;}
			g_uart5_buffer.rx_count--;
		} else {break;} //���ݳ���Ϊ0ʱ����
	}
	return s_i;
}
/*----------------------------------------------------------------------------*/
//�����ݷ��뷢�ͻ�����
void MX_UART5_output(uint8_t *p_datum, uint32_t p_length)
{
	do {
		if (UART5_TX_BUFFER_SIZE > g_uart5_buffer.tx_count)
		{
			g_uart5_buffer.tx_buf[g_uart5_buffer.tx_head] = *p_datum++;
			if (UART5_TX_BUFFER_SIZE == ++g_uart5_buffer.tx_head) {g_uart5_buffer.tx_head = 0;}
			g_uart5_buffer.tx_count++;
		} else {break;}
	} while (--p_length);
	#ifdef UART5_USE_485 //485ʹ�ܽ�
	uart5_use_485_on(); //485����
	#endif
	//__HAL_UART_ENABLE_IT(&UART5, USART_IT_TXE); //������5�����ж�
	UART5->CR1 |= USART_CR1_TXEIE;
}
/*----------------------------------------------------------------------------*/
void UART5_IRQHandler(void) //����5�жϷ������
{
	uint8_t s_datum;
	//if (__HAL_UART_GET_FLAG(&UART5, UART_FLAG_RXNE)!=RESET) //UART5�����ж�
	if ((UART5->SR&UART_FLAG_RXNE) == UART_FLAG_RXNE) //UART5�����ж�
	{
		//s_datum = USART_ReceiveData(UART5); //��ȡ���յ�������
		s_datum = UART5->DR&0xFF;
		if (UART5_RX_BUFFER_SIZE > g_uart5_buffer.rx_count)
		{
			g_uart5_buffer.rx_buf[g_uart5_buffer.rx_head] = s_datum;
			if (UART5_RX_BUFFER_SIZE == ++g_uart5_buffer.rx_head) {g_uart5_buffer.rx_head = 0;}
			g_uart5_buffer.rx_count++;
		}
    }
//--------------------------------------------------------------------------------
	//if (__HAL_UART_GET_FLAG(&UART5, UART_FLAG_TXE)!=RESET) //UART5�����ж�
	//if ((UART5->SR&UART_FLAG_TXE) == UART_FLAG_TXE) //UART5�����ж�
	if ((UART4->SR&UART_FLAG_TC) == UART_FLAG_TC) //UART5�����ж�
	{
		if (g_uart5_buffer.tx_count > 0)
		{
			//USART_SendData(UART5, g_uart5_buffer->tx_buf[g_uart5_buffer->tx_tail])
			UART5->DR = g_uart5_buffer.tx_buf[g_uart5_buffer.tx_tail];
			if (UART5_TX_BUFFER_SIZE == ++g_uart5_buffer.tx_tail) {g_uart5_buffer.tx_tail = 0;}
			g_uart5_buffer.tx_count--;
		} else 
		{
			//__HAL_UART_DISABLE_IT(&UART5, USART_IT_TXE); //�رշ����ж�
			UART5->CR1 &= ~USART_CR1_TXEIE; //�رշ����ж�
			#ifdef UART5_USE_485 //485ʹ�ܽ�
			_delay_us(11000000/9600);
			uart5_use_485_off(); //485��ȡ
			#endif
		} 
	}
//--------------------------------------------------------------------------------
	//if (__HAL_UART_GET_FLAG(&UART5, UART_FLAG_IDLE)!=RESET) //UART5�����ж�
	if ((UART5->SR&UART_FLAG_IDLE) == UART_FLAG_IDLE) //UART5�����ж�
	{
		uart5_event = 1;
		s_datum = UART5->SR;
		s_datum = UART5->DR;
	}
//--------------------------------------------------------------------------------
//	if (__HAL_UART_GET_FLAG(UART5, USART_IT_PE|USART_IT_FE|USART_IT_NE) != RESET) //����
//	if ((UART5->SR&(USART_FLAG_ORE|USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) != NULL)
//	{
//		MX_UART5_Init();
//	}
}
/*----------------------------------------------------------------------------*/
#endif
