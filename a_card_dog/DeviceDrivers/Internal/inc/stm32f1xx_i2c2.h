/**

����:������
  */
#ifndef __STM32F1XX_I2C2_H__
#define	__STM32F1XX_I2C2_H__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define I2C_OWN_ADDRESS		0x3E // stm32����I2C��ַ
#define I2C_SPEEDCLOCK		400000 // I2Cͨ������(���Ϊ400K)
#define I2C_DUTYCYCLE		I2C_DUTYCYCLE_2 // I2Cռ�ձ�ģʽ��1/2 
/* 
 * EEPROM 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */
/* EEPROM Addresses defines */ 
#define I2C_ADDRESS 0x30F
/* ��չ���� ------------------------------------------------------------------*/
extern I2C_HandleTypeDef i2c2;
/* �������� ------------------------------------------------------------------*/
extern void MX_I2C2_Init(void);

#endif /* __STM32F1XX_I2C_H__ */

