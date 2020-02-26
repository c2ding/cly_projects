/*

����:������
 */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_out.h"
/* ˽�к궨�� ----------------------------------------------------------------*/
#define __STM32_PORT(gpio, gpio_pin) {GPIO##gpio, GPIO_PIN_##gpio_pin}
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
struct pin_index
{
    GPIO_TypeDef *gpio;
    uint32_t pin;
};
static const struct pin_index out_port[] =
{
	{0,0},
	__STM32_PORT(G, 12), //1
	__STM32_PORT(G, 13), //2
	__STM32_PORT(G, 14), //3
	__STM32_PORT(B, 4), //4
	__STM32_PORT(B, 5), //5
	__STM32_PORT(B, 6), //6
	__STM32_PORT(B, 7), //7
	__STM32_PORT(B, 8), //8
	__STM32_PORT(B, 9), //9
	__STM32_PORT(E, 1), //10
};
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
void out_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	uint32_t s_i;

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //GPIO_MODE_OUTPUT_PP:���죻GPIO_MODE_OUTPUT_OD:��©
	//GPIO_InitStruct.Pull = GPIO_PULLUP; //����
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //�ٶȵ�

	for (s_i=1; s_i<(sizeof(out_port)/sizeof(out_port[0])); s_i++)
	{
		//HAL_GPIO_WritePin(out_port[s_i].gpio, out_port[s_i].pin, GPIO_PIN_RESET); //�͵�ƽ
		HAL_GPIO_WritePin(out_port[s_i].gpio, out_port[s_i].pin, GPIO_PIN_SET); //�ߵ�ƽ
		GPIO_InitStruct.Pin = out_port[s_i].pin;
		HAL_GPIO_Init(out_port[s_i].gpio, &GPIO_InitStruct);
		HAL_GPIO_LockPin(out_port[s_i].gpio, out_port[s_i].pin); //��������
	}
#ifdef USE_FULL_ASSERT
	printf("out_init(); OK\r\n"); 
#endif
}
/*----------------------------------------------------------------------------*/
void out_on(uint32_t p_n) //
{
	HAL_GPIO_WritePin(out_port[p_n].gpio, out_port[p_n].pin, GPIO_PIN_RESET);
}
/*----------------------------------------------------------------------------*/
void out_off(uint32_t p_n) //
{
	HAL_GPIO_WritePin(out_port[p_n].gpio, out_port[p_n].pin, GPIO_PIN_SET);
}
/*----------------------------------------------------------------------------*/
