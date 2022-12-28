
#ifndef __USER_USER_H__
#define __USER_USER_H__

#include <stddef.h>

#define USER_PICTURE_SIZE 4
#define USER_TOKEN_SIZE 64
#define USER_EMPTY_SIZE 100000 // 0.8 MegaBytes 100K user ids

typedef unsigned long long user_id_t;

typedef struct {
    unsigned short cc;
    char phone[12];
    unsigned char flag;  // DELETED_FLAG or anything else
    unsigned char ext;
    unsigned char picture[4];
    char token[USER_TOKEN_SIZE];
    char nickname[50];
} User;

typedef struct {
    unsigned short cc;
    char phone[12];
    char token[USER_TOKEN_SIZE];
} UserLoginArgs;

typedef struct {
    user_id_t user_id;
    User user;
    unsigned char created;
} UserLoginBody;

typedef struct {
    user_id_t user_id;
    User user;
} UserUpdateArgs;


#include "api.h"

void user_get(RequestData request, Response *response);
void user_count(RequestData request, Response *response);
void user_login(RequestData request, Response *response);
void user_update(RequestData request, Response *response);





// static asserts
_Static_assert(offsetof(UserLoginBody, user_id) == 0, "offset of LoginResponse->user_id is invalid");
_Static_assert(offsetof(UserLoginBody, created) == 142, "offset of LoginResponse->created is invalid");
_Static_assert(sizeof(UserLoginBody) == 144, "UserLoginBody size is invalid");

_Static_assert(sizeof(User) == 134, "User size is invalid");
_Static_assert(sizeof(user_id_t) == 8, "UserID size is invalid");
_Static_assert(sizeof(UserLoginArgs) == 78, "UserLoginArgs size is invalid");
_Static_assert(sizeof(UserUpdateArgs) == 144, "UserUpdateArgs size is invalid");
_Static_assert(offsetof(UserUpdateArgs, user) == 8, "offset of UserUpdateArgs->user is invalid");




#endif // __USER_USER_H__
