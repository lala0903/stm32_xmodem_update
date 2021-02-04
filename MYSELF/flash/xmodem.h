#ifndef __XMODEM_H__
#define __XMODEM_H__

#include <stdint.h>

#define SOH 0x01    /* Xmodem数据头 */
#define STX 0x02    /* 1K-Xmodem数据头 */
#define EOT 0x04    /* 发送结束 */
#define ACK 0x06    /* 认可响应 */
#define NAK 0x15    /* 不认可响应 */
#define CRC16 'c'    /* crc16校验 */
#define CAN 0x18    /* 撤销传送 */
#define CTRLZ 0x1A  /* 填充数据包  */

#define PACKAGE_LEN_XMODEM_SUM 133
#define PACKAGE_LEN_1KXMODEM_SUM 1028
#define PACKAGE_LEN_XMODEM_CRC16 134
#define PACKAGE_LEN_1KXMODEM_CRC16 1029
#define DATA_LEN_XMODEM 128
#define DATA_LEN_1KXMODEM 1024

#define XMODEM_CHECK_TYPE_CRC16 1
#define XMODEM_CHECK_TYPE_SUM 0

#define XMODEM_IDLE 0
#define XMODEM_RECIVING 1
#define XMODEM_REC_ONE_PACKED 2
#define XMODEM_STOP 2

struct XmodemPotocol {
    uint32_t curAddr; /* 当前操作的flash地址 */
    uint32_t recLen; /* 接收缓冲区已接收数据的长度 */
    uint16_t recCnt; /* 一包数据接收计数 */
    uint16_t packSize; /* 数据包的长度 */
    uint16_t dataSize; /* 数据包data的大小 */
    uint16_t calCheck; /* 计算得出的校验返回值 */
    uint8_t packSum; /* 当前待接收包的编号 */
    uint8_t checkType; /* 校验类型 和校验/crc16 */
    uint8_t status; /* xmodem当前状态 */
    uint8_t firmware; /* 操作的固件号 暂未使用 */
    uint8_t errCnt; /* 接收出错计数 */
    uint8_t time; /* 任务计数 */
    uint8_t *recBuff; /* 一包数据的接收缓冲区 */
    uint16_t (*checkFunc)(uint8_t *data, uint32_t len); /* 校验函数 */
};

void XmodemUpdate(uint8_t firmware);

#endif // !__XMODEM_H__
