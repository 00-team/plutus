
#ifndef __PLUTUS_ADMIN_H__
#define __PLUTUS_ADMIN_H__

#include <stddef.h>
#include <inttypes.h>

#include "user.h"
#include "api.h"

typedef uint32_t admin_id_t;
typedef uint8_t Perms[64]; // uint512_t

typedef struct {
    user_id_t user_id;
    Perms perms; // uint512_t
} Admin;


typedef struct {
    admin_id_t admin_id;
    Perms perms; // uint512_t
} AdminGetBody;


typedef struct {
    admin_id_t admin_id;
    Perms perms; // uint512_t
    uint8_t delete;
    // 3 pad
} AdminUpdateArgs;


void admin_get(RequestData request, Response *response);
void admin_add(RequestData request, Response *response);
void admin_update(RequestData request, Response *response);



// static asserts
_Static_assert(sizeof(Admin) == 72, "invalid Admin size");
_Static_assert(sizeof(AdminGetBody) == 68, "invalid AdminGetBody size");
_Static_assert(offsetof(Admin, perms) == 8, "invalid Admin->perms offset");
_Static_assert(offsetof(AdminGetBody, perms) == 4, "invalid AdminGetBody->perms offset");
_Static_assert(sizeof(AdminUpdateArgs) == 72, "invalid AdminUpdateArgs size");
_Static_assert(offsetof(AdminUpdateArgs, perms) == 4, "invalid AdminUpdateArgs->perms offset");
_Static_assert(offsetof(AdminUpdateArgs, delete) == 68, "invalid AdminUpdateArgs->delete offset");

#endif // __PLUTUS_ADMIN_H__
