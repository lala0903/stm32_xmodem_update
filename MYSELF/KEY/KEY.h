#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"


#define KEY0 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)
#define KEY1 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)
#define KEY2 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)
#define KEY3 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)


#define KEY0_PRES 1
#define KEY1_PRES 2
#define KEY2_PRES 3
#define WK_UP_PRES 4

void KEY_Init(void);
u8 KEY_Scan(u8 mode);


#endif
