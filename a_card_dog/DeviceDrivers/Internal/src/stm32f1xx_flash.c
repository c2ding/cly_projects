/*
�ڲ�Falsh��дʵ��
	HAL_FLASH_Write(OFFSER_BUFFER_FLASH_ADDRESS, (uint16_t *)&s_hardware_information, OFFSER_BUFFER_FLASH_LENGTH);
	HAL_FLASH_Read(OFFSER_BUFFER_FLASH_ADDRESS, (uint16_t *)&s_hardware_information, OFFSER_BUFFER_FLASH_LENGTH);

C8  64K 0x0800FFFF
CB 128K 0x0801FFFF
RC 256K 0x0803FFFF
ZE 512K 0x0807FFFF
����:������
*/
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_flash.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/************************** STM32 �ڲ� FLASH ���� *****************************/
#define STM32_FLASH_SIZE 512 // ��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1 // stm32оƬ����FLASH д��ʹ��(0������;1��ʹ��)

#if STM32_FLASH_SIZE < 256
  #define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
  #define STM_SECTOR_SIZE 2048
#endif
/* ˽�б��� ------------------------------------------------------------------*/
#if STM32_FLASH_WREN //���ʹ����д 
static uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2]; //�����2K�ֽ�
static FLASH_EraseInitTypeDef EraseInitStruct;
#endif
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

#if STM32_FLASH_WREN //���ʹ����д
/**
  * ��������: ������д��
  * �������: WriteAddr:��ʼ��ַ
  *           pBuffer:����ָ��
  *           NumToWrite:����(16λ)��
  */
void HAL_FLASH_Write_NoCheck(uint32_t WriteAddr, uint16_t * pBuffer, uint32_t NumToWrite)
{
	uint32_t i;

	for(i=0; i<NumToWrite; i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, WriteAddr, pBuffer[i]);
		WriteAddr += 2; //��ַ����2
	}
} 
/*----------------------------------------------------------------------------*/
/**
  * ��������: ��ָ����ַ��ʼд��ָ�����ȵ�����
  * �������: WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
  *           pBuffer:����ָ��
  *           NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
  */
void HAL_FLASH_Write(uint32_t WriteAddr, uint16_t * pBuffer, uint32_t NumToWrite)
{
	uint32_t SECTORError = 0;
	uint32_t secoff; //������ƫ�Ƶ�ַ(16λ�ּ���)
	uint32_t secremain; //������ʣ���ַ(16λ�ּ���)
	uint32_t i;
	uint32_t secpos; //������ַ
	uint32_t offaddr; //ȥ��0X08000000��ĵ�ַ

	if (WriteAddr<FLASH_BASE||(WriteAddr>=(FLASH_BASE+1024*STM32_FLASH_SIZE))) {return ;} //�Ƿ���ַ

	HAL_FLASH_Unlock(); //����

	offaddr = WriteAddr - FLASH_BASE; //ʵ��ƫ�Ƶ�ַ.
	secpos = offaddr / STM_SECTOR_SIZE; //������ַ 0~127 for STM32F103RBT6
	secoff = (offaddr % STM_SECTOR_SIZE) / 2; //�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain = STM_SECTOR_SIZE / 2 - secoff; //����ʣ��ռ��С   
	if (NumToWrite<=secremain) {secremain = NumToWrite;} //�����ڸ�������Χ

	for (;;)
	{
		HAL_FLASH_Read((secpos*STM_SECTOR_SIZE+FLASH_BASE), STMFLASH_BUF, (STM_SECTOR_SIZE/20)); //������������������
		for(i=0; i<secremain; i++) //У������
		{
			if (STMFLASH_BUF[secoff+i] != 0XFFFF) {break;} //��Ҫ����
		}
		if (i < secremain) //��Ҫ����
		{
			//�����������
			/* Fill EraseInit structure*/
			EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
			EraseInitStruct.PageAddress = secpos*STM_SECTOR_SIZE+FLASH_BASE;
			EraseInitStruct.NbPages = 1;
			HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);

			for (i=0; i<secremain; i++) {STMFLASH_BUF[i+secoff] = pBuffer[i];} //����
			HAL_FLASH_Write_NoCheck((secpos*STM_SECTOR_SIZE+FLASH_BASE), STMFLASH_BUF, (STM_SECTOR_SIZE/2)); //д����������
		}
		else
		{
			HAL_FLASH_Write_NoCheck(WriteAddr, pBuffer, secremain); //д�Ѿ������˵�,ֱ��д������ʣ������
		}
		if (NumToWrite == secremain) {break;}//д�������
		else//д��δ����
		{
			secpos++; //������ַ��1
			secoff = 0; //ƫ��λ��Ϊ0 	 
			pBuffer += secremain; //ָ��ƫ��
			WriteAddr += secremain; //д��ַƫ��
			NumToWrite -= secremain; //�ֽ�(16λ)���ݼ�
			if (NumToWrite>(STM_SECTOR_SIZE/2)) {secremain = (STM_SECTOR_SIZE/2);} //��һ����������д����
			else {secremain = NumToWrite;} //��һ����������д����
		}	 
	}
	HAL_FLASH_Lock(); //����
}
#endif
/*----------------------------------------------------------------------------*/
/**
  * ��������: ��ָ����ַ��ʼ����ָ�����ȵ�����
  * �������: ReadAddr:��ʼ��ַ
  *           pBuffer:����ָ��
  *           NumToRead:����(16λ)��
  */
void HAL_FLASH_Read(uint32_t ReadAddr, uint16_t *pBuffer, uint32_t NumToRead)
{
	uint32_t i;

	for(i=0; i<NumToRead; i++)
	{
		pBuffer[i] = *(__IO uint16_t*)ReadAddr; //��ȡָ����ַ�İ���(16λ����)
		ReadAddr += 2; //ƫ��2���ֽ�
	}
}
