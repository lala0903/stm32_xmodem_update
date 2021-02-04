#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <stdint.h>

#define DEVICE_RET_ERROR (-1) 
#define DEVICE_RET_SUCCESS 0 

#define DEVICE_IDLE 0 
#define DEVICE_WORK 1 
#define DEVICE_ERROR (-1) 

#pragma pack(2)
//静态区参数 配置信息
struct StaticDeviceMsg {
	int8_t firmwareInfo[64]; /* 固件信息 */
	int8_t hardVersion[32]; /* 硬件版本 */
	int8_t softVersion[64]; /* 软件版本 */
	int8_t device_sn[20]; /* 设备SN */
	int8_t device_imei[20]; /* 设备IMEI */
	int8_t bootFlag; /* flash信息更新标志 */
	uint32_t runCnt; /* 运行次数 */
	int8_t bootInfo[32]; /* 系统启动相关参数信息 */
};
#pragma pack()

void DeviceGetBootloaderMsg(void);
void DeviceGetFirmwareMsg(void);
void DeviceStartApplication(void);
#endif // !__DEVICE_H__
