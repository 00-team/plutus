#include <stdio.h>

// user database file stream

#define USER_DB_FILENAME "user.bin"
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
void die(const char* message, ...);
