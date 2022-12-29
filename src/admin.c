
// #include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "admin.h"
#include "logger.h"

#define LOG_SCTOR SECTOR_ADMIN


int adb = -1;


void admin_test(void);

// void admin_add(void);
// void admin_get(void);
// void admin_update(void);

admin_id_t admin_search(user_id_t user_id, Admin *admin) {
    admin_id_t admin_id = 0;

    // go to start of the database
    // fseek(adb, 0, SEEK_SET);
    lseek(adb, 0, SEEK_SET);

    

    // while (fread(admin, sizeof(Admin), 1, adb)) {
    while (read(adb, admin, sizeof(Admin)) == sizeof(Admin)) {
        admin_id++;

        if (user_id == admin->user_id) {
            log_info("adming search found: %d, user_id: %lld", admin_id, admin->user_id);
            // fseek(adb, -sizeof(Admin), SEEK_CUR);
            lseek(adb, -sizeof(Admin), SEEK_CUR);
            return admin_id;
        }
    }

    return 0;
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

// void admin_update(RequestData request, Response *response) {}

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

    // fseek(adb, 0, SEEK_SET);
    lseek(adb, 0, SEEK_SET);
    
    memset(&admin, 0, sizeof(Admin));
    admin.user_id = 1;
    memset(admin.perms, ' ', sizeof(admin.perms));

    // fwrite(&admin, sizeof(Admin), 1, adb);
    write(adb, &admin, sizeof(Admin));
    // fflush(adb);
    fsync(adb);
}
