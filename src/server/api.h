
#include "../user/user.h"

#define MIN_REQUEST_SIZE 3 // count
#define MAX_REQUEST_SIZE 80 // user login
#define MAX_RESPONSE_SIZE 135

enum {
    RQT_USER_GET,
    RQT_USER_DEL,
    RQT_USER_COUNT,
    RQT_USER_LOGIN,
    RQT_USER_UPDATE,
    RQT_LENGTH,
};

typedef char *Request;

typedef struct {
    void (*func)(char *, char *);
    unsigned long rs_size;
} API;

static const API apis[] = {
    [RQT_USER_GET] = { user_get, 1 + sizeof(User) },
    [RQT_USER_DEL] = { user_delete, 1 },
    [RQT_USER_COUNT] = { user_count, sizeof(user_id_t) },
    [RQT_USER_LOGIN] = { user_login, 1 + sizeof(User) },
};
