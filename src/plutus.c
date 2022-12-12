
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <openssl/sha.h>
#include <sys/random.h>
#include <unistd.h>

#include "plutus.h"

// 152 byte
typedef struct {
    user_id_t id;
    unsigned short cc;
    char phone[12];
    unsigned char token[64];
    char nickname[52];
    unsigned char picture[13];
    unsigned char ext;
} OldUser;

// 128 byte
typedef struct {
    unsigned long phone;
    unsigned char token[SHA512_DIGEST_LENGTH];
    unsigned char picture[USER_PICTURE_SIZE];
    char nickname[USER_NICNAME_SIZE];
    unsigned char flag; // USER_DELETED_FLAG or anything else
    unsigned char ext;
} User;

FILE *udb = NULL;
const char DELETED_FLAG = 'D';

// temp variables
unsigned char hash[SHA512_DIGEST_LENGTH];
unsigned char data[] = "A_TOKNE";

void user_print(User *user, user_id_t id) {
    if (user->flag == DELETED_FLAG) {
        printf("--------DELETED-------\n");
    } else {
        printf("----------------------\n");
    }
    printf("id: %ld\nphone: %ld\n", id, user->phone);
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
    user->phone = 9809133322111;
    user->ext = EXT_JPG;

    memcpy(user->token, hash, SHA512_DIGEST_LENGTH);
    memcpy(
        user->nickname, 
        "xGGEZGGEZGGEZGGEZGGEZGGEZGGEZGGEZGGEZGGEZGGEZGGEZx", 
        50
    );
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

user_id_t user_count() {
    // get the current position
    long current_pos = ftell(udb);
    // check if there was any error
    // in case if any error just return 0 as user count
    if (current_pos < 0)
        return 0;

    // go to end of the file
    fseek(udb, 0, SEEK_END);
    // get the current position
    long db_size = ftell(udb);
    // check if there was any error
    // in case if any error just return 0 as user count
    if (db_size < 0)
        return 0;

    fseek(udb, current_pos, SEEK_SET);

    // divide the size of database to the size of a user
    return db_size / sizeof(User);
}

user_id_t user_count2() {
    User user;
    user_id_t count = 0;

    // get the current position
    long current_pos = ftell(udb);

    // check if there was any error
    // in case if any error just return 0 as user count
    if (current_pos < 0)
        return 0;

    // go to end of the file
    int cur = fseek(udb, 0, SEEK_SET);
    // check if there was any error
    // in case if any error just return 0 as user count
    if (cur != 0)
        return 0;

    while (fread(&user, sizeof(User), 1, udb)) {
        if (user.flag != DELETED_FLAG)
            count++;
    }

    return count;
}

int main() {
    setup();

    write_n_users(1000);

    clean_up();
    return 0;

    // clock_t begin = clock();
    // user_read(696969);
    // clock_t end = clock();
    // printf("user read: %ld\n", end - begin);
}
