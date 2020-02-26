/*

����:������
*/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_system_led.h"
#include "entrance.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
void system_led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 

	GPIO_InitStruct.Pin = system_led_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(system_led_port, &GPIO_InitStruct);
	HAL_GPIO_LockPin(system_led_port, system_led_pin); //��������
	SYSTEM_LED_OFF();
#ifdef USE_FULL_ASSERT
	printf("system_led_init(); OK\r\n"); 
#endif
}
/*----------------------------------------------------------------------------*/
void system_led(void)
{
	static uint32_t s_time = 0;

	if (s_time==0) {SYSTEM_LED_ON();} else {SYSTEM_LED_OFF();}
	s_time++;
	switch (g_system.working_state)
	{
		case STANBDY: //����
			if (s_time>=50) {s_time = 0;} //5S
		break;
		case FUNCTION: //����
			if (s_time>=10) {s_time = 0;} //1S
		break;
		case FAULT: //����
			if (s_time>=10) {s_time = 0;} //500mS
		break;
		case ALERT: //����
			if (s_time>=2) {s_time = 0;} //200mS
		break;
		default: break;
	}

}
/*----------------------------------------------------------------------------*/
