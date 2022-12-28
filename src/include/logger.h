
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdbool.h>

#define LOG_TRACE_LVL1(LINE) #LINE
#define LOG_TRACE_LVL2(LINE) LOG_TRACE_LVL1(LINE)
#define LOG_THROW_LOCATION "[" __FILE__ ":" LOG_TRACE_LVL2(__LINE__) "] "

typedef enum {
    SECTOR_MAIN,
    SECTOR_USER,
    SECTOR_ADMIN,
    SECTOR_LENGTH,
} Sector;

#define log_info(...)  logger(LOG_SCTOR, "INFO",  __VA_ARGS__)
#define log_warn(...)  logger(LOG_SCTOR, "WANR",  __VA_ARGS__)
#define log_debug(...) logger(LOG_SCTOR, "DEBUG", __VA_ARGS__)
#define log_error(...) logger(LOG_SCTOR, "ERROR", __VA_ARGS__)
#define log_trace(...) logger(LOG_SCTOR, "ERROR", LOG_THROW_LOCATION __VA_ARGS__)

void logger(const Sector index, const char *tag, const char *format, ...);


#endif // __LOGGER_H__