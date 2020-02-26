/*

����:������
*/
#ifndef __STM32F1XX_GSM_H__
#define __STM32F1XX_GSM_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include <stm32f1xx_hal.h>
#include <stdarg.h>
#include <stdlib.h>
#include "stm32f1xx_usart2.h"
#include "gprs_app.h"
#include "stm32f1xx_delay.h"
#include "entrance.h"
/* ���Ͷ��� ------------------------------------------------------------------*/
typedef enum
{
	GSM_SUCCESS, //�ɹ�
	GSM_FAIL, //ʧ��
	GSM_ERROR, //����
	GSM_TIMEOUT, //��ʱ
} GSM_res_e;

typedef struct {
	bool connect[8]; //����
//	bool send[8]; //����
	char ccid[24];
	double longitude; //����
	double latitude; //γ��
	uint16_t year;	//��
	uint8_t month;	//��
	uint8_t mday;	//��
	uint8_t hour;	//ʱ
	uint8_t min;	//��
	uint8_t sec;	//��
} _SIM7600;
/* �궨�� --------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
extern _SIM7600 sim7600;
extern char sim7600_tx_buffer[1024]; //���ͻ�����
extern char sim7600_rx_buffer[1024]; //���ջ�����
/* �������� ------------------------------------------------------------------*/
extern GSM_res_e waiting_for_startup(void);
extern GSM_res_e sim7600_init(void); //��ʼ�������ģ��
/*************************** sim7600 ���� ***************************/
extern GSM_res_e sim7600_tcp_mode(void); //tcpģʽ��ʼ��
extern GSM_res_e sim7600_close_tcp_mode(void);
extern GSM_res_e sim7600_tcp_link(char * serverip, char * serverport); //TCP����
extern GSM_res_e sim7600_send(uint32_t length); //��������
extern GSM_res_e sim7600_get_time(void); //��ȡ ʱ��
extern GSM_res_e sim7600_get_position(void); //��ȡ λ��
/*----------------------------------------------------------------------------*/
#endif
