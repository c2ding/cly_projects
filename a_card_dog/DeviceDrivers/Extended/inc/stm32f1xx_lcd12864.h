/*
#include "lcd12864.h" //LCD12864Һ������
	lcd12864_init(); //LCD��ʼ��
	lcd12864_yx(y, x); //д��ַ(��Χ y0~3;x0~7)
	lcd12864_write_byte(function, datum); //д function:���� datum:����
	lcd12864_write_string(y, x, (uint8_t *)&"0123456789ABCDEF", length); //��ָ��λ����ʾһ���ַ�
	lcd12864_write_image((uint8_t *)&bmp); //��ʾͼƬ
	lcd12864_reverse_white(y, x, whether, words); //��y,x��ʼ/Whether����(��:0xFF;ȡ��:0x00)/Words������(ʹ��ǰ��ͼ����RAM������)
	lcd12864_loading_customation((uint8_t *)&datum, address); //װ���Զ����ַ�address(0~3)
	lcd12864_display_customation(y, x, address); //��ʾ�Զ����ַ�address(0~3)

	lcd12864_clearance_all(); //������GDRAM�ռ�
	lcd12864_clearance_txt(); //�ı�����RAM
	lcd12864_clearance_image(); //ͼ������RAM
	lcd12864_clearance_screen(); //����

	0x01:����
	0x08:��ʾ��
	0x0C:��ʾ��
	0x0D:����
	0x0E:��꿪
	0x0F:��ʾ���,��ǰ��ʾλ��������
	0x1F:�ַ�ѭ������
	0x18:�ַ�ѭ������
	0x30:����ָ��
	0x34:��չָ��
	0x34:��ͼ��
	0x36:��ͼ��

	//��ֱ������Ļ(������GDRAM�ռ������)
	lcd12864_write_byte(COMMAND, 0x36);
	lcd12864_write_byte(COMMAND, 0x03);
	lcd12864_write_byte(COMMAND, 0x60);
	
����:������ 
*/
#ifndef __STM32F1XX_12864__
#define __STM32F1XX_12864__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include <stm32f1xx_hal.h>
/* �궨�� --------------------------------------------------------------------*/
	#define __USE_THE_RESET__   //��λ(��ʹ��ʱ�Ӹߵ�ƽ������)
//	#define __SERIAL_MODE__     //ʹ�ô��ڷ�ʽ
//	#define __DEFINITION_WORD__ //ʹ���Զ����ַ�
//	#define __GRAPHIC_DISPLAY__ //ʹ��ͼ����ʾ����
//	#define __REVERSE_WHITE__   //ʹ�����ַ���Ч��

#define LCD12864_BACKLIGHT_rcc		__HAL_RCC_GPIOB_CLK_ENABLE
#define LCD12864_BACKLIGHT_gpio		GPIOB
#define LCD12864_BACKLIGHT_pin		GPIO_PIN_11
#define LCD12864_BACKLIGHT_ON		HAL_GPIO_WritePin(LCD12864_BACKLIGHT_gpio, LCD12864_BACKLIGHT_pin, GPIO_PIN_RESET)
#define LCD12864_BACKLIGHT_OFF		HAL_GPIO_WritePin(LCD12864_BACKLIGHT_gpio, LCD12864_BACKLIGHT_pin, GPIO_PIN_SET)

//Function:���ݻ�����ѡ�� Data:���ݻ�������
#ifdef __SERIAL_MODE__ //���ڷ�ʽ
#define COMMAND	0xF8 //Command
#define DATA 0xFA    //Datum
#else
#define COMMAND	0 //Command
#define DATA	1 //Datum
#endif
/* ���Ͷ��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
#ifdef __GRAPHIC_DISPLAY__ //ͼ�ο�
#include <avr/pgmspace.h> //Flash�ݴ洢����
extern PROGMEM const uint8_t bmp[];
#endif
#ifdef __DEFINITION_WORD__ //�Զ����ַ�
#include <avr/pgmspace.h> //Flash�ݴ洢����
extern PROGMEM const uint8_t zk0[];
extern PROGMEM const uint8_t zk1[];
#endif
/* �������� ------------------------------------------------------------------*/
extern void lcd12864_write_byte(uint32_t, uint32_t);
extern void lcd12864_init(void); //��ʼ��LCD��
extern void lcd12864_clearance_txt(void); //�ı�����RAM����
extern void lcd12864_clearance_image(void); //ͼ������RAM����(ͼ�������ı�����ʾ��������ͬ��RAM��)
//extern void lcd12864_clearance_all(void); //������GDRAM�ռ�
#define lcd12864_clearance_screen() lcd12864_write_byte(COMMAND, 0x01); _delay_ms(1) //����
extern void lcd12864_yx(uint32_t, uint32_t); //д��ַ(��Χ y0~3;x0~7)
//��ָ��λ����ʾһ���ַ�(��,��,����)
extern void lcd12864_write_string(uint32_t , uint32_t , uint8_t *, uint32_t);
#ifdef __DEFINITION_WORD__
//װ���Զ����ַ� Position(0~3)
extern void lcd12864_loading_customation(uint8_t *, uint32_t);
//��ʾ�Զ����ַ� Position(0~3)
extern void lcd12864_display_customation(uint32_t, uint32_t, uint32_t);
#endif
#ifdef __GRAPHIC_DISPLAY__
extern void lcd12864_write_image(uint8_t *); //��ʾͼƬ
#endif
#ifdef __REVERSE_WHITE__ //Whether�Ƿ񷴰״�/��y,x��ʼWords������
extern void lcd12864_reverse_white(uint32_t, uint32_t, uint32_t, uint32_t);
#endif

#endif
