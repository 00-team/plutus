
#include <stdio.h>

#include "plutus.h"


FILE *file_open(char *filename);


extern FILE *udb;
extern FILE *phdb;
extern FILE *pidb;


void setup_files(void) {
    udb  = file_open(USER_DB_FILENAME);
    phdb = file_open(PHONE_DB_FILENAME);
    pidb = file_open(INDEX_DB_FILENAME);
}

void clean_up_files(void) {
    if (udb  != NULL) fclose(udb);
    if (phdb != NULL) fclose(phdb);
    if (pidb != NULL) fclose(pidb);
}

FILE *file_open(char *filename) {
    FILE *f;

    f = fopen(filename, "ab+");
    if (f == NULL) die("error while opening (%s) with ab+ flag", filename);
    if (fclose(f) != 0) die("error while closing (%s) with ab+ flag", filename);

    f = fopen(filename, "rb+");
    if (f == NULL) die("error while opening (%s) with rb+ flag", filename);

    return f;
}
