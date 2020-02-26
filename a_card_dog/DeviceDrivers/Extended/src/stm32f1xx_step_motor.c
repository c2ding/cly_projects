/**
  * ��    ��: 57&42�Ͳ������������STEPMOTOR����ʵ��
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include <stdbool.h>
#include <math.h>
#include "stm32f1xx_step_motor.h"
#include "stm32f1xx_delay.h"
/* ˽�к궨�� ----------------------------------------------------------------*/
#define STEPMOTOR_TIMx                        TIM1
#define STEPMOTOR_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM1_CLK_ENABLE()
#define STEPMOTOR_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM1_CLK_DISABLE()
#define STEPMOTOR_TIM_CHANNEL_x                TIM_CHANNEL_1
#define STEPMOTOR_TIM_IT_CCx                  TIM_IT_CC1
#define STEPMOTOR_TIM_FLAG_CCx                TIM_FLAG_CC1
#define STEPMOTOR_TIMx_IRQn                   TIM1_CC_IRQn
#define STEPMOTOR_TIMx_IRQHandler             TIM1_CC_IRQHandler

//�˿�
//�������
// ��PLU-ֱ�ӽӿ������GND CC1=GPIO_Pin_8 CC2=GPIO_Pin_9 CC3=GPIO_Pin_10 CC4=GPIO_Pin_11 
// ��ӦSTEPMOTOR��PUL+��������ʹ�ù����ӷ���
#define STEP_MOTOR_TIM_PULSE_PORT		GPIOA
#define STEP_MOTOR_TIM_PULSE_PIN		GPIO_PIN_8
#define STEP_MOTOR_TIM_PULSE_ON()		HAL_GPIO_WritePin(STEP_MOTOR_TIM_PULSE_PORT, STEP_MOTOR_TIM_PULSE_PIN, GPIO_PIN_RESET)
#define STEP_MOTOR_TIM_PULSE_OFF()		HAL_GPIO_WritePin(STEP_MOTOR_TIM_PULSE_PORT, STEP_MOTOR_TIM_PULSE_PIN, GPIO_PIN_SET)
//#define STEP_MOTOR_TIM_PULSE_REVERSAL()	HAL_GPIO_TogglePin(STEP_MOTOR_TIM_PULSE_PORT, STEP_MOTOR_TIM_PULSE_PIN); //��ת
//�������
// ��DIR-ֱ�ӽӿ������GND
// ��ӦSTEPMOTOR��DIR+��������ʹ�ù����ӷ���
#define STEP_MOTOR_DIR_PULSE_PORT		GPIOB
#define STEP_MOTOR_DIR_PULSE_PIN		GPIO_PIN_15
#define STEP_MOTOR_SETDIR_CW()		HAL_GPIO_WritePin(STEP_MOTOR_DIR_PULSE_PORT, STEP_MOTOR_DIR_PULSE_PIN, GPIO_PIN_RESET)
#define STEP_MOTOR_SETDIR_CCW()		HAL_GPIO_WritePin(STEP_MOTOR_DIR_PULSE_PORT, STEP_MOTOR_DIR_PULSE_PIN, GPIO_PIN_SET)
//#define STEP_MOTOR_TIM_PULSE_REVERSAL()	HAL_GPIO_TogglePin(STEP_MOTOR_DIR_PULSE_PORT, STEP_MOTOR_DIR_PULSE_PIN); //��ת
// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��72MHz/��STEPMOTOR_TIMx_PRESCALER+1��
#define STEPMOTOR_TIM_PRESCALER			720-1 // ʵ��ʱ��Ƶ��Ϊ��2MHz

// ��ѧ����
#define ALPHA			((float)(2*3.14159/SPR)) // ��= 2*pi/spr
#define A_T_x10			((float)(10*ALPHA*T1_FREQ))
#define T1_FREQ_148		((float)((T1_FREQ*0.676)/10)) // 0.676Ϊ�������ֵ
#define A_SQ			((float)(2*100000*ALPHA)) 
#define A_x200			((float)(200*ALPHA))
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
TIM_HandleTypeDef htimx_STEPMOTOR;
SpeedRampData srd = {STOP, CW, 0, 0, 0, 0, 0}; // �Ӽ������߱���
__IO int32_t  step_position = 0; // ��ǰλ��
__IO bool MotionStatus = false; //�Ƿ����˶���false:��ֹ ture:�˶�
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

