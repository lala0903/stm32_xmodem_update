#include "timer.h"
#include <stdio.h>
#include "usrlib.h"

#define TASK_MAX 20

typedef struct {
	uint32_t time; /*TIMER周期的倍数。timer周期默认一般是1ms */
	void (*fun)(void);
}TIME_HANDLER_S;

TIME_HANDLER_S g_handle[TASK_MAX];
static int32_t g_cnt = 0;

void UsrTimerInit(void)
{
    memset_s(g_handle, 0, sizeof(g_handle));
}

/*
 * cb 注册定时执行的函数指针
 * time 每隔time时间片执行一次
 * return 成功0，失败 -1
*/
int32_t TimerFuncRegister(void (*cb)(void), uint32_t time)
{
	if ((g_cnt >= TASK_MAX) || (cb == NULL) || (time == 0)) {
		return TIMER_RET_ERROR;
	}

	g_handle[g_cnt].fun = cb;
	g_handle[g_cnt].time = time;
	g_cnt++;

	return TIMER_RET_SUCCESS;
}
/*
 * cb 注销定时执行的函数指针
 * return 成功0，失败 -1
*/
int32_t TimerFuncUnregister(void (*cb)(void))
{
    int32_t i;
    
    if ((cb == NULL)|| (g_cnt == 0)) {
        return TIMER_RET_ERROR;
    }
    
    for (i = 0; i < g_cnt; i++) {
        if (cb == g_handle[i].fun) {
            break;
        }   
    }
    if (i == g_cnt) {
    	return TIMER_RET_ERROR;
    } 
    for (; i < g_cnt - 1; i++) {
        g_handle[i].fun = g_handle[i + 1].fun;
	    g_handle[i].time = g_handle[i + 1].time;    
    }
	g_handle[i].fun = NULL;
	g_handle[i].time = 0;
    g_cnt--;

	return TIMER_RET_SUCCESS;
}

void TimerFunction(void)
{
    uint8_t i;
	static uint32_t cnt = 0;
	cnt++;
	for (i = 0; i < g_cnt ; i++) {
		if ((cnt % g_handle[i].time) == 0) {
			g_handle[i].fun();
		}
	}
}
