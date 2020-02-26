/*

����:������
*/
#ifndef __APPLICATION__
#define __APPLICATION__ 
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* �궨�� --------------------------------------------------------------------*/
/* ���Ͷ��� ------------------------------------------------------------------*/
typedef struct //
{
	uint8_t in_buffer[256];
	bool start;
	uint32_t time; //ʱ��
	uint32_t start_wait; //�����ȴ�
	uint32_t heartbeat; //����
	uint32_t er_h;
	uint32_t er_s;
}_WORK;

typedef struct //
{
	uint32_t start_wait; //�����ȴ�
	uint32_t heartbeat; //����
}_PARAMETER;
/* ��չ���� ------------------------------------------------------------------*/
extern _WORK work;
extern const _PARAMETER parameter;
/* �������� ------------------------------------------------------------------*/
extern void data_collection(void);
extern void control(void);
extern void power_out_init(void); //
extern void u3_printf(const char *fmt, ...); //��������
/*----------------------------------------------------------------------------*/

#endif
