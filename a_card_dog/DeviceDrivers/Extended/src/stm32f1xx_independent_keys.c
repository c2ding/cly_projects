/*
#include "stm32f1xx_independent_keys.h" //独立按键
	independent_keys_init(); //按键初始化
	independent_keys_scan(); //键盘扫描,并产生相应的事件
	
作者:刘东辉
*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_independent_keys.h"
#include "entrance.h"
/* 私有宏定义 ----------------------------------------------------------------*/
	#define USE_DOUBLE_CLICK //使用 双击
	#define USE_LONG_PRESS //使用 长按
	#define USE_CONTINGOUS_TRIGGER //使用 连按

#define KEY_SETUP_GPIO		GPIOC
#define KEY_SETUP_GPIO_PIN	GPIO_PIN_13

#define KEY_UP_GPIO			GPIOC
#define KEY_UP_GPIO_PIN		GPIO_PIN_14

#define KEY_DOWN_GPIO		GPIOC
#define KEY_DOWN_GPIO_PIN	GPIO_PIN_15

#define KEY_ENTER_GPIO		GPIOA
#define KEY_ENTER_GPIO_PIN	GPIO_PIN_1
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
void independent_keys_init(void) //按键初始化
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //输入
	GPIO_InitStruct.Pull = GPIO_NOPULL; //上拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //低速

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
void independent_keys_scan(void) //键盘扫描,并产生相应的事件
{
	uint8_t current_key = 0xFF; //当前按键
	static uint8_t break_key = 0xFF; //备份键值
	static uint8_t break_state = 0xFF; //备份状态(按键为高电平有效时应为0x00)
	static uint8_t press_time = 0xFF; //按下时长
	#ifdef USE_DOUBLE_CLICK
	static uint8_t lifting_time = 0xFF; //抬起时长
	static uint8_t single_click = 0xFF; //单击 标志
	#endif

//获取键值(按键为高电平有效时应为 GPIO_PIN_SET )
	if (HAL_GPIO_ReadPin(KEY_SETUP_GPIO, KEY_SETUP_GPIO_PIN) == GPIO_PIN_RESET) {current_key = KEY_SETUP;}
	else if (HAL_GPIO_ReadPin(KEY_UP_GPIO, KEY_UP_GPIO_PIN) == GPIO_PIN_RESET) {current_key = KEY_UP;}
	else if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO, KEY_DOWN_GPIO_PIN) == GPIO_PIN_RESET) {current_key = KEY_DOWN;}
	else if (HAL_GPIO_ReadPin(KEY_ENTER_GPIO, KEY_ENTER_GPIO_PIN) == GPIO_PIN_RESET) {current_key = KEY_ENTER;}

//分析
	if (break_state != current_key)
	{
		break_state = current_key;
		if (current_key != 0xFF) {press_time = 0, break_key = current_key;} //按下
		#ifdef USE_DOUBLE_CLICK
		else {lifting_time = 0;} //抬起
		#endif
	} 

	if (current_key != 0xFF) //按下
	{
		if (press_time <= 10)
		{
			press_time++;
			#ifdef USE_LONG_PRESS
			if (press_time == 10) {button_event = 1, g_system.button_datum = break_key|LONG_PRESS;} //长按
			#endif
		}
		#ifdef USE_CONTINGOUS_TRIGGER
		else {button_event = 1, g_system.button_datum = break_key|CONTINGOUS_TRIGGER;} //连按
		#endif
	} else //抬起
	{
		#ifdef USE_DOUBLE_CLICK
		if (lifting_time < 6)
		{
			lifting_time++;
			if (press_time <= 3) //短按(0.3s)
			{
				if (single_click == 0xFF)
				{
					single_click = break_key;
					press_time = 0xFF;
				} else //双击
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
		if (press_time <= 3) //短按
		{
			button_event = 1, g_system.button_datum = break_key|SINGLE_CLICK;
			press_time = 0xFF;
		}
		#endif
	}
}
/*----------------------------------------------------------------------------*/
