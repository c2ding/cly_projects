/*
#include "stm32f1xx_uart4.h" //����4
	MX_UART4_Init(); //����4��ʼ��
	uint32_t = MX_UART4_input((uint8_t *)&*p_datum, p_length); //��ȡ���� ���س���
	MX_UART4_output((uint8_t *)&*p_datum, p_length); //��������
	
����:������
  */

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_uart4.h"
/* ˽�к궨�� ----------------------------------------------------------------*/
//	#define USE_PRINTING //ʹ�õ���ģʽ
//	#define UART4_USE_485 //ʹ��485��ʽ

#ifndef USE_PRINTING
#define UART4_RX_BUFFER_SIZE 256 //���ջ��峤��
#define UART4_TX_BUFFER_SIZE 256 //���ͻ��峤��
#ifdef UART4_USE_485 //485
#define uart4_use_485_gpio		GPIOA
#define uart4_use_485_pin		GPIO_PIN_15
#define uart4_use_485_off()		HAL_GPIO_WritePin(uart4_use_485_gpio, uart4_use_485_pin, GPIO_PIN_RESET) //��
#define uart4_use_485_on()		HAL_GPIO_WritePin(uart4_use_485_gpio, uart4_use_485_pin, GPIO_PIN_SET) //��
#endif
/* ˽�����Ͷ��� --------------------------------------------------------------*/
typedef struct
{
	uint32_t rx_head; //����ͷ
	uint32_t rx_tail; //����β
	uint32_t rx_count; //���ݳ���
	uint8_t rx_buf[UART4_RX_BUFFER_SIZE]; //����

	uint32_t tx_head;
	uint32_t tx_tail;
	uint32_t tx_count;
	uint8_t tx_buf[UART4_TX_BUFFER_SIZE];
} UART4_BUFFER; //�������ݽṹ
/* ˽�б��� ------------------------------------------------------------------*/
static UART4_BUFFER g_uart4_buffer; //����
#endif
//UART_HandleTypeDef huart4;
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
void MX_UART4_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 
	UART_HandleTypeDef huart4;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_UART4_CLK_DISABLE(); // ��������ʱ�ӽ���
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10); // �������蹦��GPIO����
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_11); // �������蹦��GPIO����
	HAL_NVIC_DisableIRQ(UART4_IRQn); // �����жϽ���
#endif
	__HAL_RCC_UART4_CLK_ENABLE(); //UART4ʱ��ʹ��
//--------------------------------------------------------------------------------
    //PC10     ------> UART4_TX
    //PC11     ------> UART4_RX 
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//485ʹ�ܽ�
#ifdef UART4_USE_485 //485ʹ�ܽ�
	GPIO_InitStruct.Pin = uart4_use_485_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(uart4_use_485_gpio, &GPIO_InitStruct);
	HAL_GPIO_LockPin(uart4_use_485_gpio, uart4_use_485_pin); //��������
	uart4_use_485_off(); //485��ȡ
#endif
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
	HAL_NVIC_SetPriority(UART4_IRQn, 1, 3);
	HAL_NVIC_EnableIRQ(UART4_IRQn);
#endif
//--------------------------------------------------------------------------------
	huart4.Instance = UART4;
	huart4.Init.BaudRate = 115200;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	s_error |= HAL_UART_Init(&huart4);
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
	//__HAL_UART_ENABLE_IT(&huart4, UART_IT_TXE); //�����ж�
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE); //�����ж�
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE); //�����ж�
#endif
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
//���ݻ�����
	g_uart4_buffer.rx_head = 0;
	g_uart4_buffer.rx_tail = 0;
	g_uart4_buffer.rx_count = 0;
	g_uart4_buffer.tx_head = 0;
	g_uart4_buffer.tx_tail = 0;
	g_uart4_buffer.tx_count = 0;
