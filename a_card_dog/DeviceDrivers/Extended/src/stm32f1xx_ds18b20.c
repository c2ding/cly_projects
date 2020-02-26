/*

����:������
 */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_ds18b20.h"
#include "stm32f1xx_delay.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
#define ds18b20_port		GPIOG
#define ds18b20_pin			GPIO_PIN_9

#define ds18b20_low()	HAL_GPIO_WritePin(ds18b20_port, ds18b20_pin, GPIO_PIN_RESET) 
#define ds18b20_high()	HAL_GPIO_WritePin(ds18b20_port, ds18b20_pin, GPIO_PIN_SET)
#define ds18b20_in()	HAL_GPIO_ReadPin(ds18b20_port, ds18b20_pin)

#define OUT_MODE true
#define IN_MODE false
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ���ö˿�ģʽ
  * �������: OUT_MODE,IN_MODE
  */
static void ds18b20_mode(bool p_mode)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  
	GPIO_InitStruct.Pin = ds18b20_pin;
	if (p_mode == OUT_MODE) //���
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
  * ��������: DS18B20 ��ʼ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
bool ds18b20_reset(void)
{
	uint8_t pulse_time = 0;
	
	ds18b20_mode(OUT_MODE); // ��������Ϊ�������
	ds18b20_low(), _delay_us(750); // �������ٲ���480us�ĵ͵�ƽ��λ�ź�
	ds18b20_high(); // �����ڲ�����λ�źź��轫��������

	//�ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ����������
	ds18b20_mode(IN_MODE); // ��������Ϊ��������
	// �ȴ���������ĵ�������������Ϊһ��60~240us�ĵ͵�ƽ�ź� 
	// �����������û����������ʱ�����ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ����������
	for (pulse_time = 0; ((ds18b20_in())&&(pulse_time<100)); pulse_time++) {_delay_us(1);}
	if (pulse_time >=100) {return true;} // ����100us�󣬴������嶼��û�е���
	for (pulse_time = 0; ((!ds18b20_in())&&(pulse_time<240)); pulse_time++) {_delay_us(1);} // �������嵽�����Ҵ��ڵ�ʱ�䲻�ܳ���240us
	if (pulse_time >=240) {return true;}

	return false;
}
/*----------------------------------------------------------------------------*/
/**
  * ��������: ��DS18B20��һ���ֽڣ���λ����
  * �������: ��
  * �� �� ֵ: ����������
  * ˵    ������
  */
static uint8_t ds18b20_read(void)
{
	uint8_t i, s_datum = 0;
	
	for (i=0x01; i>0; i<<=1)
	{
		ds18b20_mode(OUT_MODE); // ��0�Ͷ�1��ʱ������Ҫ����60us 
		ds18b20_low(), _delay_us(5); // ��ʱ�����ʼ���������������� >1us <15us �ĵ͵�ƽ�ź�
		ds18b20_mode(IN_MODE); // ���ó����룬�ͷ����ߣ����ⲿ�������轫��������
		_delay_us(5);
		if (ds18b20_in() == SET) {s_datum |= i;}
		_delay_us(80); // �����ʱ������ο�ʱ��ͼ
	}
	return s_datum;
}
/*----------------------------------------------------------------------------*/
/**
  * ��������: дһ���ֽڵ�DS18B20����λ����
  * �������: dat����д������
  * �� �� ֵ: ��
  * ˵    ������
  */
static void ds18b20_write(uint8_t p_datum)
{
	uint8_t i;

	ds18b20_mode(OUT_MODE);
	for (i=0x01; i>0; i<<=1)
	{
		ds18b20_low();
		/* д0��д1��ʱ������Ҫ����60us */
		if ((p_datum&i) != 0)
		{
			/* 1us < �����ʱ < 15us */
			_delay_us(10);
			ds18b20_high();
			_delay_us(80);
		} else
		{
			/* 60us < Tx 0 < 120us */
			_delay_us(80);
			ds18b20_high();
			/* 1us < Trec(�ָ�ʱ��) < �����*/
			_delay_us(10);
		}
	}
}
/*----------------------------------------------------------------------------*/
/*
 * �洢���¶���16 λ�Ĵ�������չ�Ķ����Ʋ�����ʽ
 * ��������12λ�ֱ���ʱ������5������λ��7������λ��4��С��λ
 *
 *         |---------����----------|-----С�� �ֱ��� 1/(2^4)=0.0625----|
 * ���ֽ�  | 2^3 | 2^2 | 2^1 | 2^0 | 2^(-1) | 2^(-2) | 2^(-3) | 2^(-4) |
 *
 *
 *         |-----����λ��0->��  1->��-------|-----------����-----------|
 * ���ֽ�  |  s  |  s  |  s  |  s  |    s   |   2^6  |   2^5  |   2^4  |
 *
 * 
 * �¶� = ����λ + ���� + С��*0.0625
 */
/**
  * ��������: ������ƥ�� ROM ����»�ȡ DS18B20 �¶�ֵ 
  * �������: ��
  * �� �� ֵ: �¶�ֵ
  * ˵    ������
  */
bool DS18B20_GetTemp_SkipRom(float * p_datum)
{
	uint8_t tpmsb, tplsb;
	short s_tem;

	if (ds18b20_reset() == true) {return true;}
	ds18b20_write(0xCC); // ���� ROM
	ds18b20_write(0x44); // ��ʼת��
	if (ds18b20_reset() == true) {return true;}
	ds18b20_write(0xCC); // ���� ROM
	ds18b20_write(0xBE); // ���¶�ֵ
	tplsb = ds18b20_read();
	tpmsb = ds18b20_read();
	
	s_tem = tpmsb<<8;
	s_tem = s_tem|tplsb;
	
	*p_datum = s_tem*0.0625;
	return false;
}
/*----------------------------------------------------------------------------*/
/**
  * ��������: ��ƥ�� ROM ����»�ȡ DS18B20 �¶�ֵ 
  * �������: ds18b20_id�����ڴ�� DS18B20 ���кŵ�������׵�ַ
  * �� �� ֵ: ��
  * ˵    ������
  */
void ds18b20_reset_id (uint8_t * ds18b20_id)
{
	uint8_t uc;

	ds18b20_write(0x33); //��ȡ���к�
	for (uc=0; uc<8; uc++) {ds18b20_id[uc] = ds18b20_read();}
}
/*----------------------------------------------------------------------------*/
/**
  * ��������: ��ƥ�� ROM ����»�ȡ DS18B20 �¶�ֵ
  * �������: ds18b20_id����� DS18B20 ���кŵ�������׵�ַ
  * �� �� ֵ: �¶�ֵ
  * ˵    ������
  */
bool DS18B20_GetTemp_MatchRom (uint8_t * ds18b20_id,float * p_datum)
{
	uint8_t tpmsb, tplsb, i;
	short s_tem;

	if (ds18b20_reset() == true) {return true;}
	ds18b20_write(0x55); // ƥ�� ROM 
	for (i=0; i<8; i++) {ds18b20_write(ds18b20_id[i]);}
	ds18b20_write(0x44); // ��ʼת��
	if (ds18b20_reset() == true) {return true;}
	ds18b20_write(0x55); // ƥ�� ROM 
	for (i=0; i<8; i++) {ds18b20_write(ds18b20_id[i]);}
	ds18b20_write(0xBE); // ���¶�ֵ
	tplsb = ds18b20_read();
	tpmsb = ds18b20_read();
	
	s_tem = tpmsb<<8;
	s_tem = s_tem|tplsb;
	
	*p_datum = s_tem*0.0625;
	return false;
}
/*----------------------------------------------------------------------------*/
