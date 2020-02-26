/*
#include "lcd12864.h" //LCD12864液晶驱动
	lcd12864_init(); //LCD初始化
	lcd12864_yx(y, x); //写地址(范围 y0~3;x0~7)
	lcd12864_write_byte(function, datum); //写 function:功能 datum:数据
	lcd12864_write_string(y, x, (uint8_t *)&"0123456789ABCDEF", length); //按指定位置显示一串字符
	lcd12864_write_image((uint8_t *)&bmp); //显示图片
	lcd12864_reverse_white(y, x, whether, words); //从y,x开始/Whether反白(是:0xFF;取消:0x00)/Words几个字(使用前清图形区RAM否则花屏)
	lcd12864_loading_customation((uint8_t *)&datum, address); //装入自定义字符address(0~3)
	lcd12864_display_customation(y, x, address); //显示自定义字符address(0~3)

	lcd12864_clearance_all(); //清整个GDRAM空间
	lcd12864_clearance_txt(); //文本区清RAM
	lcd12864_clearance_image(); //图形区清RAM
	lcd12864_clearance_screen(); //清屏

	0x01:清屏
	0x08:显示关
	0x0C:显示开
	0x0D:光标关
	0x0E:光标开
	0x0F:显示光标,当前显示位反白闪动
	0x1F:字符循环右移
	0x18:字符循环左移
	0x30:基本指集
	0x34:扩展指集
	0x34:绘图关
	0x36:绘图开

	//垂直滚动屏幕(清整个GDRAM空间否则花屏)
	lcd12864_write_byte(COMMAND, 0x36);
	lcd12864_write_byte(COMMAND, 0x03);
	lcd12864_write_byte(COMMAND, 0x60);
	
作者:刘东辉 
*/
#ifndef __STM32F1XX_12864__
#define __STM32F1XX_12864__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stm32f1xx_hal.h>
/* 宏定义 --------------------------------------------------------------------*/
	#define __USE_THE_RESET__   //复位(不使用时接高电平或悬空)
//	#define __SERIAL_MODE__     //使用串口方式
//	#define __DEFINITION_WORD__ //使用自定义字符
//	#define __GRAPHIC_DISPLAY__ //使用图形显示函数
//	#define __REVERSE_WHITE__   //使用文字翻白效果

#define LCD12864_BACKLIGHT_rcc		__HAL_RCC_GPIOB_CLK_ENABLE
#define LCD12864_BACKLIGHT_gpio		GPIOB
#define LCD12864_BACKLIGHT_pin		GPIO_PIN_11
#define LCD12864_BACKLIGHT_ON		HAL_GPIO_WritePin(LCD12864_BACKLIGHT_gpio, LCD12864_BACKLIGHT_pin, GPIO_PIN_RESET)
#define LCD12864_BACKLIGHT_OFF		HAL_GPIO_WritePin(LCD12864_BACKLIGHT_gpio, LCD12864_BACKLIGHT_pin, GPIO_PIN_SET)

//Function:数据或命令选择 Data:数据或命内容
#ifdef __SERIAL_MODE__ //串口方式
#define COMMAND	0xF8 //Command
#define DATA 0xFA    //Datum
#else
#define COMMAND	0 //Command
#define DATA	1 //Datum
#endif
/* 类型定义 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
#ifdef __GRAPHIC_DISPLAY__ //图形库
#include <avr/pgmspace.h> //Flash据存储定义
extern PROGMEM const uint8_t bmp[];
#endif
#ifdef __DEFINITION_WORD__ //自定义字符
#include <avr/pgmspace.h> //Flash据存储定义
extern PROGMEM const uint8_t zk0[];
extern PROGMEM const uint8_t zk1[];
#endif
/* 函数声明 ------------------------------------------------------------------*/
extern void lcd12864_write_byte(uint32_t, uint32_t);
extern void lcd12864_init(void); //初始化LCD屏
extern void lcd12864_clearance_txt(void); //文本区清RAM函数
extern void lcd12864_clearance_image(void); //图形区清RAM函数(图形区和文本区显示在两个不同的RAM区)
//extern void lcd12864_clearance_all(void); //清整个GDRAM空间
#define lcd12864_clearance_screen() lcd12864_write_byte(COMMAND, 0x01); _delay_ms(1) //清屏
extern void lcd12864_yx(uint32_t, uint32_t); //写地址(范围 y0~3;x0~7)
//按指定位置显示一串字符(行,列,数据)
extern void lcd12864_write_string(uint32_t , uint32_t , uint8_t *, uint32_t);
#ifdef __DEFINITION_WORD__
//装入自定义字符 Position(0~3)
extern void lcd12864_loading_customation(uint8_t *, uint32_t);
//显示自定义字符 Position(0~3)
extern void lcd12864_display_customation(uint32_t, uint32_t, uint32_t);
#endif
#ifdef __GRAPHIC_DISPLAY__
extern void lcd12864_write_image(uint8_t *); //显示图片
#endif
#ifdef __REVERSE_WHITE__ //Whether是否反白从/从y,x开始Words几个字
extern void lcd12864_reverse_white(uint32_t, uint32_t, uint32_t, uint32_t);
#endif

#endif
