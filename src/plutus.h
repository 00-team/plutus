
#ifndef PLUTUS_H
#define PLUTUS_H

#define USER_DB_FILENAME  "data/user.bin"
#define PHONE_DB_FILENAME "data/phone.bin"



// enum {
//     EXT_JPG,
//     EXT_PNG,
//     EXT_GIF,
// };

// utils
void setup_files(void);
void clean_up_files(void);
long fsize(FILE *f);


// components
void server_run(void);

void phone_setup(void);
void user_setup(void);

#endif // PLUTUS_H
