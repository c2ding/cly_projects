/*

����:������
*/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
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
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
__IO _SYSTEM_STATUS g_system;

RTC_HandleTypeDef RtcHandle;
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ϵͳʱ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	HAL_StatusTypeDef s_error = HAL_OK;

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE; // �ⲿ����8MHz
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS; //RCC_HSE_OFF:��ʱ�� / RCC_HSE_ON:������ / RCC_HSE_BYPASS:�ⲿʱ��(��Դ����)
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON; //�ڲ�����ʱ��
	//RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9; // 9��Ƶ,�õ�72MHz��ʱ��
	//RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
	s_error |= HAL_RCC_OscConfig(&RCC_OscInitStruct);


	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // ϵͳʱ�ӣ�72MHz
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // AHBʱ��:72MHz
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2; // APB1ʱ��:36MHz
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; // APB2ʱ��:72MHz
	s_error |= HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

 	// HAL_RCC_GetHCLKFreq()/1000    1ms�ж�һ��
	// HAL_RCC_GetHCLKFreq()/100000	 10us�ж�һ��
	// HAL_RCC_GetHCLKFreq()/1000000 1us�ж�һ��
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000/8);  // ���ò�����ϵͳ�δ�ʱ��
	/* ϵͳ�δ�ʱ��ʱ��Դ */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);
	/* ϵͳ�δ�ʱ���ж����ȼ����� */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

	if (s_error != HAL_OK)
	{
		Soft_Reset(); // ��λ
	}
}
/*----------------------------------------------------------------------------*/
/**
  * ��������: �������Ź���ʼ������
  * ���ʱ��(����) = hiwdg.Init.Prescaler/40 * hiwdg.Init.Reload 
  * hiwdg.Init.Prescaler : (4,8,16,32,64,128,256)
  * hiwdg.Init.Reload < 0xFFF
  */
IWDG_HandleTypeDef hiwdg;
void IWDG_Configuration(void) //���Ź�����
{
	HAL_StatusTypeDef s_error = HAL_OK;
	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_256; //ʱ�ӷ�Ƶ
	hiwdg.Init.Reload = 0x0FFF; //����
	s_error |= HAL_IWDG_Init(&hiwdg); //��ʼ�����Ź�

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
//��ʼ��
void initialization(void)
{
	HAL_Init(); // ��λ�������裬��ʼ��Flash�ӿں�ϵͳ�δ�ʱ��
	SystemClock_Config(); // ����ϵͳʱ��
	
	__HAL_AFIO_REMAP_PD01_ENABLE(); //PD1��IO����
//�˿�
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
	
//����
//������ ���ڵ�һ������
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

//��չ
	system_led_init(); //����ָʾ��
	power_out_init();

//����

#if 0
	//if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) {printf("���Ÿ�λ\r\n");} 
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) {printf("�ϵ縴λ\r\n");} 
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {printf("�����λ\r\n");} 
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {printf("�������Ź���λ\r\n");} 
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) {printf("���ڿ��Ź���λ\r\n");} 
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) {printf("�͵�����λ\r\n");} 
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {g_datum.switcher = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1);} //�����λ
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {g_datum.switcher = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1);} //�������Ź���λ
	__HAL_RCC_CLEAR_RESET_FLAGS(); //�����λԴ��־λ
#endif
//����ģʽѡ��

	memset((uint8_t*)&work, 0x00, sizeof(work));

	g_system.event.whole = 0; //���� �¼�
//	g_system.button_datum = 0; //���� ����
	g_system.working_state = STANBDY; //����״̬ ����
	g_system.time_SysTick = 0;

#ifdef USE_FULL_ASSERT
	//printf("��ʼ����� ���ո�� ����\r\n");
	//while (getchar()!=' ') {;}
	printf("Initialization complete\r\n");
#endif
	IWDG_Configuration();
	//__enable_irq(); // �������ж�
}
/*----------------------------------------------------------------------------*/
const uint8_t git_power[]={"0123456789"};
//����
void function(void)
{

	for (;;)
	{
		if (second_event != NULL) //��
		{
			HAL_IWDG_Refresh(&hiwdg); //ι��
			control();
			second_event = NULL;
		}
		else if (millisecond_event != NULL) //100mS
		{
			system_led();
			millisecond_event = NULL;
		}
		#ifdef __STM32F1XX_USART3_H__
		else if (usart3_event != NULL) //usart3 (GPRS�������ݴ���)
		{
			uint32_t s_length;
			s_length = MX_USART3_input((uint8_t *)&*work.in_buffer, 255), work.in_buffer[s_length] = '\0';
			data_collection();
			
			//MX_USART3_output((uint8_t *)&*sim7600_rx_buffer);
			usart3_event = NULL;
		}
		#endif
		//else {while (whole_event == NULL) {HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);}} //�ٴ���д�������
		else {whole_event = NULL; while (whole_event == NULL) {;}} //����ʱ��
	}
}
/*----------------------------------------------------------------------------*/
int main(void) {initialization(); function(); return 0;}
/*----------------------------------------------------------------------------*/
/**
  *	�û�����ѡ���Ƿ�ʹ��HAL��Ķ��Թ��ܡ�ʹ�ܶ��Եķ��������֣�
  * (1) ��C��������Ԥ�����ѡ���ж���USE_FULL_ASSERT��
  *	(2) ��stm32f1xx_hal_conf.h�ļ�ȡ��"#define USE_FULL_ASSERT    1"�е�ע�͡�	
  * ��������: ����ʧ�ܷ�����
  * �������: file : Դ�����ļ����ơ��ؼ���__FILE__��ʾԴ�����ļ�����
  *           line �������кš��ؼ��� __LINE__ ��ʾԴ�����к�
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
  * ��������: ϵͳ�����λ
  */
void Soft_Reset(void)
{
	__disable_irq(); // �ر����ж�
	NVIC_SystemReset(); // ϵͳ��λ
}
/*----------------------------------------------------------------------------*/
