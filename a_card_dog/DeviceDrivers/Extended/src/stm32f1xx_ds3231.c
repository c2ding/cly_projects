/*
#include "stm32f1xx_ds3231.h"
	ds1302_init();//ʱ��оƬ��ʼ��
	ds1302_switch(OFF/ON);//���񿪹�
	ds1302_write_clock((uint8_t *)&datum);//д��DS1302ʱ������7λ
	ds1302_read_clock((uint8_t *)&datum);//��ȡDS1302ʱ������
	ds1302_write_rom((uint8_t *)&datum);//д��DS1302����31λ
	ds1302_read_rom((uint8_t *)&datum);//��ȡDS1302����

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
2,3��32.768HZ���� 8�ŽӺ󱸵�Դ

����:������
*/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_ds3231.h"
#include "stm32f1xx_i2c1.h"
/* ˽�к궨�� ----------------------------------------------------------------*/
#define DS3231_Address		0xD0    //������ַ 

#define DS3231_SECOND       0x00    //��
#define DS3231_MINUTE       0x01    //��
#define DS3231_HOUR         0x02    //ʱ
#define DS3231_WEEK         0x03    //����                           
#define DS3231_DAY          0x04    //��
#define DS3231_MONTH        0x05    //��
#define DS3231_YEAR         0x06    //��
//����1            
#define DS3231_SALARM1ECOND 0x07    //��
#define DS3231_ALARM1MINUTE 0x08    //��
#define DS3231_ALARM1HOUR   0x09    //ʱ
#define DS3231_ALARM1WEEK   0x0A    //����/��
//����2
#define DS3231_ALARM2MINUTE 0x0B    //��
#define DS3231_ALARM2HOUR   0x0C    //ʱ
#define DS3231_ALARM2WEEK   0x0D    //����/��

#define DS3231_CONTROL      0x0E    //���ƼĴ���
#define DS3231_STATUS       0x0F    //״̬�Ĵ���
#define DS3231_XTAL         0x10    //�����ϻ��Ĵ���
#define DS3231_TEMPERATUREH 0x11    //�¶ȼĴ������ֽ�(8λ)
#define DS3231_TEMPERATUREL 0x12    //�¶ȼĴ������ֽ�(��2λ) 
#define BSY                 2       //æ
#define OSF                 7       //����ֹͣ��־

#define bcd_to_hex(p_val) (((p_val%10)&0x0F) | (((p_val/10)<<4)&0xF0)) //BCDת��ΪByte 
#define hex_to_bcd(p_val) ((p_val&0x0F) + ((p_val>>4)&0x0F)*10) //Byte��ת��ΪBCD��
//ʹ�õ�I2C�˿�
#if defined(__STM32F1XX_I2C1_H__)
#define ds3231_i2c i2c1
#endif
#if defined(__STM32F1XX_I2C2_H__)
#define ds3231_i2c i2c2
#endif
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
RTCTIME g_ds3231;
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
//static uint8_t bcd_to_hex(uint8_t p_val) {return (((p_val%10)&0x0F) | (((p_val/10)<<4)&0xF0));} //BCDת��ΪByte
/*----------------------------------------------------------------------------*/
//static uint8_t hex_to_bcd(uint8_t p_val) {return ((p_val&0x0F) + ((p_val>>4)&0x0F)*10);} //Byte��ת��ΪBCD��
/*----------------------------------------------------------------------------*/
//д��DS3231ʱ������
HAL_StatusTypeDef ds3231_write_clock(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t s_buffer[7];

	s_buffer[DS3231_YEAR] = bcd_to_hex(g_ds3231.year); //��
	s_buffer[DS3231_MONTH] = bcd_to_hex(g_ds3231.month); //��
	s_buffer[DS3231_DAY] = bcd_to_hex(g_ds3231.mday); //��
	s_buffer[DS3231_HOUR] = bcd_to_hex(g_ds3231.hour); //ʱ
	s_buffer[DS3231_MINUTE] = bcd_to_hex(g_ds3231.min); //��
	s_buffer[DS3231_SECOND] = bcd_to_hex(g_ds3231.sec); //��
	s_buffer[DS3231_WEEK] = bcd_to_hex(g_ds3231.wday); //����

	status |= HAL_I2C_Mem_Write(&ds3231_i2c, DS3231_Address, 0, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&s_buffer, 7, 3000);
	return status;
}
/*----------------------------------------------------------------------------*/
//��ȡDS3231ʱ��
HAL_StatusTypeDef ds3231_read_clock(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t s_buffer[7];

	status |= HAL_I2C_Mem_Read(&ds3231_i2c, DS3231_Address, 0, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&s_buffer, 7, 3000); //ʱ��

	g_ds3231.year = hex_to_bcd(s_buffer[DS3231_YEAR]); //��
	g_ds3231.month = hex_to_bcd(s_buffer[DS3231_MONTH]); //��
	g_ds3231.mday = hex_to_bcd(s_buffer[DS3231_DAY]); //��
	g_ds3231.hour = hex_to_bcd(s_buffer[DS3231_HOUR]); //ʱ
	g_ds3231.min = hex_to_bcd(s_buffer[DS3231_MINUTE]); //��
	g_ds3231.sec = hex_to_bcd(s_buffer[DS3231_SECOND]); //��
	g_ds3231.wday = hex_to_bcd(s_buffer[DS3231_WEEK]); //����
	
	status |= HAL_I2C_Mem_Read(&ds3231_i2c, DS3231_Address, DS3231_TEMPERATUREH, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&s_buffer, 1, 3000); //ʱ��
	g_ds3231.temperature |= s_buffer[0]; //ֻ��ȡ����λ

	s_buffer[0] = 0x20;
	status |= HAL_I2C_Mem_Write(&ds3231_i2c, DS3231_Address, DS3231_CONTROL, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&s_buffer, 1, 3000); //�����¶�ת��
	
	//printf("%d:%d:%d-%d:%d:%d-%d %d\r\n",g_ds3231.hour, g_ds3231.minute, g_ds3231.second, g_ds3231.year, g_ds3231.month, g_ds3231.date, g_ds3231.week, g_ds3231.temperature);
	return status;
} 
/*----------------------------------------------------------------------------*/
