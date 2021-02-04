#include "usrlib.h"

#include <stdio.h>

int32_t memset_s(void *buff, char value, uint32_t len)
{
    char *temp = NULL;
    uint32_t i;
    if (buff == NULL) {
        return -1;
    }
    temp = (char *)buff;
    for (i = 0; i < len; i++) {
        temp[i] = value;
    }
    return 0;
}

int32_t memcpy_s(void *buff, void *buff1, uint32_t len)
{
    char *temp = NULL;
    char *temp1 = NULL;
    uint32_t i;
    if ((buff == NULL)||(buff1 == NULL)) {
        return -1;
    }
    temp = (char *)buff;
    temp1 = (char *)buff1;
    for (i = 0; i < len; i++) {
        temp[i] = temp1[i];
    }
    return 0;
}

int32_t memcmp_s(void *buff, char value, uint32_t len)
{
    char *temp = NULL;
    uint32_t i;
    if (buff == NULL) {
        return -1;
    }
    temp = (char *)buff;
    for (i = 0; i < len; i++) {
        temp[i] = value;
    }
    return 0;
}
/* 
int strcpy_s()
{

}

int strncpy_s()
{

}
 */
