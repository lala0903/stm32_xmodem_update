/*******************************************************************************
 * Copyleft (c) 2021 hx
 * @file    usrlib.h
 * @brief   c库内存操作函数，调用时需保证不会访问越界的问题。
 * @author  huangxin
 * @version 1.0
 * @date    2021-01-31
 ******************************************************************************/

#ifndef __USRLIB_H__
#define __USRLIB_H__

#include <stdint.h>

int32_t memset_s(void *buff, char value, uint32_t len);
int32_t memcpy_s(void *buff, void *buff1, uint32_t len);
int32_t memcmp_s(void *buff, char value, uint32_t len);

#endif // DEBUG