// ��������: STEPMOTOR���GPIO��ʼ������
static void step_motor_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //�����������
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //����

	/* ���������������������� */
	GPIO_InitStruct.Pin = STEP_MOTOR_TIM_PULSE_PIN;
	HAL_GPIO_Init(STEP_MOTOR_TIM_PULSE_PORT, &GPIO_InitStruct);
	HAL_GPIO_LockPin(STEP_MOTOR_TIM_PULSE_PORT, STEP_MOTOR_TIM_PULSE_PIN); //��������

	/* ���������������������� */
	GPIO_InitStruct.Pin = STEP_MOTOR_TIM_PULSE_PIN;
	HAL_GPIO_Init(STEP_MOTOR_TIM_PULSE_PORT, &GPIO_InitStruct);
	HAL_GPIO_LockPin(STEP_MOTOR_TIM_PULSE_PORT, STEP_MOTOR_TIM_PULSE_PIN); //��������

	/* ���������������ʹ�ܿ��� */
	STEP_MOTOR_OUTPUT_DISABLE(); //�ѻ�
	//STEPMOTOR_OUTPUT_ENABLE(); //����
	GPIO_InitStruct.Pin = STEP_MOTOR_ENA_PIN;
	HAL_GPIO_Init(STEP_MOTOR_ENA_PORT, &GPIO_InitStruct);
	HAL_GPIO_LockPin(STEP_MOTOR_ENA_PORT, STEP_MOTOR_ENA_PIN); //��������

	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //����
	GPIO_InitStruct.Pull = GPIO_PULLUP; //����
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //����
	/* ����������������������� */
	
	GPIO_InitStruct.Pin = STEP_MOTOR_ALM_PIN;
	HAL_GPIO_Init(STEP_MOTOR_ALM_PORT, &GPIO_InitStruct);
	HAL_GPIO_LockPin(STEP_MOTOR_ALM_PORT, STEP_MOTOR_ALM_PIN); //��������
	
	/* �����������������λ���� */
	GPIO_InitStruct.Pin = STEP_MOTOR_PEND_PIN;
	HAL_GPIO_Init(STEP_MOTOR_PEND_PORT, &GPIO_InitStruct);
	HAL_GPIO_LockPin(STEP_MOTOR_PEND_PORT, STEP_MOTOR_PEND_PIN); //��������
}
/*----------------------------------------------------------------------------*/
// ��������: ���������������ʱ����ʼ��
static void step_motor_time_init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig; // ��ʱ��ʱ��
	TIM_OC_InitTypeDef sConfigOC; // ��ʱ��ͨ���Ƚ����
	HAL_StatusTypeDef s_error = HAL_OK;
//--------------------------------------------------------------------------------
#if 0
	STEPMOTOR_TIM_RCC_CLK_DISABLE(); // ������ʱ������ʱ�ӽ���
	//HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10); // ������ʱ�����蹦��GPIO����
	//HAL_NVIC_DisableIRQ(TIM1_CC_IRQn); // ������ʱ���жϽ���
