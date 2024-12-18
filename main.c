#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>
#include "include/utils.h"
#include "include/server.h"

int main(void) {
    int conn_fd, status;
    struct addrinfo hints, *res;
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    struct sigaction action;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(nullptr, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return EXIT_FAILURE;
    }

    // SOCKET and BIND
    int server_fd = get_socket_bind(res);

    // LISTEN
    if (listen(server_fd, BACKLOG) == -1) {
        perror("listen");
        return EXIT_FAILURE;
    }

    // destroying child processes using handler function
    action.sa_handler = sigchld_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &action, nullptr) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    while (1) {
        char addr[INET6_ADDRSTRLEN];
        char port[64];
        // ACCEPT connection
        sin_size = sizeof client_addr;
        if ((conn_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size)) == -1) {
            perror("accept");
            continue;
        }

        printf("Connection accepted\n");
        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr*)&client_addr), addr, sizeof addr);
        inet_ntop(client_addr.ss_family, get_in_port((struct sockaddr*)&client_addr), port, sizeof port);
        printf("[%s:%s]\n", addr, port);

        // child process
        if (!fork()) {
            close(server_fd);
            http_hello_world(conn_fd);
        }
        // parent process does not need the connected socket
        close(conn_fd);
    }
}