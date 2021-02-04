#include "flash.h"

#include <stdio.h>
#include "sys.h"
#include "debug.h"
#include "stmflash.h"
#include "core_cm3.h"



int32_t FlashJumpToApp(volatile uint32_t applicationAddress)
{
	uint32_t temp;
	int32_t i;
	FunVoidType JumpToApplication;
	LOG_INFO("stack is 0x%x", *(volatile uint32_t *)applicationAddress);
	if (((*(volatile uint32_t *)applicationAddress) & 0x2FFE0000 ) == 0x20000000) {
		for(i = 0; i < 8; i++) {			
			NVIC->ICER[i] = 0xFFFFFFFF;	/* 关闭中断*/
			NVIC->ICPR[i] = 0xFFFFFFFF;	/* 清除中断标志位 */
		}
		LOG_INFO("JumpToApp");
		temp = (*(volatile uint32_t *)(applicationAddress + 4));
		JumpToApplication = (FunVoidType)temp;
		MSR_MSP(*(volatile uint32_t *)applicationAddress);
		JumpToApplication();
	} else {
		/* 栈顶地址非法 */
		LOG_ERROR("firmware stack top is error, address is 0x%x", applicationAddress);
	}

	return FLASH_RET_ERROR;
}

int32_t FlashRead(uint32_t address, void *buf, uint32_t len)
{
	uint16_t *temp = NULL;
	if ((buf == NULL) || (address < STM32_FLASH_BASE) ||
		(address >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE))) {
		return FLASH_RET_ERROR;//非法地址
	}
	temp = (uint16_t *)buf;
	STMFLASH_Read(address, temp, len / 2);


	return FLASH_RET_SUCCESS;
}

int32_t FlashWrite(uint32_t address, const void *buf, uint32_t len)	
{
	uint16_t *temp = NULL;
	if ((buf == NULL) || (address < STM32_FLASH_BASE) ||
		(address >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE))) {
		return FLASH_RET_ERROR;//非法地址
	}
	temp = (uint16_t *)buf;
	STMFLASH_Write(address, temp, len / 2);

	return FLASH_RET_SUCCESS;
}
