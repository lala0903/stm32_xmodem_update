#ifndef __FLASH_H__
#define __FLASH_H__

#include <stdint.h>

#define FLASH_RET_ERROR (-1)
#define FLASH_RET_SUCCESS 0
// FLASH 分区 配置
#define FIRMWARE_ONE 0
#define FIRMWARE_TWO 1

#define FLASH_BASE_ADDR  ((uint32_t)0x08000000)  
#define NLEDBOOTLOADER_SIZE  ((uint32_t)100*1024)   // Bootloader 大小为 64KB
#define FIRMWARE_ONE_SIZE ((uint32_t)200*1024)   // 固件1  大小为 200KB
#define FIRMWARE_TWO_SIZE ((uint32_t)200*1024)   // 固件2  大小为 200KB
#define BOOTLOAD_MSG_SIZE ((uint32_t)2*1024)
#define NLED_CONFIG_PARAM_SIZE ((uint32_t)10*1024)

#define BOOTLOAD_OFFSET ((uint32_t)0)
#define FIRMWARE_ONE_OFFSET (BOOTLOAD_OFFSET + NLEDBOOTLOADER_SIZE) // 固件1偏移量
#define FIRMWARE_TWO_OFFSET (FIRMWARE_ONE_OFFSET + FIRMWARE_ONE_SIZE)  // 固件2偏移量
#define BOOTLOAD_MSG_OFFSET (FIRMWARE_TWO_OFFSET + FIRMWARE_TWO_SIZE)
#define NLED_CONFIG_PARAM_OFFSET (BOOTLOAD_MSG_OFFSET + BOOTLOAD_MSG_SIZE) 

#define BOOTLOADER_START_ADDR (FLASH_BASE_ADDR + BOOTLOAD_OFFSET)   //Bootloader 启动地址
#define FIRMWARE_ONE_START_ADDR (FLASH_BASE_ADDR + FIRMWARE_ONE_OFFSET) // 固件 1  启动地址
#define FIRMWARE_TWO_START_ADDR (FLASH_BASE_ADDR + FIRMWARE_TWO_OFFSET)  // 固件 2 启动地址
#define BOOTLOAD_MSG_START_ADDR (FLASH_BASE_ADDR + BOOTLOAD_MSG_OFFSET) 
#define NLED_CONFIG_PARAM_START_ADDR (FLASH_BASE_ADDR + NLED_CONFIG_PARAM_OFFSET)

typedef void (* FunVoidType) (void);

int32_t FlashJumpToApp(volatile uint32_t applicationAddress);
int32_t FlashRead(uint32_t address, void *buf, uint32_t len);
int32_t FlashWrite(uint32_t address, const void *buf, uint32_t len);

#endif // !__FLASH_H__
