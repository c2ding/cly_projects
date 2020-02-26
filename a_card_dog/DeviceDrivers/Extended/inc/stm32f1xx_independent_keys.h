/*
#include "independent_keys.h" //��������
	independent_keys_init(); //������ʼ��
	independent_keys_scan(); //����ɨ��,��������Ӧ���¼�
	
����:������
*/
#ifndef __STM32F1xx_INDEPENDENT_KEYS__
#define __STM32F1xx_INDEPENDENT_KEYS__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* �궨�� --------------------------------------------------------------------*/
#define KEY_SETUP 0
#define KEY_UP    1
#define KEY_DOWN  2
#define KEY_ENTER 3

#define SINGLE_CLICK		0x00 //����
#define DOUBLE_CLICK		0x40 //˫��
#define LONG_PRESS			0x80 //����
#define CONTINGOUS_TRIGGER	0xC0 //��������
/* ���Ͷ��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
extern void independent_keys_init(void); //������ʼ��
extern void independent_keys_scan(void); //����ɨ��,��������Ӧ���¼�
#endif
