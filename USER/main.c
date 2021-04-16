#include "stm32f10x.h"
#include "stm32_timer.h"
#include "delay.h"
#include "LED.h"
#include "KEY.h"
#include "BEEP.h"
#include "usart.h"
#include "uart.h"
#include "debug.h"
#include "timer.h"
#include "device.h"
#include "flash.h"
#include "xmodem.h"

void TaskInit(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	Uart3Init(115200);
	delay_init();
	LED_Init(); 
	KEY_Init();
	BEEP_Init();
	UsrTimerInit();
	TIM6Init(1000, 72, TimerFunction);

	DeviceGetBootloaderMsg();
	DeviceGetFirmwareMsg();
}

/* 
 * 先做简单5秒之内按下按键进行升级
 */
int main(void)
{
	int temp = 0;
	TaskInit();
	printf("\r\n-----------------------------------\r\n");
	printf("------------bootloader-------------\r\n");
	printf("-----------------------------------\r\n");
	while(1) {
		delay_ms(1000);
		temp++;
		if (temp == 5) {
			XmodemUpdate(FIRMWARE_ONE);
		} else if(temp > 5) {
			DeviceStartApplication();
		}
		LOG_INFO("just for bootloader, temp is %d", temp);
	}
}