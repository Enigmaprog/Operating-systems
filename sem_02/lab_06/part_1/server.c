#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include "params.h"

#define SOCKET_ERR -1
#define BIND_ERR -2
#define RECV_ERR -3

bool sigint_flag = false;
int sockfd = 0;

void close_socket(const int sockfd, const char *socket_name) {
    close(sockfd);
    unlink(socket_name);
}

void sigint_handler(int signum) {
    close_socket(sockfd, SOKET_NAME);
    printf("Socked closed, signum = %d\n", signum);
    sigint_flag = true;
}

int main() {
    sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Socket error\n");
        return SOCKET_ERR;
    }
    struct sockaddr client_addr = {
        .sa_family = AF_LOCAL,
        .sa_data = SOKET_NAME
    };
    if (bind(sockfd, &client_addr, sizeof(client_addr)) < 0) {
        close_socket(sockfd, SOKET_NAME);
        printf("Bind error\n");
        return BIND_ERR;
    }

    printf("Server %d started successfully\n", getpid());
    signal(SIGINT, sigint_handler);

    char msg[MSG_SIZE];
    for (int i = 0;; i++) {
        int sz = recv(sockfd, msg, sizeof(msg), 0);
        if (sigint_flag) {
            return 0;
        }
        else if (sz < 0) {
            close_socket(sockfd, SOKET_NAME);
            printf("Socket closed. RECV error\n");
            return RECV_ERR;
        }
        msg[sz] = 0; //EOL
        printf("Client %d sent: %s\n", i, msg);
    }

    close_socket(sockfd, SOKET_NAME);
    printf("Socket closed.\n");
    return 0;
}