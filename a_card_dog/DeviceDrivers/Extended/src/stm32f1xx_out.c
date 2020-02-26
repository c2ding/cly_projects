/*

作者:刘东辉
 */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_out.h"
/* 私有宏定义 ----------------------------------------------------------------*/
#define __STM32_PORT(gpio, gpio_pin) {GPIO##gpio, GPIO_PIN_##gpio_pin}
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
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
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
void out_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	uint32_t s_i;

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //GPIO_MODE_OUTPUT_PP:推挽；GPIO_MODE_OUTPUT_OD:开漏
	//GPIO_InitStruct.Pull = GPIO_PULLUP; //上拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //速度低

	for (s_i=1; s_i<(sizeof(out_port)/sizeof(out_port[0])); s_i++)
	{
		//HAL_GPIO_WritePin(out_port[s_i].gpio, out_port[s_i].pin, GPIO_PIN_RESET); //低电平
		HAL_GPIO_WritePin(out_port[s_i].gpio, out_port[s_i].pin, GPIO_PIN_SET); //高电平
		GPIO_InitStruct.Pin = out_port[s_i].pin;
		HAL_GPIO_Init(out_port[s_i].gpio, &GPIO_InitStruct);
		HAL_GPIO_LockPin(out_port[s_i].gpio, out_port[s_i].pin); //锁定配置
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
