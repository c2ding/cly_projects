/*

作者:刘东辉
*/
/* 包含头文件 ----------------------------------------------------------------*/
#ifdef USE_FULL_ASSERT
//#include "stm32f1xx_usart1.h"
#endif
#include "stm32f1xx_delay.h"
//#include "stm32f1xx_delay.h"
//#include "stm32f1xx_time1.h"
#include "stm32f1xx_usart3.h"
//#include "stm32f1xx_uart4.h"
//#include "stm32f1xx_uart5.h"
#include "stm32f1xx_flash.h"

#include "port_bit.h"
#include "stm32f1xx_system_led.h"
//#include "stm32f1xx_led.h"

#include "entrance.h"
#include "application.h"
/* 私有宏定义 ----------------------------------------------------------------*/
#define power_out_port			GPIOB
#define power_out_pin			GPIO_PIN_4
#define POWER_OUT_OFF()			HAL_GPIO_WritePin(power_out_port, power_out_pin, GPIO_PIN_RESET)
#define POWER_OUT_ON()			HAL_GPIO_WritePin(power_out_port, power_out_pin, GPIO_PIN_SET)
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
_WORK work;
const _PARAMETER parameter __attribute__((at(OFFSER_BUFFER_FLASH_ADDRESS))) = {
	.start_wait = 480, //启动等待
	.heartbeat = 60, //心跳
};
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

void data_collection(void) //采集数据
{
	char *str;
	_PARAMETER s_parameter;

	str = strstr((char*)&work.in_buffer, "Heartbeat"); //心跳
	if (str != NULL)
	{
		work.time = 0;
		u3_printf("%d %d\n", work.er_h, work.er_s);
		printf("Heartbeat\r\n");
	}
	str = strstr((char*)&work.in_buffer, "StartOn"); //监控启动
	if (str != NULL)
	{
		work.start = true;
		work.time = 0;
		g_system.working_state = FUNCTION; //工作状态 待机
		printf("StartOn\r\n");
	}
	str = strstr((char*)&work.in_buffer, "StartOff"); //监控停止
	if (str != NULL)
	{
		work.start = false;
		work.time = 0xFFFFFFFF;
		g_system.working_state = STANBDY; //工作状态 待机
		printf("StartOff\r\n");
	}
	str = strstr((char*)&work.in_buffer, "SW_time:"); //参数设置
	if (str != NULL)
	{
		HAL_FLASH_Read(OFFSER_BUFFER_FLASH_ADDRESS, (uint16_t *)&s_parameter, sizeof(s_parameter)/2);
		s_parameter.start_wait = atoi(str + strlen("SW_time:"));
		HAL_FLASH_Write(OFFSER_BUFFER_FLASH_ADDRESS, (uint16_t *)&s_parameter, sizeof(s_parameter)/2);
		printf("SW_time:%d\r\n", parameter.start_wait);
	}
	str = strstr((char*)&work.in_buffer, "HB_time:"); //参数设置
	if (str != NULL)
	{
		HAL_FLASH_Read(OFFSER_BUFFER_FLASH_ADDRESS, (uint16_t *)&s_parameter, sizeof(s_parameter)/2);
		s_parameter.heartbeat = atoi(str + strlen("HB_time:"));
		HAL_FLASH_Write(OFFSER_BUFFER_FLASH_ADDRESS, (uint16_t *)&s_parameter, sizeof(s_parameter)/2);
		printf("HB_time:%d\r\n", parameter.heartbeat);
	}
}
/*----------------------------------------------------------------------------*/
void control(void) //控制
{
	if (work.start == false) //等待开机
	{
		if (work.time==parameter.start_wait) {POWER_OUT_OFF();}
		if (work.time==parameter.start_wait+1) {POWER_OUT_ON();}
		if (work.time==parameter.start_wait+2) {work.start = false, work.time = 0, work.er_s++, printf("er_h:%d; er_s:%d\r\n", work.er_h, work.er_s);}
	} else //启动心跳
	{
		if (work.time==parameter.heartbeat) {POWER_OUT_OFF();}
		if (work.time==parameter.heartbeat+1) {POWER_OUT_ON();}
		if (work.time==parameter.heartbeat+2) {work.start = false, work.time = 0, work.er_h++, g_system.working_state = STANBDY, printf("er_h:%d; er_s:%d\r\n", work.er_h, work.er_s);}
	}
	if (work.time < 0xFFFFFFFF) {work.time++;}
}
/*----------------------------------------------------------------------------*/
void power_out_init(void) //
{
	GPIO_InitTypeDef GPIO_InitStruct; 

	GPIO_InitStruct.Pin = power_out_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(power_out_port, &GPIO_InitStruct);
	HAL_GPIO_LockPin(power_out_port, power_out_pin); //锁定配置
	POWER_OUT_ON();
}
/*----------------------------------------------------------------------------*/
void u3_printf(const char *fmt, ...) //发送数据
{
    va_list ap;
    char string[128];
	uint32_t length;

	va_start(ap, fmt);
	length = vsprintf(string, fmt, ap);
	va_end(ap);
	
	MX_USART3_output((uint8_t*)&string, length);
}
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

