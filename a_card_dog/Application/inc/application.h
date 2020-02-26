/*

作者:刘东辉
*/
#ifndef __APPLICATION__
#define __APPLICATION__ 
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* 宏定义 --------------------------------------------------------------------*/
/* 类型定义 ------------------------------------------------------------------*/
typedef struct //
{
	uint8_t in_buffer[256];
	bool start;
	uint32_t time; //时间
	uint32_t start_wait; //启动等待
	uint32_t heartbeat; //心跳
	uint32_t er_h;
	uint32_t er_s;
}_WORK;

typedef struct //
{
	uint32_t start_wait; //启动等待
	uint32_t heartbeat; //心跳
}_PARAMETER;
/* 扩展变量 ------------------------------------------------------------------*/
extern _WORK work;
extern const _PARAMETER parameter;
/* 函数声明 ------------------------------------------------------------------*/
extern void data_collection(void);
extern void control(void);
extern void power_out_init(void); //
extern void u3_printf(const char *fmt, ...); //发送数据
/*----------------------------------------------------------------------------*/

#endif
