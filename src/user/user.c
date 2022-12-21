
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>

#include <sys/random.h>
#include <openssl/sha.h>

#include "../plutus.h"
#include "phone.h"


FILE *udb = NULL;
const char DELETED_FLAG = 'D';
static user_id_t users_counts = 0;
static user_id_t empty_ids[USER_EMPTY_SIZE];
static user_id_t empty_ids_count = 0;


// temp variables
unsigned char hash[SHA512_DIGEST_LENGTH];
unsigned char data[] = "A_TOKNE";


void user_print(User *user, user_id_t id) {
    if (user->flag == DELETED_FLAG) {
        printf("--------DELETED-------\n");
    } else {
        printf("----------------------\n");
    }
    printf("id: %ld\nphone: +%d %s\n", id, user->cc, user->phone);
    printf("token: ");

    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
        printf("%02x", user->token[i]);

    printf("\nnickname: %s\npicture: ", user->nickname);

    for (int i = 0; i < USER_PICTURE_SIZE; i++)
        printf("%02x", user->picture[i]);

    printf("\n----------------------\n");
}

void user_set(User *user) {
    user->ext = 0;
    user->cc = 98;
    user->flag = 0;

    memcpy(user->phone, "09223334444", 12);
    memcpy(user->token, hash, SHA512_DIGEST_LENGTH);
    memcpy(user->nickname, "xGGEZGGEZGGEZGGEZGGEZGGEZGGEZGGEZGGEZGGEZGGEZGGEZx",
           50);
    getrandom(user->picture, USER_PICTURE_SIZE, GRND_NONBLOCK);
}

void write_n_users(user_id_t N) {
    // set the hash
    SHA512(data, strlen((char *)data), hash);

    user_id_t i = 1;
    User user;

    // rewrite the current database
    // basicly clear it out
    fclose(udb);
    fclose(fopen(USER_DB_FILENAME, "wb"));
    udb = fopen(USER_DB_FILENAME, "r+b");
    if (udb == NULL)
        die("error opening the (%s)!", USER_DB_FILENAME);

    // write N users
    for (; i <= N; i++) {
        user_set(&user);
        fwrite(&user, sizeof(User), 1, udb);
    }
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
    for (user_id_t i = 0; i < USER_EMPTY_SIZE; i++) {
        if (empty_ids[i] == 0) {
            empty_ids[i] = user_id;
            empty_ids_count++;
            break;
        }
    }
}

// convert user_id to User
char check_user_id(char *buffer, user_id_t *user_id, User *user) {
    *user_id = *(user_id_t *)buffer;
    long pos = sizeof(User) * (*user_id - 1);

    /*
       response[0] is type of the response
       value 4 for means "user not found / out of range / deleted"
       and any other value means "success"
    */

    if (pos < 0 || pos >= fsize(udb))
        return -1;

    fseek(udb, pos, SEEK_SET);
    fread(user, sizeof(User), 1, udb);
    fseek(udb, pos, SEEK_SET);

    if (user->flag == DELETED_FLAG)
        return -1;

    return 0;
}

// user get api
void user_get(char *request, char *response) {
    User user;
    user_id_t user_id = 0;

    /*
       response[0] is type of the response
       value 4 for means "user not found / out of range / deleted"
       and any other value means "success"
    */
    if (check_user_id(request, &user_id, &user) == -1) {
        response[0] = 4;
        return;
    }

    response[0] = 0;
    response++;
    memcpy(response, &user, sizeof(User));
}

// user delete api
void user_delete(char *request, char *response) {
    User user;
    user_id_t user_id = 0;

    /*
       response[0] is type of the response
       value 4 for means "user not found / out of range / deleted"
       and any other value means "success"
    */
    if (check_user_id(request, &user_id, &user) == -1) {
        response[0] = 4;
        return;
    }

    user.flag = DELETED_FLAG;
    fwrite(&user, sizeof(User), 1, udb);

    users_counts--;
    append_empty_user_id(user_id);

    response[0] = 0;
}

// user count api
void user_count(char *request, char *response) {
    bool exact = *(bool *)request;
    user_id_t count;

    if (exact) {
        memcpy(response, &users_counts, sizeof(user_id_t));
        return;
    }

    count = fsize(udb) / sizeof(User);
    memcpy(response, &count, sizeof(user_id_t));
}

void user_login(char *request, char *response) {
    // request
    // cc 2 bytes - phone 12 bytes - token 64 bytes
    // response
    // 1 byte: created - User

    unsigned short cc = *(unsigned short *)request;
    char *phone = &request[2];
    // char *token = &request[14];


    printf("cc: %d\n", cc);
    printf("phone: ");

    for (int i = 0; i < 12; i++) {
        printf("%c", phone[i]);
    }

    printf("\nrs: %s\n", response);
}

void user_setup(void) {
    User user;

    // clear the empty_ids list
    memset(empty_ids, 0, sizeof(user_id_t) * USER_EMPTY_SIZE);

    users_counts = fsize(udb) / sizeof(User);

    fseek(udb, 0, SEEK_SET);
    user_id_t current_user_id = 0;

    while (fread(&user, sizeof(User), 1, udb)) {
        current_user_id++;

        if (user.flag == DELETED_FLAG) {
            users_counts--;
            append_empty_user_id(current_user_id);
        }
    }
}