#endif
//--------------------------------------------------------------------------------
	//TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; // ��ʱ��ʱ��
	//TIM_OCInitTypeDef  TIM_OCInitStructure; // ��ʱ��ͨ���Ƚ����
	//NVIC_InitTypeDef NVIC_InitStructure; // ����Ƕ�������жϿ�������ʼ���ṹ�����

	STEPMOTOR_TIM_RCC_CLK_ENABLE(); // ʹ�ܶ�ʱ��
	
	// ��ʱ��������������
	htimx_STEPMOTOR.Instance = STEPMOTOR_TIMx; // ��ʱ�����
	htimx_STEPMOTOR.Init.Prescaler = STEPMOTOR_TIM_PRESCALER; // ��ʱ��Ԥ��Ƶ��
	htimx_STEPMOTOR.Init.CounterMode = TIM_COUNTERMODE_UP; // �����������ϼ���
	//htimx_STEPMOTOR.Init.Period = STEPMOTOR_TIM_PERIOD; // ��ʱ������
	htimx_STEPMOTOR.Init.Period = 0xFFFF; //���������0xFFFF
	htimx_STEPMOTOR.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1; // ʱ�ӷ�Ƶ
	s_error |= HAL_TIM_Base_Init(&htimx_STEPMOTOR);
  
	// ��ʱ��ʱ��Դ����
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL; // ʹ���ڲ�ʱ��Դ
	s_error |= HAL_TIM_ConfigClockSource(&htimx_STEPMOTOR, &sClockSourceConfig);

	// ��ʱ���Ƚ��������
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE; // �Ƚ����ģʽ����ת���
	sConfigOC.Pulse = 0xFFFF; // ������
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW; // �������
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW; // ����ʱ������ֵС��CCR1_ValʱΪ(���� ���� �ӷ� ��Ҫ����)
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE; // ����ģʽ
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET; // ���е�ƽ
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET; // ����ͨ�����е�ƽ
	s_error |= HAL_TIM_OC_ConfigChannel(&htimx_STEPMOTOR, &sConfigOC, STEPMOTOR_TIM_CHANNEL_x);
  
	// ʹ�ܱȽ����ͨ��
	TIM_CCxChannelCmd(STEPMOTOR_TIMx, STEPMOTOR_TIM_CHANNEL_x, TIM_CCx_DISABLE);
  
	// ���ö�ʱ���ж����ȼ���ʹ��
	HAL_NVIC_SetPriority(STEPMOTOR_TIMx_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(STEPMOTOR_TIMx_IRQn);
  
	__HAL_TIM_CLEAR_FLAG(&htimx_STEPMOTOR, STEPMOTOR_TIM_FLAG_CCx);
	// ʹ�ܶ�ʱ���Ƚ����
	__HAL_TIM_ENABLE_IT(&htimx_STEPMOTOR, STEPMOTOR_TIM_IT_CCx);
	// Enable the main output
	//__HAL_TIM_MOE_ENABLE(&htimx_STEPMOTOR); //����TIM���
	__HAL_TIM_MOE_DISABLE(&htimx_STEPMOTOR); //�ر�TIM���
	
	//s_error |= HAL_TIM_Base_Start(&htimx_STEPMOTOR); // ʹ�ܶ�ʱ��
	s_error |= HAL_TIM_Base_Stop(&htimx_STEPMOTOR); // ֹͣ��ʱ��
 
	STEP_MOTOR_OUTPUT_ENABLE(); //�ѻ��ر�
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
  * ��������: ���λ���˶����˶������Ĳ���
  * �������: step���ƶ��Ĳ��� (����Ϊ˳ʱ�룬����Ϊ��ʱ��).
              accel  ���ٶ�,ʵ��ֵΪaccel*0.1*rad/sec^2
              speed  ����ٶ�,ʵ��ֵΪspeed*0.1*rad/sec
              decel  ���ٶ�,ʵ��ֵΪdecel*0.1*rad/sec^2
  * �� �� ֵ: ��
  * ˵    ��: �Ը����Ĳ����ƶ�����������ȼ��ٵ�����ٶȣ�Ȼ���ں���λ�ÿ�ʼ
  *           ������ֹͣ��ʹ�������˶�����Ϊָ���Ĳ���������Ӽ��ٽ׶κ̲ܶ���
  *           �ٶȺ������ǻ�û�ﵽ����ٶȾ�Ҫ��ʼ����
  */
void step_motor_move_relative(__IO int32_t p_step, __IO uint32_t p_accel, __IO uint32_t p_speed, __IO uint32_t p_decel)
{
	__IO uint16_t s_tim_count;
	__IO uint32_t s_max_s_lim; // �ﵽ����ٶ�ʱ�Ĳ���
	__IO uint32_t s_accel_lim; // ����Ҫ��ʼ���ٵĲ������������û�дﵽ����ٶȣ�

	if (p_step < 0) // ����Ϊ����
	{
		srd.dir = CCW; // ��ʱ�뷽����ת
		STEP_MOTOR_SETDIR_CCW();
		p_step = ~p_step + 1; // ��ȡ��������ֵ
	} else
	{
		srd.dir = CW; // ˳ʱ�뷽����ת
		STEP_MOTOR_SETDIR_CW();
	}

	if (p_step == 1) // ����Ϊ1
	{
		srd.accel_count = -1; // ֻ�ƶ�һ��
		srd.run_state = DECEL; // ����״̬.
		srd.step_delay = 1000; // ����ʱ
	}
	else if(p_step != 0) // ���Ŀ���˶�������Ϊ0
	{
		// ���ǵ��������û��ֲ�����ϸ�ļ��㼰�Ƶ�����

		// ��������ٶȼ���, ����õ�min_delay���ڶ�ʱ���ļ�������ֵ��
		// min_delay = (alpha / tt)/ w
		srd.min_delay = (int32_t)(A_T_x10 / p_speed);

		// ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶȣ�����accel��λΪ0.1rad/sec^2
		// step_delay = 1/tt * sqrt(2*alpha/accel)
		// step_delay = ( tfreq*0.676/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100
		srd.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / p_accel))/10);

		// ������ٲ�֮��ﵽ����ٶȵ�����
		// max_s_lim = speed^2 / (2*alpha*accel)
		s_max_s_lim = (uint32_t)(p_speed * p_speed / (A_x200*p_accel / 10));
		// ����ﵽ����ٶ�С��0.5�������ǽ���������Ϊ0
		// ��ʵ�����Ǳ����ƶ�����һ�����ܴﵽ��Ҫ���ٶ�
		if (s_max_s_lim == 0) {s_max_s_lim = 1;}

		// ������ٲ�֮�����Ǳ��뿪ʼ����
		// n1 = (n1 + n2)decel / (accel + decel)
		s_accel_lim = (uint32_t)(p_step * p_decel / (p_accel + p_decel));
		if (s_accel_lim == 0) {s_accel_lim = 1;} // ���Ǳ����������1�����ܲ��ܿ�ʼ����.

		// ʹ�������������ǿ��Լ�������ٽ׶β���
		if (s_accel_lim <= s_max_s_lim) {srd.decel_val = s_accel_lim - p_step;} else {srd.decel_val = -(s_max_s_lim * p_accel / p_decel);}
		   
		if (srd.decel_val == 0) {srd.decel_val = -1;} // ��ֻʣ��һ�����Ǳ������
		srd.decel_start = p_step + srd.decel_val; // ���㿪ʼ����ʱ�Ĳ���
		// �������ٶȺ��������ǾͲ���Ҫ���м����˶�
		if(srd.step_delay <= srd.min_delay)
		{
			srd.step_delay = srd.min_delay;
			srd.run_state = RUN;
		} else {srd.run_state = ACCEL;}
		srd.accel_count = 0; // ��λ���ٶȼ���ֵ
	}
	MotionStatus = true; // ���Ϊ�˶�״̬

	s_tim_count = __HAL_TIM_GET_COUNTER(&htimx_STEPMOTOR);
	__HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR,STEPMOTOR_TIM_CHANNEL_x, (s_tim_count+srd.step_delay)); // ���ö�ʱ���Ƚ�ֵ
	TIM_CCxChannelCmd(STEPMOTOR_TIMx, STEPMOTOR_TIM_CHANNEL_x, TIM_CCx_ENABLE);// ʹ�ܶ�ʱ��ͨ�� 
	
	__HAL_TIM_MOE_ENABLE(&htimx_STEPMOTOR); //����TIM���
	HAL_TIM_Base_Start(&htimx_STEPMOTOR); // ʹ�ܶ�ʱ��

	STEP_MOTOR_OUTPUT_ENABLE(); //����
}
/*----------------------------------------------------------------------------*/
/**
  * ��������: ��ʱ���жϷ�����
  * ˵    ��: ʵ�ּӼ��ٹ���
  */
