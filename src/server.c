
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

#include "plutus.h"
#include "user.h"
#include "admin.h"


#define SOCK_PATH "/tmp/plutus.server.sock"

#define RMDS sizeof(ResponseMetaData)

const ResponseMetaData INVALID_REQUEST_ARGS = { 403, 0 };
const ResponseMetaData REQUEST_NOT_FOUND    = { 404, 0 };

static const API apis[] = {
    /*                                   args size               */
    [RQT_USER_GET]    = { user_get,      sizeof(user_id_t)        },
    [RQT_USER_COUNT]  = { user_count,    sizeof(bool)             },
    [RQT_USER_LOGIN]  = { user_login,    sizeof(UserLoginArgs)    },
    [RQT_USER_UPDATE] = { user_update,   sizeof(UserUpdateArgs)   },

    [RQT_ADMIN_GET]   = { admin_get,     sizeof(user_id_t)        },
};


void server_run(void) {
    printf("------------ SERVER RUNNING ------------\n");

    int request_size = 0;
    Request request;
    Response response;
    API route;

    struct sockaddr_un server_addr;
    struct sockaddr ca;
    socklen_t cal = sizeof(struct sockaddr_un);

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
        request_size = recvfrom(sockfd, &request, sizeof(request), 0, &ca, &cal);

        if (request_size < 0) {
            printf("error receiving bytes!\n");
            break;
        }

        // debug
        if (request_size == 0) break;

        // invalid request
        if (request.type >= RQT_LENGTH) {
            sendto(sockfd, &REQUEST_NOT_FOUND, RMDS, 0, &ca, cal);
            continue;
        }
        
        printf("request_size: %d\n", request_size);
        printf("request.type: %d\n", request.type);

        route = apis[request.type];

        // invalid request
        if (route.args_size != request_size - sizeof(request.type)) {
            sendto(sockfd, &INVALID_REQUEST_ARGS, RMDS, 0, &ca, cal);
            continue;
        }

        route.func(request.data, &response);

        if (sendto(sockfd, &response, RMDS + response.md.size, 0, &ca, cal) == -1) {
            printf("ERROR SENDING RESPONSE!\n");
        }
    }

    close(sockfd);
    unlink(SOCK_PATH);
}
