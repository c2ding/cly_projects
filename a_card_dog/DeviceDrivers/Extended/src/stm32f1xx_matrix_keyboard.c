/*
#include "stm32f1xx_matrix_keyboard.h"//����ʶ���밴���¼�����
	matrix_keyboard_init(); //������ʼ��
	matrix_keyboard_scan(); //����ɨ��,��������Ӧ���¼�
	
����:������ 
*/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_matrix_keyboard.h"
#include "port_bit.h"
#include "entrance.h"
/*
00 //����ģʽ
01 //���ģʽ 10MHZ
02 //���ģʽ 2MHZ
03 //���ģʽ 50MHZ

00 //���� ģ������ģʽ
04 //���� ��������ģʽ
08 //���� ����\��������ģʽ
12 //���� ����
00 //��� �������ģʽ
04 //��� ��©���ģʽ
08 //��� ���ù��� �������ģʽ
12 //��� ���ù��ܿ�©���ģʽ
3:�������7:��©���4:��©����8:����������
*/
/* ˽�к궨�� ----------------------------------------------------------------*/
	#define USE_DOUBLE_CLICK //ʹ�� ˫��
	#define USE_LONG_PRESS //ʹ�� ����
	#define USE_CONTINGOUS_TRIGGER //ʹ�� ����
//��
#define PORT_TRANSVERSE_CRL SFR(GPIOA->CRL, 0, 4) //����λ(P0~P7)
#define PORT_TRANSVERSE_CRH SFR(GPIOA->CRH, 0, 4) //����λ(P8~P15)
#define PORT_TRANSVERSE_IDR SFR(GPIOA->IDR, 0, 4) //д
#define PORT_TRANSVERSE_ODR SFR(GPIOA->ODR, 0, 4) //��
//��
#define PORT_VERTICAL_CRL SFR(GPIOA->CRL, 4, 4)
#define PORT_VERTICAL_CRH SFR(GPIOA->CRH, 4, 4)
#define PORT_VERTICAL_IDR SFR(GPIOA->IDR, 4, 4)
#define PORT_VERTICAL_ODR SFR(GPIOA->ODR, 4, 4)
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
const uint16_t key_coding[] = {
~0x0101, //0
~0x0102, //1
~0x0104, //2
~0x0108, //3
~0x0201, //4
~0x0202, //5
~0x0204, //6
~0x0208, //7
~0x0401, //8
~0x0402, //9
~0x0404, //A
~0x0408, //B
~0x0801, //C
~0x0802, //D
~0x0804, //E
~0x0808, //F
};
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
void matrix_keyboard_init(void) //������ʼ��
{
	PORT_TRANSVERSE_OUT = 0x0;
	PORT_VERTICAL_OUT = 0x0;
}
/*----------------------------------------------------------------------------*/
void matrix_keyboard_scan(void) //����ɨ��,��������Ӧ���¼�
{
	union {
		uint16_t key; //��ȡ�ļ�ֵ
		struct {
			uint16_t transverse : 8; //��
			uint16_t vertical : 8; //��
		} bus;
	} obtain;
	uint8_t s_i;
	uint8_t current_key = 0xFF; //��ǰ����
	static uint8_t break_key = 0xFF; //���ݼ�ֵ
	static uint8_t break_state = 0xFF; //����״̬(����Ϊ�ߵ�ƽ��ЧʱӦΪ0x00)
	static uint8_t press_time = 0xFF; //����ʱ��
	#ifdef USE_DOUBLE_CLICK
	static uint8_t lifting_time = 0xFF; //̧��ʱ��
	static uint8_t single_click = 0xFF; //���� ��־
	#endif

//��ȡ��ֵ
	//PORT_TRANSVERSE_OUT = 0x0;
	PORT_TRANSVERSE_DIR = 0xF;
	//PORT_VERTICAL_DIR = 0x0;
	_delay_us(5);
	obtain.bus.vertical = PORT_VERTICAL_IN;
	//PORT_VERTICAL_OUT = 0x0;
	PORT_VERTICAL_DIR = 0xF;
	PORT_TRANSVERSE_DIR = 0x0;
	_delay_us(5);
	obtain.bus.transverse = PORT_TRANSVERSE_IN;
	PORT_VERTICAL_DIR = 0x0;
//����ɼ�ֵ
	for (s_i=0; s_i<(sizeof(key_coding)/2); s_i++) {if (obtain.key == key_coding[s_i]) {current_key = s_i;}}
//����
	if (break_state != current_key)
	{
		break_state = current_key;
		if (current_key != 0xFF) {press_time = 0, break_key = current_key;} //����
		#ifdef USE_DOUBLE_CLICK
		else {lifting_time = 0;} //̧��
		#endif
	} 

	if (current_key != 0xFF) //����
	{
		if (press_time <= 10)
		{
			press_time++;
			#ifdef USE_LONG_PRESS
			if (press_time == 10) {button_event = 1, g_system.button_datum = break_key|LONG_PRESS;} //����
			#endif
		}
		#ifdef USE_CONTINGOUS_TRIGGER
		else {button_event = 1, g_system.button_datum = break_key|CONTINGOUS_TRIGGER;} //����
		#endif
	} else //̧��
	{
		#ifdef USE_DOUBLE_CLICK
		if (lifting_time < 6)
		{
			lifting_time++;
			if (press_time <= 3) //�̰�(0.3s)
			{
				if (single_click == 0xFF)
				{
					single_click = break_key;
					press_time = 0xFF;
				} else //˫��
				{
					if (single_click == break_key)
					{
						button_event = 1, g_system.button_datum = break_key|DOUBLE_CLICK;
					}
					single_click = 0xFF;
					lifting_time = 0xFF;
				}
			} else if (lifting_time > 3)
			{
				if (single_click == break_key)
				{
					button_event = 1, g_system.button_datum = break_key|SINGLE_CLICK;
					lifting_time = 0xFF;
				}
				single_click = 0xFF;
			}
		}
		#else
		if (press_time <= 3) //�̰�
		{
			button_event = 1, g_system.button_datum = break_key|SINGLE_CLICK;
			press_time = 0xFF;
		}
		#endif
	}
}
/*----------------------------------------------------------------------------*/
