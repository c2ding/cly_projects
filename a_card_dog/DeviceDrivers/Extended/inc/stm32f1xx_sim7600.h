/*

作者:刘东辉
*/
#ifndef __STM32F1XX_GSM_H__
#define __STM32F1XX_GSM_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stm32f1xx_hal.h>
#include <stdarg.h>
#include <stdlib.h>
#include "stm32f1xx_usart2.h"
#include "gprs_app.h"
#include "stm32f1xx_delay.h"
#include "entrance.h"
/* 类型定义 ------------------------------------------------------------------*/
typedef enum
{
	GSM_SUCCESS, //成功
	GSM_FAIL, //失败
	GSM_ERROR, //错误
	GSM_TIMEOUT, //超时
} GSM_res_e;

typedef struct {
	bool connect[8]; //连接
//	bool send[8]; //发送
	char ccid[24];
	double longitude; //经度
	double latitude; //纬度
	uint16_t year;	//年
	uint8_t month;	//月
	uint8_t mday;	//日
	uint8_t hour;	//时
	uint8_t min;	//分
	uint8_t sec;	//秒
} _SIM7600;
/* 宏定义 --------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
extern _SIM7600 sim7600;
extern char sim7600_tx_buffer[1024]; //发送缓冲区
extern char sim7600_rx_buffer[1024]; //接收缓冲区
/* 函数声明 ------------------------------------------------------------------*/
extern GSM_res_e waiting_for_startup(void);
extern GSM_res_e sim7600_init(void); //初始化并检测模块
/*************************** sim7600 功能 ***************************/
extern GSM_res_e sim7600_tcp_mode(void); //tcp模式初始化
extern GSM_res_e sim7600_close_tcp_mode(void);
extern GSM_res_e sim7600_tcp_link(char * serverip, char * serverport); //TCP连接
extern GSM_res_e sim7600_send(uint32_t length); //发送数据
extern GSM_res_e sim7600_get_time(void); //获取 时间
extern GSM_res_e sim7600_get_position(void); //获取 位置
/*----------------------------------------------------------------------------*/
#endif
