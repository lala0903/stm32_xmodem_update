/*******************************************************************************
 * Copyleft (c) 2021 hx
 * @file    timer.h
 * @brief   建立一个定时任务模块，需要将TimerFunction函数放置在定时器中断中执行，其他模块
 *          只需调用注册和注销函数即可将定时任务传递给模块执行
 * @author  huangxin
 * @version 1.0
 * @date    2021-01-31
 ******************************************************************************/

#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

#define TIMER_RET_ERROR (-1)
#define TIMER_RET_SUCCESS 0

#define TIMER_SET_MS(x) (x)
#define TIMER_SET_S(x) (1000 * (x))

int32_t TimerFuncRegister(void (*cb)(void), uint32_t time);
int32_t TimerFuncUnregister(void (*cb)(void));
void TimerFunction(void);
void UsrTimerInit(void);

#endif // !__TIMER_H__
