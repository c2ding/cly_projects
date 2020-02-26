/*
#include "stm32f1xx_tf_spi.h" //TF 驱动
	hspi2.Init.Mode = SPI_MODE_MASTER; //主机模式
	hspi2.Init.Direction = SPI_DIRECTION_2LINES_RXONLY; //选择全双工SPI模式
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT; //8位SPI
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW; //时钟悬空高电平
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE; //在第二个时钟采集数据
	hspi2.Init.NSS = SPI_NSS_SOFT; //Nss使用软件控制
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB; //从最高位开始传输
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 7;

作者:刘东辉
*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_tf_spi.h"
/* 私有宏定义 ----------------------------------------------------------------*/
//片选
#define TF_CS_PORT	GPIOD
#define TF_CS_PIN	GPIO_PIN_8
#define TF_CS_SET()	HAL_GPIO_WritePin(TF_CS_PORT, TF_CS_PIN, GPIO_PIN_SET)
#define TF_CS_CLR()	HAL_GPIO_WritePin(TF_CS_PORT, TF_CS_PIN, GPIO_PIN_RESET)

//使用的SPI端口
#if defined(__STM32F1XX_SPI1_H__)
#define TF_SPI SPI1
#endif
#if defined(__STM32F1XX_SPI2_H__)
#define TF_SPI SPI2
#endif
#if defined(__STM32F1XX_SPI3_H__)
#define TF_SPI SPI3
#endif

/* 定义卡的类型 */
#define SD_TYPE_ERR     ((uint8_t)0X00)
#define SD_TYPE_MMC     ((uint8_t)0X01)
#define SD_TYPE_V1      ((uint8_t)0X02)
#define SD_TYPE_V2      ((uint8_t)0X04)
#define SD_TYPE_V2HC    ((uint8_t)0X06)
/* SD卡指令表 */  	   
#define SD_CMD0    0       //卡复位
#define SD_CMD1    1
#define SD_CMD8    8       //命令8 ，SEND_IF_COND
#define SD_CMD9    9       //命令9 ，读CSD数据
#define SD_CMD10   10      //命令10，读CID数据
#define SD_CMD12   12      //命令12，停止数据传输
#define SD_CMD16   16      //命令16，设置SectorSize 应返回0x00
#define SD_CMD17   17      //命令17，读sector
#define SD_CMD18   18      //命令18，读Multi sector
#define SD_CMD23   23      //命令23，设置多sector写入前预先擦除N个block
#define SD_CMD24   24      //命令24，写sector
#define SD_CMD25   25      //命令25，写Multi sector
#define SD_CMD41   41      //命令41，应返回0x00
#define SD_CMD55   55      //命令55，应返回0x01
#define SD_CMD58   58      //命令58，读OCR信息
#define SD_CMD59   59      //命令59，使能/禁止CRC，应返回0x00
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
uint8_t SD_TYPE; //保存SD卡的类型
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

