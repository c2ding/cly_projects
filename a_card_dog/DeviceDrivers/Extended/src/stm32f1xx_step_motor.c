/**
  * 功    能: 57&42型步进电机驱动器STEPMOTOR控制实现
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include <stdbool.h>
#include <math.h>
#include "stm32f1xx_step_motor.h"
#include "stm32f1xx_delay.h"
/* 私有宏定义 ----------------------------------------------------------------*/
#define STEPMOTOR_TIMx                        TIM1
#define STEPMOTOR_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM1_CLK_ENABLE()
#define STEPMOTOR_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM1_CLK_DISABLE()
#define STEPMOTOR_TIM_CHANNEL_x                TIM_CHANNEL_1
#define STEPMOTOR_TIM_IT_CCx                  TIM_IT_CC1
#define STEPMOTOR_TIM_FLAG_CCx                TIM_FLAG_CC1
#define STEPMOTOR_TIMx_IRQn                   TIM1_CC_IRQn
#define STEPMOTOR_TIMx_IRQHandler             TIM1_CC_IRQHandler

//端口
//脉冲输出
// 而PLU-直接接开发板的GND CC1=GPIO_Pin_8 CC2=GPIO_Pin_9 CC3=GPIO_Pin_10 CC4=GPIO_Pin_11 
// 对应STEPMOTOR的PUL+（控制器使用共阴接法）
#define STEP_MOTOR_TIM_PULSE_PORT		GPIOA
#define STEP_MOTOR_TIM_PULSE_PIN		GPIO_PIN_8
#define STEP_MOTOR_TIM_PULSE_ON()		HAL_GPIO_WritePin(STEP_MOTOR_TIM_PULSE_PORT, STEP_MOTOR_TIM_PULSE_PIN, GPIO_PIN_RESET)
#define STEP_MOTOR_TIM_PULSE_OFF()		HAL_GPIO_WritePin(STEP_MOTOR_TIM_PULSE_PORT, STEP_MOTOR_TIM_PULSE_PIN, GPIO_PIN_SET)
//#define STEP_MOTOR_TIM_PULSE_REVERSAL()	HAL_GPIO_TogglePin(STEP_MOTOR_TIM_PULSE_PORT, STEP_MOTOR_TIM_PULSE_PIN); //反转
//方向控制
// 而DIR-直接接开发板的GND
// 对应STEPMOTOR的DIR+（控制器使用共阴接法）
#define STEP_MOTOR_DIR_PULSE_PORT		GPIOB
#define STEP_MOTOR_DIR_PULSE_PIN		GPIO_PIN_15
#define STEP_MOTOR_SETDIR_CW()		HAL_GPIO_WritePin(STEP_MOTOR_DIR_PULSE_PORT, STEP_MOTOR_DIR_PULSE_PIN, GPIO_PIN_RESET)
#define STEP_MOTOR_SETDIR_CCW()		HAL_GPIO_WritePin(STEP_MOTOR_DIR_PULSE_PORT, STEP_MOTOR_DIR_PULSE_PIN, GPIO_PIN_SET)
//#define STEP_MOTOR_TIM_PULSE_REVERSAL()	HAL_GPIO_TogglePin(STEP_MOTOR_DIR_PULSE_PORT, STEP_MOTOR_DIR_PULSE_PIN); //反转
// 定义定时器预分频，定时器实际时钟频率为：72MHz/（STEPMOTOR_TIMx_PRESCALER+1）
#define STEPMOTOR_TIM_PRESCALER			720-1 // 实际时钟频率为：2MHz

// 数学常数
#define ALPHA			((float)(2*3.14159/SPR)) // α= 2*pi/spr
#define A_T_x10			((float)(10*ALPHA*T1_FREQ))
#define T1_FREQ_148		((float)((T1_FREQ*0.676)/10)) // 0.676为误差修正值
#define A_SQ			((float)(2*100000*ALPHA)) 
#define A_x200			((float)(200*ALPHA))
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
TIM_HandleTypeDef htimx_STEPMOTOR;
SpeedRampData srd = {STOP, CW, 0, 0, 0, 0, 0}; // 加减速曲线变量
__IO int32_t  step_position = 0; // 当前位置
__IO bool MotionStatus = false; //是否在运动？false:静止 ture:运动
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

