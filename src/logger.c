
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <errno.h>

#include "logger.h"


#define MESSAGE_MAX 8196
#define INFO_MAX 512


typedef struct {
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t ms;
} DateTime;

typedef struct {
    FILE *file;
    const char *name;
} SectorFile;


static uint8_t sector_file_day = 0;
static bool log_to_screen = true;
static bool log_to_file = true;

static SectorFile SECTORS[] = {
    [SECTOR_MAIN]  = { NULL, "main"  },
    [SECTOR_USER]  = { NULL, "user"  },
    [SECTOR_ADMIN] = { NULL, "admin" },
};


static void get_datetime(DateTime *datetime) {
    struct timeval tv;
    time_t rawtime = time(NULL);
    struct tm *timeinfo = localtime(&rawtime);

    datetime->month = timeinfo->tm_mon + 1;
    datetime->day = timeinfo->tm_mday;
    datetime->hour = timeinfo->tm_hour;
    datetime->minutes = timeinfo->tm_min;
    datetime->seconds = timeinfo->tm_sec;

    if (gettimeofday(&tv, NULL) < 0)
        datetime->ms = 0;
    else
        datetime->ms = (uint8_t)(tv.tv_usec / 10000);
}

static void update_sectors(DateTime *datetime) {
    char logpath[40];

    for (uint8_t i = 0; i < SECTOR_LENGTH; i++) {
        snprintf(
            logpath, sizeof(logpath), "logs/%s/%02d_%02d.log", 
            SECTORS[i].name, datetime->month, datetime->day
        );

        SECTORS[i].file = fopen(logpath, "a");
    }

    sector_file_day = datetime->day;
}


void logger(const Sector index, const char *tag, const char *format, ...) {

    DateTime datetime;
    va_list args;

    SectorFile *sctor = &SECTORS[index];

    char message[MESSAGE_MAX];
    char info[INFO_MAX];

    // format the message
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    // update the datetime
    get_datetime(&datetime);
    snprintf(
        info, sizeof(info), "%02d:%02d:%02d.%03d <%s>", 
        datetime.hour, datetime.minutes, datetime.seconds, datetime.ms,
        tag
    );

    if (datetime.day != sector_file_day) {
        update_sectors(&datetime);
    }


    if (log_to_screen) {
        printf("%s %s\n", info, message);
    }
    
    if (log_to_file) {
        if (sctor->file != NULL) {
            fprintf(sctor->file, "%s %s\n", info, message);
            fflush(sctor->file);
        } else {
            update_sectors(&datetime);
        }
    }
}


void logger_setup(void) {
    char dirname[32];
    DateTime datetime;

    get_datetime(&datetime);

    mkdir("logs", 0755);

    for (uint8_t i = 0; i < SECTOR_LENGTH; i++) {
        snprintf(dirname, sizeof(dirname), "logs/%s", SECTORS[i].name);
        mkdir(dirname, 0755);
    }

    update_sectors(&datetime);
}

void logger_cleanup(void) {
    for (uint8_t i = 0; i < SECTOR_LENGTH; i++) {
        if (SECTORS[i].file != NULL)
            fclose(SECTORS[i].file);
    }
}
