
INCS = -I /usr/include/openssl
LIBS = -lcrypto 
CFLAGS = -pedantic -Wall -Wno-deprecated-declarations -Wextra -Werror -Os ${INCS} ${LIBS}

main:
	@cc main.c -o plutus ${CFLAGS}
