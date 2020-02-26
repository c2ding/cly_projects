/*
#include "stm32f1xx_lcd12864.h" //LCD12864液晶驱动
	lcd12864_init(); //LCD初始化
	lcd12864_yx(y, x); //写地址(范围 y0~3;x0~7)
	lcd12864_write_byte(function, datum); //写 function:功能 datum:数据
	lcd12864_write_string(y, x, (uint8_t *)&"0123456789ABCDEF", length); //按指定位置显示一串字符
	lcd12864_write_image((uint8_t *)&bmp); //显示图片
	lcd12864_reverse_white(y, x, whether, words); //从y,x开始/Whether反白(是:0xFF;取消:0x00)/Words几个字(使用前清图形区RAM否则花屏)
	lcd12864_loading_customation((uint8_t *)&datum, address); //装入自定义字符address(0~3)
	lcd12864_display_customation(y, x, address); //显示自定义字符address(0~3)

	lcd12864_clearance_all(); //清整个GDRAM空间
	lcd12864_clearance_txt(); //文本区清RAM
	lcd12864_clearance_image(); //图形区清RAM
	lcd12864_clearance_screen(); //清屏

	0x01:清屏
	0x08:显示关
	0x0C:显示开
	0x0D:光标关
	0x0E:光标开
	0x0F:显示光标,当前显示位反白闪动
	0x1F:字符循环右移
	0x18:字符循环左移
	0x30:基本指集
	0x34:扩展指集
	0x34:绘图关
	0x36:绘图开

	//垂直滚动屏幕(清整个GDRAM空间否则花屏)
	lcd12864_write_byte(COMMAND, 0x36);
	lcd12864_write_byte(COMMAND, 0x03);
	lcd12864_write_byte(COMMAND, 0x60);
	
作者:刘东辉 
*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_lcd12864.h"
#include "stm32f1xx_delay.h"
/* 私有宏定义 ----------------------------------------------------------------*/
//引脚定义
//1:VSS;2:VDD;3:VO;4:RS(CS);5:RW(SID);6:E(SCLK);7:DB0;....14:DB7;15:PSB;16:NC;17:/RST;18:VOUT;19:LED+;20:LED-; 
//(偏压:(VOUT/VO/GND)10~20K可调电阻)(PSB->H并行方式/PSB->L串行方式)
#ifdef __USE_THE_RESET__ //复位
#define LCD12864_RST_gpio	GPIOB
#define LCD12864_RST_pin	GPIO_PIN_0
#define LCD12864_RST_THE_0	HAL_GPIO_WritePin(LCD12864_RST_gpio, LCD12864_RST_pin, GPIO_PIN_RESET)
#define LCD12864_RST_THE_1	HAL_GPIO_WritePin(LCD12864_RST_gpio, LCD12864_RST_pin, GPIO_PIN_SET)
#endif
#ifdef __SERIAL_MODE__ //串口方式
//LCD 片选端
#define LCD12864_CS_port C
#define LCD12864_CS_bit  6
//LCD 串行数据输入/输出端
#define LCD12864_SID_port C
#define LCD12864_SID_bit  5
//LCD 串行同步时钟,上升沿时读取SID数据
#define LCD12864_SCLK_port C
#define LCD12864_SCLK_bit  7
#else
//数据命令选择
#define LCD12864_RS_gpio	GPIOB
#define LCD12864_RS_pin		GPIO_PIN_1
#define LCD12864_RS_THE_0	HAL_GPIO_WritePin(LCD12864_RS_gpio, LCD12864_RS_pin, GPIO_PIN_RESET)
#define LCD12864_RS_THE_1	HAL_GPIO_WritePin(LCD12864_RS_gpio, LCD12864_RS_pin, GPIO_PIN_SET)
//读写控制信号
#define LCD12864_RW_gpio	GPIOB
#define LCD12864_RW_pin		GPIO_PIN_2
#define LCD12864_RW_THE_0	HAL_GPIO_WritePin(LCD12864_RW_gpio, LCD12864_RW_pin, GPIO_PIN_RESET)
#define LCD12864_RW_THE_1	HAL_GPIO_WritePin(LCD12864_RW_gpio, LCD12864_RW_pin, GPIO_PIN_SET)
//使能
#define LCD12864_EN_gpio		GPIOB
#define LCD12864_EN_pin		GPIO_PIN_10
#define LCD12864_EN_THE_0	HAL_GPIO_WritePin(LCD12864_EN_gpio, LCD12864_EN_pin, GPIO_PIN_RESET)
#define LCD12864_EN_THE_1	HAL_GPIO_WritePin(LCD12864_EN_gpio, LCD12864_EN_pin, GPIO_PIN_SET)

