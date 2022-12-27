
#ifndef ADMIN_ADMIN_H
#define ADMIN_ADMIN_H

#include <stddef.h>
#include "../user/user.h"

typedef unsigned int admin_id_t;

typedef struct {
    user_id_t user_id;
    unsigned char perms[128];
} Admin;


typedef struct {
    admin_id_t admin_id;
    unsigned char perms[128];
} AdminGetBody;




#include "../server/api.h"

void admin_get(RequestData request, Response *response);
// void admin_update(RequestData request, Response *response);



// static asserts
_Static_assert(sizeof(Admin) == 136, "invalid Admin size");
_Static_assert(sizeof(AdminGetBody) == 132, "invalid AdminGetBody size");
_Static_assert(offsetof(AdminGetBody, perms) == 4, "invalid AdminGetBody->perms offset");

#endif
