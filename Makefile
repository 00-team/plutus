# plutus version
VERSION = 0.1.0
# C Compiler
CC = cc

# includes and libs
INCS = -I/usr/include/openssl
LIBS = -lcrypto

# flags
LDFLAGS  = ${LIBS}
CFLAGS   =  -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Wextra -Werror \
			-Os -DVERSION=\"${VERSION}\" ${INCS}


FILES = plutus setup utils components/trie
SRC = $(addprefix ./src/, $(addsuffix .c, $(FILES)))
OBJ = $(addprefix ./build/, $(addsuffix .o, $(FILES)))

all: plutus

build/%.o: src/%.c src/plutus.h build
	@${CC} -c ${CFLAGS} $< -o $@

plutus: ${OBJ}
	@${CC} -o $@ ${OBJ} ${LDFLAGS}
	@rm -rf build

build:
	@mkdir -p build/components

.PHONY: all
