/*
#include "stm32f1xx_lcd12864.h" //LCD12864Һ������
	lcd12864_init(); //LCD��ʼ��
	lcd12864_yx(y, x); //д��ַ(��Χ y0~3;x0~7)
	lcd12864_write_byte(function, datum); //д function:���� datum:����
	lcd12864_write_string(y, x, (uint8_t *)&"0123456789ABCDEF", length); //��ָ��λ����ʾһ���ַ�
	lcd12864_write_image((uint8_t *)&bmp); //��ʾͼƬ
	lcd12864_reverse_white(y, x, whether, words); //��y,x��ʼ/Whether����(��:0xFF;ȡ��:0x00)/Words������(ʹ��ǰ��ͼ����RAM������)
	lcd12864_loading_customation((uint8_t *)&datum, address); //װ���Զ����ַ�address(0~3)
	lcd12864_display_customation(y, x, address); //��ʾ�Զ����ַ�address(0~3)

	lcd12864_clearance_all(); //������GDRAM�ռ�
	lcd12864_clearance_txt(); //�ı�����RAM
	lcd12864_clearance_image(); //ͼ������RAM
	lcd12864_clearance_screen(); //����

	0x01:����
	0x08:��ʾ��
	0x0C:��ʾ��
	0x0D:����
	0x0E:��꿪
	0x0F:��ʾ���,��ǰ��ʾλ��������
	0x1F:�ַ�ѭ������
	0x18:�ַ�ѭ������
	0x30:����ָ��
	0x34:��չָ��
	0x34:��ͼ��
	0x36:��ͼ��

	//��ֱ������Ļ(������GDRAM�ռ������)
	lcd12864_write_byte(COMMAND, 0x36);
	lcd12864_write_byte(COMMAND, 0x03);
	lcd12864_write_byte(COMMAND, 0x60);
	
����:������ 
*/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_lcd12864.h"
#include "stm32f1xx_delay.h"
/* ˽�к궨�� ----------------------------------------------------------------*/
//���Ŷ���
//1:VSS;2:VDD;3:VO;4:RS(CS);5:RW(SID);6:E(SCLK);7:DB0;....14:DB7;15:PSB;16:NC;17:/RST;18:VOUT;19:LED+;20:LED-; 
//(ƫѹ:(VOUT/VO/GND)10~20K�ɵ�����)(PSB->H���з�ʽ/PSB->L���з�ʽ)
#ifdef __USE_THE_RESET__ //��λ
#define LCD12864_RST_gpio	GPIOB
#define LCD12864_RST_pin	GPIO_PIN_0
#define LCD12864_RST_THE_0	HAL_GPIO_WritePin(LCD12864_RST_gpio, LCD12864_RST_pin, GPIO_PIN_RESET)
#define LCD12864_RST_THE_1	HAL_GPIO_WritePin(LCD12864_RST_gpio, LCD12864_RST_pin, GPIO_PIN_SET)
#endif
#ifdef __SERIAL_MODE__ //���ڷ�ʽ
//LCD Ƭѡ��
#define LCD12864_CS_port C
#define LCD12864_CS_bit  6
//LCD ������������/�����
#define LCD12864_SID_port C
#define LCD12864_SID_bit  5
//LCD ����ͬ��ʱ��,������ʱ��ȡSID����
#define LCD12864_SCLK_port C
#define LCD12864_SCLK_bit  7
#else
//��������ѡ��
#define LCD12864_RS_gpio	GPIOB
#define LCD12864_RS_pin		GPIO_PIN_1
#define LCD12864_RS_THE_0	HAL_GPIO_WritePin(LCD12864_RS_gpio, LCD12864_RS_pin, GPIO_PIN_RESET)
#define LCD12864_RS_THE_1	HAL_GPIO_WritePin(LCD12864_RS_gpio, LCD12864_RS_pin, GPIO_PIN_SET)
//��д�����ź�
#define LCD12864_RW_gpio	GPIOB
#define LCD12864_RW_pin		GPIO_PIN_2
#define LCD12864_RW_THE_0	HAL_GPIO_WritePin(LCD12864_RW_gpio, LCD12864_RW_pin, GPIO_PIN_RESET)
#define LCD12864_RW_THE_1	HAL_GPIO_WritePin(LCD12864_RW_gpio, LCD12864_RW_pin, GPIO_PIN_SET)
//ʹ��
#define LCD12864_EN_gpio		GPIOB
#define LCD12864_EN_pin		GPIO_PIN_10
#define LCD12864_EN_THE_0	HAL_GPIO_WritePin(LCD12864_EN_gpio, LCD12864_EN_pin, GPIO_PIN_RESET)
#define LCD12864_EN_THE_1	HAL_GPIO_WritePin(LCD12864_EN_gpio, LCD12864_EN_pin, GPIO_PIN_SET)

