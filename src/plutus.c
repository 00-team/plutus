
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
    unsigned long id;
    unsigned short cc;
    char phone[12];
    unsigned char token[64];
    char nickname[52];
    unsigned char picture[13];
    unsigned char ext;
} User;


FILE *udb = NULL;

// temp variables
unsigned char hash[SHA512_DIGEST_LENGTH];
unsigned char data[] = "A_TOKNE";

// 8 + 2 + 12 + 64 + 52 + 13 + 1 = 152

void print_user(User* user) {
    printf("----------------------\n");
    printf("id: %ld\ncc: %d\nphone: %s\n", user->id, user->cc, user->phone);
    printf("token: ");

    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
        printf("%02x", user->token[i]);

    // printf("\nnickname: %s\npicture: %s\n", user->nickname, user->picture);
    printf("\nnickname: %s\npicture: ", user->nickname);

    for (int i = 0; i < 13; i++)
        printf("%02x", user->picture[i]);

    printf("\n----------------------\n");
}

// return a pointer to a user
void get_user(User* user, unsigned long user_id) {
    user->id = user_id;
    user->cc = 98;

    memcpy(user->phone, "09440001122", 12);
    memcpy(user->token, hash, SHA512_DIGEST_LENGTH);
    memcpy(user->nickname, "nickname gg ez", 15);
    // memcpy(user.picture, "aGGGGGGGGGGGGG", 14);
    // memset(user.picture, 71, 14);
    getrandom(user->picture, member_size(User, picture), GRND_NONBLOCK);
}

// write 100k user into db
void stage_1() {
    unsigned int i = 1;

    // rewrite the current database
    // basicly clear it out
    fclose(udb);
    fclose(fopen(USER_DB_FILENAME, "wb"));
    udb = fopen(USER_DB_FILENAME, "r+b");
    if (udb == NULL) die("error opening the (%s)!", USER_DB_FILENAME);
    

    // write 100K users
    for (; i <= 100000; i++) {
        User *user = malloc(sizeof(User));
        get_user(user, i);
        fwrite(user, sizeof(User), 1, udb);
        free(user);
    }

}

// read one random user from db
void user_read(FILE *db, unsigned long id) {
    User user;

    // fseek(f, 0, SEEK_END);
    // long db_size = ftell(f);

    // printf("size: %ld | users: %ld\n", db_size, db_size / sizeof(User));

    fseek(db, sizeof(User) * (id - 1), SEEK_SET);

    fread(&user, sizeof(User), 1, db);
    // print_user(&user);
}

unsigned long user_count(FILE *db) {
    // get the current position
    long current_pos = ftell(db);
    // check if there was any error
    // in case if any error just return 0 as user count
    if (current_pos < 0)
        return 0;

    // go to end of the file
    fseek(db, 0, SEEK_END);
    // get the current position
    long db_size = ftell(db);
    // check if there was any error
    // in case if any error just return 0 as user count
    if (db_size < 0)
        return 0;
    
    fseek(db, current_pos, SEEK_SET);

    // divide the size of database to the size of a user
    return db_size / sizeof(User);
}

unsigned long user_count2(FILE *db) {
    // get the current position
    long current_pos = ftell(db);
    // check if there was any error
    // in case if any error just return 0 as user count
    if (current_pos < 0)
        return 0;
    

    // go to end of the file
    int cur = fseek(db, 0, SEEK_SET);
    // check if there was any error
    // in case if any error just return 0 as user count
    if (cur != 0)
        return 0;
    
    User user;
    unsigned long count = 0;
    
    while (fread(&user, sizeof(User), 1, db)) {
        if (user.id != 0) count++;
    }
    // for (int i = 0; i < 20; i++) {
    //     // printf("i: %d - %ld ", i, ftell(db));
    //     int s = fread(&user, sizeof(User), 1, db);
    //     // fseek(db, sizeof(User), SEEK_CUR);
    //     if (user.id != 0) count++;
    //     printf("user_id: %ld - s: %d\n", user.id, s);
    // }


    return count;
    
    
}

int main() {
    setup();
    stage_1();













    clean_up();
    return 0;
    // stage_1();
    // return 0;

    // FILE* db;
    // db = fopen(DB_FILE, "r+b");
    

    // bool x = false;
    // bool y = true;
    // SHA512(data, strlen((char *)data), hash);

    // clock_t begin = clock();

    // stage_1();

    // unsigned long gg = 0;

    // fseek(db, sizeof(User) * 2, SEEK_SET);
    // fwrite(&gg, sizeof(unsigned long), 1, db);

    // fseek(db, sizeof(User) * 6802, SEEK_SET);
    // fwrite(&gg, sizeof(unsigned long), 1, db);

    // fseek(db, sizeof(User) * 99994, SEEK_SET);
    // fwrite(&gg, sizeof(unsigned long), 1, db);


    // clock_t begin = clock();
    // user_read(db, 696969);
    // clock_t end = clock();
    // printf("user read: %ld\n", end - begin);
    

    // clock_t end = clock();

    // printf("timeit: %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);

    // double tries[10];
    // unsigned short i = 0;

    // for (; i < 10; i++) {
    //     clock_t begin = clock();
    //     unsigned long count = user_count(db);
    //     clock_t end = clock();
    //     // tries[i] = (double)(end - begin) / CLOCKS_PER_SEC;
    //     printf("try[%d]: %ld | %ld\n", i, end - begin, count);
    // }

    // clock_t begin = clock();
    // unsigned long count = user_count2(db);
    // clock_t end = clock();
    // printf("time: %ld | %ld\n", end - begin, count);

    // printf("user count %ld\n", user_count(db));
    // printf("%15d:\n", 234324);

    // printf("sizeof User: %ld\n", sizeof(User));

    // fclose(db);
}
