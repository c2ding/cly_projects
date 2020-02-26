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
#ifndef __STMFLASH_H__
#define __STMFLASH_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* 宏定义 --------------------------------------------------------------------*/
/* 类型定义 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
extern void HAL_FLASH_Write(uint32_t WriteAddr, uint16_t * pBuffer, uint32_t NumToWrite ); //从指定地址开始写入指定长度的数据
extern void HAL_FLASH_Read(uint32_t ReadAddr, uint16_t * pBuffer, uint32_t NumToRead ); //从指定地址开始读出指定长度的数据
/*----------------------------------------------------------------------------*/
#endif
