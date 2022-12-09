
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/sha.h>
#include <sys/random.h>

#define DB_FILE "database.bin" 
#define member_size(type, member) sizeof(((type *)0)->member)

typedef struct {
    unsigned long id;
    unsigned short cc;
    char phone[12];
    unsigned char token[64];
    char nickname[52];
    unsigned char picture[13];
    unsigned char ext;
} User;


unsigned char hash[SHA512_DIGEST_LENGTH];
unsigned char data[] = "A_TOKNE";

// 8 + 2 + 12 + 64 + 52 + 13 + 1 = 152

void print_user(User *user) {
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
User* get_user(unsigned long user_id) {
    User *user = malloc(sizeof(User));
    
    user->id = user_id;
    user->cc = 98;

    memcpy(user->phone, "09440001122", 12);
    memcpy(user->token, hash, SHA512_DIGEST_LENGTH);
    memcpy(user->nickname, "nickname gg ez", 15);
    // memcpy(user.picture, "aGGGGGGGGGGGGG", 14);
    // memset(user.picture, 71, 14);
    getrandom(user->picture, member_size(User, picture), GRND_NONBLOCK);

    return user;
}


// write 100k user into db
void stage_1() {
    FILE *f;
    unsigned int i = 1;

    f = fopen(DB_FILE, "wb");

    // write 100K users
    for (; i <= 100000; i++) {
        User *user = get_user(i);
        fwrite(user, sizeof(User), 1, f);
        free(user);
    }

    fclose(f);
}

// read one random user from db
void stage_2() {
    FILE *f;
    User user;

    f = fopen(DB_FILE, "rb");

    // fseek(f, 0, SEEK_END);
    // long db_size = ftell(f);

    // printf("size: %ld | users: %ld\n", db_size, db_size / sizeof(User));


    fseek(f, sizeof(User) * (696969 - 1), SEEK_SET);

    fread(&user, sizeof(User), 1, f);
    print_user(&user);

    fclose(f);
}

int main() {
    SHA512(data, strlen((char *)data), hash);

    clock_t begin = clock();

    stage_1();
    stage_2();

    clock_t end = clock();

    printf("timeit: %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);


    // printf("sizeof User: %ld\n", sizeof(User));

    return 0;
}
