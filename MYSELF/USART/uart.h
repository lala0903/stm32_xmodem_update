#ifndef __USART_H__
#define __USART_H__

#include <stdint.h> 

#define UART_RET_ERROR (-1)
#define UART_RET_SUCCESS 0

//如果想串口中断接收，请不要注释以下宏定义
void Uart3Init(uint32_t bound);
int32_t UartFuncRegister(void (*cb)(uint8_t byte));
int32_t UartFuncUnregister(void (*cb)(uint8_t byte));
#endif


