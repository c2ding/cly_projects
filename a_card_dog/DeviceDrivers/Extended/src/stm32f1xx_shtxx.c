/*
#include "stm32f1xx_shtxx.h" //SHTxxϵ�ж�ȡ
	shtxx_init();//��ʼ��
	shtxx_data_acquisition(); //�ɼ���ʪ������ (100mS����)
	
����:������
*/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_shtxx.h"
/* ˽�к궨�� ----------------------------------------------------------------*/
#define SHT_SDA_GPIO_PIN		GPIO_PIN_10
#define SHT_SDA_GPIO			GPIOB
#define sht_sda_h()				HAL_GPIO_WritePin(SHT_SDA_GPIO, SHT_SDA_GPIO_PIN, GPIO_PIN_SET);
#define sht_sda_l()				HAL_GPIO_WritePin(SHT_SDA_GPIO, SHT_SDA_GPIO_PIN, GPIO_PIN_RESET);
#define sht_sda_r()				HAL_GPIO_ReadPin(SHT_SDA_GPIO, SHT_SDA_GPIO_PIN)

#define SHT_SCK_GPIO_PIN		GPIO_PIN_11
#define SHT_SCK_GPIO			GPIOB
#define sht_sck_h()				HAL_GPIO_WritePin(SHT_SCK_GPIO, SHT_SCK_GPIO_PIN, GPIO_PIN_SET);
#define sht_sck_l()				HAL_GPIO_WritePin(SHT_SCK_GPIO, SHT_SCK_GPIO_PIN, GPIO_PIN_RESET);

	#define RESOLUTION 0.01 //14bit:
//	#define RESOLUTION 0.04 //12bit:

//	#define WORKING_VOLTAGE	40.1 //5V
//	#define WORKING_VOLTAGE	39.8 //4V
	#define WORKING_VOLTAGE	39.7 //3.5V
//	#define WORKING_VOLTAGE	39.6 //3V
//	#define WORKING_VOLTAGE	39.4 //2.5V

#define noACK	0 //��Ӧ��
#define ACK		1 //Ӧ��

#define STATUS_REG_W	0x06 //000 0011 0
#define STATUS_REG_R	0x07 //000 0011 1
#define MEASURE_TEMP	0x03 //000 0001 1
#define MEASURE_HUMI	0x05 //000 0010 1
#define RESET			0x1E //000 1111 0

#define CONNECTION_RESET	0X00 //���Ӹ�λ
#define TEMPERATURE_MODE	0x01 //�¶�ģʽ
#define TEMPERATURE_READ	0x02 //���¶�
#define HUMIDITY_RMODE		0x03 //ʪ��ģʽ
#define HUMIDITY_READ		0x04 //��ʪ��
#define CALCULATION_DATA	0x05 //��������
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
__IO SHTXX g_shtxx = {true,0,0};
/* ˽�к���ԭ�� --------------------------------------------------------------*/
//sda����״̬
static void shtxx_sda_in(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	//SDA ������������
	GPIO_InitStruct.Pin = SHT_SDA_GPIO_PIN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP; 
	HAL_GPIO_Init(SHT_SDA_GPIO, &GPIO_InitStruct);   
}
/*----------------------------------------------------------------------------*/
//sda���״̬
static void shtxx_sda_out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	//SDA �������
	GPIO_InitStruct.Pin = SHT_SCK_GPIO_PIN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(SHT_SDA_GPIO, &GPIO_InitStruct);
}
/*----------------------------------------------------------------------------*/
//д����
static uint8_t shtxx_write(uint8_t value)	 
{
	uint8_t s_i, error = 0;
	shtxx_sda_out();
	for (s_i=0x80; s_i>0; s_i>>=1)
	{
		if (s_i&value) {sht_sda_h();} else {sht_sda_l();}
		_delay_us(1);
		sht_sck_h();
		_delay_us(3);
		sht_sck_l();
	}
	shtxx_sda_in(); //�ͷ�data��
	_delay_us(2);
	sht_sck_h();
	_delay_us(1);
	//sht1x_sda_in();
	error = sht_sda_r();
	sht_sck_l();
	//sht_sck_h();
	shtxx_sda_out();
	sht_sda_h();
	return error;
}
/*----------------------------------------------------------------------------*/
//������
static uint8_t shtxx_read(uint8_t Ack)
{
	uint8_t s_i, val = 0;

	shtxx_sda_in();
	for (s_i=0x80; s_i>0; s_i>>=1)
	{
		_delay_us(1);
		sht_sck_h();
		_delay_us(1);
		if (sht_sda_r()) {val = (val|s_i);} //������
		sht_sck_l();
	}
	shtxx_sda_out();
	if (Ack) {sht_sda_l();} else {sht_sda_h();} //�ж��Ƿ�Ӧ�� ÿ8λУ��
	_delay_us(1);
	sht_sck_h();
	_delay_us(3);
	sht_sck_l();
	_delay_us(3);
	return val;
}
/*----------------------------------------------------------------------------*/
//��������
static void shtxx_start(void)
{
	shtxx_sda_out();
	sht_sda_h();
	sht_sck_l();
	_delay_us(1);
	sht_sck_h();
	_delay_us(1);
	sht_sda_l();
	_delay_us(1);
	sht_sck_l();
	_delay_us(3);
	sht_sck_h();
	_delay_us(1);
	sht_sda_h();
	_delay_us(1);
	sht_sck_l();
	_delay_us(1);
}
/*----------------------------------------------------------------------------*/
//���Ӹ�λ
static void shtxx_connection_reset(void)
{
	uint8_t s_i;

	shtxx_sda_out();
	sht_sda_h();
	sht_sck_l();
	for (s_i=0; s_i<9; s_i++)
	{
		sht_sck_h();
		_delay_us(1);
		sht_sck_l();
		_delay_us(1);
	}
	shtxx_start();
}
/*----------------------------------------------------------------------------*/
//��λ
static uint8_t shtxx_reset(void)
{
	uint8_t error = 0;
	shtxx_connection_reset();
	error += shtxx_write(RESET);
	return error;
}
/*----------------------------------------------------------------------------*/
//�����¶Ⱥ�ʪ��
static void shtxx_calculation_datum(uint16_t p_measuring_temperature, uint16_t p_measuring_humidity)
{
	const float C1 = -2.0468;
	const float C2 = +0.0367;
	const float C3 = -0.0000015955;
	const float T1 = +0.01;
	const float T2 = +0.00008;
	
	float rh = (float)p_measuring_humidity;
	float t  = (float)p_measuring_temperature;
	float rh_lin; //ʪ������ֵ
	float rh_true; //ʪ����ʵֵ
	float t_c; //�¶�

	//RH���Խ��
	t_c = t * RESOLUTION - WORKING_VOLTAGE; //��ѹ �ֱ��� ����
	rh_lin = C3*rh*rh + C2*rh + C1;
	rh_true = (t_c-25)*(T1+T2*rh) + rh_lin;
	//�޶���Χ
	if (rh_true>100) {rh_true = 100;}
	if (rh_true<0.1) {rh_true = 0.1;}
	
	g_shtxx.temperature = t_c; //�����¶�ֵ
	g_shtxx.humidity = rh_true; //����ʪ��ֵ
}
/* ������ --------------------------------------------------------------------*/
//��ʼ��SHT1X
void shtxx_init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStruct;

	//SDA �������
	GPIO_InitStruct.Pin = SHT_SCK_GPIO_PIN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(SHT_SDA_GPIO, &GPIO_InitStruct);
	//SCK ������� sck��������
	GPIO_InitStruct.Pin = SHT_SCK_GPIO_PIN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(SHT_SCK_GPIO, &GPIO_InitStruct);
