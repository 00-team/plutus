
#ifndef __PLUTUS_LOGGER_H__
#define __PLUTUS_LOGGER_H__

#include "common.h"

#define LOG_TRACE_LVL1(LINE) #LINE
#define LOG_TRACE_LVL2(LINE) LOG_TRACE_LVL1(LINE)
#define LOG_THROW_LOCATION "[" __FILE__ ":" LOG_TRACE_LVL2(__LINE__) "] "

typedef enum {
    SECTOR_MAIN,
    SECTOR_USER,
    SECTOR_ADMIN,
    SECTOR_SERVER,
    SECTOR_LENGTH,
} Sector;


typedef enum {
    LFLAG_INFO,
    LFLAG_WARN,
    LFLAG_DEBUG,
    LFLAG_ERROR,
} Flag;

#define log_info(...)  logger(LOG_SCTOR, LFLAG_INFO,  __VA_ARGS__)
#define log_warn(...)  logger(LOG_SCTOR, LFLAG_WARN,  __VA_ARGS__)
#define log_debug(...) logger(LOG_SCTOR, LFLAG_DEBUG, __VA_ARGS__)
#define log_error(...) logger(LOG_SCTOR, LFLAG_ERROR, __VA_ARGS__)
#define log_trace(...) logger(LOG_SCTOR, LFLAG_ERROR, LOG_THROW_LOCATION __VA_ARGS__)

void logger(const Sector index, const Flag flag, const char *format, ...);


#endif // __PLUTUS_LOGGER_H__