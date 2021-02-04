#include "debug.h"

#include "sys.h"
#ifdef DEBUG

#include <stdarg.h>

const int8_t *g_levelInfo[LOG_MAX] = {
	[LOG_ERROR] = "ERROR",
	[LOG_WARN] = "WARN",
	[LOG_INFO] = "INFO",
	[LOG_DEBUG] = "DEBUG",
};

int32_t LogPrint(LogLevel level, const int8_t* fmt, ...)
{
	va_list valist;

	if ((level > LOG_MAX) || (fmt== NULL)) {
		printf("[LOG_IGNORE]: log input para invaild\n");
		return -1;
	}
	if (level > PRINT_LEVEL) {
		return 0;
	}
	printf("[%s] ", g_levelInfo[level]);
	va_start(valist, fmt);
	vprintf(fmt, valist);
	va_end(valist);

	return 0;
}
#endif

int32_t PrintString(int8_t *buff, int32_t len)
{
    int32_t i;
    if ((buff == NULL) || (len == 0)) {
		return -1;
	}

    for (i = 0; i < len; i++){
        USART_SendData(USART1, (uint16_t)buff[i]);
    }
    
    return len;
}