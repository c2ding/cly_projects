/*

作者:刘东辉
*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_usart1.h"
//#include "stm32f1xx_usart2.h"
#include "stm32f1xx_usart3.h"
//#include "stm32f1xx_uart4.h"
//#include "stm32f1xx_uart5.h"
//#include "stm32f1xx_can.h"
//#include "stm32f1xx_i2c1.h"
//#include "stm32f1xx_i2c2.h"
//#include "stm32f1xx_spi1.h"
//#include "stm32f1xx_spi2.h"
//#include "stm32f1xx_spi3.h"
//#include "stm32f1xx_tim1.h"
//#include "stm32f1xx_tim2.h"
//#include "stm32f1xx_tim3.h"
//#include "stm32f1xx_tim4.h"
//#include "stm32f1xx_tim5.h"
//#include "stm32f1xx_tim6.h"
//#include "stm32f1xx_tim7.h"
//#include "stm32f1xx_tim8.h"
//#include "stm32f1xx_flash.h"
//#include "stm32f1xx_adc1.h"
//#include "stm32f1xx_adc2.h"
//#include "stm32f1xx_adc3.h"

#include "stm32f1xx_delay.h"
#include "stm32f1xx_system_led.h"

#include "entrance.h"
#include "application.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
__IO _SYSTEM_STATUS g_system;

RTC_HandleTypeDef RtcHandle;
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 系统时钟配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	HAL_StatusTypeDef s_error = HAL_OK;

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE; // 外部晶振，8MHz
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS; //RCC_HSE_OFF:关时钟 / RCC_HSE_ON:开晶体 / RCC_HSE_BYPASS:外部时钟(有源晶振)
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON; //内部高速时钟
	//RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9; // 9倍频,得到72MHz主时钟
	//RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
	s_error |= HAL_RCC_OscConfig(&RCC_OscInitStruct);


	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // 系统时钟：72MHz
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // AHB时钟:72MHz
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2; // APB1时钟:36MHz
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; // APB2时钟:72MHz
	s_error |= HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

 	// HAL_RCC_GetHCLKFreq()/1000    1ms中断一次
	// HAL_RCC_GetHCLKFreq()/100000	 10us中断一次
	// HAL_RCC_GetHCLKFreq()/1000000 1us中断一次
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000/8);  // 配置并启动系统滴答定时器
	/* 系统滴答定时器时钟源 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);
	/* 系统滴答定时器中断优先级配置 */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

	if (s_error != HAL_OK)
	{
		Soft_Reset(); // 复位
	}
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 独立看门狗初始化配置
  * 溢出时间(毫秒) = hiwdg.Init.Prescaler/40 * hiwdg.Init.Reload 
  * hiwdg.Init.Prescaler : (4,8,16,32,64,128,256)
  * hiwdg.Init.Reload < 0xFFF
  */
