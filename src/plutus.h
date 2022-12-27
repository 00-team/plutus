
#ifndef PLUTUS_H
#define PLUTUS_H

#define USER_DB_FILENAME  "data/user.bin"
#define PHONE_DB_FILENAME "data/phone.bin"
#define ADMIN_DB_FILENAME "data/admin.bin"


// utils
void setup_files(void);
void clean_up_files(void);
long fsize(FILE *f);


// components
void server_run(void);

void phone_setup(void);
void user_setup(void);

#endif // PLUTUS_H
