
#ifndef __PLUTUS_API_H__
#define __PLUTUS_API_H__

#include "common.h"

#define MAX_REQUEST_SIZE 1024
#define RESPONSE_BODY_SIZE 102400 // 100K
#define PAGE_SIZE 100 // 100 item per page request

enum {
    RQT_USER_GET,
    RQT_USER_COUNT,
    RQT_USER_LOGIN,
    RQT_USER_UPDATE,
    RQT_USERS_GET,

    RQT_ADMIN_GET,
    RQT_ADMIN_ADD,
    RQT_ADMIN_UPDATE,
    RQT_ADMINS_GET,

    RQT_LENGTH,
};

typedef struct {
    uint16_t type;
    bytes data[MAX_REQUEST_SIZE];
} Request;

typedef struct {
    uint32_t status; // html status codes
    uint32_t size;
    clock_t  time; // time took to return the response
} ResponseMetaData;

typedef struct {
    ResponseMetaData md;
    bytes body[RESPONSE_BODY_SIZE];
} Response;

typedef bytes RequestData[MAX_REQUEST_SIZE];

typedef struct {
    void (*func)(RequestData, Response*);
    uint32_t args_size;
} API;


// static size chekcing
_Static_assert(offsetof(Request, data) == 2, "offset of Request->Data is invalid");
_Static_assert(sizeof(Request) == 1026, "Request Size is invalid");
_Static_assert(sizeof(ResponseMetaData) == 16, "ResponseMetaData Size is invalid");
_Static_assert(sizeof(Response) == 102416, "Response size is invalid");
_Static_assert(sizeof(bool) == 1, "bool size is invalid");


#endif // __PLUTUS_API_H__
