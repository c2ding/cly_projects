/*
内部Falsh读写实现
	HAL_FLASH_Write(OFFSER_BUFFER_FLASH_ADDRESS, (uint16_t *)&s_hardware_information, OFFSER_BUFFER_FLASH_LENGTH);
	HAL_FLASH_Read(OFFSER_BUFFER_FLASH_ADDRESS, (uint16_t *)&s_hardware_information, OFFSER_BUFFER_FLASH_LENGTH);

C8  64K 0x0800FFFF
CB 128K 0x0801FFFF
RC 256K 0x0803FFFF
ZE 512K 0x0807FFFF
作者:刘东辉
*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_flash.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/************************** STM32 内部 FLASH 配置 *****************************/
#define STM32_FLASH_SIZE 512 // 所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1 // stm32芯片内容FLASH 写入使能(0，禁用;1，使能)

#if STM32_FLASH_SIZE < 256
  #define STM_SECTOR_SIZE 1024 //字节
#else 
  #define STM_SECTOR_SIZE 2048
#endif
/* 私有变量 ------------------------------------------------------------------*/
#if STM32_FLASH_WREN //如果使能了写 
static uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2]; //最多是2K字节
static FLASH_EraseInitTypeDef EraseInitStruct;
#endif
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

#if STM32_FLASH_WREN //如果使能了写
/**
  * 函数功能: 不检查的写入
  * 输入参数: WriteAddr:起始地址
  *           pBuffer:数据指针
  *           NumToWrite:半字(16位)数
  */
void HAL_FLASH_Write_NoCheck(uint32_t WriteAddr, uint16_t * pBuffer, uint32_t NumToWrite)
{
	uint32_t i;

	for(i=0; i<NumToWrite; i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, WriteAddr, pBuffer[i]);
		WriteAddr += 2; //地址增加2
	}
} 
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 从指定地址开始写入指定长度的数据
  * 输入参数: WriteAddr:起始地址(此地址必须为2的倍数!!)
  *           pBuffer:数据指针
  *           NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
  */
void HAL_FLASH_Write(uint32_t WriteAddr, uint16_t * pBuffer, uint32_t NumToWrite)
{
	uint32_t SECTORError = 0;
	uint32_t secoff; //扇区内偏移地址(16位字计算)
	uint32_t secremain; //扇区内剩余地址(16位字计算)
	uint32_t i;
	uint32_t secpos; //扇区地址
	uint32_t offaddr; //去掉0X08000000后的地址

	if (WriteAddr<FLASH_BASE||(WriteAddr>=(FLASH_BASE+1024*STM32_FLASH_SIZE))) {return ;} //非法地址

	HAL_FLASH_Unlock(); //解锁

	offaddr = WriteAddr - FLASH_BASE; //实际偏移地址.
	secpos = offaddr / STM_SECTOR_SIZE; //扇区地址 0~127 for STM32F103RBT6
	secoff = (offaddr % STM_SECTOR_SIZE) / 2; //在扇区内的偏移(2个字节为基本单位.)
	secremain = STM_SECTOR_SIZE / 2 - secoff; //扇区剩余空间大小   
	if (NumToWrite<=secremain) {secremain = NumToWrite;} //不大于该扇区范围

	for (;;)
	{
		HAL_FLASH_Read((secpos*STM_SECTOR_SIZE+FLASH_BASE), STMFLASH_BUF, (STM_SECTOR_SIZE/20)); //读出整个扇区的内容
		for(i=0; i<secremain; i++) //校验数据
		{
			if (STMFLASH_BUF[secoff+i] != 0XFFFF) {break;} //需要擦除
		}
		if (i < secremain) //需要擦除
		{
			//擦除这个扇区
			/* Fill EraseInit structure*/
			EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
			EraseInitStruct.PageAddress = secpos*STM_SECTOR_SIZE+FLASH_BASE;
			EraseInitStruct.NbPages = 1;
			HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);

			for (i=0; i<secremain; i++) {STMFLASH_BUF[i+secoff] = pBuffer[i];} //复制
			HAL_FLASH_Write_NoCheck((secpos*STM_SECTOR_SIZE+FLASH_BASE), STMFLASH_BUF, (STM_SECTOR_SIZE/2)); //写入整个扇区
		}
		else
		{
			HAL_FLASH_Write_NoCheck(WriteAddr, pBuffer, secremain); //写已经擦除了的,直接写入扇区剩余区间
		}
		if (NumToWrite == secremain) {break;}//写入结束了
		else//写入未结束
		{
			secpos++; //扇区地址增1
			secoff = 0; //偏移位置为0 	 
			pBuffer += secremain; //指针偏移
			WriteAddr += secremain; //写地址偏移
			NumToWrite -= secremain; //字节(16位)数递减
			if (NumToWrite>(STM_SECTOR_SIZE/2)) {secremain = (STM_SECTOR_SIZE/2);} //下一个扇区还是写不完
			else {secremain = NumToWrite;} //下一个扇区可以写完了
		}	 
	}
	HAL_FLASH_Lock(); //上锁
}
#endif
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 从指定地址开始读出指定长度的数据
  * 输入参数: ReadAddr:起始地址
  *           pBuffer:数据指针
  *           NumToRead:半字(16位)数
  */
void HAL_FLASH_Read(uint32_t ReadAddr, uint16_t *pBuffer, uint32_t NumToRead)
{
	uint32_t i;

	for(i=0; i<NumToRead; i++)
	{
		pBuffer[i] = *(__IO uint16_t*)ReadAddr; //读取指定地址的半字(16位数据)
		ReadAddr += 2; //偏移2个字节
	}
}
