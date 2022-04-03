#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include "params.h"

#define MSG_COUNT 3
#define MAX_SLEEP_TIME 3
#define SOCKET_ERR -1
#define CONNECT_ERR -2
#define SEND_ERR -3

int main() {
    srand(time(NULL));

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket error\n");
        return SOCKET_ERR;
    }
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SOCKET_PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Socket error\n");
        return CONNECT_ERR;
    }
    char msg[MSG_SIZE];
    for (int i = 0; i < MSG_COUNT; i++) {
        sprintf(msg, "pid %d msg %d", getpid(), i);
        if (send(sockfd, msg, strlen(msg), 0) < 0) {
            printf("Send error\n");
        return SEND_ERR;
        }

        printf("Sent message: %s\n", msg);
        sleep(1 + rand() % MAX_SLEEP_TIME);
    }
    printf("Client end\n");
    return 0;
}