#ifdef USE_FULL_ASSERT
	printf("sht1x_init(); OK\r\n");
#endif
}
/*----------------------------------------------------------------------------*/
//����¶��
#if defined(__CALCULATED_DEW_POINT__)
static shtxx_calculated_dew_point(void)
{
	float s_k;
	s_k = (log10(g_shtxx.humidity)-2)/0.4343+(17.62*g_shtxx.temperature)/(243.12+g_shtxx.temperature);
	g_shtxx.the_dew_point = 243.12*s_k/(17.62-s_k);
}
#endif
/*----------------------------------------------------------------------------*/
//�ɼ���ʪ������
void shtxx_data_acquisition(void)
{
    static uint16_t s_measuring_temperature; //�����¶�
    static uint16_t s_measuring_humidity; //����ʪ��
	static uint8_t s_heartbeat = 0;
	static uint8_t s_time;
    //uint8_t s_check_value; //У��ֵ
	uint8_t s_value_h = 0;
	uint8_t s_value_l = 0;

	switch (s_heartbeat)
	{
		case CONNECTION_RESET:
			shtxx_connection_reset(); //���Ӹ�λ
			s_heartbeat = TEMPERATURE_MODE;
		break;
		case TEMPERATURE_MODE:
			shtxx_start();
			if (shtxx_write(MEASURE_TEMP)==0) {shtxx_sda_in(); s_time = 0; s_heartbeat = TEMPERATURE_READ;} else {s_heartbeat = RESET;} 
		break;
		case TEMPERATURE_READ:
			if (sht_sda_r() == GPIO_PIN_RESET)
			{
				s_value_h = shtxx_read(ACK); //����λ
				s_value_l = shtxx_read(ACK); //����λ
				//s_check_value = shtxx_read(noACK);
				shtxx_read(noACK);
				s_measuring_temperature = (s_value_h<<8) | s_value_l; //���ߵ�λ���
				s_heartbeat = HUMIDITY_RMODE;
			} else {if (++s_time>32) {s_heartbeat = RESET;}}
		break;
		case HUMIDITY_RMODE:
			shtxx_start();
			if (shtxx_write(MEASURE_HUMI)==0) {shtxx_sda_in(); s_time = 0; s_heartbeat = HUMIDITY_READ;} else {s_heartbeat = RESET;} 
		break;
		case HUMIDITY_READ:
			if (sht_sda_r() == GPIO_PIN_RESET)
			{
				s_value_h = shtxx_read(ACK); //����λ
				s_value_l = shtxx_read(ACK); //����λ
				//s_check_value = shtxx_read(noACK);
				shtxx_read(noACK);
				s_measuring_humidity = (s_value_h<<8) | s_value_l; //���ߵ�λ���
				s_heartbeat = CALCULATION_DATA;
			} else {if (++s_time>32) {s_heartbeat = RESET;}}
		break;
		case CALCULATION_DATA:
			shtxx_calculation_datum(s_measuring_temperature, s_measuring_humidity);
			g_shtxx.error = false;
			s_heartbeat = CONNECTION_RESET;
		break;
		default:
			g_shtxx.error = true;
			shtxx_reset();
			s_heartbeat = TEMPERATURE_MODE;
		break;
	}
}
/*----------------------------------------------------------------------------*/

