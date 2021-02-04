#ifndef __BEEP_H
#define __BEEP_H
#include "stm32f10x.h"

#define BEEP PBout(8)

void BEEP_Init(void);


#endif
