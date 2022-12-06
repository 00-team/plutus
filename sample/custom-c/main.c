
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

typedef struct {
    unsigned long id;
    unsigned short cc;
    char phone[12];
    unsigned char token[64];
    char nickname[52];
    char picture[14];
} User;


// 8 + 2 + 12 + 64 + 52 + 14 = 152

void print_user(User *user) {
    printf("----------------------\n");
    printf("id: %ld\ncc: %d\nphone: %s\n", user->id, user->cc, user->phone);
    printf("token: ");

    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
        printf("%02x", user->token[i]);
    
    // printf("\nnickname: %s\npicture: %s\n", user->nickname, user->picture);
    printf("\nnickname: %s\npicture: ", user->nickname);

    for (int i = 0; i < 14; i++)
        printf("%02x", user->picture[i]);

    printf("\n----------------------\n");
}

// void sha512(unsigned char *data) {
//     // unsigned char data[] = "some text";
//     unsigned char hash[SHA512_DIGEST_LENGTH];
//     SHA512(data, strlen((char *)data), hash);

//     for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
//         printf("%02x", hash[i]);
//     }
//     printf("\n");
// }

void stage_1() {
    FILE *f;
    unsigned int i = 1;

    unsigned char hash[SHA512_DIGEST_LENGTH];
    unsigned char data[] = "A_TOKNE";
    SHA512(data, strlen((char *)data), hash);

    User user;
    user.id = 1;
    user.cc = 98;

    memcpy(user.phone, "09440001122", 12);
    memcpy(user.token, hash, SHA512_DIGEST_LENGTH);
    memcpy(user.nickname, "nickname gg ez", 15);
    // memcpy(user.picture, "aGGGGGGGGGGGGG", 14);
    memset(user.picture, 71, 14);
    

    // printf("sizeof user: %ld\n", sizeof(user));
    // print_user(&user);

    f = fopen("./test.bin", "wb");
    

    for (; i <= 100001; i++) {
        fwrite(&user, sizeof(User), 1, f);
    }
    


    fclose(f);
}

int main() {
    stage_1();

    return 0;
}
