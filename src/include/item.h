
#ifndef __PLUTUS_ITEM_H__
#define __PLUTUS_ITEM_H__

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <fcntl.h>

bool item_write(int db, void *item, ssize_t size, ssize_t *write_size);
bool item_read(int db, void *item, ssize_t size, ssize_t *read_size);


#endif // __PLUTUS_ITEM_H__