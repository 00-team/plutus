
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

#include "plutus.h"

#define SOCK_PATH "/tmp/plutus.sock"


void server_run(void) {
    printf("------------ SERVER RUNNING ------------\n");

    ssize_t bytes_rec = 0;

    socklen_t len;

    struct sockaddr_un server_sockaddr, client_addr;

    char buf[256];

    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, 256);

    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        die("SOCKET ERROR");
        return;
    }

    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, SOCK_PATH);

    len = sizeof(server_sockaddr);

    unlink(SOCK_PATH);

    int rc = bind(sockfd, (struct sockaddr *)&server_sockaddr, len);
    if (rc == -1) {
        die("BIND ERROR");
        close(sockfd);
        return;
    }

    while (1) {
        // bytes_rec = recv(sockfd, buf, 256, 0);
        bytes_rec = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *)&client_addr, &len);

        printf("client path: %s\n", client_addr.sun_path);
        
        if (bytes_rec < 0)
            break;
        if (bytes_rec == 0)
            continue;

        printf("\n\nbytes_rec: %ld\n", bytes_rec);
        printf("D: %s\n\n", buf);
        int res = sendto(sockfd, buf, strlen(buf)+1, 0, (struct sockaddr *)&client_addr, len);
        printf("res: %d\n", res);
    }

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
