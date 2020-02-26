/*
#include "shtxx.h" //SHTxxϵ�ж�ȡ
	shtxx_init();//��ʼ��
	shtxx_data_acquisition(); //�ɼ���ʪ������ (100mS����)
	
����:������
*/

#ifndef __STM32F1XX_SHTXX_H__
#define __STM32F1XX_SHTXX_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include <stm32f1xx_hal.h>
#include <math.h>
#include "bsp_delay.h"
/* �궨�� --------------------------------------------------------------------*/
//#define __CALCULATED_DEW_POINT__ //���� ¶��
/* ���Ͷ��� ------------------------------------------------------------------*/
typedef struct //
{
	bool error;
	float temperature; //�¶�
	float humidity; //ʪ��
#if defined(__CALCULATED_DEW_POINT__)
	float the_dew_point; //¶��
#endif
}SHTXX;
/* ��չ���� ------------------------------------------------------------------*/
extern __IO SHTXX g_shtxx;
/* �������� ------------------------------------------------------------------*/
extern void shtxx_init(void);//��ʼ��
extern void shtxx_data_acquisition(void); //�ɼ���ʪ������
#endif
