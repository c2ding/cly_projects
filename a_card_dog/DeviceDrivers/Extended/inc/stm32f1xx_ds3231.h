/*
#include "ds3231.h"
	ds1302_init();//ʱ��оƬ��ʼ��
	ds1302_switch(OFF/ON);//���񿪹�
	ds1302_write_clock((uint8_t *)&datum);//д��DS1302ʱ������7λ
	ds1302_read_clock((uint8_t *)&datum);//��ȡDS1302ʱ������


Vcc2 SCLK IO RST
  |   |   |   |
 --------------
| 8   7   6   5|
|    DS1302    |
|              |
| 1   2   3   4|
 --------------
  |   |   |   |
Vcc1  X1  X2 GND
2,3��32.768HZ���� 8�ŽӺ󱸵�Դ

����:������ 
*/
#ifndef __STM32F1XX_DS3231__
#define __STM32F1XX_DS3231__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h" 
/* �궨�� --------------------------------------------------------------------*/
/* ���Ͷ��� ------------------------------------------------------------------*/
typedef struct {
	uint16_t	year;	//��
	uint8_t		month;	//��
	uint8_t		mday;	//��
	uint8_t		hour;	//ʱ
	uint8_t		min;	//��
	uint8_t		sec;	//��
	uint8_t		wday;	//����
	int8_t temperature; //�¶�
} RTCTIME;
/* ��չ���� ------------------------------------------------------------------*/
extern RTCTIME g_ds3231;
/* �������� ------------------------------------------------------------------*/
extern HAL_StatusTypeDef ds3231_write_clock(void); //д��DS3231ʱ��
extern HAL_StatusTypeDef ds3231_read_clock(void); //��ȡDS3231ʱ��
#endif
