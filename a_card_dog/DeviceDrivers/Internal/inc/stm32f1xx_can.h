/**

����:������
*/
#ifndef __STM32F1XX_CAN_H__
#define __STM32F1XX_CAN_H__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "can_id.h"
/* �궨�� --------------------------------------------------------------------*/
//#define USE_CAN2 //ʹ�� CAN2
/* ���Ͷ��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
extern CAN_HandleTypeDef hcan1;
#ifdef USE_CAN2
extern CAN_HandleTypeDef hcan2;
#endif
typedef enum
{
	CAN_BAND_1000 = 0, //
	CAN_BAND_800 = 1, //
	CAN_BAND_500 = 2, //
	CAN_BAND_250 = 3, //
	CAN_BAND_125 = 4, //
	CAN_BAND_100 = 5, //
	CAN_BAND_50 = 6, //
	CAN_BAND_20 = 7, //
	CAN_BAND_10 = 8, //
}Can_Band;

/* �������� ------------------------------------------------------------------*/
extern void MX_CAN1_Init(Can_Band);
extern void CAN1_send_message(uint8_t TxDataBuffer[], uint32_t p_frame_id, uint8_t p_length);
#ifdef USE_CAN2
extern void MX_CAN2_Init(void);
extern void CAN2_send_message(uint8_t TxDataBuffer[], uint32_t p_frame_id, uint8_t p_length);
#endif
/*----------------------------------------------------------------------------*/
#endif
