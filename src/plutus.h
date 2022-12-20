
#define USER_DB_FILENAME  "data/user.bin"
#define PHONE_DB_FILENAME "data/phone.bin"
#define INDEX_DB_FILENAME "data/phone_index.bin"



// enum {
//     EXT_JPG,
//     EXT_PNG,
//     EXT_GIF,
// };

// setup
void setup_files(void);
void clean_up_files(void);

// utils
void die(const char *message, ...);
long fsize(FILE *f);


// components
void server_run(void);

void phone_setup(void);
void user_setup(void);
