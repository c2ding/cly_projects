/*
#include "matrix_keyboard.h"//����ʶ���밴���¼�����
	matrix_keyboard_init(); //������ʼ��
	matrix_keyboard_scan(); //����ɨ��,��������Ӧ���¼�
	
����:������ 
*/
#ifndef __STM32F1xx_MATRIX_KEYBOARD__
#define __STM32F1xx_MATRIX_KEYBOARD__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* �궨�� --------------------------------------------------------------------*/
/* ���Ͷ��� ------------------------------------------------------------------*/

#define SINGLE_CLICK		0x00 //����
#define DOUBLE_CLICK		0x40 //˫��
#define LONG_PRESS			0x80 //����
#define CONTINGOUS_TRIGGER	0xC0 //��������
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
extern void matrix_keyboard_init(void); //������ʼ��
extern void matrix_keyboard_scan(void); //����ɨ��,��������Ӧ���¼�
#endif