// 函数功能: STEPMOTOR相关GPIO初始化配置
static void step_motor_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //复用推挽输出
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //高速

	/* 步进电机驱动器：脉冲输出 */
	GPIO_InitStruct.Pin = STEP_MOTOR_TIM_PULSE_PIN;
	HAL_GPIO_Init(STEP_MOTOR_TIM_PULSE_PORT, &GPIO_InitStruct);
	HAL_GPIO_LockPin(STEP_MOTOR_TIM_PULSE_PORT, STEP_MOTOR_TIM_PULSE_PIN); //锁定配置

	/* 步进电机驱动器：方向控制 */
	GPIO_InitStruct.Pin = STEP_MOTOR_TIM_PULSE_PIN;
	HAL_GPIO_Init(STEP_MOTOR_TIM_PULSE_PORT, &GPIO_InitStruct);
	HAL_GPIO_LockPin(STEP_MOTOR_TIM_PULSE_PORT, STEP_MOTOR_TIM_PULSE_PIN); //锁定配置

	/* 步进电机驱动器：使能控制 */
	STEP_MOTOR_OUTPUT_DISABLE(); //脱机
	//STEPMOTOR_OUTPUT_ENABLE(); //联机
	GPIO_InitStruct.Pin = STEP_MOTOR_ENA_PIN;
	HAL_GPIO_Init(STEP_MOTOR_ENA_PORT, &GPIO_InitStruct);
	HAL_GPIO_LockPin(STEP_MOTOR_ENA_PORT, STEP_MOTOR_ENA_PIN); //锁定配置

	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //输入
	GPIO_InitStruct.Pull = GPIO_PULLUP; //上拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //低速
	/* 步进电机驱动器：报警输入 */
	
	GPIO_InitStruct.Pin = STEP_MOTOR_ALM_PIN;
	HAL_GPIO_Init(STEP_MOTOR_ALM_PORT, &GPIO_InitStruct);
	HAL_GPIO_LockPin(STEP_MOTOR_ALM_PORT, STEP_MOTOR_ALM_PIN); //锁定配置
	
	/* 步进电机驱动器：就位输入 */
	GPIO_InitStruct.Pin = STEP_MOTOR_PEND_PIN;
	HAL_GPIO_Init(STEP_MOTOR_PEND_PORT, &GPIO_InitStruct);
	HAL_GPIO_LockPin(STEP_MOTOR_PEND_PORT, STEP_MOTOR_PEND_PIN); //锁定配置
}
/*----------------------------------------------------------------------------*/
// 函数功能: 步进电机驱动器定时器初始化
static void step_motor_time_init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig; // 定时器时钟
	TIM_OC_InitTypeDef sConfigOC; // 定时器通道比较输出
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	STEPMOTOR_TIM_RCC_CLK_DISABLE(); // 基本定时器外设时钟禁用
	//HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10); // 基本定时器外设功能GPIO配置
	//HAL_NVIC_DisableIRQ(TIM1_CC_IRQn); // 基本定时器中断禁用
