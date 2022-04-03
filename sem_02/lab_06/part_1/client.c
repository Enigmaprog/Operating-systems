#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "params.h"

#define SOCKET_ERR -1

int main() {
    int sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Socket error\n");
        return SOCKET_ERR;
    }
    struct sockaddr server_addr = {
        .sa_family = AF_LOCAL,
        .sa_data = SOKET_NAME
    };
    char msg[MSG_SIZE];
    sprintf(msg, "pid %d", getpid());
    sendto(sockfd, msg, strlen(msg), 0, &server_addr,
    sizeof(server_addr));
    printf("Sent message: %s\n", msg);
    return 0;
}