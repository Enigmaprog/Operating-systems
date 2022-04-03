#include <stdio.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "params.h"

#define MAX_CLIENTS 5
#define SOCKET_ERR -1
#define BIND_ERR -2
#define LISTEN_ERR -3
#define ACCEPT_ERR -4
#define SELECT_ERR -5

int clients[MAX_CLIENTS] = {0};

int set_connection(const int fd) {
    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);
    int confd = accept(fd, (struct sockaddr*)&client_addr, (socklen_t *)&client_addr_size);
    if (confd < 0) {
        printf("Accept error\n");
        return ACCEPT_ERR;
    }

    printf("New connection: FD = %d, IP = %s:%d\n", confd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    bool flag = true;
    for (int i = 0; flag && i < MAX_CLIENTS; i++) {
        if (clients[i] == 0) {
            clients[i] = confd;
            printf("Connected client %d\n", i);
            flag = false;
        }
    }
    return 0;
}


void get_client_info(const int fd, const int client_id) {
    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);

    char msg[MSG_SIZE];
    int sz = recv(fd, msg, MSG_SIZE, 0);
    if (sz == 0) {
        getpeername(fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_size);
        printf("Disconnected: %d, IP = %s:%d\n", client_id, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        close(fd);
        clients[client_id] = 0;
    }
    else {
        msg[sz] = 0;
        printf("Client %d sent: %s\n", client_id, msg);
    }
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket error\n");
        return SOCKET_ERR;
    }
    struct sockaddr_in client_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SOCKET_PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    if (bind(sockfd, (struct sockaddr*) &client_addr, sizeof(client_addr)) < 0) {
        printf("Bind error\n");
        return BIND_ERR;
    }

    printf("Server %d started successfully, port: %d\n", getpid(), SOCKET_PORT);

    if (listen(sockfd, MAX_CLIENTS) < 0) {
        printf("Listen error\n");
        return LISTEN_ERR;
    }

    while(1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        
        int max_fd = sockfd;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = clients[i];
            if (fd > 0)
                FD_SET(fd, &readfds);
            if (fd > max_fd)
                max_fd = fd;
        }
        
        int active = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        
        if ((active < 0) && (errno != EINTR)) {
            printf("Select error\n");
            return SELECT_ERR;
        }
        if (FD_ISSET(sockfd, &readfds))
            set_connection(sockfd);
        
        
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = clients[i];
            if ((fd > 0) && FD_ISSET(fd, &readfds))
                get_client_info(fd, i);
        }
    }
    
    return 0;
}