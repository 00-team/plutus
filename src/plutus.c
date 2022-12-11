
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <openssl/sha.h>
#include <sys/random.h>
#include <unistd.h>

#include "plutus.h"

typedef struct {
    user_id_t id;
    unsigned short cc;
    char phone[12];
    unsigned char token[64];
    char nickname[52];
    unsigned char picture[13];
    unsigned char ext;
} User;

FILE *udb = NULL;
user_id_t DELETED_USER_ID = 0;

// temp variables
unsigned char hash[SHA512_DIGEST_LENGTH];
unsigned char data[] = "A_TOKNE";

void user_print(User *user) {
    printf("----------------------\n");
    printf("id: %ld\ncc: %d\nphone: %s\n", user->id, user->cc, user->phone);
    printf("token: ");

    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
        printf("%02x", user->token[i]);

    printf("\nnickname: %s\npicture: ", user->nickname);

    for (int i = 0; i < 13; i++)
        printf("%02x", user->picture[i]);

    printf("\n----------------------\n");
}

// return a pointer to a user
void user_set(User *user, user_id_t user_id) {
    user->id = user_id;
    user->cc = 98;

    memcpy(user->phone, "09440001122", 12);
    memcpy(user->token, hash, SHA512_DIGEST_LENGTH);
    memcpy(user->nickname, "nickname gg ez", 15);
    getrandom(user->picture, member_size(User, picture), GRND_NONBLOCK);
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
        user_set(&user, i);
        fwrite(&user, sizeof(User), 1, udb);
    }
}

// read one random user from db
void user_read(User *user, user_id_t id) {
    fseek(udb, sizeof(User) * (id - 1), SEEK_SET);
    fread(user, sizeof(User), 1, udb);
}

void user_delete(user_id_t id) {
    fseek(udb, sizeof(User) * (id - 1), SEEK_SET);
    fwrite(&DELETED_USER_ID, sizeof(user_id_t), 1, udb);
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
        if (user.id != 0)
            count++;
    }

    return count;
}

int main() {
    setup();

    write_n_users(100000);

    clean_up();
    return 0;

    // clock_t begin = clock();
    // user_read(696969);
    // clock_t end = clock();
    // printf("user read: %ld\n", end - begin);
}
