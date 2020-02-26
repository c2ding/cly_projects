#ifndef __STM32F1XX_STEPMOTOR_TIM_H__
#define __STM32F1XX_STEPMOTOR_TIM_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* 宏定义 --------------------------------------------------------------------*/
//脱机使能
// 电机使能控制，如果悬空不接默认使能电机
// 而ENA-直接接开发板的GND 
// 对应STEPMOTOR的ENA+（控制器使用共阴接法）
#define STEP_MOTOR_ENA_PORT			GPIOB
#define STEP_MOTOR_ENA_PIN			GPIO_PIN_14
#define STEP_MOTOR_OUTPUT_ENABLE()	HAL_GPIO_WritePin(STEP_MOTOR_ENA_PORT, STEP_MOTOR_ENA_PIN, GPIO_PIN_RESET)
#define STEP_MOTOR_OUTPUT_DISABLE()	HAL_GPIO_WritePin(STEP_MOTOR_ENA_PORT, STEP_MOTOR_ENA_PIN, GPIO_PIN_SET)
//#define STEP_MOTOR_ENA_REVERSAL()	HAL_GPIO_TogglePin(STEP_MOTOR_ENA_PORT, STEP_MOTOR_ENA_PIN); //反转
//报警输入
#define STEP_MOTOR_ALM_PORT			GPIOB
#define STEP_MOTOR_ALM_PIN			GPIO_PIN_12
#define STEP_MOTOR_ALM_ON()			HAL_GPIO_WritePin(STEP_MOTOR_ALM_PORT, STEP_MOTOR_ALM_PIN, GPIO_PIN_RESET)
#define STEP_MOTOR_ALM_OFF()		HAL_GPIO_WritePin(STEP_MOTOR_ALM_PORT, STEP_MOTOR_ALM_PIN, GPIO_PIN_SET)
//#define STEP_MOTOR_ALM_REVERSAL()	HAL_GPIO_TogglePin(STEP_MOTOR_ALM_PORT, STEP_MOTOR_ALM_PIN); //反转
//就位输入
#define STEP_MOTOR_PEND_PORT		GPIOB
#define STEP_MOTOR_PEND_PIN			GPIO_PIN_13
#define STEP_MOTOR_PEND_ON()		HAL_GPIO_WritePin(STEP_MOTOR_PEND_PORT, STEP_MOTOR_PEND_PIN, GPIO_PIN_RESET)
#define STEP_MOTOR_PEND_OFF()		HAL_GPIO_WritePin(STEP_MOTOR_PEND_PORT, STEP_MOTOR_PEND_PIN, GPIO_PIN_SET)
//#define STEP_MOTOR_PEND_REVERSAL()	HAL_GPIO_TogglePin(STEP_MOTOR_PEND_PORT, STEP_MOTOR_PEND_PIN); //反转
/*----------------------------------------------------------------------------*/
#define CW		0 // 顺时针
#define CCW		1 // 逆时针

#define STOP	0 // 停止
#define ACCEL	1 // 加速阶段
#define DECEL	2 // 减速阶段
#define RUN		3 // 匀速阶段

#define T1_FREQ		(SystemCoreClock/(STEPMOTOR_TIM_PRESCALER+1)) // 频率ft值
#define FSPR		200 // 步进电机单圈步数
#define MICRO_STEP	10 // 步进电机驱动器细分数
#define SPR			(FSPR*MICRO_STEP) // 旋转一圈需要的脉冲数

// 毫米 换算 步数 
#define mm_to_step(x)	((int32_t)(((x*SPR*50)/942+0.5)/10)) //大 130步进
/* 类型定义 ------------------------------------------------------------------*/
typedef struct
{
	__IO uint8_t	run_state ;  // 电机旋转状态
	__IO uint8_t	dir ;        // 电机旋转方向
	__IO int32_t	step_delay;  // 下个脉冲周期(时间间隔),启动时为加速度
	__IO uint32_t	decel_start; // 启动减速位置
	__IO int32_t	decel_val;   // 减速阶段步数
	__IO int32_t	min_delay;   // 最小脉冲周期(最大速度,即匀速段速度)
	__IO int32_t	accel_count; // 加减速阶段计数值
}SpeedRampData;

/* 扩展变量 ------------------------------------------------------------------*/
extern SpeedRampData srd;
extern __IO int32_t step_position; // 当前位置
extern __IO bool MotionStatus; //是否在运动？false:停止, ture:运动
/* 函数声明 ------------------------------------------------------------------*/
extern void step_motor_init(void);
extern void step_motor_move_relative(int32_t, uint32_t, uint32_t, uint32_t); //相对运动
#define step_motor_move_absolutely(p_step, p_accel, p_decel, p_speed)\
step_motor_move_relative((p_step - step_position), p_accel, p_decel, p_speed) //绝对运动
#endif
