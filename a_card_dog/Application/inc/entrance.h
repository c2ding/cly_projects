/*

����:������
*/
#ifndef __ENTRANCE_H__
#define __ENTRANCE_H__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* �궨�� --------------------------------------------------------------------*/
#define whole_event g_system.event.whole //ȫ���¼�
#define second_event g_system.event.each.event0 //��
#define millisecond_event g_system.event.each.event1 //����
#define button_event g_system.event.each.event2 //����
#define display_event g_system.event.each.event3 //��ʾ
#define can1_event g_system.event.each.event4 //CAN1
#define can2_event g_system.event.each.event5 //CAN2
#define usart1_event g_system.event.each.event6 //����1
#define usart2_event g_system.event.each.event7 //����2
#define usart3_event g_system.event.each.event8 //����3
#define uart4_event g_system.event.each.event9 //����4
#define uart5_event g_system.event.each.event10 //����5


#define OFFSER_BUFFER_FLASH_ADDRESS 0x800F000 // ƫ���� ������ ��FLASH�е�ַ
//#define OFFSER_BUFFER_FLASH_LENGTH sizeof(g_time_control)/2 // ƫ���� ������ ����
/* ���Ͷ��� ------------------------------------------------------------------*/
//��������
typedef enum
{
	STANBDY = 0, //����
	FUNCTION = 1, //����
	FAULT = 2, //����
	ALERT = 3, //����
}State_Type;

typedef union {
	struct{uint32_t event0:1, event1:1, event2:1, event3:1, event4:1, event5:1, event6:1, event7:1,
					event8:1, event9:1, event10:1, event11:1, event12:1, event13:1, event14:1, event15:1,
					event16:1, event17:1, event18:1, event19:1, event20:1, event21:1, event22:1, event23:1,
					event24:1, event25:1, event26:1, event27:1, event28:1, event29:1, event30:1, event31:1;
	}each;
	uint32_t whole;
}_TEVENT;
//#pragma pack(1)
typedef struct //
{
	bool equipment_information; //�����豸��Ϣ(���� ���� γ��)
	uint32_t time_SysTick;
	_TEVENT event; //�¼�
	State_Type working_state; //����״̬
	//uint32_t button_datum; //���� ����
	//void (*dialogue_control)(void); //�Ի��������
	//void (*operation_mode)(void); //����ģʽ
}_SYSTEM_STATUS;
typedef struct //
{
	uint8_t timelist[12][4]; //���ػ�ʱ��
	bool active; //�Ƿ�ʹ��ʱ��ƻ�
}_TIME_CONTROL;
/* ��չ���� ------------------------------------------------------------------*/
extern __IO _SYSTEM_STATUS g_system;
extern IWDG_HandleTypeDef hiwdg;
extern RTC_HandleTypeDef RtcHandle;
/* �������� ------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
extern void Soft_Reset(void);
/*----------------------------------------------------------------------------*/
#endif

