#ifndef __XMODEM_CHECK_H__
#define __XMODEM_CHECK_H__

#include <stdint.h>

uint16_t XmodemCheckSum(uint8_t *data, uint32_t len);
uint16_t XmodemCheckCRC8(uint8_t *data, uint32_t len);
uint16_t XmodemCheckCRC16(uint8_t *data, uint32_t len);

#endif // !__XMODEM_CHECK_H__