#endif
//--------------------------------------------------------------------------------
	//TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; // 定时器时钟
	//TIM_OCInitTypeDef  TIM_OCInitStructure; // 定时器通道比较输出
	//NVIC_InitTypeDef NVIC_InitStructure; // 定义嵌套向量中断控制器初始化结构体变量

	STEPMOTOR_TIM_RCC_CLK_ENABLE(); // 使能定时器
	
	// 定时器基本环境配置
	htimx_STEPMOTOR.Instance = STEPMOTOR_TIMx; // 定时器编号
	htimx_STEPMOTOR.Init.Prescaler = STEPMOTOR_TIM_PRESCALER; // 定时器预分频器
	htimx_STEPMOTOR.Init.CounterMode = TIM_COUNTERMODE_UP; // 计数方向：向上计数
	//htimx_STEPMOTOR.Init.Period = STEPMOTOR_TIM_PERIOD; // 定时器周期
	htimx_STEPMOTOR.Init.Period = 0xFFFF; //这里必须是0xFFFF
	htimx_STEPMOTOR.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1; // 时钟分频
	s_error |= HAL_TIM_Base_Init(&htimx_STEPMOTOR);
  
	// 定时器时钟源配置
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL; // 使用内部时钟源
	s_error |= HAL_TIM_ConfigClockSource(&htimx_STEPMOTOR, &sClockSourceConfig);

	// 定时器比较输出配置
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE; // 比较输出模式：反转输出
	sConfigOC.Pulse = 0xFFFF; // 脉冲数
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW; // 输出极性
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW; // 当定时器计数值小于CCR1_Val时为(共阳 共阴 接发 需要调整)
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE; // 快速模式
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET; // 空闲电平
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET; // 互补通道空闲电平
	s_error |= HAL_TIM_OC_ConfigChannel(&htimx_STEPMOTOR, &sConfigOC, STEPMOTOR_TIM_CHANNEL_x);
  
	// 使能比较输出通道
	TIM_CCxChannelCmd(STEPMOTOR_TIMx, STEPMOTOR_TIM_CHANNEL_x, TIM_CCx_DISABLE);
  
	// 配置定时器中断优先级并使能
	HAL_NVIC_SetPriority(STEPMOTOR_TIMx_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(STEPMOTOR_TIMx_IRQn);
  
	__HAL_TIM_CLEAR_FLAG(&htimx_STEPMOTOR, STEPMOTOR_TIM_FLAG_CCx);
	// 使能定时器比较输出
	__HAL_TIM_ENABLE_IT(&htimx_STEPMOTOR, STEPMOTOR_TIM_IT_CCx);
	// Enable the main output
	//__HAL_TIM_MOE_ENABLE(&htimx_STEPMOTOR); //启用TIM输出
	__HAL_TIM_MOE_DISABLE(&htimx_STEPMOTOR); //关闭TIM输出
	
	//s_error |= HAL_TIM_Base_Start(&htimx_STEPMOTOR); // 使能定时器
	s_error |= HAL_TIM_Base_Stop(&htimx_STEPMOTOR); // 停止定时器
 
	STEP_MOTOR_OUTPUT_ENABLE(); //脱机关闭
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("step_motor_time_init(); OK\r\n"); 
	} else
	{
		printf("step_motor_time_init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/
void step_motor_init(void)
{
	step_motor_gpio_init();
	step_motor_time_init();
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 相对位置运动：运动给定的步数
  * 输入参数: step：移动的步数 (正数为顺时针，负数为逆时针).
              accel  加速度,实际值为accel*0.1*rad/sec^2
              speed  最大速度,实际值为speed*0.1*rad/sec
              decel  减速度,实际值为decel*0.1*rad/sec^2
  * 返 回 值: 无
  * 说    明: 以给定的步数移动步进电机，先加速到最大速度，然后在合适位置开始
  *           减速至停止，使得整个运动距离为指定的步数。如果加减速阶段很短并且
  *           速度很慢，那还没达到最大速度就要开始减速
  */
void step_motor_move_relative(__IO int32_t p_step, __IO uint32_t p_accel, __IO uint32_t p_speed, __IO uint32_t p_decel)
{
	__IO uint16_t s_tim_count;
	__IO uint32_t s_max_s_lim; // 达到最大速度时的步数
	__IO uint32_t s_accel_lim; // 必须要开始减速的步数（如果加速没有达到最大速度）

	if (p_step < 0) // 步数为负数
	{
		srd.dir = CCW; // 逆时针方向旋转
		STEP_MOTOR_SETDIR_CCW();
		p_step = ~p_step + 1; // 获取步数绝对值
	} else
	{
		srd.dir = CW; // 顺时针方向旋转
		STEP_MOTOR_SETDIR_CW();
	}

	if (p_step == 1) // 步数为1
	{
		srd.accel_count = -1; // 只移动一步
		srd.run_state = DECEL; // 减速状态.
		srd.step_delay = 1000; // 短延时
	}
	else if(p_step != 0) // 如果目标运动步数不为0
	{
		// 我们的驱动器用户手册有详细的计算及推导过程

		// 设置最大速度极限, 计算得到min_delay用于定时器的计数器的值。
		// min_delay = (alpha / tt)/ w
		srd.min_delay = (int32_t)(A_T_x10 / p_speed);

		// 通过计算第一个(c0) 的步进延时来设定加速度，其中accel单位为0.1rad/sec^2
		// step_delay = 1/tt * sqrt(2*alpha/accel)
		// step_delay = ( tfreq*0.676/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100
		srd.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / p_accel))/10);

		// 计算多少步之后达到最大速度的限制
		// max_s_lim = speed^2 / (2*alpha*accel)
		s_max_s_lim = (uint32_t)(p_speed * p_speed / (A_x200*p_accel / 10));
		// 如果达到最大速度小于0.5步，我们将四舍五入为0
		// 但实际我们必须移动至少一步才能达到想要的速度
		if (s_max_s_lim == 0) {s_max_s_lim = 1;}

		// 计算多少步之后我们必须开始减速
		// n1 = (n1 + n2)decel / (accel + decel)
		s_accel_lim = (uint32_t)(p_step * p_decel / (p_accel + p_decel));
		if (s_accel_lim == 0) {s_accel_lim = 1;} // 我们必须加速至少1步才能才能开始减速.

		// 使用限制条件我们可以计算出减速阶段步数
		if (s_accel_lim <= s_max_s_lim) {srd.decel_val = s_accel_lim - p_step;} else {srd.decel_val = -(s_max_s_lim * p_accel / p_decel);}
		   
		if (srd.decel_val == 0) {srd.decel_val = -1;} // 当只剩下一步我们必须减速
		srd.decel_start = p_step + srd.decel_val; // 计算开始减速时的步数
		// 如果最大速度很慢，我们就不需要进行加速运动
		if(srd.step_delay <= srd.min_delay)
		{
			srd.step_delay = srd.min_delay;
			srd.run_state = RUN;
		} else {srd.run_state = ACCEL;}
		srd.accel_count = 0; // 复位加速度计数值
	}
	MotionStatus = true; // 电机为运动状态

	s_tim_count = __HAL_TIM_GET_COUNTER(&htimx_STEPMOTOR);
	__HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR,STEPMOTOR_TIM_CHANNEL_x, (s_tim_count+srd.step_delay)); // 设置定时器比较值
	TIM_CCxChannelCmd(STEPMOTOR_TIMx, STEPMOTOR_TIM_CHANNEL_x, TIM_CCx_ENABLE);// 使能定时器通道 
	
	__HAL_TIM_MOE_ENABLE(&htimx_STEPMOTOR); //启用TIM输出
	HAL_TIM_Base_Start(&htimx_STEPMOTOR); // 使能定时器

	STEP_MOTOR_OUTPUT_ENABLE(); //联机
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 定时器中断服务函数
  * 说    明: 实现加减速过程
  */
