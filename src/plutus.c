
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "plutus.h"
#include "user/phone.h"


int main() {
    setup_files();
    user_setup();
    phone_setup();

    server_run();

    clean_up_files();
    return 0;

    // clock_t begin = clock();
    // user_read(696969);
    // clock_t end = clock();
    // printf("user read: %ld\n", end - begin);
}
