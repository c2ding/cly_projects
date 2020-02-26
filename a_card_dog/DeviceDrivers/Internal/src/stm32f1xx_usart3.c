/*
#include "stm32f1xx_usart3.h" //串口3
	MX_USART3_Init(); //串口3初始化
	uint32_t = MX_USART3_input((uint8_t *)&*p_datum, p_length); //读取数据 返回长度
	MX_USART3_output((uint8_t *)&*p_datum, p_length); //发送数据
	
作者:刘东辉
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_usart3.h"
/* 私有宏定义 ----------------------------------------------------------------*/
//	#define USE_PRINTING //使用调试模式
//	#define USART3_USE_485 //使用485方式

	#define USART3_USE_PB10_PB11 //使用B10/PB11
//	#define USART3_USE_PD8_PD9 //使用PD8/PD9
//	#define USART3_USE_PC10_PC11 //使用PC10/PC11
#ifndef USE_PRINTING
#define USART3_RX_BUFFER_SIZE 256 //接收缓冲长度
#define USART3_TX_BUFFER_SIZE 256 //发送缓冲长度
#ifdef USART3_USE_485 //485
#define usart3_use_485_gpio		GPIOC
#define usart3_use_485_pin		GPIO_Pin_13
#define usart3_use_485_off()	HAL_GPIO_WritePin(usart3_use_485_gpio, usart3_use_485_pin, GPIO_PIN_RESET) //低
#define usart3_use_485_on()		HAL_GPIO_WritePin(usart3_use_485_gpio, usart3_use_485_pin, GPIO_PIN_SET) //高
#endif
/* 私有类型定义 --------------------------------------------------------------*/
typedef struct
{
	uint32_t rx_head; //数据头
	uint32_t rx_tail; //数据尾
	uint32_t rx_count; //数据长度
	uint8_t rx_buf[USART3_RX_BUFFER_SIZE]; //缓冲

	uint32_t tx_head;
	uint32_t tx_tail;
	uint32_t tx_count;
	uint8_t tx_buf[USART3_TX_BUFFER_SIZE];
} USART3_BUFFER; //队列数据结构
/* 私有变量 ------------------------------------------------------------------*/
static USART3_BUFFER g_usart3_buffer; //缓冲
#endif
//UART_HandleTypeDef huart3;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
void MX_USART3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	UART_HandleTypeDef huart3;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_USART3_CLK_DISABLE(); // 串口外设时钟禁用
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11); // 串口外设功能GPIO配置
	HAL_NVIC_DisableIRQ(USART3_IRQn); // 串口中断禁用
#endif
	_delay_ms(100);
	__HAL_RCC_USART3_CLK_ENABLE(); //USART3时钟使能
//--------------------------------------------------------------------------------
#ifdef USART3_USE_PB10_PB11
    //PB10     ------> USART3_TX
    //PB11     ------> USART3_RX 
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	//__HAL_AFIO_REMAP_USART3_DISABLE();
#endif
#ifdef USART3_USE_PD8_PD9
	//PD8     ------> USART3_TX
	//PD9     ------> USART3_RX 
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	__HAL_AFIO_REMAP_USART3_ENABLE();
#endif
#if USART3_USE_PC10_PC11
	//PC10     ------> USART3_TX
	//PC11     ------> USART3_RX 
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	__HAL_AFIO_REMAP_USART3_PARTIAL();
#endif

//485使能脚
#ifdef USART3_USE_485 //485使能脚
	GPIO_InitStructure.GPIO_Pin = huart3_use_485_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init(huart3_use_485_gpio, &GPIO_InitStructure); //
	huart3_use_485_off(); //485读取
#endif
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
	HAL_NVIC_SetPriority(USART3_IRQn, 1, 2);
	HAL_NVIC_EnableIRQ(USART3_IRQn);
#endif
//--------------------------------------------------------------------------------
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 9600;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	s_error |= HAL_UART_Init(&huart3);
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
	//__HAL_UART_ENABLE_IT(&huart3, USART_IT_TXE); //发送中断
	__HAL_UART_ENABLE_IT(&huart3, USART_IT_RXNE); //接收中断
	__HAL_UART_ENABLE_IT(&huart3, USART_IT_IDLE); //空闲中断
#endif
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
//数据缓冲区
	g_usart3_buffer.rx_head = 0;
	g_usart3_buffer.rx_tail = 0;
	g_usart3_buffer.rx_count = 0;
	g_usart3_buffer.tx_head = 0;
	g_usart3_buffer.tx_tail = 0;
	g_usart3_buffer.tx_count = 0;
