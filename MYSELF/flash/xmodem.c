#include "xmodem.h"
#include "xmodem_check.h"
#include "sys.h"
#include "usrlib.h"
#include "flash.h"
#include "debug.h"
#include "timer.h"
#include "uart.h"

#define XMODEM_RET_ERROR (-1)
#define XMODEM_RET_SUCCESS 0
#define XMODEM_RET_CONTINUE 1

#define XMODEM_REC_ERROR_TIMES 3 /* 一包数据如果连续接收错3次退出xmodem */
#define XMODEM_BUFF_MAX 20*1024
#define XMODEM_WORK_TIMEOUT 10
//接收缓冲,最大20k个字节,起始地址为0X20001000. 
static uint8_t g_uartRecbuff[XMODEM_BUFF_MAX] __attribute__((at(0X20001000)));
static uint8_t g_recbuf[2][PACKAGE_LEN_1KXMODEM_CRC16] = {0};
static struct XmodemPotocol g_xmodem;

struct XmodemPotocol *GetXmodemInfo(void)
{
    return &g_xmodem;
}

static void XmodemInit(uint32_t address)
{
    struct XmodemPotocol *info = GetXmodemInfo();

    memset_s(info, 0, sizeof(struct XmodemPotocol));
    info->curAddr = address;
    info->checkType = XMODEM_CHECK_TYPE_CRC16;
    info->packSize = PACKAGE_LEN_1KXMODEM_CRC16;
    info->dataSize = DATA_LEN_1KXMODEM;
    info->status = XMODEM_IDLE;
    info->recBuff = g_recbuf[0];
    info->data = g_recbuf[1];
    info->checkFunc = XmodemCheckCRC16;
}
/*------------------------硬件层-------------------*/

static void XmodemResponse(uint8_t byte)
{
    USART_SendData(USART3, byte);
}

/*
 * 接收一个字节的数据包并且    
 */
void XmodemReciveOneByte(uint8_t byte)
{
    static uint8_t i = 0;
    struct XmodemPotocol *info = GetXmodemInfo();
    info->recBuff[info->recCnt++] = byte;
    /* 一包数据接收完成 */
    if (info->recCnt >= info->packSize) {
        info->status = XMODEM_REC_ONE_PACKED;
        info->recCnt = 0;       
        return;  
    }
    /* 一包数据没有接收完成 */
    if (info->recCnt != 0) {
        return;
    }
    /* 包头解析 */
    switch (byte) {
    case SOH:
        if (info->checkType == XMODEM_CHECK_TYPE_CRC16) {
           info->packSize = PACKAGE_LEN_XMODEM_CRC16;
        } else {
           info->packSize = PACKAGE_LEN_XMODEM_SUM;
        }
        info->data = g_recbuf[i % 2];
        i++;
        info->recBuff = g_recbuf[i % 2];
        info->dataSize = DATA_LEN_XMODEM;
        info->status = XMODEM_RECIVING;
        break;
    case STX:
        if (info->checkType == XMODEM_CHECK_TYPE_CRC16) {
            info->packSize = PACKAGE_LEN_1KXMODEM_CRC16;
        } else {
           info->packSize = PACKAGE_LEN_1KXMODEM_SUM;
        }
        info->data = g_recbuf[i % 2];
        i++;
        info->recBuff = g_recbuf[i % 2];
        info->dataSize = DATA_LEN_1KXMODEM;
        info->status = XMODEM_RECIVING;
        break;
    case EOT:
        info->status = XMODEM_STOP;
        info->recCnt = 0;
        break;
    default:
        break;
    }
}

/*------------------------链路层-------------------*/

static uint8_t g_idleCnt = 0;
static void TimerTask(void)
{
    g_idleCnt++;
    g_xmodem.time++;
}

/* 
 * 空闲状态系统还么进入数据接收阶段，此阶段每隔3s向上位机发送字符c或NAK，c代表使用crc16校验Nak代表使用和校验    
 */
static int32_t XmodemIdleTask(void)
{
    int32_t ret = XMODEM_RET_SUCCESS;
    struct XmodemPotocol *info = GetXmodemInfo();

    LOG_INFO("xmodem entry idle task");
    /* 注册一个定时器任务来3秒定时阻塞，为了能快速退出空闲任务 */
    info->time = 0;
    g_idleCnt = 0;
    if (TimerFuncRegister(TimerTask, 3000) != TIMER_RET_SUCCESS) {
        LOG_ERROR("xmodem register idle timer error");
        return XMODEM_RET_ERROR;
    }
    
    while (info->status == XMODEM_IDLE) {
        if (info->time == 0) {
            info->checkFunc = XmodemCheckCRC16;
            info->checkType = XMODEM_CHECK_TYPE_CRC16;
            info->time++; /* 这里不++会导致这3秒内一直再发CRC16 */
            XmodemResponse(CRC16);
        } else if (info->time == 4) {
            info->checkFunc = XmodemCheckSum;
            info->checkType = XMODEM_CHECK_TYPE_SUM;
            info->time++; /* 同上*/
            XmodemResponse(NAK);
        } else if (info->time > 7) {
           info->time = 0;
        }

        /* 两分钟还没退出这个状态，取消这一次xmodem通信，退出 */
        if (g_idleCnt > 40) {
            LOG_ERROR("xmodem idle task more than 2 mintue");
            ret = XMODEM_RET_ERROR;
            XmodemResponse(CAN);
            XmodemResponse(CAN);
            break;
        }
    }

    if (TimerFuncUnregister(TimerTask) != TIMER_RET_SUCCESS) {
        LOG_ERROR("xmodem unregister idle timer error");
        return XMODEM_RET_ERROR;
    }

    return ret;
}

