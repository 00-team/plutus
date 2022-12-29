
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "logger.h"


#define MESSAGE_MAX 8196
#define INFO_MAX 512
#define PATH_MAX 40


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
    const char name[PATH_MAX - 15];
    char path[PATH_MAX];
} SectorFile;


static uint8_t sector_file_day = 0;
static bool log_to_screen = true;
static bool log_to_file = true;

static SectorFile SECTORS[] = {
    [SECTOR_MAIN]  = { NULL, "main"  },
    [SECTOR_USER]  = { NULL, "user"  },
    [SECTOR_ADMIN] = { NULL, "admin" },
};

static void make_dirs() {
    char dirname[PATH_MAX - 10];

    mkdir("logs", 0755);

    for (uint8_t i = 0; i < SECTOR_LENGTH; i++) {
        snprintf(dirname, sizeof(dirname), "logs/%s", SECTORS[i].name);
        mkdir(dirname, 0755);
    }
}

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
    bool called_make_dirs = false;
    FILE *fd = NULL;

    for (uint8_t i = 0; i < SECTOR_LENGTH; i++) {
        snprintf(
            SECTORS[i].path, PATH_MAX, "logs/%s/%02d-%02d.log", 
            SECTORS[i].name, datetime->month, datetime->day
        );

        fd = fopen(SECTORS[i].path, "a");
        if (!called_make_dirs && fd == NULL && errno == ENOENT) {
            make_dirs();
            called_make_dirs = true;
            fd = fopen(SECTORS[i].path, "a");
        }

        SECTORS[i].file = fd;
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

    if (log_to_screen)
        printf("%s %s\n", info, message);


    if (log_to_file) {
        if (datetime.day != sector_file_day || access(sctor->path, F_OK))
            update_sectors(&datetime);

        if (sctor->file != NULL) {
            fprintf(sctor->file, "%s %s\n", info, message);
            fflush(sctor->file);
        } else {
            update_sectors(&datetime);
        }
    }
}


void logger_setup(void) {
    DateTime datetime;
    get_datetime(&datetime);
    make_dirs();
    update_sectors(&datetime);
}

void logger_cleanup(void) {
    for (uint8_t i = 0; i < SECTOR_LENGTH; i++) {
        if (SECTORS[i].file != NULL)
            fclose(SECTORS[i].file);
    }
}
