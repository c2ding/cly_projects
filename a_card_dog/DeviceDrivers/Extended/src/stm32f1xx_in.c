/*

����:������
 */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_in.h"
/* ˽�к궨�� ----------------------------------------------------------------*/

#define __STM32_PORT(gpio, gpio_pin) {GPIO##gpio, GPIO_PIN_##gpio_pin}
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
struct pin_index
{
    GPIO_TypeDef *gpio;
    uint32_t pin;
};
static const struct pin_index in_port[] =
{
	{0,0},
	__STM32_PORT(E, 10), //1
	__STM32_PORT(E, 9), //2
	__STM32_PORT(E, 8), //3
	__STM32_PORT(E, 7), //4
	__STM32_PORT(G, 1), //5
	__STM32_PORT(G, 0), //6
	__STM32_PORT(F, 15), //7
	__STM32_PORT(F, 14), //8
	__STM32_PORT(F, 13), //9
	__STM32_PORT(F, 12), //10
	__STM32_PORT(F, 11), //11
	__STM32_PORT(B, 2), //12
	__STM32_PORT(B, 1), //13
	__STM32_PORT(B, 0), //14
	__STM32_PORT(C, 5), //15
	__STM32_PORT(C, 4), //16
//������
	__STM32_PORT(A, 7), //17
	__STM32_PORT(A, 6), //18
//	__STM32_PORT(A, 5), //19
//	__STM32_PORT(A, 4), //20
};
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
void in_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	uint32_t s_i;

	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //����
	GPIO_InitStruct.Pull = GPIO_PULLUP; //����
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //�ٶȵ�
	for (s_i=1; s_i<(sizeof(in_port)/sizeof(in_port[0])); s_i++)
	{
		GPIO_InitStruct.Pin = in_port[s_i].pin;
		HAL_GPIO_Init(in_port[s_i].gpio, &GPIO_InitStruct);
		HAL_GPIO_LockPin(in_port[s_i].gpio, in_port[s_i].pin); //��������
	}
#ifdef USE_FULL_ASSERT
	printf("in_init(); OK\r\n"); 
#endif
}
/*----------------------------------------------------------------------------*/
bool read_in(uint32_t p_n)
{
	if (HAL_GPIO_ReadPin(in_port[p_n].gpio, in_port[p_n].pin)) {return false;} else {return true;}
}
/*----------------------------------------------------------------------------*/
