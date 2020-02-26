/*
#include "stm32f1xx_usart2.h" //串口2
	MX_USART2_Init(); //串口2初始化
	uint32_t = MX_USART2_input((uint8_t *)&*p_datum, p_length); //读取数据 返回长度
	MX_USART2_output((uint8_t *)&*p_datum, p_length); //发送数据
	
作者:刘东辉
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_usart2.h"
/* 私有宏定义 ----------------------------------------------------------------*/
//	#define USE_PRINTING //使用调试模式
//	#define USART2_USE_485 //使用485方式

	#define USART2_USE_PA2_PA3 //使用PA2/PA3
//	#define USART2_USE_PD5_PD6 //使用PD5/PD6
#ifndef USE_PRINTING
#define USART2_RX_BUFFER_SIZE 256 //接收缓冲长度
#define USART2_TX_BUFFER_SIZE 256 //发送缓冲长度
#ifdef USART2_USE_485 //485
#define usart2_use_485_gpio		GPIOC
#define usart2_use_485_pin		GPIO_Pin_13
#define usart2_use_485_off()	HAL_GPIO_WritePin(usart2_use_485_gpio, usart2_use_485_pin, GPIO_PIN_RESET) //低
#define usart2_use_485_on()		HAL_GPIO_WritePin(usart2_use_485_gpio, usart2_use_485_pin, GPIO_PIN_SET) //高
#endif
/* 私有类型定义 --------------------------------------------------------------*/
typedef struct
{
	uint32_t rx_head; //数据头
	uint32_t rx_tail; //数据尾
	uint32_t rx_count; //数据长度
	uint8_t rx_buf[USART2_RX_BUFFER_SIZE]; //缓冲

	uint32_t tx_head;
	uint32_t tx_tail;
	uint32_t tx_count;
	uint8_t tx_buf[USART2_TX_BUFFER_SIZE];
} USART2_BUFFER; //队列数据结构
/* 私有变量 ------------------------------------------------------------------*/
static USART2_BUFFER g_usart2_buffer; //缓冲
#endif
//UART_HandleTypeDef huart2;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
void MX_USART2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	UART_HandleTypeDef huart2;
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	__HAL_RCC_USART2_CLK_DISABLE(); // 串口外设时钟禁用
#ifdef USART2_USE_PA2_PA3
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3); // 串口外设功能GPIO配置
#endif
#ifdef USART2_USE_PD5_PD6
	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5|GPIO_PIN_6); // 串口外设功能GPIO配置
#endif
	HAL_NVIC_DisableIRQ(USART2_IRQn); // 串口中断禁用
#endif
	__HAL_RCC_USART2_CLK_ENABLE();  //USART2时钟使能
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

//485使能脚
#ifdef USART2_USE_485 //485使能脚
	GPIO_InitStructure.GPIO_Pin = usart2_use_485_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init(usart2_use_485_gpio, &GPIO_InitStructure); //
	usart2_use_485_off(); //485读取
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
	//__HAL_UART_ENABLE_IT(&huart2, USART_IT_TXE); //发送中断
	__HAL_UART_ENABLE_IT(&huart2, USART_IT_RXNE); //接收中断
	__HAL_UART_ENABLE_IT(&huart2, USART_IT_IDLE); //空闲中断
#endif
//	__HAL_UART_ENABLE_IT(&huart2, USART_IT_ERR);
//--------------------------------------------------------------------------------
#ifndef USE_PRINTING
//数据缓冲区
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
		printf("< %s %s >\r\n", __DATE__, __TIME__); //显示 日期  时间
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
  * 函数功能: 重定向c库函数printf到DEBUG_USART2
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
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USART2
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
//从接收缓冲区中提取字符
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
		} else {break;} //数据长度为0时跳出
	}
	return s_i;
}
/*----------------------------------------------------------------------------*/
//将数据放入发送缓冲区
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
	#ifdef USART2_USE_485 //485使能脚
	usart2_use_485_on(); //485发送
	#endif
	//__HAL_UART_ENABLE_IT(&usart2, USART_IT_TXE); //允许串口2发送中断
	USART2->CR1 |= USART_CR1_TXEIE;
}
/*----------------------------------------------------------------------------*/
void USART2_IRQHandler(void) //串口2中断服务程序
{
	uint8_t s_datum;
	//if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE)!=RESET) //USART2接收中断
	if ((USART2->SR&UART_FLAG_RXNE) == UART_FLAG_RXNE) //USART2接收中断
	{
		//s_datum = USART_ReceiveData(USART2); //读取接收到的数据
		s_datum = USART2->DR&0xFF;
		if (USART2_RX_BUFFER_SIZE > g_usart2_buffer.rx_count)
		{
			g_usart2_buffer.rx_buf[g_usart2_buffer.rx_head] = s_datum;
			if (USART2_RX_BUFFER_SIZE == ++g_usart2_buffer.rx_head) {g_usart2_buffer.rx_head = 0;}
			g_usart2_buffer.rx_count++;
		}
    }
//--------------------------------------------------------------------------------
	//if (__HAL_UART_GET_FLAG(&usart2, UART_FLAG_TXE)!=RESET) //USART2发送中断
	//if ((USART2->SR&UART_FLAG_TXE) == UART_FLAG_TXE) //USART2发送中断
	if ((USART2->SR&UART_FLAG_TC) == UART_FLAG_TC) //USART2发送中断
	{
		if (g_usart2_buffer.tx_count > 0)
		{
			//USART_SendData(USART2, g_usart2_buffer->tx_buf[g_usart2_buffer->tx_tail])
			USART2->DR = g_usart2_buffer.tx_buf[g_usart2_buffer.tx_tail];
			if (USART2_TX_BUFFER_SIZE == ++g_usart2_buffer.tx_tail) {g_usart2_buffer.tx_tail = 0;}
			g_usart2_buffer.tx_count--;
		} else 
		{
			//__HAL_UART_DISABLE_IT(&usart2, USART_IT_TXE); //关闭发送中断
			USART2->CR1 &= ~USART_CR1_TXEIE; //关闭发送中断
			#ifdef USART2_USE_485 //485使能脚
			_delay_us(11000000/115200);
			usart2_use_485_off(); //485读取
			#endif
		}
		//printf("\r\nOK\r\n");
	}
//--------------------------------------------------------------------------------
	//if (__HAL_UART_GET_FLAG(&usart2,UART_FLAG_IDLE)!=RESET) //USART2空闲中断
	if ((USART2->SR&UART_FLAG_IDLE) == UART_FLAG_IDLE) //USART2空闲中断
	{
		usart2_event = 1;
		s_datum = USART2->SR;
		s_datum = USART2->DR;
	}
//--------------------------------------------------------------------------------
//	if (__HAL_UART_GET_FLAG(USART2, USART_IT_PE|USART_IT_FE|USART_IT_NE) != RESET) //出错
//	if ((USART2->SR&(USART_FLAG_ORE|USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) != NULL)
//	{
//		MX_USART2_Init();
//	}
}
/*----------------------------------------------------------------------------*/
#endif
