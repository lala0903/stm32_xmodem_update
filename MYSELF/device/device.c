#include "device.h"

#include <stdio.h>
#include "flash.h"
#include "debug.h"

const int8_t *g_bootloaderMsessage[5] __attribute__((at(BOOTLOAD_MSG_START_ADDR))) = {
	"board stm32f103\r\n",
	"soft bootloader\r\n",
	"version is 1.0\r\n",
	"author huangxin\r\n",
	"date 2021/01/29\r\n",
};

struct StaticDeviceMsg g_device[2];

void DeviceGetBootloaderMsg(void)
{
    int32_t i;
    for (i = 0; i < 5; i++) {
        (void)PrintString(g_bootloaderMsessage[i], sizeof(g_bootloaderMsessage[i]));
    }
}

void DeviceGetFirmwareMsg(void)
{
    FlashRead(NLED_CONFIG_PARAM_START_ADDR, g_device, sizeof(g_device));
}

void DeviceSetFirmwareBootFlag(int8_t firmware, int8_t bootFlag)
{
    if ((firmware != FIRMWARE_ONE) && (firmware != FIRMWARE_TWO) && 
        (bootFlag != DEVICE_IDLE) && (bootFlag != DEVICE_WORK) && (bootFlag != DEVICE_ERROR)) {
        return;
    }
    
    g_device[firmware].bootFlag = bootFlag;
    if (bootFlag == DEVICE_ERROR) {
        g_device[firmware].runCnt = 0;
    }
    
    (void)FlashWrite(NLED_CONFIG_PARAM_START_ADDR, g_device, sizeof(g_device));
}

static int8_t DeviceGetFirmwareCanWork(void)
{
    if (g_device[FIRMWARE_ONE].bootFlag == DEVICE_IDLE) {
        g_device[FIRMWARE_ONE].bootFlag = DEVICE_WORK;
        g_device[FIRMWARE_ONE].runCnt++;
        (void)FlashWrite(NLED_CONFIG_PARAM_START_ADDR, g_device, sizeof(g_device));
        return FIRMWARE_ONE;
    } else if (g_device[FIRMWARE_TWO].bootFlag == DEVICE_IDLE) {
        g_device[FIRMWARE_TWO].bootFlag = DEVICE_WORK;
        g_device[FIRMWARE_TWO].runCnt++;
        (void)FlashWrite(NLED_CONFIG_PARAM_START_ADDR, g_device, sizeof(g_device));
        return FIRMWARE_TWO;   
    } else {
        return DEVICE_RET_ERROR;
    }
}


void DeviceStartApplication(void)
{
    int32_t ret;
    int8_t firmNum;
    (void)FlashRead(NLED_CONFIG_PARAM_START_ADDR, g_device, sizeof(g_device));

START:
    if ((g_device[FIRMWARE_ONE].bootFlag == DEVICE_ERROR) && (g_device[FIRMWARE_ONE].bootFlag == DEVICE_ERROR)) {
        LOG_ERROR("the two firmware can not start please update");
        while (1) {

        }
                
    }

    if (g_device[FIRMWARE_ONE].bootFlag == DEVICE_WORK) {
        g_device[FIRMWARE_ONE].runCnt++;
        (void)FlashWrite(NLED_CONFIG_PARAM_START_ADDR, g_device, sizeof(g_device));
        ret = FlashJumpToApp(FIRMWARE_ONE_START_ADDR);
        if (ret != FLASH_RET_SUCCESS) {
            g_device[FIRMWARE_ONE].bootFlag = DEVICE_ERROR;
            (void)FlashWrite(NLED_CONFIG_PARAM_START_ADDR, g_device, sizeof(g_device));
            goto START;
        }
        
    } else if (g_device[FIRMWARE_TWO].bootFlag == DEVICE_WORK) {
        g_device[FIRMWARE_TWO].runCnt++;
        (void)FlashWrite(NLED_CONFIG_PARAM_START_ADDR, g_device, sizeof(g_device));
        ret != FlashJumpToApp(FIRMWARE_TWO_START_ADDR);
        if (ret != FLASH_RET_SUCCESS) {
            goto START;
        }
    } else {
        LOG_ERROR("firmware bootflag is can not jump, will check and select one");
        firmNum = DeviceGetFirmwareCanWork();
        if (firmNum == FIRMWARE_ONE) {
            ret = FlashJumpToApp(FIRMWARE_ONE_START_ADDR);
        } else if (firmNum == FIRMWARE_TWO) {
            ret = FlashJumpToApp(FIRMWARE_TWO_START_ADDR);
        } else {
            goto START; /* can not be here */    
        }
        
        if (ret != FLASH_RET_SUCCESS) {
            g_device[firmNum].bootFlag = DEVICE_ERROR;
            (void)FlashWrite(NLED_CONFIG_PARAM_START_ADDR, g_device, sizeof(g_device));
            goto START;
        }
    }
}

