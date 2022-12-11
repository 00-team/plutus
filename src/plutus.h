#include <stdio.h>

// user database file stream

#define USER_DB_FILENAME "user.bin"
#define member_size(type, member) sizeof(((type*)0)->member)


// setup
void setup(void);
void clean_up(void);


// utils
void die(const char* message, ...);