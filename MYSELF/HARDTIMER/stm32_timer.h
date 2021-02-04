#ifndef __STM32_TIMER_H__
#define __STM32_TIMER_H__
#include <stdint.h>

void TIM6Init(uint16_t arr, uint16_t psc, void (*cb)(void));

#endif