//数据接口
//#define LCD12864_DATA_WRITE(datum)			{GPIO_ResetBits(GPIOA, 0xFF00);GPIO_SetBits(GPIOA, datum&0x00FF);}
#define LCD12864_DATA_WRITE(x) GPIOA->ODR=(GPIOA->ODR&0xff00)|(x&0x00FF)
#define LCD12864_DATA_READ() GPIOA->IDR
#endif
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
#ifdef __GRAPHIC_DISPLAY__ //图形库
PROGMEM const uint8_t bmp[] = {
};
#endif

#ifdef __DEFINITION_WORD__ //自定义字符
const uint8_t zk0[]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x01,0x80,0x07,0xE0,
0x07,0xE0,0x01,0x80,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const uint8_t zk1[]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,
0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
#endif
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

//************************************************************************************
#ifdef __SERIAL_MODE__ //串口方式
static uint8_t lcd12864_read(void) //读
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
static void lcd12864_write(uint8_t p_Datum) //写
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
static uint8_t lcd12864_busy(void) //检查忙位
{
#ifdef __SERIAL_MODE__ //串口方式
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
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //速度高

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
//Function:数据或命令选择 Datum:数据或命内容
void lcd12864_write_byte(uint32_t p_function, uint32_t p_datum)
{
#ifdef __SERIAL_MODE__ //串口方式
	LCD12864_CS_PORT = 1;
	if (lcd12864_busy()!=0) {return ;} //检测忙
	lcd12864_write(p_function);
	lcd12864_write(p_datum&0xF0);
	lcd12864_write(p_datum<<4);
	LCD12864_CS_PORT = 0;
#else
	if (lcd12864_busy()!=0) {return ;} //检测忙
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
void lcd12864_clearance_all(void) //清整个GDRAM空间
{
	for (unsigned char y=0x80; y<0xC0; y++) //y+0x80 64
	{
		for (unsigned char x=0x80; x<0x90; x++) //x+0x80 16
		{
			lcd12864_write_byte(COMMAND, 0x34);
			lcd12864_write_byte(COMMAND, y); //行地址
			lcd12864_write_byte(COMMAND, x); //列地址
			lcd12864_write_byte(COMMAND, 0x30);
			lcd12864_write_byte(DATA, 0x00);
			lcd12864_write_byte(DATA, 0x00);
		}
	}
}*/
//------------------------------------------------------------------------------------
void lcd12864_clearance_txt(void) //文本区清RAM函数
{
	uint32_t i;
	lcd12864_write_byte(COMMAND, 0x30); //基本指令集
	lcd12864_write_byte(COMMAND, 0x80); //AC归起始位
	for (i=0; i<64; i++) {lcd12864_write_byte(DATA, 0x20);}
}
//------------------------------------------------------------------------------------
void lcd12864_clearance_image(void) //图形区清RAM函数(图形区和文本区显示在两个不同的RAM区)
{
	uint32_t i,j;
	lcd12864_write_byte(COMMAND, 0x36);
	for (i=0; i<32; i++) //12864实际为256x32
	{
		lcd12864_write_byte(COMMAND, 0x80|i); //行位置
		lcd12864_write_byte(COMMAND, 0x80); //列位置
		for (j=0; j<32; j++)
			{lcd12864_write_byte(DATA, 0x00);}
	}
}
//------------------------------------------------------------------------------------
void lcd12864_init(void) //初始化LCD屏
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //速度高
	//GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	
#ifdef __SERIAL_MODE__ //串口方式
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
#ifdef __USE_THE_RESET__ //复位
	GPIO_InitStruct.Pin = LCD12864_RST_pin;
	HAL_GPIO_Init(LCD12864_RST_gpio, &GPIO_InitStruct);

	LCD12864_RST_THE_0;
	_delay_ms(10);
	LCD12864_RST_THE_1;
#endif
	GPIO_InitStruct.Pin = LCD12864_BACKLIGHT_pin;
	HAL_GPIO_Init(LCD12864_BACKLIGHT_gpio, &GPIO_InitStruct);

	LCD12864_BACKLIGHT_ON;

	lcd12864_write_byte(COMMAND, 0x30); //基本指令集
	lcd12864_write_byte(COMMAND, 0x06); //显示开,光标关
	lcd12864_write_byte(COMMAND, 0x01); //进入点设定,光标向右移动,AC+1
	lcd12864_write_byte(COMMAND, 0x0C); //清屏
	lcd12864_write_byte(COMMAND, 0x02); //清屏
	lcd12864_clearance_txt(); //文本区清RAM
	lcd12864_clearance_image(); //图形区清RAM
#ifdef USE_FULL_ASSERT
	printf("lcd12864_init(); OK\r\n"); 
#endif
}
//************************************************************************************
const uint8_t lcd12864_address[4] = {0x80,0x90,0x88,0x98};
void lcd12864_yx(uint32_t p_y, uint32_t p_x) //写地址(范围 y0~3;x0~7)
{
	lcd12864_write_byte(COMMAND, 0x30);
	lcd12864_write_byte(COMMAND, lcd12864_address[p_y]|p_x);
}
//------------------------------------------------------------------------------------
//按指定位置显示一串字符(行,列,数据)
void lcd12864_write_string(uint32_t p_y, uint32_t p_x, uint8_t *p_datum, uint32_t p_length)
{
	lcd12864_yx(p_y, p_x);
	do {lcd12864_write_byte(DATA, *p_datum++);} while(--p_length);
}
//------------------------------------------------------------------------------------
#ifdef __DEFINITION_WORD__
PROGMEM const uint8_t customation_address[4] = {0x40,0x50,0x60,0x70};
//装入自定义字符//address(0~3)
void lcd12864_loading_customation(uint8_t *p_datum, uint32_t p_address)
{
	lcd12864_write_byte(COMMAND, 0x30);
	lcd12864_write_byte(COMMAND, pgm_read_byte(&customation_address[p_address]));
	for (uint8_t i=0; i<32; i++) {lcd12864_write_byte(DATA, pgm_read_byte(&p_datum[i]));}
}
//------------------------------------------------------------------------------------
//显示自定义字符 Address(0~3)
void lcd12864_display_customation(uint32_t p_y, uint32_t p_x, uint32_t p_address)
{
	lcd12864_yx(p_y, p_x);
	lcd12864_write_byte(DATA, 0);
	lcd12864_write_byte(DATA, p_address*2);
}
#endif
//------------------------------------------------------------------------------------
#ifdef __GRAPHIC_DISPLAY__ //显示图片
void lcd12864_write_image(uint8_t *p_image)
{
	uint16_t j = 0;
	lcd12864_write_byte(COMMAND, 0x34); //关显示
	for (uint8_t i=0; i<9; i+=8)
	{
		for (uint8_t y=0x80; y<0xA0; y++)
		{
			for (uint8_t x=0x80; x<0x88; x++)
			{
				lcd12864_write_byte(COMMAND, y); //行地址
				lcd12864_write_byte(COMMAND, x|i); //列地址
				lcd12864_write_byte(DATA, pgm_read_byte(&p_image[j++]));
				lcd12864_write_byte(DATA, pgm_read_byte(&p_image[j++]));
			}
		}
	}
	lcd12864_write_byte(COMMAND, 0x36); //开绘图显示
}
#endif
//------------------------------------------------------------------------------------
#ifdef __REVERSE_WHITE__ //从y,x开始/Whether反白(是:0xFF;取消:0x00)/Words几个字
PROGMEM const uint8_t reverse_white_address[4][2] = {{0x80,0x80},{0x80,0x90},{0x88,0x80},{0x88,0x90}};
void lcd12864_reverse_white(uint32_t p_y, uint32_t p_x, uint32_t p_whether, uint32_t p_words)
{
//	p_whether = (p_whether>0) ? 0xFF : 0x00; //是否反白
	lcd12864_write_byte(COMMAND,0x36); //扩充指令集

	p_x |= pgm_read_byte(&reverse_white_address[p_y][0]);
	p_y  = pgm_read_byte(&reverse_white_address[p_y][1]);

	for(uint8_t i=16; i>0; i--) //16行，因为是16*16汉字
	{
		lcd12864_write_byte(COMMAND,p_y++); //设置绘图区的Y地址坐标0
		lcd12864_write_byte(COMMAND,p_x); //设置绘图区的X地址坐标0
		for(uint8_t j=p_words<<1; j>0; j--) {lcd12864_write_byte(DATA,p_whether);}
	}
}
#endif