//���ݽӿ�
//#define LCD12864_DATA_WRITE(datum)			{GPIO_ResetBits(GPIOA, 0xFF00);GPIO_SetBits(GPIOA, datum&0x00FF);}
#define LCD12864_DATA_WRITE(x) GPIOA->ODR=(GPIOA->ODR&0xff00)|(x&0x00FF)
#define LCD12864_DATA_READ() GPIOA->IDR
#endif
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
#ifdef __GRAPHIC_DISPLAY__ //ͼ�ο�
PROGMEM const uint8_t bmp[] = {
};
#endif

#ifdef __DEFINITION_WORD__ //�Զ����ַ�
const uint8_t zk0[]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x01,0x80,0x07,0xE0,
0x07,0xE0,0x01,0x80,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const uint8_t zk1[]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,
0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
#endif
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

//************************************************************************************
#ifdef __SERIAL_MODE__ //���ڷ�ʽ
static uint8_t lcd12864_read(void) //��
{
	uint8_t s_datum = 0, i;
	for (i=8; i>0; i--)
	{
		LCD12864_SCLK_PORT = 1;
		s_datum <<= 1;
		LCD12864_SCLK_PORT = 0;
		if (LCD12864_SID_PIN) {s_datum |= 0x01;}
	}
	return s_datum;
}
//------------------------------------------------------------------------------------
static void lcd12864_write(uint8_t p_Datum) //д
{
	uint8_t i;
	for (i=8; i>0; i--)
	{
		LCD12864_SID_PORT = p_Datum&0x80 ? 1 : 0;
		LCD12864_SCLK_PORT = 1;
		p_Datum <<= 1;
		LCD12864_SCLK_PORT = 0;
	}
}
#endif
//------------------------------------------------------------------------------------
static uint8_t lcd12864_busy(void) //���æλ
{
#ifdef __SERIAL_MODE__ //���ڷ�ʽ
	uint8_t s_datum = 0;
	uint8_t s_temp = 255;//(2500.0*F_CPU/1000000.0)/330;
	do{
		lcd12864_write(0xFC);
		LCD12864_SCLK_DDR = 0;
		s_datum |= lcd12864_read()&0xF0;
		s_datum |= lcd12864_read()>>4;
		LCD12864_SCLK_DDR = 1;
		if ((s_temp--)==0) {return 1;}
	} while ((s_datum&0x80)==0x80);
#else
	GPIO_InitTypeDef GPIO_InitStruct;
	uint32_t s_temp;

	GPIO_InitStruct.Pin = 0x00FF;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //�ٶȸ�

	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	LCD12864_RS_THE_0;
	LCD12864_RW_THE_1;
	//_delay_us(1);
	for (;;)
	{
		LCD12864_EN_THE_1;
		_delay_us(1);
		s_temp = GPIOA->IDR;
		LCD12864_EN_THE_0;
		if ((s_temp&0x0080) == 0) {break ;}
	}
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
	return 0;
}
//------------------------------------------------------------------------------------
//Function:���ݻ�����ѡ�� Datum:���ݻ�������
void lcd12864_write_byte(uint32_t p_function, uint32_t p_datum)
{
#ifdef __SERIAL_MODE__ //���ڷ�ʽ
	LCD12864_CS_PORT = 1;
	if (lcd12864_busy()!=0) {return ;} //���æ
	lcd12864_write(p_function);
	lcd12864_write(p_datum&0xF0);
	lcd12864_write(p_datum<<4);
	LCD12864_CS_PORT = 0;
#else
	if (lcd12864_busy()!=0) {return ;} //���æ
	//_delay_ms(2);
	LCD12864_RW_THE_0;
	if (p_function>0) {LCD12864_RS_THE_1;} else {LCD12864_RS_THE_0;}
	_delay_us(1);
	LCD12864_DATA_WRITE(p_datum);
	LCD12864_EN_THE_1;
	_delay_us(1);
	LCD12864_EN_THE_0;
#endif
}
//------------------------------------------------------------------------------------
/*
void lcd12864_clearance_all(void) //������GDRAM�ռ�
{
	for (unsigned char y=0x80; y<0xC0; y++) //y+0x80 64
	{
		for (unsigned char x=0x80; x<0x90; x++) //x+0x80 16
		{
			lcd12864_write_byte(COMMAND, 0x34);
			lcd12864_write_byte(COMMAND, y); //�е�ַ
			lcd12864_write_byte(COMMAND, x); //�е�ַ
			lcd12864_write_byte(COMMAND, 0x30);
			lcd12864_write_byte(DATA, 0x00);
			lcd12864_write_byte(DATA, 0x00);
		}
	}
}*/
//------------------------------------------------------------------------------------
void lcd12864_clearance_txt(void) //�ı�����RAM����
{
	uint32_t i;
	lcd12864_write_byte(COMMAND, 0x30); //����ָ�
	lcd12864_write_byte(COMMAND, 0x80); //AC����ʼλ
	for (i=0; i<64; i++) {lcd12864_write_byte(DATA, 0x20);}
}
//------------------------------------------------------------------------------------
void lcd12864_clearance_image(void) //ͼ������RAM����(ͼ�������ı�����ʾ��������ͬ��RAM��)
{
	uint32_t i,j;
	lcd12864_write_byte(COMMAND, 0x36);
	for (i=0; i<32; i++) //12864ʵ��Ϊ256x32
	{
		lcd12864_write_byte(COMMAND, 0x80|i); //��λ��
		lcd12864_write_byte(COMMAND, 0x80); //��λ��
		for (j=0; j<32; j++)
			{lcd12864_write_byte(DATA, 0x00);}
	}
}
//------------------------------------------------------------------------------------
void lcd12864_init(void) //��ʼ��LCD��
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //�ٶȸ�
	//GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	
