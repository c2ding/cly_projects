/*

作者:刘东辉
 */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_ds18b20.h"
#include "stm32f1xx_delay.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
#define ds18b20_port		GPIOG
#define ds18b20_pin			GPIO_PIN_9

#define ds18b20_low()	HAL_GPIO_WritePin(ds18b20_port, ds18b20_pin, GPIO_PIN_RESET) 
#define ds18b20_high()	HAL_GPIO_WritePin(ds18b20_port, ds18b20_pin, GPIO_PIN_SET)
#define ds18b20_in()	HAL_GPIO_ReadPin(ds18b20_port, ds18b20_pin)

#define OUT_MODE true
#define IN_MODE false
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 设置端口模式
  * 输入参数: OUT_MODE,IN_MODE
  */
static void ds18b20_mode(bool p_mode)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  
	GPIO_InitStruct.Pin = ds18b20_pin;
	if (p_mode == OUT_MODE) //输出
	{
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	} else
	{
		GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull  = GPIO_PULLUP;
	}
	HAL_GPIO_Init(ds18b20_port, &GPIO_InitStruct);
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: DS18B20 初始化函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
bool ds18b20_reset(void)
{
	uint8_t pulse_time = 0;
	
	ds18b20_mode(OUT_MODE); // 主机设置为推挽输出
	ds18b20_low(), _delay_us(750); // 主机至少产生480us的低电平复位信号
	ds18b20_high(); // 主机在产生复位信号后，需将总线拉高

	//从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲
	ds18b20_mode(IN_MODE); // 主机设置为上拉输入
	// 等待存在脉冲的到来，存在脉冲为一个60~240us的低电平信号 
	// 如果存在脉冲没有来则做超时处理，从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲
	for (pulse_time = 0; ((ds18b20_in())&&(pulse_time<100)); pulse_time++) {_delay_us(1);}
	if (pulse_time >=100) {return true;} // 经过100us后，存在脉冲都还没有到来
	for (pulse_time = 0; ((!ds18b20_in())&&(pulse_time<240)); pulse_time++) {_delay_us(1);} // 存在脉冲到来，且存在的时间不能超过240us
	if (pulse_time >=240) {return true;}

	return false;
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 从DS18B20读一个字节，低位先行
  * 输入参数: 无
  * 返 回 值: 读到的数据
  * 说    明：无
  */
static uint8_t ds18b20_read(void)
{
	uint8_t i, s_datum = 0;
	
	for (i=0x01; i>0; i<<=1)
	{
		ds18b20_mode(OUT_MODE); // 读0和读1的时间至少要大于60us 
		ds18b20_low(), _delay_us(5); // 读时间的起始：必须由主机产生 >1us <15us 的低电平信号
		ds18b20_mode(IN_MODE); // 设置成输入，释放总线，由外部上拉电阻将总线拉高
		_delay_us(5);
		if (ds18b20_in() == SET) {s_datum |= i;}
		_delay_us(80); // 这个延时参数请参考时序图
	}
	return s_datum;
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 写一个字节到DS18B20，低位先行
  * 输入参数: dat：待写入数据
  * 返 回 值: 无
  * 说    明：无
  */
static void ds18b20_write(uint8_t p_datum)
{
	uint8_t i;

	ds18b20_mode(OUT_MODE);
	for (i=0x01; i>0; i<<=1)
	{
		ds18b20_low();
		/* 写0和写1的时间至少要大于60us */
		if ((p_datum&i) != 0)
		{
			/* 1us < 这个延时 < 15us */
			_delay_us(10);
			ds18b20_high();
			_delay_us(80);
		} else
		{
			/* 60us < Tx 0 < 120us */
			_delay_us(80);
			ds18b20_high();
			/* 1us < Trec(恢复时间) < 无穷大*/
			_delay_us(10);
		}
	}
}
/*----------------------------------------------------------------------------*/
/*
 * 存储的温度是16 位的带符号扩展的二进制补码形式
 * 当工作在12位分辨率时，其中5个符号位，7个整数位，4个小数位
 *
 *         |---------整数----------|-----小数 分辨率 1/(2^4)=0.0625----|
 * 低字节  | 2^3 | 2^2 | 2^1 | 2^0 | 2^(-1) | 2^(-2) | 2^(-3) | 2^(-4) |
 *
 *
 *         |-----符号位：0->正  1->负-------|-----------整数-----------|
 * 高字节  |  s  |  s  |  s  |  s  |    s   |   2^6  |   2^5  |   2^4  |
 *
 * 
 * 温度 = 符号位 + 整数 + 小数*0.0625
 */
/**
  * 函数功能: 在跳过匹配 ROM 情况下获取 DS18B20 温度值 
  * 输入参数: 无
  * 返 回 值: 温度值
  * 说    明：无
  */
bool DS18B20_GetTemp_SkipRom(float * p_datum)
{
	uint8_t tpmsb, tplsb;
	short s_tem;

	if (ds18b20_reset() == true) {return true;}
	ds18b20_write(0xCC); // 跳过 ROM
	ds18b20_write(0x44); // 开始转换
	if (ds18b20_reset() == true) {return true;}
	ds18b20_write(0xCC); // 跳过 ROM
	ds18b20_write(0xBE); // 读温度值
	tplsb = ds18b20_read();
	tpmsb = ds18b20_read();
	
	s_tem = tpmsb<<8;
	s_tem = s_tem|tplsb;
	
	*p_datum = s_tem*0.0625;
	return false;
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 在匹配 ROM 情况下获取 DS18B20 温度值 
  * 输入参数: ds18b20_id：用于存放 DS18B20 序列号的数组的首地址
  * 返 回 值: 无
  * 说    明：无
  */
void ds18b20_reset_id (uint8_t * ds18b20_id)
{
	uint8_t uc;

	ds18b20_write(0x33); //读取序列号
	for (uc=0; uc<8; uc++) {ds18b20_id[uc] = ds18b20_read();}
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 在匹配 ROM 情况下获取 DS18B20 温度值
  * 输入参数: ds18b20_id：存放 DS18B20 序列号的数组的首地址
  * 返 回 值: 温度值
  * 说    明：无
  */
bool DS18B20_GetTemp_MatchRom (uint8_t * ds18b20_id,float * p_datum)
{
	uint8_t tpmsb, tplsb, i;
	short s_tem;

	if (ds18b20_reset() == true) {return true;}
	ds18b20_write(0x55); // 匹配 ROM 
	for (i=0; i<8; i++) {ds18b20_write(ds18b20_id[i]);}
	ds18b20_write(0x44); // 开始转换
	if (ds18b20_reset() == true) {return true;}
	ds18b20_write(0x55); // 匹配 ROM 
	for (i=0; i<8; i++) {ds18b20_write(ds18b20_id[i]);}
	ds18b20_write(0xBE); // 读温度值
	tplsb = ds18b20_read();
	tpmsb = ds18b20_read();
	
	s_tem = tpmsb<<8;
	s_tem = s_tem|tplsb;
	
	*p_datum = s_tem*0.0625;
	return false;
}
/*----------------------------------------------------------------------------*/
