/*

作者:刘东辉
*/
#ifndef __ENTRANCE_H__
#define __ENTRANCE_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* 宏定义 --------------------------------------------------------------------*/
#define whole_event g_system.event.whole //全部事件
#define second_event g_system.event.each.event0 //秒
#define millisecond_event g_system.event.each.event1 //毫秒
#define button_event g_system.event.each.event2 //按键
#define display_event g_system.event.each.event3 //显示
#define can1_event g_system.event.each.event4 //CAN1
#define can2_event g_system.event.each.event5 //CAN2
#define usart1_event g_system.event.each.event6 //串口1
#define usart2_event g_system.event.each.event7 //串口2
#define usart3_event g_system.event.each.event8 //串口3
#define uart4_event g_system.event.each.event9 //串口4
#define uart5_event g_system.event.each.event10 //串口5


#define OFFSER_BUFFER_FLASH_ADDRESS 0x800F000 // 偏移量 缓冲区 在FLASH中地址
//#define OFFSER_BUFFER_FLASH_LENGTH sizeof(g_time_control)/2 // 偏移量 缓冲区 长度
/* 类型定义 ------------------------------------------------------------------*/
//声明变量
typedef enum
{
	STANBDY = 0, //待机
	FUNCTION = 1, //运行
	FAULT = 2, //故障
	ALERT = 3, //警报
}State_Type;

typedef union {
	struct{uint32_t event0:1, event1:1, event2:1, event3:1, event4:1, event5:1, event6:1, event7:1,
					event8:1, event9:1, event10:1, event11:1, event12:1, event13:1, event14:1, event15:1,
					event16:1, event17:1, event18:1, event19:1, event20:1, event21:1, event22:1, event23:1,
					event24:1, event25:1, event26:1, event27:1, event28:1, event29:1, event30:1, event31:1;
	}each;
	uint32_t whole;
}_TEVENT;
//#pragma pack(1)
typedef struct //
{
	bool equipment_information; //发送设备信息(卡号 经度 纬度)
	uint32_t time_SysTick;
	_TEVENT event; //事件
	State_Type working_state; //工作状态
	//uint32_t button_datum; //按键 数据
	//void (*dialogue_control)(void); //对话函数入口
	//void (*operation_mode)(void); //运行模式
}_SYSTEM_STATUS;
typedef struct //
{
	uint8_t timelist[12][4]; //开关机时间
	bool active; //是否使用时间计划
}_TIME_CONTROL;
/* 扩展变量 ------------------------------------------------------------------*/
extern __IO _SYSTEM_STATUS g_system;
extern IWDG_HandleTypeDef hiwdg;
extern RTC_HandleTypeDef RtcHandle;
/* 函数声明 ------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
extern void Soft_Reset(void);
/*----------------------------------------------------------------------------*/
#endif

