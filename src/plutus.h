#include <stdio.h>


#define USER_DB_FILENAME  "data/user.bin"
#define PHONE_DB_FILENAME "data/phone.bin"
#define INDEX_DB_FILENAME "data/phone_indexe.bin"

#define USER_PICTURE_SIZE 4
#define USER_NICNAME_SIZE 50

typedef unsigned long user_id_t;

enum {
    EXT_JPG,
    EXT_PNG,
    EXT_GIF,
};

// setup
void setup(void);
void clean_up(void);

// utils
void die(const char *message, ...);
long fsize(FILE *f);
