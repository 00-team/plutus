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

## Schema

```c
// 152 bytes in total
typedef struct {
    unsigned long id; // 8 bytes - 18,446,744,073,709,551,616
    unsigned short cc; // 2 bytes - country code e.g. +1
    char phone[12]; // temperary ...
    unsigned char token[64]; // sha512 hash of token
    char nickname[52];
    unsigned char picture[13];
    unsigned char ext; // type of the picture png/jpg/gif
} User;
```
