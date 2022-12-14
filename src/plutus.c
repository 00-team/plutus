
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "plutus.h"


FILE *udb = NULL;
const char DELETED_FLAG = 'D';

void trie(void);

int main() {
    setup();
    trie();

    // write_n_users(1000);

    clean_up();
    return 0;

    // clock_t begin = clock();
    // user_read(696969);
    // clock_t end = clock();
    // printf("user read: %ld\n", end - begin);
}
