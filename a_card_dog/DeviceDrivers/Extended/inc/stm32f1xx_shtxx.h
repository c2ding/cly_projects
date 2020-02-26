/*
#include "shtxx.h" //SHTxx系列读取
	shtxx_init();//初始化
	shtxx_data_acquisition(); //采集温湿度数据 (100mS调用)
	
作者:刘东辉
*/

#ifndef __STM32F1XX_SHTXX_H__
#define __STM32F1XX_SHTXX_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stm32f1xx_hal.h>
#include <math.h>
#include "bsp_delay.h"
/* 宏定义 --------------------------------------------------------------------*/
//#define __CALCULATED_DEW_POINT__ //计算 露点
/* 类型定义 ------------------------------------------------------------------*/
typedef struct //
{
	bool error;
	float temperature; //温度
	float humidity; //湿度
#if defined(__CALCULATED_DEW_POINT__)
	float the_dew_point; //露点
#endif
}SHTXX;
/* 扩展变量 ------------------------------------------------------------------*/
extern __IO SHTXX g_shtxx;
/* 函数声明 ------------------------------------------------------------------*/
extern void shtxx_init(void);//初始化
extern void shtxx_data_acquisition(void); //采集温湿度数据
#endif
