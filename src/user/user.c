#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdbool.h>

#include "../plutus.h"
#include "user.h"

FILE *udb = NULL;
const char DELETED_FLAG = 'D';
static user_id_t users_counts = 0;

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


// return a pointer to a user
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

// write N user into db
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

// read one random user from db
void user_read(User *user, user_id_t id) {
    fseek(udb, sizeof(User) * (id - 1), SEEK_SET);
    fread(user, sizeof(User), 1, udb);
}

void user_delete(user_id_t id) {
    fseek(udb, sizeof(User) * (id - 1) + 126, SEEK_SET);
    fwrite(&DELETED_FLAG, sizeof(char), 1, udb);
}


void user_get(char *request, char *response) {
    User user;
    user_id_t user_id = *(user_id_t *)request;
    long pos = sizeof(User) * (user_id - 1);

    /*
       response[0] is type of the response 
       value 4 for means "user not found / out of range / deleted"
       and any other value means "success"
    */

    if (pos >= fsize(udb)) {
        response[0] = 4;
        return;
    }

    fseek(udb, pos, SEEK_SET);
    fread(&user, sizeof(User), 1, udb);
    
    if (user.flag == DELETED_FLAG) {
        response[0] = 4;
        return;
    }

    response[0] = 0;
    response++;
    memcpy(response, &user, sizeof(User));
}


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


void user_setup(void) {
    User user;
    users_counts = fsize(udb) / sizeof(User);
    fseek(udb, 0, SEEK_SET);

    while (fread(&user, sizeof(User), 1, udb)) {
        if (user.flag == DELETED_FLAG)
            users_counts--;
    }
}
