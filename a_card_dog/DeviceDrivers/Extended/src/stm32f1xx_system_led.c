/*

作者:刘东辉
*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_system_led.h"
#include "entrance.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
void system_led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 

	GPIO_InitStruct.Pin = system_led_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(system_led_port, &GPIO_InitStruct);
	HAL_GPIO_LockPin(system_led_port, system_led_pin); //锁定配置
	SYSTEM_LED_OFF();
#ifdef USE_FULL_ASSERT
	printf("system_led_init(); OK\r\n"); 
#endif
}
/*----------------------------------------------------------------------------*/
void system_led(void)
{
	static uint32_t s_time = 0;

	if (s_time==0) {SYSTEM_LED_ON();} else {SYSTEM_LED_OFF();}
	s_time++;
	switch (g_system.working_state)
	{
		case STANBDY: //待机
			if (s_time>=50) {s_time = 0;} //5S
		break;
		case FUNCTION: //运行
			if (s_time>=10) {s_time = 0;} //1S
		break;
		case FAULT: //故障
			if (s_time>=10) {s_time = 0;} //500mS
		break;
		case ALERT: //警报
			if (s_time>=2) {s_time = 0;} //200mS
		break;
		default: break;
	}

}
/*----------------------------------------------------------------------------*/
