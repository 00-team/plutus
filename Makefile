
VERSION = 0.1.0
CC = cc

# includes and libs
INCS = -Isrc/include # -I/usr/include/openssl
LIBS = # -lcrypto

# flags
LDFLAGS  = $(LIBS)
CFLAGS   = -pedantic -Wall -Wno-deprecated-declarations -Wextra -Werror \
			-Os -DVERSION=\"$(VERSION)\" $(INCS)


FILES = plutus utils item server admin logger \
		user/user user/phone

HEADERS = $(wildcard ./src/include/*.h)
SRC = $(addprefix ./src/, $(addsuffix .c, $(FILES)))
OBJ = $(addprefix ./build/, $(addsuffix .o, $(FILES)))

build/%.o: src/%.c dirs $(HEADERS)
	@$(CC) -c $(CFLAGS) $< -o $@

plutus: clear $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)
	rm -rf build

dirs:
	mkdir -p data build/user

clear:
	printf "\E[H\E[3J"
	clear

dev: clear plutus
	./plutus


.PHONY: dirs dev clear
.SILENT: dirs dev clear plutus
.DEFAULT: plutus