void TIM1_CC_IRQHandler(void) // ��ʱ���жϴ���
{
	uint16_t s_new_step_delay = 0; // ���ٹ��������һ����ʱ(��������)
	__IO uint16_t s_tim_count = 0; // ��������һ����ʱ����
	__IO static uint32_t s_last_accel_delay = 0;
	__IO static uint32_t s_step_count = 0; // ���ƶ�����������
	__IO static int32_t s_rest = 0; // ��¼new_step_delay�е������������һ������ľ���
	__IO static uint32_t s_i = 0; // ��ʱ��ʹ�÷�תģʽ����Ҫ���������жϲ����һ����������

	if (__HAL_TIM_GET_IT_SOURCE(&htimx_STEPMOTOR, STEPMOTOR_TIM_IT_CCx) == SET)
	{
		__HAL_TIM_CLEAR_IT(&htimx_STEPMOTOR, STEPMOTOR_TIM_IT_CCx); // �����ʱ���ж�
		s_tim_count=__HAL_TIM_GET_COUNTER(&htimx_STEPMOTOR);
		__HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR,STEPMOTOR_TIM_CHANNEL_x, (s_tim_count+srd.step_delay)); // ���ñȽ�ֵ
		if ((++s_i&0x1) == 0) // 2��,˵���Ѿ����һ����������
		{
			switch (srd.run_state) // �Ӽ������߽׶�
			{
				case STOP: // ֹͣ
					s_step_count = 0; // ���㲽��������
					s_rest = 0; // ������ֵ
					__HAL_TIM_MOE_ENABLE(&htimx_STEPMOTOR); //����TIM���
					HAL_TIM_Base_Start(&htimx_STEPMOTOR); // ʹ�ܶ�ʱ��
					STEP_MOTOR_OUTPUT_DISABLE(); //�ѻ�ʹ��
					MotionStatus = false; // ���Ϊֹͣ״̬
				break;

				case ACCEL: // ���ٽ׶�
					s_step_count++; // ������1
					if (srd.dir==CW) {step_position++;} else {step_position--;} // ����λ�üӼ�1
					srd.accel_count++; // ���ټ���ֵ��1
					s_new_step_delay = srd.step_delay - ((2 * srd.step_delay + s_rest)/(4 * srd.accel_count + 1)); // ������(��)һ����������(ʱ����)
					s_rest = (2 * srd.step_delay + s_rest)%(4 * srd.accel_count + 1); // �����������´μ��㲹���������������
					if (s_step_count >= srd.decel_start) // ����ǹ�Ӧ�ÿ�ʼ����
					{
						srd.accel_count = srd.decel_val; // ���ټ���ֵΪ���ٽ׶μ���ֵ�ĳ�ʼֵ
						srd.run_state = DECEL; // �¸����������ٽ׶�
					}
					else if(s_new_step_delay <= srd.min_delay) // ����Ƿ񵽴�����������ٶ�
					{
						s_last_accel_delay = s_new_step_delay; // ������ٹ��������һ����ʱ���������ڣ�
						s_new_step_delay = srd.min_delay; // ʹ��min_delay����Ӧ����ٶ�speed��
						s_rest = 0; // ������ֵ
						srd.run_state = RUN; // ����Ϊ��������״̬
					}
				break;

				case RUN: // ���ٽ׶�
					s_step_count++;  // ������1
					if (srd.dir==CW) {step_position++;} else {step_position--;} // ����λ�üӼ�1
					s_new_step_delay = srd.min_delay; // ʹ��min_delay����Ӧ����ٶ�speed��
					if (s_step_count >= srd.decel_start) // ��Ҫ��ʼ����
					{
						srd.accel_count = srd.decel_val; // ���ٲ�����Ϊ���ټ���ֵ
						s_new_step_delay = s_last_accel_delay; // �ӽ׶�������ʱ��Ϊ���ٽ׶ε���ʼ��ʱ(��������)
						srd.run_state = DECEL; // ״̬�ı�Ϊ����
					}
				break;

				case DECEL: // ���ٽ׶�
					s_step_count++;  // ������1
					if (srd.dir==CW) {step_position++;} else {step_position--;} // ����λ�üӼ�1
					srd.accel_count++;
					s_new_step_delay = srd.step_delay - ((2 * srd.step_delay + s_rest) / (4 * srd.accel_count + 1)); //������(��)һ����������(ʱ����)
					s_rest = (2 * srd.step_delay + s_rest) % (4 * srd.accel_count + 1); // �����������´μ��㲹���������������
					if(srd.accel_count >= 0)
					{
						__HAL_TIM_MOE_DISABLE(&htimx_STEPMOTOR); //�ر�TIM���
						srd.run_state = STOP;
					} // ����Ƿ�Ϊ���һ��
				break;

				default:
					__HAL_TIM_MOE_DISABLE(&htimx_STEPMOTOR); //�ر�TIM���
					HAL_TIM_Base_Stop(&htimx_STEPMOTOR); // ֹͣ��ʱ��
					srd.run_state = STOP;
				break;
			}
			srd.step_delay = s_new_step_delay; // Ϊ�¸�(�µ�)��ʱ(��������)��ֵ
		}
	}
}


