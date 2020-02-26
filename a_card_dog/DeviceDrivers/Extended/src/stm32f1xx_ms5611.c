/*
#include "stm32f1xx_ms5611.h" //MS5611��ѹ������ ��ȡ����
	ms5611_init(); //��ʼ��MS5611
	ms5611_read_datum(); //��ȡMS5611

����:������ 
*/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_ms5611.h"
#include "stm32f1xx_i2c1.h"
#include "stm32f1xx_delay.h"
/* ˽�к궨�� ----------------------------------------------------------------*/
//����������IIC�����еĴӵ�ַ,����CSB���Ų�ͬ�޸�
//#define MS561101BA_ADDR  0xEC   //CBR=1 0x76 I2C address when CSB is connected to HIGH (VCC)
#define MS561101BA_ADDR   0xEE   //CBR=0 0x77 I2C address when CSB is connected to LOW (GND)

// ����MS561101BA�ڲ���ַ
// registers of the device
#define MS561101BA_D1 0x40
#define MS561101BA_D2 0x50
#define MS561101BA_RESET 0x1E

// D1 and D2 result size (bytes)
#define MS561101BA_D1D2_SIZE 3

// OSR (Over Sampling Ratio) constants
//#define MS561101BA_OSR_256 0x00
//#define MS561101BA_OSR_512 0x02
//#define MS561101BA_OSR_1024 0x04
//#define MS561101BA_OSR_2048 0x06
//#define MS561101BA_OSR_4096 0x08
//#define  MS561101BA_D1_OSR_256 0x40 
//#define  MS561101BA_D1_OSR_512 0x42 
//#define  MS561101BA_D1_OSR_1024 0x44 
//#define  MS561101BA_D1_OSR_2048 0x46 
#define  MS561101BA_D1_OSR_4096 0x48 

//#define  MS561101BA_D2_OSR_256 0x50 
//#define  MS561101BA_D2_OSR_512 0x52 
//#define  MS561101BA_D2_OSR_1024 0x54 
//#define  MS561101BA_D2_OSR_2048 0x56 
#define  MS561101BA_D2_OSR_4096 0x58 

#define MS561101BA_PROM_BASE_ADDR 0xA0 // by adding ints from 0 to 6 we can read all the prom configuration values. 
// C1 will be at 0xA2 and all the subsequent are multiples of 2
#define MS561101BA_PROM_REG_COUNT 6 // number of registers in the PROM
#define MS561101BA_PROM_REG_SIZE 2 // size in bytes of a prom registry.

//ʹ�õ�I2C�˿�
#if defined(__STM32F1XX_I2C1_H__)
#define MS5611_i2c i2c1
#endif
#if defined(__STM32F1XX_I2C2_H__)
#define MS5611_i2c i2c2
#endif
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/

_MS5611 g_ms5611;
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/    
//MS5611��ʼ��
void ms5611_init(void)
{
	HAL_StatusTypeDef s_error = HAL_OK;
	uint8_t s_temp[2];
	uint8_t i;
//��λ MS5611
	s_error |= HAL_I2C_Mem_Write(&MS5611_i2c, MS561101BA_ADDR, MS561101BA_RESET, I2C_MEMADD_SIZE_8BIT, NULL, 0, 3000);
	_delay_ms(100);
//��PROM��ȡ����У׼����   
	for (i=0; i<=MS561101BA_PROM_REG_COUNT; i++) 
	{
		s_error |= HAL_I2C_Mem_Read(&MS5611_i2c, MS561101BA_ADDR, (MS561101BA_PROM_BASE_ADDR+(i*MS561101BA_PROM_REG_SIZE)), I2C_MEMADD_SIZE_8BIT, &*s_temp, MS561101BA_PROM_REG_SIZE, 3000);
		g_ms5611.correct[i] = ((uint16_t)s_temp[0]<<8)|s_temp[1];
	}
	//printf("\r\nC1=%d ; C2=%d ; C3=%d ; C4=%d ; C5=%d ; C6=%d\r\n",g_ms5611.correc[1],g_ms5611.correc[2],g_ms5611.correc[3],g_ms5611.correc[4],g_ms5611.correc[5],g_ms5611.correc[6]);
#ifdef USE_FULL_ASSERT
	if (s_error == HAL_OK)
	{
		printf("ms5611_init(); OK\r\n");
	} else
	{
		printf("ms5611_init(); ERROR\r\n"); 
	}
#endif
}
/*----------------------------------------------------------------------------*/   
// MS5611 ת��
uint32_t ms5611_conversion(uint8_t command)
{
	uint32_t conversion = 0;
	uint8_t s_temp[3];
	
	HAL_I2C_Mem_Write(&MS5611_i2c, MS561101BA_ADDR, command, I2C_MEMADD_SIZE_8BIT, NULL, 0, 3000);
	_delay_ms(10);//��ʱ,  ȥ�����ݴ���
	HAL_I2C_Mem_Read(&MS5611_i2c, MS561101BA_ADDR, 0, I2C_MEMADD_SIZE_8BIT, &*s_temp, MS561101BA_D1D2_SIZE, 3000);
	conversion |=  s_temp[0];
	conversion <<= 8;
	conversion |=  s_temp[1];
	conversion <<= 8;
	conversion |=  s_temp[2];

	return conversion;
}
/*----------------------------------------------------------------------------*/  
// ��ȡ MS5611
void ms5611_read_datum(void)
{
	uint32_t dT, D2_Temp, D1_Pres;
	double OFF, SENS; //ʵ���¶ȵ���,ʵ���¶�������
	float Aux, OFF2, SENS2, s_temperature; //�¶�У��ֵ
//��ȡ�¶�
	D2_Temp = ms5611_conversion(MS561101BA_D2_OSR_4096); //��������
	dT = D2_Temp - (((uint32_t)g_ms5611.correct[5])<<8);
	g_ms5611.temperature = 2000 + dT*((uint32_t)g_ms5611.correct[6])/8388608; //����¶�ֵ��100����2001��ʾ20.01��
//��ȡ��ѹ
	D1_Pres= ms5611_conversion(MS561101BA_D1_OSR_4096); //��������
	OFF = ((uint32_t)g_ms5611.correct[2]<<16)+((uint32_t)g_ms5611.correct[4]*dT)/128.0;
	SENS = ((uint32_t)g_ms5611.correct[1]<<15)+((uint32_t)g_ms5611.correct[3]*dT)/256.0;
//�¶Ȳ���
	if (g_ms5611.temperature < 2000)// second order g_ms5611.temperature compensation when under 20 degrees C
	{
		s_temperature = (dT*dT) / 0x80000000;
		Aux = (g_ms5611.temperature-2000)*(g_ms5611.temperature-2000);
		OFF2 = 2.5*Aux;
		SENS2 = 1.25*Aux;
		if(g_ms5611.temperature < -1500)
		{
			Aux = (g_ms5611.temperature+1500)*(g_ms5611.temperature+1500);
			OFF2 = OFF2 + 7*Aux;
			SENS2 = SENS + 5.5*Aux;
		}
	} else  //(g_ms5611.temperature > 2000)
	{
		s_temperature = 0;
		OFF2 = 0;
		SENS2 = 0;
	}
	g_ms5611.temperature = g_ms5611.temperature - s_temperature;
//������ѹ
	OFF = OFF - OFF2;
	SENS = SENS - SENS2;
	g_ms5611.pressure = (D1_Pres*SENS/2097152.0-OFF)/32768.0;
//
	g_ms5611.temperature /= 100;
	g_ms5611.pressure /= 100;
}
/*----------------------------------------------------------------------------*/

