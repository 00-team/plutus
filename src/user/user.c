
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/random.h>
#include <errno.h>

#include "plutus.h"
#include "logger.h"
#include "phone.h"

#define LOG_SCTOR SECTOR_USER


int udb = -1;

static user_id_t users_counts = 0;
static user_id_t empty_ids[USER_EMPTY_SIZE];
static user_id_t empty_ids_count = 0;


bool user_write(User *user) {
    return obj_write(udb, user, sizeof(User), NULL);
}

bool user_read(User *user) {
    return obj_read(udb, user, sizeof(User), NULL);
}


void user_print(User *user, user_id_t id) {
    if (user->flag == USER_DELETED) {
        printf("--------DELETED-------\n");
    } else {
        printf("----------------------\n");
    }
    printf("id: %ld\nphone: +%d %s\n", id, user->cc, user->phone);
    printf("token: ");

    for (size_t i = 0; i < sizeof(user->token); i++)
        printf("%02x", user->token[i]);

    printf("\nnickname: %s\npicture: ", user->nickname);

    for (size_t i = 0; i < sizeof(user->picture); i++)
        printf("%02x", user->picture[i]);

    printf("\n----------------------\n");
}


// empty user ids
user_id_t get_empty_user_id(void) {
    user_id_t user_id = 0;

    if (empty_ids_count == 0)
        return 0;

    for (user_id_t i = 0; i < USER_EMPTY_SIZE; i++) {
        if (empty_ids[i] != 0) {
            user_id = empty_ids[i];
            empty_ids[i] = 0;
            empty_ids_count--;
            return user_id;
        }
    }

    empty_ids_count = 0;
    return 0;
}

void append_empty_user_id(user_id_t user_id) {
    int empty_idx = -1;

    for (int i = 0; i < USER_EMPTY_SIZE; i++) {
        if (empty_ids[i] == user_id) {
            // if alreadey exists return
            return;
        }

        if (empty_ids[i] == 0 && empty_idx == -1)
            empty_idx = i;
    }

    if (empty_idx != -1) {
        empty_ids[empty_idx] = user_id;
        empty_ids_count++;
    }
}

// convert user_id to User
char check_user_id(user_id_t user_id, User *user) {
    long pos = sizeof(User) * (user_id - 1);
    long max_pos = fsize(udb) - sizeof(User);

    if (pos < 0 || pos > max_pos)
        return -1;

    lseek(udb, pos, SEEK_SET);
    user_read(user);
    lseek(udb, pos, SEEK_SET);

    if (user->flag == USER_DELETED)
        return -1;

    return 0;
}

// add a new user
user_id_t user_add(User *user) {
    log_info("adding a user with phone: +%d %s", user->cc, user->phone);
    user_id_t user_id = get_empty_user_id();
    log_info("empty user_id: %"PRIu64, user_id);
    Phone phone = phone_convert(user->phone);
    log_info("phone index: %"PRIu64, phone.index);

    getrandom(user->picture, sizeof(user->picture), GRND_NONBLOCK);
    log_info(
        "picture: %02x%02x%02x%02x", 
        user->picture[0], user->picture[1], user->picture[2], user->picture[3]
    );

    if (user_id == 0)
        user_id = (seek_append(udb, sizeof(User)) / sizeof(User)) + 1;
    else
        lseek(udb, (user_id - 1) * sizeof(User), SEEK_SET);
    
    log_info("user_id: %"PRIu64, user_id);
    
    if (!user_write(user))
        return 0;

    phone_update(&phone, user_id);
    users_counts++;
    return user_id;
}


// user get api
void user_get(RequestData request, Response *response) {
    // request args:
    // 8 byte user_id
    User user;
    user_id_t user_id = *(user_id_t *)request;

    if (check_user_id(user_id, &user) == -1) {
        response->md.status = 401;
        response->md.size = 0;
        return;
    }

    response->md.status = 200;
    response->md.size = sizeof(User);
    memcpy(response->body, &user, sizeof(User));
}