#endif
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		#ifdef USE_PRINTING
		printf("\r\n \\ | / \r\n-  L  -\r\n / | \\ \r\n");
		printf("< %s %s >\r\n", __DATE__, __TIME__); //��ʾ ����  ʱ��
		#endif
		printf("MX_UART4_Init(); OK\r\n");
	} else
	{
		printf("MX_UART4_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
#if defined(USE_FULL_ASSERT) && defined(USE_PRINTING)
//void _sys_exit(int x) {x = x;}
/**
  * ��������: �ض���c�⺯��printf��DEBUG_UART4
  */
int fputc(int ch, FILE *f)
{
	//HAL_UART_Transmit(&huart4, (uint8_t *)&ch, 1, 0xFFFF);
	while ((UART4->SR & UART_FLAG_TC) == RESET) {;}
	UART4->DR = (uint8_t)ch;
	return ch;
}
/*----------------------------------------------------------------------------*/
/**
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_UART4
  */
int fgetc(FILE * f)
{
	//uint8_t ch = 0;
	//HAL_UART_Receive(&huart4,&ch, 1, 0xFFFF);
	//return ch;
	while ((UART4->SR & UART_FLAG_RXNE) == RESET) {;}
	return ((int)(UART4->DR&0xFF));
}
#endif
#ifndef USE_PRINTING
/*----------------------------------------------------------------------------*/
//�ӽ��ջ���������ȡ�ַ�
uint32_t MX_UART4_input(uint8_t *p_datum, uint32_t p_length)
{
	uint32_t s_i;
	for (s_i=0; s_i<p_length; s_i++) //������󳤶�
	{
		if (g_uart4_buffer.rx_count > 0)
		{
			*p_datum++ = g_uart4_buffer.rx_buf[g_uart4_buffer.rx_tail];
			if (UART4_RX_BUFFER_SIZE == ++g_uart4_buffer.rx_tail) {g_uart4_buffer.rx_tail = 0;}
			g_uart4_buffer.rx_count--;
		} else {break;} //���ݳ���Ϊ0ʱ����
	}
	return s_i;
}
/*----------------------------------------------------------------------------*/
//�����ݷ��뷢�ͻ�����
void MX_UART4_output(uint8_t *p_datum, uint32_t p_length)
{
	do {
		if (UART4_TX_BUFFER_SIZE > g_uart4_buffer.tx_count)
		{
			g_uart4_buffer.tx_buf[g_uart4_buffer.tx_head] = *p_datum++;
			if (UART4_TX_BUFFER_SIZE == ++g_uart4_buffer.tx_head) {g_uart4_buffer.tx_head = 0;}
			g_uart4_buffer.tx_count++;
		} else {break;}
	} while (--p_length);
	#ifdef UART4_USE_485 //485ʹ�ܽ�
	uart4_use_485_on(); //485����
	#endif
	//__HAL_UART_ENABLE_IT(&UART4, USART_IT_TXE); //������4�����ж�
	UART4->CR1 |= USART_CR1_TXEIE;
}
/*----------------------------------------------------------------------------*/
void UART4_IRQHandler(void) //����4�жϷ������
{
	uint8_t s_datum;
	//if (__HAL_UART_GET_FLAG(&UART4, UART_FLAG_RXNE)!=RESET) //UART4�����ж�
	if ((UART4->SR&UART_FLAG_RXNE) == UART_FLAG_RXNE) //UART4�����ж�
	{
		//s_datum = USART_ReceiveData(UART4); //��ȡ���յ�������
		s_datum = UART4->DR&0xFF;
		if (UART4_RX_BUFFER_SIZE > g_uart4_buffer.rx_count)
		{
			g_uart4_buffer.rx_buf[g_uart4_buffer.rx_head] = s_datum;
			if (UART4_RX_BUFFER_SIZE == ++g_uart4_buffer.rx_head) {g_uart4_buffer.rx_head = 0;}
			g_uart4_buffer.rx_count++;
		}
    }
//--------------------------------------------------------------------------------
	//if (__HAL_UART_GET_FLAG(&UART4, UART_FLAG_TXE)!=RESET) //UART4�����ж�
	//if ((UART4->SR&UART_FLAG_TXE) == UART_FLAG_TXE) //UART4�����ж�
	if ((UART4->SR&UART_FLAG_TC) == UART_FLAG_TC) //UART4�����ж�
	{
		if (g_uart4_buffer.tx_count > 0)
		{
			//USART_SendData(UART4, g_uart4_buffer->tx_buf[g_uart4_buffer->tx_tail])
			UART4->DR = g_uart4_buffer.tx_buf[g_uart4_buffer.tx_tail];
			if (UART4_TX_BUFFER_SIZE == ++g_uart4_buffer.tx_tail) {g_uart4_buffer.tx_tail = 0;}
			g_uart4_buffer.tx_count--;
		} else 
		{
			//__HAL_UART_DISABLE_IT(&UART4, USART_IT_TXE); //�رշ����ж�
			UART4->CR1 &= ~USART_CR1_TXEIE; //�رշ����ж�
			#ifdef UART4_USE_485 //485ʹ�ܽ�
			_delay_us(11000000/9600);
			uart4_use_485_off(); //485��ȡ
			#endif
		} 
	}
//--------------------------------------------------------------------------------
	//if (__HAL_UART_GET_FLAG(&UART4, UART_FLAG_IDLE)!=RESET) //UART4�����ж�
	if ((UART4->SR&UART_FLAG_IDLE) == UART_FLAG_IDLE) //UART4�����ж�
	{
		uart4_event = 1;
		s_datum = UART4->SR;
		s_datum = UART4->DR;
	}
//--------------------------------------------------------------------------------
//	if (__HAL_UART_GET_FLAG(UART4, USART_IT_PE|USART_IT_FE|USART_IT_NE) != RESET) //����
//	if ((UART4->SR&(USART_FLAG_ORE|USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) != NULL)
//	{
//		MX_UART4_Init();
//	}
}
/*----------------------------------------------------------------------------*/
#endif
