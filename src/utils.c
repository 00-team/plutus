
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

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

    _exit(0);
}

int file_open(char *filename) {

    int fd = open(filename, O_RDWR | O_CREAT | O_NOATIME, 00644);

    if (fd == -1) {
        log_trace("error while opening (%s)!", filename);
        return -1;
    }

    if (lseek(fd, 0, SEEK_CUR) == -1) {
        log_trace("(%s) is not seekable!", filename);
        return -1;
    }

    return fd;
}

off_t fsize(int fd) {
    return lseek(fd, 0, SEEK_END);
    // fseek(f, 0, SEEK_END);
    // return ftell(f);
}

off_t seek_append(int fd, size_t size) {
    off_t pos = lseek(fd, 0, SEEK_END);
    off_t offset = pos % size;

    if (offset != 0) {
        log_warn("seek_append: offset is wrong: %ld", offset);
        return lseek(fd, size - offset, SEEK_CUR);
    }

    return pos;
}

bool obj_write(int db, void *obj, ssize_t size, ssize_t *write_size) {
    ssize_t ws = write(db, obj, size);

    if (write_size != NULL)
        *write_size = ws;

    if (ws != size) {
        log_error("[obj_write]: %u/%u %d. %s", ws, size, errno, strerror(errno));
        return false;
    }

    return true;
}

bool obj_read(int db, void *obj, ssize_t size, ssize_t *read_size) {
    ssize_t rs = read(db, obj, size);

    if (read_size != NULL)
        *read_size = rs;
    
    if (rs != size) {
        if (rs == 0)
            log_info("obj_read end of file!");

        else if (rs < 0)
            log_error("[obj_read]: %d. %s", errno, strerror(errno));

        else
            log_warn("[obj_read]: invalid object size: %d/%u", rs, size);

        return false;
    }

    return true;
}