//
// Created by Martin Appel on 17.12.24.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "include/server.h"

#include <string.h>

#include "include/utils.h"

void hello_world_stream(int client_socket) {
    printf("%ld\n", (long)getpid());
    // stdout is copied to connection socket file descriptor
    // dup2(conn_fd, STDOUT_FILENO);
    if (send(client_socket, "Hello World!\n", 13, 0) == -1)
        perror("send");
    close(client_socket);
    exit(EXIT_SUCCESS);
}

void ping_pong(int client_socket) {
    printf("%d\n", getpid());
    // READ
    char buffer[BUFFER_SIZE];
    ssize_t valread = read(client_socket, buffer, BUFFER_SIZE);
    if (valread == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    // WRITE
    if (strcmp(buffer, "ping\n") == 0) {
        int valwrite = write(client_socket, "pong\n", 6);
        if (valwrite == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }
    close(client_socket);
    exit(EXIT_SUCCESS);
}