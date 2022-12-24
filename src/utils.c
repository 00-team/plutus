
#include <stdio.h>

#include "plutus.h"


FILE *file_open(char *filename);


extern FILE *udb;
extern FILE *phdb;

void setup_files(void) {
    udb  = file_open(USER_DB_FILENAME);
    phdb = file_open(PHONE_DB_FILENAME);
}

void clean_up_files(void) {
    if (udb  != NULL) fclose(udb);
    if (phdb != NULL) fclose(phdb);
}

FILE *file_open(char *filename) {
    FILE *f;

    f = fopen(filename, "ab+");

    if (f == NULL) {
        printf("error while opening (%s) with ab+ flag", filename);
        return NULL;
    }

    if (fclose(f) != 0) {
        printf("error while closing (%s) with ab+ flag", filename);
        return NULL;
    }

    f = fopen(filename, "rb+");
    if (f == NULL) {
        printf("error while opening (%s) with rb+ flag", filename);
        return NULL;
    }

    return f;
}

long fsize(FILE *f) {
    fseek(f, 0, SEEK_END);
    return ftell(f);
}