IWDG_HandleTypeDef hiwdg;
void IWDG_Configuration(void) //看门狗配置
{
	HAL_StatusTypeDef s_error = HAL_OK;
	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_256; //时钟分频
	hiwdg.Init.Reload = 0x0FFF; //计数
	s_error |= HAL_IWDG_Init(&hiwdg); //初始化看门狗

#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("IWDG_Configuration(); OK\r\n"); 
	} else
	{
		printf("IWDG_Configuration(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
//初始化
void initialization(void)
{
	HAL_Init(); // 复位所有外设，初始化Flash接口和系统滴答定时器
	SystemClock_Config(); // 配置系统时钟
	
	__HAL_AFIO_REMAP_PD01_ENABLE(); //PD1做IO口用
//端口
#ifdef __HAL_RCC_GPIOA_CLK_ENABLE
	__HAL_RCC_GPIOA_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_GPIOB_CLK_ENABLE
	__HAL_RCC_GPIOB_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_GPIOC_CLK_ENABLE
	__HAL_RCC_GPIOC_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_GPIOD_CLK_ENABLE
	__HAL_RCC_GPIOD_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_GPIOE_CLK_ENABLE
	__HAL_RCC_GPIOE_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_GPIOF_CLK_ENABLE
	__HAL_RCC_GPIOF_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_GPIOG_CLK_ENABLE
	__HAL_RCC_GPIOG_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_GPIOH_CLK_ENABLE
	__HAL_RCC_GPIOH_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_GPIOI_CLK_ENABLE
	__HAL_RCC_GPIOI_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_GPIOJ_CLK_ENABLE
	__HAL_RCC_GPIOJ_CLK_ENABLE();
#endif
#ifdef __HAL_RCC_GPIOK_CLK_ENABLE
	__HAL_RCC_GPIOK_CLK_ENABLE();
#endif
	
//外设
//调试用 放在第一个启动
#ifdef __STM32F1XX_USART1_H__
	MX_USART1_Init();
#endif
#ifdef __STM32F1XX_USART2_H__
	MX_USART2_Init();
#endif
#ifdef __STM32F1XX_USART3_H__
	MX_USART3_Init();
#endif
#ifdef __STM32F1XX_UART4_H__
	MX_UART4_Init();
#endif
#ifdef __STM32F1XX_UART5_H__
	MX_UART5_Init();
#endif
#ifdef __STM32F1XX_CAN_H__
	MX_CAN1_Init(CAN_BAND_125);
#ifdef USE_CAN2
	MX_CAN2_Init(CAN_BAND_125);
#endif
#endif
#ifdef __STM32F1XX_I2C1_H__
	MX_I2C1_Init();
#endif
#ifdef __STM32F1XX_I2C2_H__
	MX_I2C2_Init();
#endif
#ifdef __STM32F1XX_SPI1_H__
	MX_SPI1_Init();
#endif
#ifdef __STM32F1XX_SPI2_H__
	MX_SPI2_Init();
#endif
#ifdef __STM32F1XX_SPI3_H__
	MX_SPI3_Init();
#endif
#ifdef __STM32F1XX_TIM1_H__
	MX_TIM1_Init();
#endif
#ifdef __STM32F1XX_TIM2_H__
	MX_TIM2_Init();
#endif
#ifdef __STM32F1XX_TIM3_H__
	MX_TIM3_Init();
#endif
#ifdef __STM32F1XX_TIM4_H__
	MX_TIM4_Init();
#endif
#ifdef __STM32F1XX_TIM5_H__
	MX_TIM5_Init();
#endif
#ifdef __STM32F1XX_TIM6_H__
	MX_TIM6_Init();
#endif
#ifdef __STM32F1XX_TIM7_H__
	MX_TIM7_Init();
#endif
#ifdef __STM32F1XX_TIM8_H__
	MX_TIM8_Init();
#endif
#ifdef __STM32F1XX_ADC1_H__
	MX_ADC1_Init();
#endif
#ifdef __STM32F1XX_ADC2_H__
	MX_ADC2_Init();
#endif
#ifdef __STM32F1XX_ADC3_H__
	MX_ADC3_Init();
#endif
//	IWDG_Configuration();

//扩展
	system_led_init(); //运行指示灯
	power_out_init();

//数据

#if 0
	//if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) {printf("引脚复位\r\n");} 
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) {printf("上电复位\r\n");} 
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {printf("软件复位\r\n");} 
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {printf("独立看门狗复位\r\n");} 
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) {printf("窗口看门狗复位\r\n");} 
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) {printf("低电量复位\r\n");} 
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {g_datum.switcher = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1);} //软件复位
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {g_datum.switcher = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1);} //独立看门狗复位
	__HAL_RCC_CLEAR_RESET_FLAGS(); //清除复位源标志位
#endif
//运行模式选择

	memset((uint8_t*)&work, 0x00, sizeof(work));

	g_system.event.whole = 0; //任务 事件
//	g_system.button_datum = 0; //按键 数据
	g_system.working_state = STANBDY; //工作状态 待机
	g_system.time_SysTick = 0;

#ifdef USE_FULL_ASSERT
	//printf("初始化完成 按空格键 继续\r\n");
	//while (getchar()!=' ') {;}
	printf("Initialization complete\r\n");
#endif
	IWDG_Configuration();
	//__enable_irq(); // 开启总中断
}
/*----------------------------------------------------------------------------*/
const uint8_t git_power[]={"0123456789"};
//运行
void function(void)
{

	for (;;)
	{
		if (second_event != NULL) //秒
		{
			HAL_IWDG_Refresh(&hiwdg); //喂狗
			control();
			second_event = NULL;
		}
		else if (millisecond_event != NULL) //100mS
		{
			system_led();
			millisecond_event = NULL;
		}
		#ifdef __STM32F1XX_USART3_H__
		else if (usart3_event != NULL) //usart3 (GPRS接收数据处理)
		{
			uint32_t s_length;
			s_length = MX_USART3_input((uint8_t *)&*work.in_buffer, 255), work.in_buffer[s_length] = '\0';
			data_collection();
			
			//MX_USART3_output((uint8_t *)&*sim7600_rx_buffer);
			usart3_event = NULL;
		}
		#endif
		//else {while (whole_event == NULL) {HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);}} //再次烧写会出问题
		else {whole_event = NULL; while (whole_event == NULL) {;}} //调试时用
	}
}
/*----------------------------------------------------------------------------*/
int main(void) {initialization(); function(); return 0;}
/*----------------------------------------------------------------------------*/
/**
  *	用户可以选择是否使能HAL库的断言供能。使能断言的方法有两种：
  * (1) 在C编译器的预定义宏选项中定义USE_FULL_ASSERT。
  *	(2) 在stm32f1xx_hal_conf.h文件取消"#define USE_FULL_ASSERT    1"行的注释。	
  * 函数功能: 断言失败服务函数
  * 输入参数: file : 源代码文件名称。关键字__FILE__表示源代码文件名。
  *           line ：代码行号。关键字 __LINE__ 表示源代码行号
  */
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
	printf("\r\nWrong parameters value: file %s on line %d\r\n", file, line);
	//printf("\n\r Wrong parameter value detected on\r\n");
	//printf("       file  %s\r\n", file);
	//printf("       line  %d\r\n", line);
	for (;;) {;}
}
#endif
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 系统软件复位
  */
void Soft_Reset(void)
{
	__disable_irq(); // 关闭总中断
	NVIC_SystemReset(); // 系统复位
}
/*----------------------------------------------------------------------------*/
