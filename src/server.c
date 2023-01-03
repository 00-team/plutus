
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

#include "plutus.h"
#include "user.h"
#include "admin.h"
#include "logger.h"

#define LOG_SCTOR SECTOR_SERVER
#define SOCK_PATH "/tmp/plutus.server.sock"
#define RMDS sizeof(ResponseMetaData)

const ResponseMetaData INVALID_REQUEST_ARGS = { 400, 0 };
const ResponseMetaData REQUEST_NOT_FOUND    = { 404, 0 };

static const API apis[] = {
    /*                                         args size               */
    [RQT_USER_GET]       = { user_get,      sizeof(user_id_t)           },
    [RQT_USER_COUNT]     = { user_count,    sizeof(bool)                },
    [RQT_USER_LOGIN]     = { user_login,    sizeof(UserLoginArgs)       },
    [RQT_USER_UPDATE]    = { user_update,   sizeof(UserUpdateArgs)      },

    [RQT_USERS_GET]      = { users_get,     sizeof(uint32_t)            },

    [RQT_ADMIN_GET]      = { admin_get,     sizeof(user_id_t)           },
    [RQT_ADMIN_ADD]      = { admin_add,     sizeof(Admin)               },
    [RQT_ADMIN_UPDATE]   = { admin_update,  sizeof(AdminUpdateArgs)     },
};


void server_run(void) {
    log_info("server is running");

    int request_size = 0;
    Request request;
    Response response;
    API route;

    struct sockaddr_un server_addr;
    struct sockaddr ca;
    socklen_t cal = sizeof(struct sockaddr_un);

    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        log_error("socket error!");
        return;
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCK_PATH);
    unlink(SOCK_PATH);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        log_error("socket bind error");
        close(sockfd);
        return;
    }

    while (1) {
        request_size = recvfrom(sockfd, &request, sizeof(request), 0, &ca, &cal);

        if (request_size < 0) {
            log_error("error receiving bytes!");
            break;
        }

        // debug
        if (request_size == 0) break;

        // invalid request
        if (request.type >= RQT_LENGTH) {
            log_info("invalid request type: %d", request.type);
            sendto(sockfd, &REQUEST_NOT_FOUND, RMDS, 0, &ca, cal);
            continue;
        }
        
        log_info("request size: %d", request_size);
        log_info("request type: %d", request.type);

        route = apis[request.type];

        // invalid request
        if (route.args_size != request_size - sizeof(request.type)) {
            log_error(
                "invalid request args: %u / %u", 
                route.args_size, request_size - sizeof(request.type)
            );
            sendto(sockfd, &INVALID_REQUEST_ARGS, RMDS, 0, &ca, cal);
            continue;
        }

        route.func(request.data, &response);

        log_info("response size  : %d", response.md.size);
        log_info("response status: %d", response.md.status);

        if (sendto(sockfd, &response, RMDS + response.md.size, 0, &ca, cal) == -1) {
            log_error(
                "error sending the response to: %s", 
                ((struct sockaddr_un *)&ca)->sun_path
            );
        }
    }

    close(sockfd);
    unlink(SOCK_PATH);
}
