
VERSION = 0.1.0
CC = cc

# includes and libs
INCS = -I/usr/include/openssl
LIBS = -lcrypto

# flags
LDFLAGS  = $(LIBS)
CFLAGS   =  -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Wextra -Werror \
			-Os -DVERSION=\"$(VERSION)\" $(INCS)


FILES = plutus setup utils \
		server/server \
		user/user user/phone

HEADER = plutus user/user user/phone server/api

HEADERS = $(addprefix ./src/, $(addsuffix .h, $(HEADER)))
SRC = $(addprefix ./src/, $(addsuffix .c, $(FILES)))
OBJ = $(addprefix ./build/, $(addsuffix .o, $(FILES)))


build/%.o: src/%.c dirs $(HEADERS)
	@$(CC) -c $(CFLAGS) $< -o $@

plutus: clear $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)
	rm -rf build

dirs:
	mkdir -p data build/user
	mkdir -p build/server

clear:
	printf "\E[H\E[3J"
	clear

dev: clear plutus
	./plutus

test: clear
	python scripts/client.py


.PHONY: dirs dev test clear
.SILENT: dirs dev test clear plutus
.DEFAULT: plutus
