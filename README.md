# Plutus

a database

## Roadmap

* [x] define the fix table schema\
   user id | phone | nickname | picture | token

## Notos

1. database should be able to get/delete/insert/update and count the data

1. files should olny be stored as names and one byte for there extention

1. test must be a realistic scenario (random read/write in this case)

1. flag the users for deleation and keep a list of them in to registery
   so the new user will be replacing the old ones

1. about tries:\
   first we need a dynamic length keys\
   then we need dynamic caching of the first few layers\
   read the first layers from the file and thats it we dont even need to update them

## Schema

```c
// 152 byte
typedef struct {
    user_id_t id;
    unsigned short cc;
    char phone[12];
    unsigned char token[64];
    char nickname[52];
    unsigned char picture[13];
    unsigned char ext;
} OldUser;

// 128 byte
typedef struct {
    unsigned long phone;
    unsigned char token[64];
    unsigned char picture[4];
    char nickname[50];
    unsigned char flag; // USER_DELETED_FLAG or anything else
    unsigned char ext;
} User;

```

## Todos

* [ ] user_id + some random data = user_pic
* [x] find and implement the best data structure for phone number indexing: Tries
* [ ] test all native types to ensure that all the types sizes are as expected\
      for example in my machine type `long` is `8` byte but the standard says that its `4` byte
* [ ] a visual representation for the phone index
* [ ] check if the user picture is all null or not
      if it is get random bytes again
