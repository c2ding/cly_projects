#ifndef __STM32F1XX_STEPMOTOR_TIM_H__
#define __STM32F1XX_STEPMOTOR_TIM_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* �궨�� --------------------------------------------------------------------*/
//�ѻ�ʹ��
// ���ʹ�ܿ��ƣ�������ղ���Ĭ��ʹ�ܵ��
// ��ENA-ֱ�ӽӿ������GND 
// ��ӦSTEPMOTOR��ENA+��������ʹ�ù����ӷ���
#define STEP_MOTOR_ENA_PORT			GPIOB
#define STEP_MOTOR_ENA_PIN			GPIO_PIN_14
#define STEP_MOTOR_OUTPUT_ENABLE()	HAL_GPIO_WritePin(STEP_MOTOR_ENA_PORT, STEP_MOTOR_ENA_PIN, GPIO_PIN_RESET)
#define STEP_MOTOR_OUTPUT_DISABLE()	HAL_GPIO_WritePin(STEP_MOTOR_ENA_PORT, STEP_MOTOR_ENA_PIN, GPIO_PIN_SET)
//#define STEP_MOTOR_ENA_REVERSAL()	HAL_GPIO_TogglePin(STEP_MOTOR_ENA_PORT, STEP_MOTOR_ENA_PIN); //��ת
//��������
#define STEP_MOTOR_ALM_PORT			GPIOB
#define STEP_MOTOR_ALM_PIN			GPIO_PIN_12
#define STEP_MOTOR_ALM_ON()			HAL_GPIO_WritePin(STEP_MOTOR_ALM_PORT, STEP_MOTOR_ALM_PIN, GPIO_PIN_RESET)
#define STEP_MOTOR_ALM_OFF()		HAL_GPIO_WritePin(STEP_MOTOR_ALM_PORT, STEP_MOTOR_ALM_PIN, GPIO_PIN_SET)
//#define STEP_MOTOR_ALM_REVERSAL()	HAL_GPIO_TogglePin(STEP_MOTOR_ALM_PORT, STEP_MOTOR_ALM_PIN); //��ת
//��λ����
#define STEP_MOTOR_PEND_PORT		GPIOB
#define STEP_MOTOR_PEND_PIN			GPIO_PIN_13
#define STEP_MOTOR_PEND_ON()		HAL_GPIO_WritePin(STEP_MOTOR_PEND_PORT, STEP_MOTOR_PEND_PIN, GPIO_PIN_RESET)
#define STEP_MOTOR_PEND_OFF()		HAL_GPIO_WritePin(STEP_MOTOR_PEND_PORT, STEP_MOTOR_PEND_PIN, GPIO_PIN_SET)
//#define STEP_MOTOR_PEND_REVERSAL()	HAL_GPIO_TogglePin(STEP_MOTOR_PEND_PORT, STEP_MOTOR_PEND_PIN); //��ת
/*----------------------------------------------------------------------------*/
#define CW		0 // ˳ʱ��
#define CCW		1 // ��ʱ��

#define STOP	0 // ֹͣ
#define ACCEL	1 // ���ٽ׶�
#define DECEL	2 // ���ٽ׶�
#define RUN		3 // ���ٽ׶�

#define T1_FREQ		(SystemCoreClock/(STEPMOTOR_TIM_PRESCALER+1)) // Ƶ��ftֵ
#define FSPR		200 // ���������Ȧ����
#define MICRO_STEP	10 // �������������ϸ����
#define SPR			(FSPR*MICRO_STEP) // ��תһȦ��Ҫ��������

// ���� ���� ���� 
#define mm_to_step(x)	((int32_t)(((x*SPR*50)/942+0.5)/10)) //�� 130����
/* ���Ͷ��� ------------------------------------------------------------------*/
typedef struct
{
	__IO uint8_t	run_state ;  // �����ת״̬
	__IO uint8_t	dir ;        // �����ת����
	__IO int32_t	step_delay;  // �¸���������(ʱ����),����ʱΪ���ٶ�
	__IO uint32_t	decel_start; // ��������λ��
	__IO int32_t	decel_val;   // ���ٽ׶β���
	__IO int32_t	min_delay;   // ��С��������(����ٶ�,�����ٶ��ٶ�)
	__IO int32_t	accel_count; // �Ӽ��ٽ׶μ���ֵ
}SpeedRampData;

/* ��չ���� ------------------------------------------------------------------*/
extern SpeedRampData srd;
extern __IO int32_t step_position; // ��ǰλ��
extern __IO bool MotionStatus; //�Ƿ����˶���false:ֹͣ, ture:�˶�
/* �������� ------------------------------------------------------------------*/
extern void step_motor_init(void);
extern void step_motor_move_relative(int32_t, uint32_t, uint32_t, uint32_t); //����˶�
#define step_motor_move_absolutely(p_step, p_accel, p_decel, p_speed)\
step_motor_move_relative((p_step - step_position), p_accel, p_decel, p_speed) //�����˶�
#endif
