
#ifndef USER_USER_H
#define USER_USER_H

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
} UserLoginResponse;

typedef struct {
    user_id_t user_id;
    User user;
} UserUpdateArgs;

_Static_assert(offsetof(UserLoginResponse, user_id) == 0, "offset of LoginResponse->user_id is invalid");
_Static_assert(offsetof(UserLoginResponse, created) == 142, "offset of LoginResponse->created is invalid");
_Static_assert(sizeof(UserLoginResponse) == 144, "UserLoginResponse size is invalid");

_Static_assert(sizeof(User) == 134, "User size is invalid");
_Static_assert(sizeof(user_id_t) == 8, "UserID size is invalid");
_Static_assert(sizeof(UserLoginArgs) == 78, "UserLoginArgs size is invalid");
_Static_assert(sizeof(UserUpdateArgs) == 144, "UserUpdateArgs size is invalid");
_Static_assert(offsetof(UserUpdateArgs, user) == 8, "offset of UserUpdateArgs->user is invalid");


#include "../server/api.h"

void user_get(Request request, Response response);
void user_count(Request request, Response response);
void user_login(Request request, Response response);
void user_update(Request request, Response response);

#endif // USER_USER_H