void TIM1_CC_IRQHandler(void) // 定时器中断处理
{
	uint16_t s_new_step_delay = 0; // 加速过程中最后一次延时(脉冲周期)
	__IO uint16_t s_tim_count = 0; // 保存新下一个延时周期
	__IO static uint32_t s_last_accel_delay = 0;
	__IO static uint32_t s_step_count = 0; // 总移动步数计数器
	__IO static int32_t s_rest = 0; // 记录new_step_delay中的余数，提高下一步计算的精度
	__IO static uint32_t s_i = 0; // 定时器使用翻转模式，需要进入两次中断才输出一个完整脉冲

	if (__HAL_TIM_GET_IT_SOURCE(&htimx_STEPMOTOR, STEPMOTOR_TIM_IT_CCx) == SET)
	{
		__HAL_TIM_CLEAR_IT(&htimx_STEPMOTOR, STEPMOTOR_TIM_IT_CCx); // 清楚定时器中断
		s_tim_count=__HAL_TIM_GET_COUNTER(&htimx_STEPMOTOR);
		__HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR,STEPMOTOR_TIM_CHANNEL_x, (s_tim_count+srd.step_delay)); // 设置比较值
		if ((++s_i&0x1) == 0) // 2次,说明已经输出一个完整脉冲
		{
			switch (srd.run_state) // 加减速曲线阶段
			{
				case STOP: // 停止
					s_step_count = 0; // 清零步数计数器
					s_rest = 0; // 清零余值
					__HAL_TIM_MOE_ENABLE(&htimx_STEPMOTOR); //启用TIM输出
					HAL_TIM_Base_Start(&htimx_STEPMOTOR); // 使能定时器
					STEP_MOTOR_OUTPUT_DISABLE(); //脱机使能
					MotionStatus = false; // 电机为停止状态
				break;

				case ACCEL: // 加速阶段
					s_step_count++; // 步数加1
					if (srd.dir==CW) {step_position++;} else {step_position--;} // 绝对位置加减1
					srd.accel_count++; // 加速计数值加1
					s_new_step_delay = srd.step_delay - ((2 * srd.step_delay + s_rest)/(4 * srd.accel_count + 1)); // 计算新(下)一步脉冲周期(时间间隔)
					s_rest = (2 * srd.step_delay + s_rest)%(4 * srd.accel_count + 1); // 计算余数，下次计算补上余数，减少误差
					if (s_step_count >= srd.decel_start) // 检查是够应该开始减速
					{
						srd.accel_count = srd.decel_val; // 加速计数值为减速阶段计数值的初始值
						srd.run_state = DECEL; // 下个脉冲进入减速阶段
					}
					else if(s_new_step_delay <= srd.min_delay) // 检查是否到达期望的最大速度
					{
						s_last_accel_delay = s_new_step_delay; // 保存加速过程中最后一次延时（脉冲周期）
						s_new_step_delay = srd.min_delay; // 使用min_delay（对应最大速度speed）
						s_rest = 0; // 清零余值
						srd.run_state = RUN; // 设置为匀速运行状态
					}
				break;

				case RUN: // 匀速阶段
					s_step_count++;  // 步数加1
					if (srd.dir==CW) {step_position++;} else {step_position--;} // 绝对位置加减1
					s_new_step_delay = srd.min_delay; // 使用min_delay（对应最大速度speed）
					if (s_step_count >= srd.decel_start) // 需要开始减速
					{
						srd.accel_count = srd.decel_val; // 减速步数做为加速计数值
						s_new_step_delay = s_last_accel_delay; // 加阶段最后的延时做为减速阶段的起始延时(脉冲周期)
						srd.run_state = DECEL; // 状态改变为减速
					}
				break;

				case DECEL: // 减速阶段
					s_step_count++;  // 步数加1
					if (srd.dir==CW) {step_position++;} else {step_position--;} // 绝对位置加减1
					srd.accel_count++;
					s_new_step_delay = srd.step_delay - ((2 * srd.step_delay + s_rest) / (4 * srd.accel_count + 1)); //计算新(下)一步脉冲周期(时间间隔)
					s_rest = (2 * srd.step_delay + s_rest) % (4 * srd.accel_count + 1); // 计算余数，下次计算补上余数，减少误差
					if(srd.accel_count >= 0)
					{
						__HAL_TIM_MOE_DISABLE(&htimx_STEPMOTOR); //关闭TIM输出
						srd.run_state = STOP;
					} // 检查是否为最后一步
				break;

				default:
					__HAL_TIM_MOE_DISABLE(&htimx_STEPMOTOR); //关闭TIM输出
					HAL_TIM_Base_Stop(&htimx_STEPMOTOR); // 停止定时器
					srd.run_state = STOP;
				break;
			}
			srd.step_delay = s_new_step_delay; // 为下个(新的)延时(脉冲周期)赋值
		}
	}
}


