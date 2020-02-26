/*
#include "stm32f1xx_tf_spi.h" //TF ����
	hspi2.Init.Mode = SPI_MODE_MASTER; //����ģʽ
	hspi2.Init.Direction = SPI_DIRECTION_2LINES_RXONLY; //ѡ��ȫ˫��SPIģʽ
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT; //8λSPI
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW; //ʱ�����ոߵ�ƽ
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE; //�ڵڶ���ʱ�Ӳɼ�����
	hspi2.Init.NSS = SPI_NSS_SOFT; //Nssʹ���������
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB; //�����λ��ʼ����
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 7;

����:������
*/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_tf_spi.h"
/* ˽�к궨�� ----------------------------------------------------------------*/
//Ƭѡ
#define TF_CS_PORT	GPIOD
#define TF_CS_PIN	GPIO_PIN_8
#define TF_CS_SET()	HAL_GPIO_WritePin(TF_CS_PORT, TF_CS_PIN, GPIO_PIN_SET)
#define TF_CS_CLR()	HAL_GPIO_WritePin(TF_CS_PORT, TF_CS_PIN, GPIO_PIN_RESET)

//ʹ�õ�SPI�˿�
#if defined(__STM32F1XX_SPI1_H__)
#define TF_SPI SPI1
#endif
#if defined(__STM32F1XX_SPI2_H__)
#define TF_SPI SPI2
#endif
#if defined(__STM32F1XX_SPI3_H__)
#define TF_SPI SPI3
#endif

/* ���忨������ */
#define SD_TYPE_ERR     ((uint8_t)0X00)
#define SD_TYPE_MMC     ((uint8_t)0X01)
#define SD_TYPE_V1      ((uint8_t)0X02)
#define SD_TYPE_V2      ((uint8_t)0X04)
#define SD_TYPE_V2HC    ((uint8_t)0X06)
/* SD��ָ��� */  	   
#define SD_CMD0    0       //����λ
#define SD_CMD1    1
#define SD_CMD8    8       //����8 ��SEND_IF_COND
#define SD_CMD9    9       //����9 ����CSD����
#define SD_CMD10   10      //����10����CID����
#define SD_CMD12   12      //����12��ֹͣ���ݴ���
#define SD_CMD16   16      //����16������SectorSize Ӧ����0x00
#define SD_CMD17   17      //����17����sector
#define SD_CMD18   18      //����18����Multi sector
#define SD_CMD23   23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define SD_CMD24   24      //����24��дsector
#define SD_CMD25   25      //����25��дMulti sector
#define SD_CMD41   41      //����41��Ӧ����0x00
#define SD_CMD55   55      //����55��Ӧ����0x01
#define SD_CMD58   58      //����58����OCR��Ϣ
#define SD_CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
uint8_t SD_TYPE; //����SD��������
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

