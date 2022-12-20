
#define USER_PICTURE_SIZE 4
#define USER_NICNAME_SIZE 50
#define USER_TOKEN_SIZE 64

typedef unsigned long user_id_t;

// 134 byte
typedef struct {
    unsigned short cc;
    char phone[12];
    unsigned char flag;  // DELETED_FLAG or anything else
    unsigned char ext;
    unsigned char picture[USER_PICTURE_SIZE];
    char token[USER_TOKEN_SIZE];
    char nickname[USER_NICNAME_SIZE];
} User;

void user_get(char *request, char *response);
void user_count(char *request, char *response);
