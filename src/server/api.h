
#ifndef SERVER_API_H
#define SERVER_API_H

#include <stddef.h>
#include "../user/user.h"

#define MIN_REQUEST_SIZE 3   // count
// user login: cc - phone - token
#define MAX_REQUEST_SIZE sizeof(UserUpdateArgs)
// user login: created - user_id - user
#define MAX_RESPONSE_SIZE sizeof(UserLoginResponse)

enum {
    RQT_USER_GET,
    RQT_USER_COUNT,
    RQT_USER_LOGIN,
    RQT_USER_UPDATE,
    RQT_LENGTH,
};

typedef struct {
    unsigned short type;
    unsigned char data[MAX_REQUEST_SIZE];
} RequestData;

_Static_assert(offsetof(RequestData, data) == 2, "offset of Request->Data is invalid");
_Static_assert(sizeof(RequestData) == 146, "Request Size is invalid");
_Static_assert(MAX_RESPONSE_SIZE == 144, "MAX_RESPONSE_SIZE Size is invalid");

typedef unsigned char Request[MAX_REQUEST_SIZE];
typedef unsigned char Response[MAX_RESPONSE_SIZE];

typedef struct {
    void (*func)(Request, Response);
    unsigned long rs_size;
} API;

#endif // SERVER_API_H
