
// #include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "admin.h"
#include "logger.h"
#include "plutus.h"

#define LOG_SCTOR SECTOR_ADMIN


int adb = -1;

void admin_test(void);

bool admin_write(Admin *admin) {
    if (write(adb, admin, sizeof(Admin)) != sizeof(Admin)) {
        log_error("admin_write went wrong!");
        return false;
    }

    return true;
}

bool admin_read(Admin *admin) {
    ssize_t size = read(adb, admin, sizeof(Admin));
    if (size != sizeof(Admin)) {
        if (size == 0)
            log_info("admin_read end of file!");
        else
            log_error("admin_read went wrong!");

        return false;
    }

    return true;
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

    log_debug("admin_id: %"PRIu32" user_id: %"PRIu64, args->admin_id, admin.user_id);

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

/*
void admin_login(RequestData request, Response *response) {
    // user_id
    // token
    // check if user_id exists
    // if it exists update the admin token
    // and send back the admin
    // res: 
    // admin perms and admin_id

    /

    each api will return a response size
    and each recv call will be call twice

    first recv is the metadata
    how big is the response and the status code of it
    

    each api also have a fix args size
    for example this api endpoint have user_id and a token as args
    which is 8 + 64

    /

    Admin admin;
    admin_id_t admin_id;
    AdminLoginArgs *args = (AdminLoginArgs *)request;
    AdminLoginBody *body = (AdminLoginBody *)response->body;

    admin_id = admin_search(args->user_id, &admin);

    if (admin_id == 0) {
        response->md.status = 401;
        response->md.size = 0;
        return;
    }

    response->md.status = 200;
    response->md.size = sizeof(AdminLoginBody);

    body->admin_id = admin_id;
    memcpy(body->perms, admin.perms, sizeof(admin.perms));

    // write the previous token into response
    memcpy(body->token, admin.token, sizeof(admin.token));

    // update the token
    memcpy(admin.token, args->token, sizeof(admin.token));
    fwrite(&admin, sizeof(Admin), 1, adb);

    // debug
    fflush(adb);
}
*/

void admin_test(void) {
    Admin admin;

    lseek(adb, 0, SEEK_SET);
    
    memset(&admin, 0, sizeof(Admin));
    admin.user_id = 3;
    memset(admin.perms, ' ', sizeof(admin.perms));

    admin_write(&admin);
    fsync(adb);
}
