
#ifndef __PLUTUS_H__
#define __PLUTUS_H__

#include <stdbool.h>

#define USER_DB_FILENAME  "data/user.bin"
#define PHONE_DB_FILENAME "data/phone.bin"
#define ADMIN_DB_FILENAME "data/admin.bin"


// utils
bool setup_files(void);
void clean_up_files(void);
void cleanup_handler(int signum);
long fsize(FILE *f);


// components
void server_run(void);

void phone_setup(void);
void user_setup(void);

void logger_setup(void);
void logger_cleanup(void);

#endif // __PLUTUS_H__
