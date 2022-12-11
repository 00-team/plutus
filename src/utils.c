
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "plutus.h"


char *argv0;

static void verr(const char *fmt, va_list ap) {
    if (argv0 && strncmp(fmt, "usage", sizeof("usage") - 1)) {
        fprintf(stderr, "%s: ", argv0);
    }

    vfprintf(stderr, fmt, ap);

    if (fmt[0] && fmt[strlen(fmt) - 1] == ':') {
        fputc(' ', stderr);
        perror(NULL);
    } else {
        fputc('\n', stderr);
    }
}

void die(const char *message, ...) {
    va_list ap;

    va_start(ap, message);
    verr(message, ap);
    va_end(ap);

    exit(1);
}
