
#ifndef __SERVER_API_H__
#define __SERVER_API_H__

#include <stddef.h>
#include <sys/socket.h>
#include <stdbool.h>
#include "user.h"

#define MAX_REQUEST_SIZE 1024
#define RESPONSE_BODY_SIZE 1024 // 1K

enum {
    RQT_USER_GET,
    RQT_USER_COUNT,
    RQT_USER_LOGIN,
    RQT_USER_UPDATE,

    RQT_ADMIN_GET,

    RQT_LENGTH,
};

typedef struct {
    unsigned short type;
    char data[MAX_REQUEST_SIZE];
} Request;

typedef struct {
    unsigned int status; // html status codes
    unsigned int size;
} ResponseMetaData;

typedef struct {
    ResponseMetaData md;
    char body[RESPONSE_BODY_SIZE];
} Response;

typedef char RequestData[MAX_REQUEST_SIZE];

typedef struct {
    void (*func)(RequestData, Response*);
    unsigned int args_size;
} API;


// static size chekcing
_Static_assert(offsetof(Request, data) == 2, "offset of Request->Data is invalid");
_Static_assert(sizeof(Request) == 1026, "Request Size is invalid");
_Static_assert(sizeof(ResponseMetaData) == 8, "ResponseMetaData Size is invalid");
_Static_assert(sizeof(Response) == 1032, "Response size is invalid");
_Static_assert(sizeof(bool) == 1, "bool size is invalid");


#endif // __SERVER_API_H__