// user count api
void user_count(RequestData request, Response *response) {
    // args: 1 byte bool exact
    bool exact = *(bool *)request;
    user_id_t count;

    response->md.status = 200;
    response->md.size = sizeof(user_id_t);

    if (exact) {
        memcpy(response->body, &users_counts, sizeof(user_id_t));
        return;
    }

    count = fsize(udb) / sizeof(User);
    memcpy(response->body, &count, sizeof(user_id_t));
}

void user_login(RequestData request, Response *response) {
    User user;
    UserLoginArgs *args = (UserLoginArgs *)request;
    UserLoginBody *body = (UserLoginBody *)response->body;

    response->md.size = sizeof(UserLoginBody);
    response->md.status = 200;

    Phone phone = phone_convert(args->phone);
    user_id_t user_id = phone_search(&phone);

    if (user_id == 0 || check_user_id(user_id, &user) == -1) {
        memset(&body->created, 1, 1);

        // clear the user
        memset(&user, 0, sizeof(User));
        // set the token, phone and cc of the new user
        memcpy(user.token, args->token, sizeof(user.token));
        memcpy(user.phone, args->phone, sizeof(user.phone));
        user.cc = args->cc;

        // add it
        user_id = user_add(&user);
    } else {
        memset(&body->created, 0, 1);

        // update the user token
        memcpy(user.token, args->token, sizeof(user.token));
        user_write(&user);
    }

    memcpy(&body->user_id, &user_id, sizeof(user_id_t));
    memcpy(&body->user, &user, sizeof(User));
}

void user_update(RequestData request, Response *response) {
    User user;
    UserUpdateArgs *args = (UserUpdateArgs *)request;

    response->md.status = 200;
    response->md.size = 0;

    if (check_user_id(args->user_id, &user) == -1) {
        response->md.status = 401;
        return;
    }

    if (args->user.flag == USER_DELETED) {
        user.flag = USER_DELETED;

        if (!user_write(&user)) {
            response->md.status = 500;
            return;
        }

        // delete the index
        Phone phone = phone_convert(user.phone);
        phone_update(&phone, 0);

        users_counts--;
        append_empty_user_id(args->user_id);

        response->md.status = 200;
        return;
    }

    if (!user_write(&args->user)) {
        response->md.status = 500;
        return;
    }
}


void users_get(RequestData request, Response *response) {
    uint32_t page = *(uint32_t *)request;
    off_t pos = page * USER_PAGE_SIZE * sizeof(User);
    off_t max_pos = fsize(udb) - sizeof(User);
    ssize_t read_size;

    if (pos > max_pos) {
        response->md.status = 404;
        response->md.size = 0;
        return;
    }

    lseek(udb, pos, SEEK_SET);
    read_size = read(udb, response->body, sizeof(User) * USER_PAGE_SIZE);
    
    if (read_size < 0) {
        response->md.size = 0;
        response->md.status = 500;
        log_error("[users_get]: %d. %s", errno, strerror(errno));
        return;
    }

    response->md.status = 200;
    response->md.size = read_size;
}


bool check_valid_user(User *user) {
    if (user->cc == 0 || user->cc > 999)
        return false;

    if (user->flag > 2) 
        return false;

    if (user->phone[0] == 0)
        return false;
    
    for (uint8_t i = 0; i < sizeof(user->phone); i++) {
        if (user->phone[i] == 0) continue;
        if (user->phone[i] > '9' || user->phone[i] < '0') return false;
    }

    return true;
}

void user_setup(void) {
    log_info("starting user_setup");
    User user;

    // clear the empty_ids list
    memset(empty_ids, 0, sizeof(user_id_t) * USER_EMPTY_SIZE);

    users_counts = fsize(udb) / sizeof(User);

    lseek(udb, 0, SEEK_SET);
    user_id_t current_user_id = 0;

    while (user_read(&user)) {
        current_user_id++;

        if (user.flag == USER_DELETED) {
            users_counts--;
            append_empty_user_id(current_user_id);
            log_info("found a empty user slot: %lld", current_user_id);
            continue;
        }

        if (!check_valid_user(&user)) {
            users_counts--;
            user.flag = USER_DELETED;
            lseek(udb, -sizeof(User), SEEK_CUR);
            user_write(&user);
            append_empty_user_id(current_user_id);
            log_warn("found an invalid user: %lld", current_user_id);
        }
    }
}
