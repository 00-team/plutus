
#ifndef __PLUTUS_ADMIN_H__
#define __PLUTUS_ADMIN_H__

#include <stddef.h>
#include <inttypes.h>

#include "user.h"

typedef uint32_t admin_id_t;

typedef struct {
    user_id_t user_id;
    uint8_t perms[128]; // uint128
} Admin;


typedef struct {
    admin_id_t admin_id;
    uint8_t perms[128];  // uint128
} AdminGetBody;




#include "api.h"

void admin_get(RequestData request, Response *response);
// void admin_update(RequestData request, Response *response);



// static asserts
_Static_assert(sizeof(Admin) == 136, "invalid Admin size");
_Static_assert(sizeof(AdminGetBody) == 132, "invalid AdminGetBody size");
_Static_assert(offsetof(AdminGetBody, perms) == 4, "invalid AdminGetBody->perms offset");

#endif // __PLUTUS_ADMIN_H__
