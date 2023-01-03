
#include "item.h"
#include "logger.h"

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
