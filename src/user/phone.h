
#ifndef USER_PHONE_H
#define USER_PHONE_H

#include <stddef.h>
#include <stdbool.h>
#include "user.h"

// digits 0-9
#define NODE_ABC 10

// this two compine must be 9
// 09xx-xxx-xxxx
#define CACHE_LVL 1
// cache size is 10 ** CACHE_LVL
// 3: 10 x 10 x 10
#define CACHE_SIZ 10
#define LINKS_LEN 9 - CACHE_LVL

// link is a number between 0 and 9
typedef unsigned char link_t;
// Node is a sequence of positions in the file to other nodes
// 0: pos of a node
// 1: empty
// ...
// 9: pos of a node
typedef size_t Node[NODE_ABC];
// links is a sequence of numbers between 0 and 9
// e.g.: 0 9 1 2 2 2
// which is the same as the phone number
typedef link_t Links[LINKS_LEN];

// phone struct
typedef struct {
    size_t index;
    Links links;
} Phone;


Phone phone_convert(char *phone_number);
bool phone_update(Phone *phone, user_id_t value);
user_id_t phone_search(Phone *phone);

#endif // USER_PHONE_H
