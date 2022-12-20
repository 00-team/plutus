
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

#include "../plutus.h"
#include "api.h"

#define SOCK_PATH "/tmp/plutus.server.sock"


void server_run(void) {
    printf("------------ SERVER RUNNING ------------\n");

    ssize_t rq_length = 0;
    Request buffer = malloc(MAX_REQUEST_SIZE);
    char response[MAX_RESPONSE_SIZE];

    struct sockaddr_un server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("SOCKET ERROR\n");
        return;
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCK_PATH);
    unlink(SOCK_PATH);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("BIND ERROR\n");
        close(sockfd);
        return;
    }

    while (1) {
        rq_length = recvfrom(
            sockfd, buffer, MAX_REQUEST_SIZE, 0,
            (struct sockaddr *)&client_addr, &addr_len
        );

        if (rq_length < 0) {
            printf("error receiving bytes!\n");
            break;
        }

        unsigned short RQT = *(unsigned short *)buffer;

        // invalid request
        if (rq_length < MIN_REQUEST_SIZE || RQT >= RQT_LENGTH)
            continue;
        
        API api = apis[RQT];
        api.func(&buffer[2], response);

        if (sendto(
            sockfd, response, api.rs_size, 0,
            (struct sockaddr *)&client_addr, addr_len
        ) == -1) {
            printf("ERROR SENDING RESPONSE!\n");
        }
    }

    free(buffer);
    close(sockfd);
    unlink(SOCK_PATH);
}

/*

request

----- 2 byte id -----
0 get_user_get
1 get_user_post
2 get_user_put
....
20 get_eatery
...
-----

response


*/
