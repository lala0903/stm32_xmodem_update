#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <stdint.h>
#include <stdio.h>

#ifdef DEBUG
typedef enum {
  LOG_ERROR,
  LOG_WARN,
  LOG_INFO,
  LOG_DEBUG,
  LOG_MAX,
} LogLevel;

#define PRINT_LEVEL     LOG_MAX    /* 默认调试级别 */
#define NameLevel(name) (#name)      

int32_t LogPrint(LogLevel level, const int8_t* fmt, ...);
#define M_LOG(level, fmt, ...) \
do { \
    LogPrint(level, "<%s()> " fmt "\r\n",  __FUNCTION__, ##__VA_ARGS__); \
} while (0)

#define LOG_ERROR(fmt, ...) M_LOG(LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) M_LOG(LOG_WARN, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) M_LOG(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) M_LOG(LOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...) printf("[ERROR] <%s> %d: " fmt, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) printf("[WARN] <%s> %d: " fmt, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) printf("[INFO] <%s> %d: " fmt, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) printf("[DEBUG] <%s> %d: " fmt, __func__, __LINE__, ##__VA_ARGS__)
#endif /* DEBUG */


int32_t PrintString(int8_t *buff, int32_t len);

#endif // !__DEBUG_H_
