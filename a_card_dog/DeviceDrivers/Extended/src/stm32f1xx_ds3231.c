/*
#include "stm32f1xx_ds3231.h"
	ds1302_init();//时钟芯片初始化
	ds1302_switch(OFF/ON);//晶振开关
	ds1302_write_clock((uint8_t *)&datum);//写入DS1302时钟数据7位
	ds1302_read_clock((uint8_t *)&datum);//读取DS1302时钟数据
	ds1302_write_rom((uint8_t *)&datum);//写入DS1302数据31位
	ds1302_read_rom((uint8_t *)&datum);//读取DS1302数据

Vcc2 SCLK IO RST
  |   |   |   |
 --------------
| 8   7   6   5|
|    DS3231    |
|              |
| 1   2   3   4|
 --------------
  |   |   |   |
Vcc1  X1  X2 GND
2,3脚32.768HZ晶振 8脚接后备电源

作者:刘东辉
*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_ds3231.h"
#include "stm32f1xx_i2c1.h"
/* 私有宏定义 ----------------------------------------------------------------*/
#define DS3231_Address		0xD0    //器件地址 

#define DS3231_SECOND       0x00    //秒
#define DS3231_MINUTE       0x01    //分
#define DS3231_HOUR         0x02    //时
#define DS3231_WEEK         0x03    //星期                           
#define DS3231_DAY          0x04    //日
#define DS3231_MONTH        0x05    //月
#define DS3231_YEAR         0x06    //年
//闹铃1            
#define DS3231_SALARM1ECOND 0x07    //秒
#define DS3231_ALARM1MINUTE 0x08    //分
#define DS3231_ALARM1HOUR   0x09    //时
#define DS3231_ALARM1WEEK   0x0A    //星期/日
//闹铃2
#define DS3231_ALARM2MINUTE 0x0B    //分
#define DS3231_ALARM2HOUR   0x0C    //时
#define DS3231_ALARM2WEEK   0x0D    //星期/日

#define DS3231_CONTROL      0x0E    //控制寄存器
#define DS3231_STATUS       0x0F    //状态寄存器
#define DS3231_XTAL         0x10    //晶体老化寄存器
#define DS3231_TEMPERATUREH 0x11    //温度寄存器高字节(8位)
#define DS3231_TEMPERATUREL 0x12    //温度寄存器低字节(高2位) 
#define BSY                 2       //忙
#define OSF                 7       //振荡器停止标志

#define bcd_to_hex(p_val) (((p_val%10)&0x0F) | (((p_val/10)<<4)&0xF0)) //BCD转换为Byte 
#define hex_to_bcd(p_val) ((p_val&0x0F) + ((p_val>>4)&0x0F)*10) //Byte码转换为BCD码
//使用的I2C端口
#if defined(__STM32F1XX_I2C1_H__)
#define ds3231_i2c i2c1
#endif
#if defined(__STM32F1XX_I2C2_H__)
#define ds3231_i2c i2c2
#endif
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
RTCTIME g_ds3231;
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
//static uint8_t bcd_to_hex(uint8_t p_val) {return (((p_val%10)&0x0F) | (((p_val/10)<<4)&0xF0));} //BCD转换为Byte
/*----------------------------------------------------------------------------*/
//static uint8_t hex_to_bcd(uint8_t p_val) {return ((p_val&0x0F) + ((p_val>>4)&0x0F)*10);} //Byte码转换为BCD码
/*----------------------------------------------------------------------------*/
//写入DS3231时钟数据
HAL_StatusTypeDef ds3231_write_clock(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t s_buffer[7];

	s_buffer[DS3231_YEAR] = bcd_to_hex(g_ds3231.year); //年
	s_buffer[DS3231_MONTH] = bcd_to_hex(g_ds3231.month); //月
	s_buffer[DS3231_DAY] = bcd_to_hex(g_ds3231.mday); //日
	s_buffer[DS3231_HOUR] = bcd_to_hex(g_ds3231.hour); //时
	s_buffer[DS3231_MINUTE] = bcd_to_hex(g_ds3231.min); //分
	s_buffer[DS3231_SECOND] = bcd_to_hex(g_ds3231.sec); //秒
	s_buffer[DS3231_WEEK] = bcd_to_hex(g_ds3231.wday); //星期

	status |= HAL_I2C_Mem_Write(&ds3231_i2c, DS3231_Address, 0, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&s_buffer, 7, 3000);
	return status;
}
/*----------------------------------------------------------------------------*/
//读取DS3231时钟
HAL_StatusTypeDef ds3231_read_clock(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t s_buffer[7];

	status |= HAL_I2C_Mem_Read(&ds3231_i2c, DS3231_Address, 0, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&s_buffer, 7, 3000); //时间

	g_ds3231.year = hex_to_bcd(s_buffer[DS3231_YEAR]); //年
	g_ds3231.month = hex_to_bcd(s_buffer[DS3231_MONTH]); //月
	g_ds3231.mday = hex_to_bcd(s_buffer[DS3231_DAY]); //日
	g_ds3231.hour = hex_to_bcd(s_buffer[DS3231_HOUR]); //时
	g_ds3231.min = hex_to_bcd(s_buffer[DS3231_MINUTE]); //分
	g_ds3231.sec = hex_to_bcd(s_buffer[DS3231_SECOND]); //秒
	g_ds3231.wday = hex_to_bcd(s_buffer[DS3231_WEEK]); //星期
	
	status |= HAL_I2C_Mem_Read(&ds3231_i2c, DS3231_Address, DS3231_TEMPERATUREH, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&s_buffer, 1, 3000); //时间
	g_ds3231.temperature |= s_buffer[0]; //只读取整数位

	s_buffer[0] = 0x20;
	status |= HAL_I2C_Mem_Write(&ds3231_i2c, DS3231_Address, DS3231_CONTROL, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&s_buffer, 1, 3000); //启动温度转换
	
	//printf("%d:%d:%d-%d:%d:%d-%d %d\r\n",g_ds3231.hour, g_ds3231.minute, g_ds3231.second, g_ds3231.year, g_ds3231.month, g_ds3231.date, g_ds3231.week, g_ds3231.temperature);
	return status;
} 
/*----------------------------------------------------------------------------*/
