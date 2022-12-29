
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include "plutus.h"
#include "logger.h"

#define LOG_SCTOR SECTOR_MAIN

int file_open(char *filename);


extern int udb;
extern int phdb;
extern int adb;

bool setup_files(void) {
    udb  = file_open(USER_DB_FILENAME);
    phdb = file_open(PHONE_DB_FILENAME);
    adb  = file_open(ADMIN_DB_FILENAME);

    // return true on failure
    return (udb == -1 || phdb == -1 || adb == -1);
}

void cleanup_files(void) {
    if (udb  != -1) close(udb);
    if (phdb != -1) close(phdb);
    if (adb  != -1) close(adb);
}

void cleanup_handler(__attribute__((unused)) int signum) {
    if (udb  != -1) close(udb);
    if (phdb != -1) close(phdb);
    if (adb  != -1) close(adb);
    
    // if (udb != NULL) {
    //     int udb_fd = fileno(udb);
    //     fsync(udb_fd);
    //     lseek(udb_fd, 0, SEEK_SET);
    //     close(udb_fd);
    // }
    // if (phdb != NULL) {
    //     // int phdb_fd = fileno(phdb);
    //     // fsync(phdb_fd);
    //     // printf("res: %ld\n", lseek(phdb_fd, 0, SEEK_SET));
    //     // fflush(phdb);
    //     fclose(phdb);
    //     // close(phdb_fd);
    // }
    // if (adb != NULL) {
    //     int adb_fd = fileno(adb);
    //     fsync(adb_fd);
    //     close(adb_fd);
    // }

    _exit(0);
}

int file_open(char *filename) {

    int fd = open(filename, O_RDWR | O_CREAT | O_NOATIME, 00644);

    /*
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
    */

    return fd;
}

off_t fsize(int fd) {
    return lseek(fd, 0, SEEK_END);
    // fseek(f, 0, SEEK_END);
    // return ftell(f);
}
