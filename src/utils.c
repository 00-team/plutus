
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "plutus.h"
#include "logger.h"

#define LOG_SCTOR SECTOR_MAIN

FILE *file_open(char *filename);


extern FILE *udb;
extern FILE *phdb;
extern FILE *adb;

bool setup_files(void) {
    udb  = file_open(USER_DB_FILENAME);
    phdb = file_open(PHONE_DB_FILENAME);
    adb  = file_open(ADMIN_DB_FILENAME);

    // return true on failure
    return (udb == NULL || phdb == NULL || adb == NULL);
}

void clean_up_files(void) {
    if (udb  != NULL) fclose(udb);
    if (phdb != NULL) fclose(phdb);
    if (adb  != NULL) fclose(adb);
}

void cleanup_handler(__attribute__((unused)) int signum) {
    if (udb != NULL) {
        int udb_fd = fileno(udb);
        fsync(udb_fd);
        close(udb_fd);
    }
    if (phdb != NULL) {
        int phdb_fd = fileno(phdb);
        fsync(phdb_fd);
        close(phdb_fd);
    }
    if (adb != NULL) {
        int adb_fd = fileno(adb);
        fsync(adb_fd);
        close(adb_fd);
    }

    _exit(0);
}

FILE *file_open(char *filename) {
    FILE *f;

    f = fopen(filename, "ab+");

    if (f == NULL) {
        log_trace("error while opening (%s) with ab+ flag", filename);
        return NULL;
    }

    if (fclose(f) != 0) {
        log_trace("error while closing (%s) with ab+ flag", filename);
        return NULL;
    }

    f = fopen(filename, "rb+");
    if (f == NULL) {
        log_trace("error while opening (%s) with rb+ flag", filename);
        return NULL;
    }

    return f;
}

long fsize(FILE *f) {
    fseek(f, 0, SEEK_END);
    return ftell(f);
}
