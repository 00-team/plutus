
#include "common.h"

#include "admin.h"
#include "logger.h"
#include "plutus.h"
#include "item.h"

#define LOG_SCTOR SECTOR_ADMIN


int adb = -1;

void admin_test(void);

bool admin_write(Admin *admin) {
    return item_write(adb, admin, sizeof(Admin), NULL);
}

bool admin_read(Admin *admin) {
    return item_read(adb, admin, sizeof(Admin), NULL);
}


admin_id_t admin_search(user_id_t user_id, Admin *admin) {
    admin_id_t admin_id = 0;

    // go to start of the database
    lseek(adb, 0, SEEK_SET);

    log_info("start searching for an admin with user_id: %lld", user_id);

    while (admin_read(admin)) {
        admin_id++;

        if (user_id == admin->user_id) {
            log_info("admin found: %d", admin_id);
            lseek(adb, -sizeof(Admin), SEEK_CUR);
            return admin_id;
        }
    }

    log_info("admin not found");

    return 0;
}


// convert admin_id to Admin
bool check_admin_id(admin_id_t admin_id, Admin *admin) {
    long pos = sizeof(Admin) * (admin_id - 1);
    long max_pos = fsize(adb) - sizeof(Admin);

    if (pos < 0 || pos > max_pos)
        return false;

    lseek(adb, pos, SEEK_SET);
    if (!admin_read(admin)) return false;
    lseek(adb, pos, SEEK_SET);

    return true;
}


void admin_get(RequestData request, Response *response) {
    Admin admin;
    admin_id_t admin_id;

    // args
    user_id_t user_id = *(user_id_t *)request;
    // body
    AdminGetBody *body = (AdminGetBody *)response->body;

    admin_id = admin_search(user_id, &admin);

    // not found
    if (admin_id == 0) {
        response->md.size = 0;
        response->md.status = 401;
        return;
    }

    response->md.size = sizeof(AdminGetBody);
    response->md.status = 200;

    body->admin_id = admin_id;
    memcpy(body->perms, admin.perms, sizeof(admin.perms));
}


void admin_add(RequestData request, Response *response) {
    Admin *args = (Admin *)request;
    Admin admin;
    admin_id_t admin_id;

    log_debug("args user_id: %"PRIu64, args->user_id);

    admin_id = admin_search(args->user_id, &admin);

    if (admin_id != 0) {
        response->md.size = 0;
        response->md.status = 400;

        return;
    }

    // search for an empty admin slot
    admin_id = admin_search(0, &admin);
    // if didn't find any append the new amdin
    if (admin_id == 0)
        admin_id = (seek_append(adb, sizeof(Admin)) / sizeof(Admin)) + 1;
    
    if (!admin_write(args)) {
        response->md.size = 0;
        response->md.status = 500;
        return;
    }

    memcpy(response->body, &admin_id, sizeof(admin_id_t));

    response->md.size = sizeof(admin_id_t);
    response->md.status = 200;
}


void admin_update(RequestData request, Response *response) {
    Admin admin;
    AdminUpdateArgs *args = (AdminUpdateArgs *)request;

    response->md.size = 0;

    if (!check_admin_id(args->admin_id, &admin)) {
        response->md.status = 400;
        return;
    }

    // BYTE_ORDER_DEPENDENT
    // admin.perms[sizeof(admin.perms) - 1] would be the big endian version
    if ((admin.perms[0] & 1) == 1) {
        log_info("MASTER admins cannot be modified - %"PRIu32, args->admin_id);
        response->md.status = 403;
        return;
    }

    if (args->delete)
        memset(&admin, 0, sizeof(Admin));
    else
        memcpy(admin.perms, args->perms, sizeof(admin.perms));

    
    if (!admin_write(&admin)) response->md.status = 500;
    else response->md.status = 200;
}


void admins_get(RequestData request, Response *response) {
    item_page(adb, *(page_t *)request, sizeof(Admin), response);
}

void admin_test(void) {
    Admin admin;

    lseek(adb, 0, SEEK_SET);
    
    memset(&admin, 0, sizeof(Admin));
    admin.user_id = 3;
    memset(admin.perms, ' ', sizeof(admin.perms));

    admin_write(&admin);
    fsync(adb);
}
