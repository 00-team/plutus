
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include "plutus.h"
#include "logger.h"

#define LOG_SCTOR SECTOR_MAIN


int main() {

    signal(SIGINT, cleanup_handler);


    logger_setup();
    if (setup_files()) {
        log_error("exiting due to an error while setting up the files");
        return EXIT_FAILURE;
    }

    user_setup();
    phone_setup();

    server_run();

    clean_up_files();
    logger_cleanup();
    return EXIT_SUCCESS;

    // clock_t begin = clock();
    // user_read(696969);
    // clock_t end = clock();
    // printf("user read: %ld\n", end - begin);
}
