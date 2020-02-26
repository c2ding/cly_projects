/*
#include "stm32f1xx_matrix_keyboard.h"//按键识别与按键事件发起
	matrix_keyboard_init(); //按键初始化
	matrix_keyboard_scan(); //键盘扫描,并产生相应的事件
	
作者:刘东辉 
*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_matrix_keyboard.h"
#include "port_bit.h"
#include "entrance.h"
/*
00 //输入模式
01 //输出模式 10MHZ
02 //输出模式 2MHZ
03 //输出模式 50MHZ

00 //输入 模拟输入模式
04 //输入 浮空输入模式
08 //输入 上拉\下拉输入模式
12 //输入 保留
00 //输出 推挽输出模式
04 //输出 开漏输出模式
08 //输出 复用功能 推挽输出模式
12 //输出 复用功能开漏输出模式
3:推挽输出7:开漏输出4:开漏输入8:上下拉输入
*/
/* 私有宏定义 ----------------------------------------------------------------*/
	#define USE_DOUBLE_CLICK //使用 双击
	#define USE_LONG_PRESS //使用 长按
	#define USE_CONTINGOUS_TRIGGER //使用 连按
//横
#define PORT_TRANSVERSE_CRL SFR(GPIOA->CRL, 0, 4) //配置位(P0~P7)
#define PORT_TRANSVERSE_CRH SFR(GPIOA->CRH, 0, 4) //配置位(P8~P15)
#define PORT_TRANSVERSE_IDR SFR(GPIOA->IDR, 0, 4) //写
#define PORT_TRANSVERSE_ODR SFR(GPIOA->ODR, 0, 4) //读
//竖
#define PORT_VERTICAL_CRL SFR(GPIOA->CRL, 4, 4)
#define PORT_VERTICAL_CRH SFR(GPIOA->CRH, 4, 4)
#define PORT_VERTICAL_IDR SFR(GPIOA->IDR, 4, 4)
#define PORT_VERTICAL_ODR SFR(GPIOA->ODR, 4, 4)
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
const uint16_t key_coding[] = {
~0x0101, //0
~0x0102, //1
~0x0104, //2
~0x0108, //3
~0x0201, //4
~0x0202, //5
~0x0204, //6
~0x0208, //7
~0x0401, //8
~0x0402, //9
~0x0404, //A
~0x0408, //B
~0x0801, //C
~0x0802, //D
~0x0804, //E
~0x0808, //F
};
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
void matrix_keyboard_init(void) //按键初始化
{
	PORT_TRANSVERSE_OUT = 0x0;
	PORT_VERTICAL_OUT = 0x0;
}
/*----------------------------------------------------------------------------*/
void matrix_keyboard_scan(void) //键盘扫描,并产生相应的事件
{
	union {
		uint16_t key; //获取的键值
		struct {
			uint16_t transverse : 8; //横
			uint16_t vertical : 8; //竖
		} bus;
	} obtain;
	uint8_t s_i;
	uint8_t current_key = 0xFF; //当前按键
	static uint8_t break_key = 0xFF; //备份键值
	static uint8_t break_state = 0xFF; //备份状态(按键为高电平有效时应为0x00)
	static uint8_t press_time = 0xFF; //按下时长
	#ifdef USE_DOUBLE_CLICK
	static uint8_t lifting_time = 0xFF; //抬起时长
	static uint8_t single_click = 0xFF; //单击 标志
	#endif

//获取键值
	//PORT_TRANSVERSE_OUT = 0x0;
	PORT_TRANSVERSE_DIR = 0xF;
	//PORT_VERTICAL_DIR = 0x0;
	_delay_us(5);
	obtain.bus.vertical = PORT_VERTICAL_IN;
	//PORT_VERTICAL_OUT = 0x0;
	PORT_VERTICAL_DIR = 0xF;
	PORT_TRANSVERSE_DIR = 0x0;
	_delay_us(5);
	obtain.bus.transverse = PORT_TRANSVERSE_IN;
	PORT_VERTICAL_DIR = 0x0;
//翻译成键值
	for (s_i=0; s_i<(sizeof(key_coding)/2); s_i++) {if (obtain.key == key_coding[s_i]) {current_key = s_i;}}
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