//设置SPI2的分频数，以改变SPI2的速度.
void SPI2_SetSpeed(uint8_t Speed)
{
	TF_SPI->CR1 &= 0xFFC7;
	TF_SPI->CR1 |= Speed;
	TF_SPI->CR1 |= ((uint16_t)0x0040);
}
/*----------------------------------------------------------------------------*/
//写入一个字节数据同时读取一个字节数据
uint8_t SPI2_WriteReadData(uint8_t dat)
{
	uint16_t retry = 0;
	while ((TF_SPI->SR&1<<1)==0) {if (++retry>=0XFFFE) {return 0;}} //等待发送区空 超时退出
	TF_SPI->DR = dat; //发送一个byte
	retry = 0;
	while ((TF_SPI->SR&1<<0)==0) {if (++retry>=0XFFFE) {return 0;}} //等待接收完一个byte 超时退出
	return TF_SPI->DR; //返回收到的数据
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_write_cmd
* Description    : 向SD卡发送一个命令.SD卡一个命令格式包括8位命令，
*                * 32位命令参数和7位CRC效验位，最后一位为1
* Input          : cmd：命令
*                * dat: 命令参数
*                * crc: CRC效验
* Output         : None
* Return         : r1：SD卡返回的数据
*                * 0xFF：表示发送失败
*/
static uint8_t tf_write_cmd(uint8_t cmd, uint32_t dat, uint8_t crc)
{	 
	uint8_t r1 = 0;
	uint16_t i = 0;

	TF_CS_SET(); // 复位SD卡,取消上次片选
	SPI2_WriteReadData(0xFF); //额外提供8个时钟

	TF_CS_CLR();
	while (SPI2_WriteReadData(0xFF) != 0xFF) //等待卡是否准备好
	{
		if(++i > 100) {return 0xFF;} //等待失败返回
	}
	SPI2_WriteReadData(cmd|0x40); // 发送数据
	SPI2_WriteReadData((uint8_t)(dat>>24)); //发送Dat的最高8位
	SPI2_WriteReadData((uint8_t)(dat>>16));
	SPI2_WriteReadData((uint8_t)(dat>>8));
	SPI2_WriteReadData((uint8_t)(dat>>0));
	SPI2_WriteReadData(crc);
	if (cmd == SD_CMD12) {SPI2_WriteReadData(0xFF);} //如果是停止数据传输命令，额外多发一个时钟
	i = 0;
	do {
		r1 = SPI2_WriteReadData(0xFF);
		if(++i > 100) {return 0xFF;}
	} while((r1 & 0x80) != 0); //发送成功的最高位是0 

	return r1;
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_init
* Description    : 初始化SD卡.
* Input          : None
* Output         : None
* Return         : 0xFF：初始化失败
*		         * 0：初始化成功
*/
int8_t tf_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	uint8_t r1, buf[4];
	uint16_t i = 0;
	
	GPIO_InitStruct.Pin = TF_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TF_CS_PORT, &GPIO_InitStruct);
	HAL_GPIO_LockPin(TF_CS_PORT, TF_CS_PIN); //锁定配置

	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_256);
	// 将SD卡通信模式转换为SPI模式，上电默认是用DAT0作数据线
	// 接到CMD0时，CS信号有效，SPI模式启动
	for (i=0; i<0x0F; i++) {SPI2_WriteReadData(0xFF);}//初始时，先发送至少74个时钟，这个是必须的

	i = 0;
	while (tf_write_cmd(SD_CMD0, 0, 0x95) != 0x01) // 当读取到0x01的时候表示初始化成功
	{
		if (++i > 100) {return 0xFF;} //初始化失败返回0
	}

	i = 0;
	do {
		if (++i > 100) {break;} //若是发送超过次数跳出循环管
		r1 = tf_write_cmd(SD_CMD8, 0x01AA, 0x87); // 发送CMD8，检测是否SD V2.0
	} while(r1 != 0x01); //发送CMD8
	
	if (r1 == 0x01) //如果CMD8有回应说明是SD V2.0协议
	{
		for (i=0; i<4; i++) {buf[i] = SPI2_WriteReadData(0xFF);} // 读取CMD8的返回值，检测是否支持电压
		if ((buf[2] != 0x01) || (buf[3] != 0xAA)) {return 0xFF;} // 卡电压不支持电压，返回错误
		
		// 初始化SD卡
		i = 0;
		do {
			if (++i > 100) {return 0xFF;} //返回失败
			tf_write_cmd(SD_CMD55, 0, 0x01);
			r1 = tf_write_cmd(SD_CMD41, 0x40000000, 0x01);
		} while(r1 != 0); //写入CMD41成功了，这里省略读取CMD41是否设置成功。

		// 检测是SDHC卡还是SD卡
		i = 0;
		while (tf_write_cmd(SD_CMD58, 0, 0x01) != 0) {if (++i > 100) {SD_TYPE = SD_TYPE_ERR; break;}}
		for (i=0; i<4; i++) {buf[i] = SPI2_WriteReadData(0xFF);} // 读取OCR
		if (buf[0] & 0x40) {SD_TYPE = SD_TYPE_V2HC;} else {SD_TYPE = SD_TYPE_V2;}
	}
	else //否则就是SD V1.0或者MMC V3
	{
		tf_write_cmd(SD_CMD55, 0x00, 0x01);
		r1 = tf_write_cmd(SD_CMD41, 0x00, 0x01);
		if (r1 <= 1) //对CMD41有回应说明是SD V1.0
		{
			SD_TYPE = SD_TYPE_V1; //是V1.0卡
			i = 0;
			do {
				if(i > 100) {return 0xFF;}
				tf_write_cmd(SD_CMD55, 0x00, 0x01);
				r1 = tf_write_cmd(SD_CMD41, 0x00, 0x01);
			} while(r1 != 0);
		}
		else //没有回应说明是MMC V3
		{
			SD_TYPE = SD_TYPE_MMC; //卡类型是MMC卡
			i = 0;
			while (tf_write_cmd(SD_CMD1, 0, 0x01) != 0) {if (++i > 100) {return 0xFF;}}
		}
	}
	TF_CS_SET(); //取消片选
	SPI2_WriteReadData(0xFF);
	//SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_128);
	return 0;
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_read_capacity
* Description    : 读取SD卡的内存大小.
* Input          : capacity：读取到的数值保存指针
* Output         : 内存大小，单位为KB
* Return         : 0xFF：初始化失败
*		         * 0：初始化成功
*/
int8_t tf_read_capacity(uint32_t *capacity)
{
	uint8_t csdValue[16];
	uint16_t n, i = 0;
	
	TF_CS_CLR();
	while (tf_write_cmd(SD_CMD9, 0, 0x01) != 0) // 发送命令
	{
		if (++i > 100) {return 0xFF;} //发送命令失败
	}

	i = 0;
	while (SPI2_WriteReadData(0xFF) != 0xFE) {if (++i > 500) {return 0xFF;}} // 等待起始令牌0XFE
	for (i=0; i<16; i++) {csdValue[i] = SPI2_WriteReadData(0xFF);} // 读取数据

	SPI2_WriteReadData(0xFF), SPI2_WriteReadData(0xFF); // 读取两位CRC效验
	
	TF_CS_SET(); // 取消片选
	SPI2_WriteReadData(0xFF);

	if ((csdValue[0] & 0xC0) == 0x40) // SD V2.0的卡CSD第一个数据是0x40
	{
		*capacity = csdValue[9] + ((uint16_t)csdValue[8] << 8) + 1; // 计算C_SIZE，在CSD[69:48]
		*capacity = (*capacity) << 10; //// 实际上就是乘以1024 得到扇区数
	}
	else
	{
		// 内存算法是 capacity = BLOCKNR * BLOCK_LEN
		// BLOCKNR = (C_SIZE + 1) * MULT
		// BLOCK_LEN = (READ_BL_LEN < 12) 或2^(READ_BL_LEN)
		// 计算BLOCK_LEN,C_SIZE_MULT在CSD[49:47];READ_BL_LEN在CSD[83:80]
		n = (csdValue[5] & 0x0A) + ((csdValue[10] & 0x80) >> 7) + ((csdValue[9] & 0x03) << 1) + 2;
		// 计算C_SIZE，C_SIZE在CSD[73:62]
		*capacity = (csdValue[8] >> 6) + ((uint16_t)csdValue[7] << 2) + ((uint16_t)(csdValue[6] & 3) << 10) + 1;
		*capacity = (*capacity) << (n - 9); //得到扇区数
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_read_data
* Description    : 读取512个数据数据.
* Input          : *buf：数据缓存区
* Output         : 读取到的数值
* Return         : 0xFF：初始化失败
*		         * 0：初始化成功
*/

static int8_t tf_read_data(uint8_t *buf)
{
	uint16_t i;

	i = 0;
	while (SPI2_WriteReadData(0xFF) != 0xFE) {if(++i > 0x0FFF) {return 0xFF;}} // 等待起始令牌0xFE
	for (i=0; i<512; i++) {*buf++ = SPI2_WriteReadData(0xFF);} // 接收数据
	// 读完数据再读两位CRC效验，但是我们可以不需要它们
	SPI2_WriteReadData(0xFF), SPI2_WriteReadData(0xFF);
	return 0;
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_read_disk
* Description    : 读取扇区的数据.
* Input          : *buf：数据缓存区
*                * sector：读取扇区位置
*                * num：读取的扇区数
* Output         : 读取到的扇区数值
* Return         : 0xFF：初始化失败
*		         * 0：初始化成功
*/

int8_t tf_read_disk(uint8_t *buf, uint32_t sector, uint8_t num)
{
	uint16_t i;

	TF_CS_CLR();
	if (SD_TYPE != SD_TYPE_V2HC) {sector <<= 9;} //转换位字节地址
	//SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_2);
	if (num == 1)
	{
		i = 0;
		while (tf_write_cmd(SD_CMD17, sector, 0x01) != 0) // 发送读取命令
		{
			if(++i > 100) {return 0xFF;} //命令无反应，表明发送命令失败
		}
		if (tf_read_data(buf) != 0) {return 0xFF;} // 接收数据 
	}
	else
	{
		i = 0;
		while (tf_write_cmd(SD_CMD18, sector, 0x01) != 0) // 发送连续读取命令
		{
			if (++i > 100) {return 0xFF;} //命令无反应，表明发送命令失败
		}
		
		// 接收数据
		while (num--)
		{
			if (tf_read_data(buf) != 0) {return 0xFF;} // 接收数据 
			buf += 512;
		}
		tf_write_cmd(SD_CMD12, 0, 0x01); //发送停止信号
	}
	
	// 取消片选
	TF_CS_SET();
	SPI2_WriteReadData(0xFF);
	return 0;
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_write_data
* Description    : 写512个数据数据.
* Input          : *buf：数据缓存区
*                * cmd：命令
* Output         : None
* Return         : 0xFF：初始化失败
*		         * 0：初始化成功
*/
static int8_t tf_write_data(uint8_t *buf, uint8_t cmd)
{
	uint16_t i;

	i = 0;
	while (SPI2_WriteReadData(0xFF) != 0xFF) // 发送若干个时钟，等待SD卡准备好
	{
		if(++i > 0x0FFF) {return 0xFF;}
	}
	SPI2_WriteReadData(cmd); //发送命令
	for (i = 0; i<512; i++) {SPI2_WriteReadData(*buf++);} //开始写入数据

	SPI2_WriteReadData(0xFF), SPI2_WriteReadData(0xFF); // 发送两位CRC效验码，随便发
	i = SPI2_WriteReadData(0xFF); //读取返回值
	if ((i & 0x1F) != 0x05) //判断是否写成功
	{
	 	TF_CS_SET();
		SPI2_WriteReadData(0xFF);
		return 0xFF;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_write_disk
* Description    : 写多个扇区.
* Input          : *buf：数据缓存区
*                * sector：要写的扇区数值
*                * 要写的扇区数
* Output         : None
* Return         : 0xFF：初始化失败
*		         * 0：初始化成功
*/
int8_t tf_write_disk(uint8_t *buf, uint32_t sector, uint8_t num)
{
	uint16_t i;

	TF_CS_CLR();
	if (SD_TYPE != SD_TYPE_V2HC) {sector <<= 9;} //转换位字节地址
	//SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_2);
	// 只写一个扇区
	if (num == 1)
	{
		i = 0;
		while (tf_write_cmd(SD_CMD24, sector, 0x01) != 0) // 发送写命令
		{
			if (++i > 100) {return 0xFF;} //命令无反应，表明发送命令失败
		}
		if (tf_write_data(buf, 0xFE) != 0) {return 0xFF;}
	}
	else //写多个扇区
	{
		if (SD_TYPE == SD_TYPE_MMC) //如果是MMC卡
		{
			tf_write_cmd(SD_CMD55, 0, 0X01), tf_write_cmd(SD_CMD23, num, 0X01); //写入前先擦除num个扇区里面的数据 
		}
		// 发送连续写命令
		i = 0;
		while (tf_write_cmd(SD_CMD25, sector, 0x01) != 0)
		{
			if (++i > 100) {return 0xFF;} //命令无反应，表明发送命令失败
		}
		while (num--) // 开始写数据
		{
			if (tf_write_data(buf, 0xFC) != 0) {return 0xFF;}
			buf += 512;	
		}

		i = 0;
		while (SPI2_WriteReadData(0xFF) != 0xFF) // 发送若干个时钟，等待SD卡准备好
		{
			if(++i > 0x0FFF) {return 0xFF;}	
		}
		SPI2_WriteReadData(0xFD); // 发送结束命令
	}
	TF_CS_SET();
	SPI2_WriteReadData(0xFF);

	return 0;
}
/*----------------------------------------------------------------------------*/
