
#include "../user/user.h"

#define MIN_REQUEST_SIZE 3
#define MAX_REQUEST_SIZE 10
#define MAX_RESPONSE_SIZE 135

enum {
    RQT_USER_GET,
    RQT_USER_DEL,
    RQT_USER_COUNT,
    RQT_USER_UPDATE,
    RQT_LENGTH,
};

typedef char *Request;

typedef struct {
    void (*func)(char *, char *);
    size_t rs_size;
} API;

static const API apis[] = {
    [RQT_USER_GET] = { user_get, 1 + sizeof(User) },
    [RQT_USER_COUNT] = { user_count, sizeof(user_id_t) },
};