/*
 * 解析一包数据，这里不解析头数据，具体是Xmoden还是1K-xmodem在字节处理的时候已经决定了
 */
static int32_t XmodemParseOnePackedData(void)
{
    uint8_t packNum;
    uint16_t check = 0;
    struct XmodemPotocol *info = GetXmodemInfo();
    packNum = info->data[1];
    /* 数据包序号不对重发 */
    if (info->data[2] != (~packNum)) {
        LOG_ERROR("xmodem packed number is error");
        return XMODEM_RET_ERROR;         
    }
    /* 校验包序号,与期望接收到的包序号不同，重发，不支持多包发送 */
    if (packNum != info->packSum) {
        LOG_ERROR("xmodem packed number %d is not expect %d", (int32_t)packNum, (int32_t)info->packSum);
        return XMODEM_RET_ERROR;
    }

    info->calCheck = info->checkFunc(info->data + 3, info->dataSize);
    if (info->checkType == info->recCnt) {
        check |= info->data[info->packSize - 2];
        check <<= 8;
        check |= info->data[info->packSize - 1];
    } else {
        check |= info->data[info->packSize - 1];
    }
    /* 数据包校验出错 */
    if (info->calCheck != check) {
        LOG_ERROR("xmodem packed check error");
        return XMODEM_RET_ERROR;
    }

    info->packSum++;
    return XMODEM_RET_SUCCESS;
}

static void XmodemProcessOnePackedData(void)
{
    struct XmodemPotocol *info = GetXmodemInfo();
    /*写入到缓存中，满了20k再写入flash*/
    memcpy_s(&g_uartRecbuff[info->recLen], info->data + 3, info->dataSize);
    info->recLen += info->dataSize;
    if (info->recLen >= XMODEM_BUFF_MAX) {
        FlashWrite(info->curAddr, &g_uartRecbuff, info->recLen / 2);
        info->curAddr += info->recLen;
        info->recLen = 0;
    }
    
    LOG_INFO("xmodem recive one packed");
}

static int32_t XmodemProcessMain(void)
{
    int32_t ret;
    struct XmodemPotocol *info = GetXmodemInfo();
    LOG_INFO("xmodem entry work main task");
    /* 
     * 注册一个定时器任务,10s内无操作认为取消这一次通信,
     * 正常应该放置在字节接收的函数中处理，但是为了启动前获取校验类型也是用了该变量，将工作中计时处理放置在这里 
     */
    info->time = 0;
    ret = TimerFuncRegister(TimerTask, 1000);
    if (ret != TIMER_RET_SUCCESS) {
        LOG_ERROR("xmodem register work timer error");
        return XMODEM_RET_ERROR;
    }
    
    while (info->status != XMODEM_STOP) {
        if (info->time >= XMODEM_WORK_TIMEOUT) {
            LOG_ERROR("xmodem work timeout");
            /* 发送两次CAN信号终止升级 */
            XmodemResponse(CAN);
            XmodemResponse(CAN);
            ret = XMODEM_RET_ERROR;
            goto ERROR_EXIT;
        }
        
        if (info->status != XMODEM_REC_ONE_PACKED) {
            continue;
        }
        info->time = 0;
        ret = XmodemParseOnePackedData();
        if (ret != XMODEM_RET_SUCCESS) {
            XmodemResponse(NAK);
            info->errCnt++;
            if (info->errCnt <= XMODEM_REC_ERROR_TIMES) {
                continue;
            }
            LOG_ERROR("xmodem recive error times over %d", XMODEM_REC_ERROR_TIMES);
            XmodemResponse(CAN);
            XmodemResponse(CAN);
            ret = XMODEM_RET_ERROR;
            goto ERROR_EXIT;
        }

        info->errCnt = 0;
        XmodemProcessOnePackedData();
        XmodemResponse(ACK);
    }
    /* 数据包接收完成之后需要再把最后一些数据写入 */
    FlashWrite(info->curAddr, &g_uartRecbuff, info->recLen / 2);
    ret = XMODEM_RET_SUCCESS;
    /* ETO也要应答一下 */
    XmodemResponse(ACK);
ERROR_EXIT:
    (void)TimerFuncUnregister(TimerTask);
    return ret;
}

/*------------------------应用层-------------------*/

/* 
 * 空闲任务还是升级任务都是阻塞是执行
 */
void XmodemUpdate(uint8_t firmware)
{
    int32_t ret;
    uint32_t startAddress;

    if (firmware == FIRMWARE_ONE) {
        startAddress = FIRMWARE_ONE_START_ADDR;
    } else if (firmware == FIRMWARE_TWO) {
        startAddress = FIRMWARE_TWO_START_ADDR;
    } else  {
        LOG_ERROR("xmodem firmware select error");
        return;
    }

    XmodemInit(startAddress);
    ret = UartFuncRegister(XmodemReciveOneByte);
    if (ret != UART_RET_SUCCESS) {
        LOG_ERROR("xmodem register uart recive function error");
        return;
    }

    ret = XmodemIdleTask();
    if (ret != UART_RET_SUCCESS) {
        LOG_ERROR("xmodem register uart recive function error");
        return;
    }

    (void)XmodemProcessMain();
    (void)UartFuncUnregister(XmodemReciveOneByte);
}
