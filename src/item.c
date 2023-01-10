
#include "item.h"
#include "logger.h"
#include "plutus.h"

#define LOG_SCTOR SECTOR_MAIN

bool item_write(int db, void *item, ssize_t size, ssize_t *write_size) {
    ssize_t ws = write(db, item, size);

    if (write_size != NULL)
        *write_size = ws;

    if (ws != size) {
        log_error("[item_write]: %u/%u %d. %s", ws, size, errno, strerror(errno));
        return false;
    }

    return true;
}

bool item_read(int db, void *item, ssize_t size, ssize_t *read_size) {
    ssize_t rs = read(db, item, size);

    if (read_size != NULL)
        *read_size = rs;
    
    if (rs != size) {
        if (rs == 0)
            log_info("item_read end of file!");

        else if (rs < 0)
            log_error("[item_read]: %d. %s", errno, strerror(errno));

        else
            log_warn("[item_read]: invalid item size: %d/%u", rs, size);

        return false;
    }

    return true;
}


void item_page(int db, page_t page, size_t item_size, Response *response) {
    off_t pos = page * PAGE_SIZE * item_size;
    off_t db_size = fsize(db);
    uint64_t count = db_size / item_size;
    off_t max_pos = db_size - item_size;
    ssize_t read_size;

    if (pos > max_pos) {
        response->md.status = 404;
        response->md.size = 0;
        return;
    }

    memcpy(response->body, &count, sizeof(count));

    lseek(db, pos, SEEK_SET);
    read_size = read(db, &(response->body[sizeof(count)]), PAGE_SIZE * item_size);

    if (read_size < 0) {
        response->md.size = 0;
        response->md.status = 500;
        log_error("[item_page]: %d. %s", errno, strerror(errno));
        return;
    }

    response->md.status = 200;
    response->md.size = read_size + sizeof(count);
}