//����SPI2�ķ�Ƶ�����Ըı�SPI2���ٶ�.
void SPI2_SetSpeed(uint8_t Speed)
{
	TF_SPI->CR1 &= 0xFFC7;
	TF_SPI->CR1 |= Speed;
	TF_SPI->CR1 |= ((uint16_t)0x0040);
}
/*----------------------------------------------------------------------------*/
//д��һ���ֽ�����ͬʱ��ȡһ���ֽ�����
uint8_t SPI2_WriteReadData(uint8_t dat)
{
	uint16_t retry = 0;
	while ((TF_SPI->SR&1<<1)==0) {if (++retry>=0XFFFE) {return 0;}} //�ȴ��������� ��ʱ�˳�
	TF_SPI->DR = dat; //����һ��byte
	retry = 0;
	while ((TF_SPI->SR&1<<0)==0) {if (++retry>=0XFFFE) {return 0;}} //�ȴ�������һ��byte ��ʱ�˳�
	return TF_SPI->DR; //�����յ�������
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_write_cmd
* Description    : ��SD������һ������.SD��һ�������ʽ����8λ���
*                * 32λ���������7λCRCЧ��λ�����һλΪ1
* Input          : cmd������
*                * dat: �������
*                * crc: CRCЧ��
* Output         : None
* Return         : r1��SD�����ص�����
*                * 0xFF����ʾ����ʧ��
*/
static uint8_t tf_write_cmd(uint8_t cmd, uint32_t dat, uint8_t crc)
{	 
	uint8_t r1 = 0;
	uint16_t i = 0;

	TF_CS_SET(); // ��λSD��,ȡ���ϴ�Ƭѡ
	SPI2_WriteReadData(0xFF); //�����ṩ8��ʱ��

	TF_CS_CLR();
	while (SPI2_WriteReadData(0xFF) != 0xFF) //�ȴ����Ƿ�׼����
	{
		if(++i > 100) {return 0xFF;} //�ȴ�ʧ�ܷ���
	}
	SPI2_WriteReadData(cmd|0x40); // ��������
	SPI2_WriteReadData((uint8_t)(dat>>24)); //����Dat�����8λ
	SPI2_WriteReadData((uint8_t)(dat>>16));
	SPI2_WriteReadData((uint8_t)(dat>>8));
	SPI2_WriteReadData((uint8_t)(dat>>0));
	SPI2_WriteReadData(crc);
	if (cmd == SD_CMD12) {SPI2_WriteReadData(0xFF);} //�����ֹͣ���ݴ����������෢һ��ʱ��
	i = 0;
	do {
		r1 = SPI2_WriteReadData(0xFF);
		if(++i > 100) {return 0xFF;}
	} while((r1 & 0x80) != 0); //���ͳɹ������λ��0 

	return r1;
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_init
* Description    : ��ʼ��SD��.
* Input          : None
* Output         : None
* Return         : 0xFF����ʼ��ʧ��
*		         * 0����ʼ���ɹ�
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
	HAL_GPIO_LockPin(TF_CS_PORT, TF_CS_PIN); //��������

	SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_256);
	// ��SD��ͨ��ģʽת��ΪSPIģʽ���ϵ�Ĭ������DAT0��������
	// �ӵ�CMD0ʱ��CS�ź���Ч��SPIģʽ����
	for (i=0; i<0x0F; i++) {SPI2_WriteReadData(0xFF);}//��ʼʱ���ȷ�������74��ʱ�ӣ�����Ǳ����

	i = 0;
	while (tf_write_cmd(SD_CMD0, 0, 0x95) != 0x01) // ����ȡ��0x01��ʱ���ʾ��ʼ���ɹ�
	{
		if (++i > 100) {return 0xFF;} //��ʼ��ʧ�ܷ���0
	}

	i = 0;
	do {
		if (++i > 100) {break;} //���Ƿ��ͳ�����������ѭ����
		r1 = tf_write_cmd(SD_CMD8, 0x01AA, 0x87); // ����CMD8������Ƿ�SD V2.0
	} while(r1 != 0x01); //����CMD8
	
	if (r1 == 0x01) //���CMD8�л�Ӧ˵����SD V2.0Э��
	{
		for (i=0; i<4; i++) {buf[i] = SPI2_WriteReadData(0xFF);} // ��ȡCMD8�ķ���ֵ������Ƿ�֧�ֵ�ѹ
		if ((buf[2] != 0x01) || (buf[3] != 0xAA)) {return 0xFF;} // ����ѹ��֧�ֵ�ѹ�����ش���
		
		// ��ʼ��SD��
		i = 0;
		do {
			if (++i > 100) {return 0xFF;} //����ʧ��
			tf_write_cmd(SD_CMD55, 0, 0x01);
			r1 = tf_write_cmd(SD_CMD41, 0x40000000, 0x01);
		} while(r1 != 0); //д��CMD41�ɹ��ˣ�����ʡ�Զ�ȡCMD41�Ƿ����óɹ���

		// �����SDHC������SD��
		i = 0;
		while (tf_write_cmd(SD_CMD58, 0, 0x01) != 0) {if (++i > 100) {SD_TYPE = SD_TYPE_ERR; break;}}
		for (i=0; i<4; i++) {buf[i] = SPI2_WriteReadData(0xFF);} // ��ȡOCR
		if (buf[0] & 0x40) {SD_TYPE = SD_TYPE_V2HC;} else {SD_TYPE = SD_TYPE_V2;}
	}
	else //�������SD V1.0����MMC V3
	{
		tf_write_cmd(SD_CMD55, 0x00, 0x01);
		r1 = tf_write_cmd(SD_CMD41, 0x00, 0x01);
		if (r1 <= 1) //��CMD41�л�Ӧ˵����SD V1.0
		{
			SD_TYPE = SD_TYPE_V1; //��V1.0��
			i = 0;
			do {
				if(i > 100) {return 0xFF;}
				tf_write_cmd(SD_CMD55, 0x00, 0x01);
				r1 = tf_write_cmd(SD_CMD41, 0x00, 0x01);
			} while(r1 != 0);
		}
		else //û�л�Ӧ˵����MMC V3
		{
			SD_TYPE = SD_TYPE_MMC; //��������MMC��
			i = 0;
			while (tf_write_cmd(SD_CMD1, 0, 0x01) != 0) {if (++i > 100) {return 0xFF;}}
		}
	}
	TF_CS_SET(); //ȡ��Ƭѡ
	SPI2_WriteReadData(0xFF);
	//SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_128);
	return 0;
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_read_capacity
* Description    : ��ȡSD�����ڴ��С.
* Input          : capacity����ȡ������ֵ����ָ��
* Output         : �ڴ��С����λΪKB
* Return         : 0xFF����ʼ��ʧ��
*		         * 0����ʼ���ɹ�
*/
int8_t tf_read_capacity(uint32_t *capacity)
{
	uint8_t csdValue[16];
	uint16_t n, i = 0;
	
	TF_CS_CLR();
	while (tf_write_cmd(SD_CMD9, 0, 0x01) != 0) // ��������
	{
		if (++i > 100) {return 0xFF;} //��������ʧ��
	}

	i = 0;
	while (SPI2_WriteReadData(0xFF) != 0xFE) {if (++i > 500) {return 0xFF;}} // �ȴ���ʼ����0XFE
	for (i=0; i<16; i++) {csdValue[i] = SPI2_WriteReadData(0xFF);} // ��ȡ����

	SPI2_WriteReadData(0xFF), SPI2_WriteReadData(0xFF); // ��ȡ��λCRCЧ��
	
	TF_CS_SET(); // ȡ��Ƭѡ
	SPI2_WriteReadData(0xFF);

	if ((csdValue[0] & 0xC0) == 0x40) // SD V2.0�Ŀ�CSD��һ��������0x40
	{
		*capacity = csdValue[9] + ((uint16_t)csdValue[8] << 8) + 1; // ����C_SIZE����CSD[69:48]
		*capacity = (*capacity) << 10; //// ʵ���Ͼ��ǳ���1024 �õ�������
	}
	else
	{
		// �ڴ��㷨�� capacity = BLOCKNR * BLOCK_LEN
		// BLOCKNR = (C_SIZE + 1) * MULT
		// BLOCK_LEN = (READ_BL_LEN < 12) ��2^(READ_BL_LEN)
		// ����BLOCK_LEN,C_SIZE_MULT��CSD[49:47];READ_BL_LEN��CSD[83:80]
		n = (csdValue[5] & 0x0A) + ((csdValue[10] & 0x80) >> 7) + ((csdValue[9] & 0x03) << 1) + 2;
		// ����C_SIZE��C_SIZE��CSD[73:62]
		*capacity = (csdValue[8] >> 6) + ((uint16_t)csdValue[7] << 2) + ((uint16_t)(csdValue[6] & 3) << 10) + 1;
		*capacity = (*capacity) << (n - 9); //�õ�������
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_read_data
* Description    : ��ȡ512����������.
* Input          : *buf�����ݻ�����
* Output         : ��ȡ������ֵ
* Return         : 0xFF����ʼ��ʧ��
*		         * 0����ʼ���ɹ�
*/

static int8_t tf_read_data(uint8_t *buf)
{
	uint16_t i;

	i = 0;
	while (SPI2_WriteReadData(0xFF) != 0xFE) {if(++i > 0x0FFF) {return 0xFF;}} // �ȴ���ʼ����0xFE
	for (i=0; i<512; i++) {*buf++ = SPI2_WriteReadData(0xFF);} // ��������
	// ���������ٶ���λCRCЧ�飬�������ǿ��Բ���Ҫ����
	SPI2_WriteReadData(0xFF), SPI2_WriteReadData(0xFF);
	return 0;
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_read_disk
* Description    : ��ȡ����������.
* Input          : *buf�����ݻ�����
*                * sector����ȡ����λ��
*                * num����ȡ��������
* Output         : ��ȡ����������ֵ
* Return         : 0xFF����ʼ��ʧ��
*		         * 0����ʼ���ɹ�
*/

int8_t tf_read_disk(uint8_t *buf, uint32_t sector, uint8_t num)
{
	uint16_t i;

	TF_CS_CLR();
	if (SD_TYPE != SD_TYPE_V2HC) {sector <<= 9;} //ת��λ�ֽڵ�ַ
	//SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_2);
	if (num == 1)
	{
		i = 0;
		while (tf_write_cmd(SD_CMD17, sector, 0x01) != 0) // ���Ͷ�ȡ����
		{
			if(++i > 100) {return 0xFF;} //�����޷�Ӧ��������������ʧ��
		}
		if (tf_read_data(buf) != 0) {return 0xFF;} // �������� 
	}
	else
	{
		i = 0;
		while (tf_write_cmd(SD_CMD18, sector, 0x01) != 0) // ����������ȡ����
		{
			if (++i > 100) {return 0xFF;} //�����޷�Ӧ��������������ʧ��
		}
		
		// ��������
		while (num--)
		{
			if (tf_read_data(buf) != 0) {return 0xFF;} // �������� 
			buf += 512;
		}
		tf_write_cmd(SD_CMD12, 0, 0x01); //����ֹͣ�ź�
	}
	
	// ȡ��Ƭѡ
	TF_CS_SET();
	SPI2_WriteReadData(0xFF);
	return 0;
}
/*----------------------------------------------------------------------------*/
/*
* Function Name  : tf_write_data
* Description    : д512����������.
* Input          : *buf�����ݻ�����
*                * cmd������
* Output         : None
* Return         : 0xFF����ʼ��ʧ��
*		         * 0����ʼ���ɹ�
*/
static int8_t tf_write_data(uint8_t *buf, uint8_t cmd)
{
	uint16_t i;

	i = 0;
	while (SPI2_WriteReadData(0xFF) != 0xFF) // �������ɸ�ʱ�ӣ��ȴ�SD��׼����
	{
		if(++i > 0x0FFF) {return 0xFF;}
	}
	SPI2_WriteReadData(cmd); //��������
	for (i = 0; i<512; i++) {SPI2_WriteReadData(*buf++);} //��ʼд������

	SPI2_WriteReadData(0xFF), SPI2_WriteReadData(0xFF); // ������λCRCЧ���룬��㷢
	i = SPI2_WriteReadData(0xFF); //��ȡ����ֵ
	if ((i & 0x1F) != 0x05) //�ж��Ƿ�д�ɹ�
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
* Description    : д�������.
* Input          : *buf�����ݻ�����
*                * sector��Ҫд��������ֵ
*                * Ҫд��������
* Output         : None
* Return         : 0xFF����ʼ��ʧ��
*		         * 0����ʼ���ɹ�
*/
int8_t tf_write_disk(uint8_t *buf, uint32_t sector, uint8_t num)
{
	uint16_t i;

	TF_CS_CLR();
	if (SD_TYPE != SD_TYPE_V2HC) {sector <<= 9;} //ת��λ�ֽڵ�ַ
	//SPI2_SetSpeed(SPI_BAUDRATEPRESCALER_2);
	// ֻдһ������
	if (num == 1)
	{
		i = 0;
		while (tf_write_cmd(SD_CMD24, sector, 0x01) != 0) // ����д����
		{
			if (++i > 100) {return 0xFF;} //�����޷�Ӧ��������������ʧ��
		}
		if (tf_write_data(buf, 0xFE) != 0) {return 0xFF;}
	}
	else //д�������
	{
		if (SD_TYPE == SD_TYPE_MMC) //�����MMC��
		{
			tf_write_cmd(SD_CMD55, 0, 0X01), tf_write_cmd(SD_CMD23, num, 0X01); //д��ǰ�Ȳ���num��������������� 
		}
		// ��������д����
		i = 0;
		while (tf_write_cmd(SD_CMD25, sector, 0x01) != 0)
		{
			if (++i > 100) {return 0xFF;} //�����޷�Ӧ��������������ʧ��
		}
		while (num--) // ��ʼд����
		{
			if (tf_write_data(buf, 0xFC) != 0) {return 0xFF;}
			buf += 512;	
		}

		i = 0;
		while (SPI2_WriteReadData(0xFF) != 0xFF) // �������ɸ�ʱ�ӣ��ȴ�SD��׼����
		{
			if(++i > 0x0FFF) {return 0xFF;}	
		}
		SPI2_WriteReadData(0xFD); // ���ͽ�������
	}
	TF_CS_SET();
	SPI2_WriteReadData(0xFF);

	return 0;
}
/*----------------------------------------------------------------------------*/