#endif
//--------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		#ifdef USE_PRINTING
		printf("\r\n \\ | / \r\n-  L  -\r\n / | \\ \r\n");
		printf("< %s %s >\r\n", __DATE__, __TIME__); //显示 日期  时间
		#endif
		printf("MX_USART3_Init(); OK\r\n");
	} else
	{
		printf("MX_USART3_Init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
#if defined(USE_FULL_ASSERT) && defined(USE_PRINTING)
//void _sys_exit(int x) {x = x;}
/**
  * 函数功能: 重定向c库函数printf到DEBUG_USART3
  */
int fputc(int ch, FILE *f)
{
	//HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);
	while ((USART3->SR & UART_FLAG_TC) == RESET) {;}
	USART3->DR = (uint8_t)ch;
	return ch;
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USART3
  */
int fgetc(FILE * f)
{
	//uint8_t ch = 0;
	//HAL_UART_Receive(&huart3,&ch, 1, 0xFFFF);
	//return ch;
	while ((USART3->SR & UART_FLAG_RXNE) == RESET) {;}
	return ((int)(USART3->DR&0xFF));
}
#endif
#ifndef USE_PRINTING
/*----------------------------------------------------------------------------*/
//从接收缓冲区中提取字符
uint32_t MX_USART3_input(uint8_t *p_datum, uint32_t p_length)
{
	uint32_t s_i;
	for (s_i=0; s_i<p_length; s_i++)
	{
		if (g_usart3_buffer.rx_count > 0)
		{
			*p_datum++ = g_usart3_buffer.rx_buf[g_usart3_buffer.rx_tail];
			if (USART3_RX_BUFFER_SIZE == ++g_usart3_buffer.rx_tail) {g_usart3_buffer.rx_tail = 0;}
			g_usart3_buffer.rx_count--;
		} else {break;} //数据长度为0时跳出
	}
	return s_i;
}
/*----------------------------------------------------------------------------*/
//将数据放入发送缓冲区
void MX_USART3_output(uint8_t *p_datum, uint32_t p_length)
{
	do {
		if (USART3_TX_BUFFER_SIZE > g_usart3_buffer.tx_count)
		{
			g_usart3_buffer.tx_buf[g_usart3_buffer.tx_head] = *p_datum++;
			if (USART3_TX_BUFFER_SIZE == ++g_usart3_buffer.tx_head) {g_usart3_buffer.tx_head = 0;}
			g_usart3_buffer.tx_count++;
		} else {break;}
	} while (--p_length);
	#ifdef USART3_USE_485 //485使能脚
	usart2_use_485_on(); //485发送
	#endif
	//__HAL_UART_ENABLE_IT(&usart3, USART_IT_TXE); //允许串口2发送中断
	USART3->CR1 |= USART_CR1_TXEIE;
}
/*----------------------------------------------------------------------------*/
void USART3_IRQHandler(void) //串口3中断服务程序
{
	uint8_t s_datum;
	//if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE)!=RESET) //USART3接收中断
	if ((USART3->SR&UART_FLAG_RXNE) == UART_FLAG_RXNE) //USART3接收中断
	{
		//s_datum = USART_ReceiveData(USART2); //读取接收到的数据
		s_datum = USART3->DR&0xFF;
		if (USART3_RX_BUFFER_SIZE > g_usart3_buffer.rx_count)
		{
			g_usart3_buffer.rx_buf[g_usart3_buffer.rx_head] = s_datum;
			if (USART3_RX_BUFFER_SIZE == ++g_usart3_buffer.rx_head) {g_usart3_buffer.rx_head = 0;}
			g_usart3_buffer.rx_count++;
		}
    }
//--------------------------------------------------------------------------------
	//if (__HAL_UART_GET_FLAG(&usart3, UART_FLAG_TXE)!=RESET) //USART3发送中断
	//if ((USART3->SR&UART_FLAG_TXE) == UART_FLAG_TXE) //USART3发送中断
	if ((USART3->SR&UART_FLAG_TC) == UART_FLAG_TC) //USART3发送中断
	{
		if (g_usart3_buffer.tx_count > 0)
		{
			//USART_SendData(USART2, g_usart2_buffer->tx_buf[g_usart2_buffer->tx_tail])
			USART3->DR = g_usart3_buffer.tx_buf[g_usart3_buffer.tx_tail];
			if (USART3_TX_BUFFER_SIZE == ++g_usart3_buffer.tx_tail) {g_usart3_buffer.tx_tail = 0;}
			g_usart3_buffer.tx_count--;
		} else 
		{
			//__HAL_UART_DISABLE_IT(&usart3, USART_IT_TXE); //关闭发送中断
			USART3->CR1 &= ~USART_CR1_TXEIE; //关闭发送中断
			#ifdef USART3_USE_485 //485使能脚
			_delay_us(11000000/115200);
			usart3_use_485_off(); //485读取
			#endif
		}
		//printf("\r\nOK\r\n");
	}
//--------------------------------------------------------------------------------
	//if (__HAL_UART_GET_FLAG(&usart3,UART_FLAG_IDLE)!=RESET) //USART3空闲中断
	if ((USART3->SR&UART_FLAG_IDLE) == UART_FLAG_IDLE) //USART3空闲中断
	{
		usart3_event = 1;
		s_datum = USART3->SR;
		s_datum = USART3->DR;
	}
//--------------------------------------------------------------------------------
//	if (__HAL_UART_GET_FLAG(USART3, USART_IT_PE|USART_IT_FE|USART_IT_NE) != RESET) //出错
//	if ((USART3->SR&(USART_FLAG_ORE|USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) != NULL)
//	{
//		MX_USART3_Init();
//	}
}
/*----------------------------------------------------------------------------*/
#endif
