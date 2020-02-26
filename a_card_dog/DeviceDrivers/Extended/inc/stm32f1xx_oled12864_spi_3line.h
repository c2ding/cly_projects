/*
#include "oled12864.h" //0.96OLED12864 ����
	ssd1315_init(); //OLED��ʼ��
	ssd1315_clear(); //����
	//��ָ��λ����ʾһ��5x7�ַ�
	//y:0~7 x:(0~122) ÿ�����21���ַ�
	ssd1315_write_string_5x7(y, x, &"123456", length);
	//��ָ��λ����ʾһ��8x16�ַ�
	//y:0~6 x:(0~120) ÿ�����16���ַ�
	ssd1315_write_string_8x16(y, x, &"123456", length);
	//��ָ��λ����ʾһ��16*16�ַ�(����)
	//y:0~6 x:(0~112) ÿ�����8���ַ� ?:chinese_characters�еĵڼ����ַ���
	ssd1315_write_word(y, x, ?);

	GND:��&VCC:3.3V&RST(RES):�Ӹ�&DC(DC):�ӵ�&CS:�ӵ�&D0:SCL&D1:SDA

����:������
*/
#ifndef __STM32F1XX_SSD1315__
#define __STM32F1XX_SSD1315__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* �궨�� --------------------------------------------------------------------*/
/* ���Ͷ��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
extern void ssd1315_init(void); //OLED��ʼ��
extern void ssd1315_clear(void); //����
extern void ssd1315_write_word_5X7(uint8_t, uint8_t, uint8_t); //��ָ��λ����ʾһ���ַ�
extern void ssd1315_write_string_5x7(uint8_t, uint8_t, uint8_t *, uint8_t); //��ָ��λ����ʾһ��5x7�ַ�
extern void ssd1315_write_word_8x16(uint8_t, uint8_t, uint8_t); //��ָ��λ����ʾһ���ַ�
extern void ssd1315_write_string_8x16(uint8_t, uint8_t, uint8_t *, uint8_t); //��ָ��λ����ʾһ��8x16�ַ�
extern void ssd1315_write_word(uint8_t, uint8_t, uint8_t); //��ָ��λ����ʾһ��16*16�ַ�(����)

#endif