#ifdef __SERIAL_MODE__ //���ڷ�ʽ
	LCD12864_CS_DDR  = PORT_OUT;
	LCD12864_CS_PORT = 0;
	LCD12864_SID_DDR  = PORT_OUT;
	LCD12864_SID_PORT = 0;
	LCD12864_SCLK_DDR  = PORT_OUT;
	LCD12864_SCLK_PORT = 0;
#else

    GPIO_InitStruct.Pin = LCD12864_RS_pin;
	HAL_GPIO_Init(LCD12864_RS_gpio, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD12864_RW_pin;
	HAL_GPIO_Init(LCD12864_RW_gpio, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD12864_EN_pin;
	HAL_GPIO_Init(LCD12864_EN_gpio, &GPIO_InitStruct);

#endif
#ifdef __USE_THE_RESET__ //��λ
	GPIO_InitStruct.Pin = LCD12864_RST_pin;
	HAL_GPIO_Init(LCD12864_RST_gpio, &GPIO_InitStruct);

	LCD12864_RST_THE_0;
	_delay_ms(10);
	LCD12864_RST_THE_1;
#endif
	GPIO_InitStruct.Pin = LCD12864_BACKLIGHT_pin;
	HAL_GPIO_Init(LCD12864_BACKLIGHT_gpio, &GPIO_InitStruct);

	LCD12864_BACKLIGHT_ON;

	lcd12864_write_byte(COMMAND, 0x30); //����ָ�
	lcd12864_write_byte(COMMAND, 0x06); //��ʾ��,����
	lcd12864_write_byte(COMMAND, 0x01); //������趨,��������ƶ�,AC+1
	lcd12864_write_byte(COMMAND, 0x0C); //����
	lcd12864_write_byte(COMMAND, 0x02); //����
	lcd12864_clearance_txt(); //�ı�����RAM
	lcd12864_clearance_image(); //ͼ������RAM
#ifdef USE_FULL_ASSERT
	printf("lcd12864_init(); OK\r\n"); 
#endif
}
//************************************************************************************
const uint8_t lcd12864_address[4] = {0x80,0x90,0x88,0x98};
void lcd12864_yx(uint32_t p_y, uint32_t p_x) //д��ַ(��Χ y0~3;x0~7)
{
	lcd12864_write_byte(COMMAND, 0x30);
	lcd12864_write_byte(COMMAND, lcd12864_address[p_y]|p_x);
}
//------------------------------------------------------------------------------------
//��ָ��λ����ʾһ���ַ�(��,��,����)
void lcd12864_write_string(uint32_t p_y, uint32_t p_x, uint8_t *p_datum, uint32_t p_length)
{
	lcd12864_yx(p_y, p_x);
	do {lcd12864_write_byte(DATA, *p_datum++);} while(--p_length);
}
//------------------------------------------------------------------------------------
#ifdef __DEFINITION_WORD__
PROGMEM const uint8_t customation_address[4] = {0x40,0x50,0x60,0x70};
//װ���Զ����ַ�//address(0~3)
void lcd12864_loading_customation(uint8_t *p_datum, uint32_t p_address)
{
	lcd12864_write_byte(COMMAND, 0x30);
	lcd12864_write_byte(COMMAND, pgm_read_byte(&customation_address[p_address]));
	for (uint8_t i=0; i<32; i++) {lcd12864_write_byte(DATA, pgm_read_byte(&p_datum[i]));}
}
//------------------------------------------------------------------------------------
//��ʾ�Զ����ַ� Address(0~3)
void lcd12864_display_customation(uint32_t p_y, uint32_t p_x, uint32_t p_address)
{
	lcd12864_yx(p_y, p_x);
	lcd12864_write_byte(DATA, 0);
	lcd12864_write_byte(DATA, p_address*2);
}
#endif
//------------------------------------------------------------------------------------
#ifdef __GRAPHIC_DISPLAY__ //��ʾͼƬ
void lcd12864_write_image(uint8_t *p_image)
{
	uint16_t j = 0;
	lcd12864_write_byte(COMMAND, 0x34); //����ʾ
	for (uint8_t i=0; i<9; i+=8)
	{
		for (uint8_t y=0x80; y<0xA0; y++)
		{
			for (uint8_t x=0x80; x<0x88; x++)
			{
				lcd12864_write_byte(COMMAND, y); //�е�ַ
				lcd12864_write_byte(COMMAND, x|i); //�е�ַ
				lcd12864_write_byte(DATA, pgm_read_byte(&p_image[j++]));
				lcd12864_write_byte(DATA, pgm_read_byte(&p_image[j++]));
			}
		}
	}
	lcd12864_write_byte(COMMAND, 0x36); //����ͼ��ʾ
}
#endif
//------------------------------------------------------------------------------------
#ifdef __REVERSE_WHITE__ //��y,x��ʼ/Whether����(��:0xFF;ȡ��:0x00)/Words������
PROGMEM const uint8_t reverse_white_address[4][2] = {{0x80,0x80},{0x80,0x90},{0x88,0x80},{0x88,0x90}};
void lcd12864_reverse_white(uint32_t p_y, uint32_t p_x, uint32_t p_whether, uint32_t p_words)
{
//	p_whether = (p_whether>0) ? 0xFF : 0x00; //�Ƿ񷴰�
	lcd12864_write_byte(COMMAND,0x36); //����ָ�

	p_x |= pgm_read_byte(&reverse_white_address[p_y][0]);
	p_y  = pgm_read_byte(&reverse_white_address[p_y][1]);

	for(uint8_t i=16; i>0; i--) //16�У���Ϊ��16*16����
	{
		lcd12864_write_byte(COMMAND,p_y++); //���û�ͼ����Y��ַ����0
		lcd12864_write_byte(COMMAND,p_x); //���û�ͼ����X��ַ����0
		for(uint8_t j=p_words<<1; j>0; j--) {lcd12864_write_byte(DATA,p_whether);}
	}
}
#endif
