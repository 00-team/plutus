
VERSION = 0.1.0
CC = cc

# includes and libs
INCS = -I/usr/include/openssl
LIBS = -lcrypto

# flags
LDFLAGS  = $(LIBS)
CFLAGS   =  -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Wextra -Werror \
			-Os -DVERSION=\"$(VERSION)\" $(INCS)


USER = $(addprefix user/, user phone)
FILES = plutus setup utils server $(USER)
HEADERS = src/plutus.h
SRC = $(addprefix ./src/, $(addsuffix .c, $(FILES)))
OBJ = $(addprefix ./build/, $(addsuffix .o, $(FILES)))


build/%.o: src/%.c dirs $(HEADERS)
	@$(CC) -c $(CFLAGS) $< -o $@


plutus: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)
	rm -rf build


dirs:
	mkdir -p data build/user


dev: plutus
	printf "\E[H\E[3J"
	clear
	./plutus



.PHONY: dirs dev
.SILENT: dirs dev plutus
.DEFAULT: plutus
