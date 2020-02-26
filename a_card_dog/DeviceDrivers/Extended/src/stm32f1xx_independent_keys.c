/*
#include "stm32f1xx_independent_keys.h" //��������
	independent_keys_init(); //������ʼ��
	independent_keys_scan(); //����ɨ��,��������Ӧ���¼�
	
����:������
*/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_independent_keys.h"
#include "entrance.h"
/* ˽�к궨�� ----------------------------------------------------------------*/
	#define USE_DOUBLE_CLICK //ʹ�� ˫��
	#define USE_LONG_PRESS //ʹ�� ����
	#define USE_CONTINGOUS_TRIGGER //ʹ�� ����

#define KEY_SETUP_GPIO		GPIOC
#define KEY_SETUP_GPIO_PIN	GPIO_PIN_13

#define KEY_UP_GPIO			GPIOC
#define KEY_UP_GPIO_PIN		GPIO_PIN_14

#define KEY_DOWN_GPIO		GPIOC
#define KEY_DOWN_GPIO_PIN	GPIO_PIN_15

#define KEY_ENTER_GPIO		GPIOA
#define KEY_ENTER_GPIO_PIN	GPIO_PIN_1
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
void independent_keys_init(void) //������ʼ��
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //����
	GPIO_InitStruct.Pull = GPIO_NOPULL; //����
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //����

	GPIO_InitStruct.Pin = KEY_SETUP_GPIO_PIN;
	HAL_GPIO_Init(KEY_SETUP_GPIO, &GPIO_InitStruct); 
	GPIO_InitStruct.Pin = KEY_UP_GPIO_PIN;
	HAL_GPIO_Init(KEY_UP_GPIO, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = KEY_DOWN_GPIO_PIN;
	HAL_GPIO_Init(KEY_DOWN_GPIO, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = KEY_ENTER_GPIO_PIN;
	HAL_GPIO_Init(KEY_ENTER_GPIO, &GPIO_InitStruct);

#ifdef USE_FULL_ASSERT
	printf("keyboard_init(); OK\r\n"); 
#endif
}
/*----------------------------------------------------------------------------*/
void independent_keys_scan(void) //����ɨ��,��������Ӧ���¼�
{
	uint8_t current_key = 0xFF; //��ǰ����
	static uint8_t break_key = 0xFF; //���ݼ�ֵ
	static uint8_t break_state = 0xFF; //����״̬(����Ϊ�ߵ�ƽ��ЧʱӦΪ0x00)
	static uint8_t press_time = 0xFF; //����ʱ��
	#ifdef USE_DOUBLE_CLICK
	static uint8_t lifting_time = 0xFF; //̧��ʱ��
	static uint8_t single_click = 0xFF; //���� ��־
	#endif

//��ȡ��ֵ(����Ϊ�ߵ�ƽ��ЧʱӦΪ GPIO_PIN_SET )
	if (HAL_GPIO_ReadPin(KEY_SETUP_GPIO, KEY_SETUP_GPIO_PIN) == GPIO_PIN_RESET) {current_key = KEY_SETUP;}
	else if (HAL_GPIO_ReadPin(KEY_UP_GPIO, KEY_UP_GPIO_PIN) == GPIO_PIN_RESET) {current_key = KEY_UP;}
	else if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO, KEY_DOWN_GPIO_PIN) == GPIO_PIN_RESET) {current_key = KEY_DOWN;}
	else if (HAL_GPIO_ReadPin(KEY_ENTER_GPIO, KEY_ENTER_GPIO_PIN) == GPIO_PIN_RESET) {current_key = KEY_ENTER;}

//����
	if (break_state != current_key)
	{
		break_state = current_key;
		if (current_key != 0xFF) {press_time = 0, break_key = current_key;} //����
		#ifdef USE_DOUBLE_CLICK
		else {lifting_time = 0;} //̧��
		#endif
	} 

	if (current_key != 0xFF) //����
	{
		if (press_time <= 10)
		{
			press_time++;
			#ifdef USE_LONG_PRESS
			if (press_time == 10) {button_event = 1, g_system.button_datum = break_key|LONG_PRESS;} //����
			#endif
		}
		#ifdef USE_CONTINGOUS_TRIGGER
		else {button_event = 1, g_system.button_datum = break_key|CONTINGOUS_TRIGGER;} //����
		#endif
	} else //̧��
	{
		#ifdef USE_DOUBLE_CLICK
		if (lifting_time < 6)
		{
			lifting_time++;
			if (press_time <= 3) //�̰�(0.3s)
			{
				if (single_click == 0xFF)
				{
					single_click = break_key;
					press_time = 0xFF;
				} else //˫��
				{
					if (single_click == break_key)
					{
						button_event = 1, g_system.button_datum = break_key|DOUBLE_CLICK;
					}
					single_click = 0xFF;
					lifting_time = 0xFF;
				}
			} else if (lifting_time > 3)
			{
				if (single_click == break_key)
				{
					button_event = 1, g_system.button_datum = break_key|SINGLE_CLICK;
					lifting_time = 0xFF;
				}
				single_click = 0xFF;
			}
		}
		#else
		if (press_time <= 3) //�̰�
		{
			button_event = 1, g_system.button_datum = break_key|SINGLE_CLICK;
			press_time = 0xFF;
		}
		#endif
	}
}
/*----------------------------------------------------------------------------*/
