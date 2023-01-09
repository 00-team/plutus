
#ifndef __PLUTUS_ITEM_H__
#define __PLUTUS_ITEM_H__

#include "common.h"
#include "api.h"

bool item_write(int db, void *item, ssize_t size, ssize_t *write_size);
bool item_read(int db, void *item, ssize_t size, ssize_t *read_size);
void item_page(int db, page_t page, size_t item_size, Response *response);


#endif // __PLUTUS_ITEM_H__