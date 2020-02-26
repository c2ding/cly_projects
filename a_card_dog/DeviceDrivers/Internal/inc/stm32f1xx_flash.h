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
#ifndef __STMFLASH_H__
#define __STMFLASH_H__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* �궨�� --------------------------------------------------------------------*/
/* ���Ͷ��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
extern void HAL_FLASH_Write(uint32_t WriteAddr, uint16_t * pBuffer, uint32_t NumToWrite ); //��ָ����ַ��ʼд��ָ�����ȵ�����
extern void HAL_FLASH_Read(uint32_t ReadAddr, uint16_t * pBuffer, uint32_t NumToRead ); //��ָ����ַ��ʼ����ָ�����ȵ�����
/*----------------------------------------------------------------------------*/
#endif
