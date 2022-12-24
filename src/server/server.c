
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
#include "../user/user.h"


#define SOCK_PATH "/tmp/plutus.server.sock"

static const API apis[] = {
    [RQT_USER_GET] = { user_get, 1 + sizeof(User) },
    [RQT_USER_COUNT] = { user_count, sizeof(user_id_t) },
    [RQT_USER_LOGIN] = { user_login, sizeof(UserLoginResponse) },
    [RQT_USER_UPDATE] = { user_update, 1 },
};


void server_run(void) {
    printf("------------ SERVER RUNNING ------------\n");

    ssize_t rq_length = 0;
    RequestData request;
    Response response;

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
            sockfd, &request, sizeof(request), 0,
            (struct sockaddr *)&client_addr, &addr_len
        );

        if (rq_length < 0) {
            printf("error receiving bytes!\n");
            break;
        }

        // debug
        if (rq_length == 0) break;

        // invalid request
        if (rq_length < MIN_REQUEST_SIZE || request.type >= RQT_LENGTH)
            continue;
        
        API api = apis[request.type];
        api.func(request.data, response);

        if (sendto(
            sockfd, response, api.rs_size, 0,
            (struct sockaddr *)&client_addr, addr_len
        ) == -1) {
            printf("ERROR SENDING RESPONSE!\n");
        }
    }

    close(sockfd);
    unlink(SOCK_PATH);
}
