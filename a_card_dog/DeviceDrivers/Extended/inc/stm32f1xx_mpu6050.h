

#ifndef __STM32F1XX_MPU6050_H__
#define __STM32F1XX_MPU6050_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
extern void MPU6050_Init(void); //初始化MPU6050
extern bool MPU6050ReadID(void); //读取MPU6050的ID
extern void MPU6050ReadGyro(short *gyroData); //读取MPU6050的加速度
extern void MPU6050ReadAcc(short *accData); //读取MPU6050的陀螺仪
extern void MPU6050_ReturnTemp(short*Temperature); //读取MPU6050的温度
#endif  //__MPU6050_H__
