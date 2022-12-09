
INCS = -I /usr/include/openssl
LIBS = -lcrypto 
CFLAGS = -pedantic -Wall -Wno-deprecated-declarations -Os ${INCS} -Wextra -Werror ${LIBS}

main:
	@cc main.c -o plutus ${CFLAGS}
