//
// Created by Martin Appel on 17.12.24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "include/server.h"
#include "include/utils.h"

void simple_http(int client_socket) {
    // Read from client socket
    char *buffer = receive_data(client_socket);
    if (!buffer) {
        perror("receive_data");
        send(client_socket, HTTP_ERROR_400, sizeof(HTTP_ERROR_400), 0);
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s", method, uri, version);
    printf("%s\n%s\n%s\n", method, uri, version);

    // remove leading forward slash from URI
    if (uri[0] == '/') memmove(uri, uri + 1, strlen(uri));
    if (strlen(uri) == 0) memmove(uri, "index.html", 11);

    if (strcmp(method, "GET") == 0) {
        long file_size = 0;
        char *file_contents = read_file(uri, &file_size);
        if (!file_contents) {
            fprintf(stderr, "Error reading file: %s\n", uri);
            send(client_socket, HTTP_ERROR_404, sizeof(HTTP_ERROR_404), 0);
            free(file_contents);
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        char *http_header = get_resp_header(uri);
        if (!http_header) {
            fprintf(stderr, "Error getting response header for: %s\n", uri);
            send(client_socket, HTTP_ERROR_400, strlen(HTTP_ERROR_400), 0);
            free(file_contents);
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        send(client_socket, http_header, strlen(http_header), 0);
        send(client_socket, file_contents, file_size, 0);
        free(file_contents);
    }

    free(buffer);
    close(client_socket);
    exit(EXIT_SUCCESS);
}

void http_hello_world(int client_socket) {
    // Read from client socket
    char *buffer = receive_data(client_socket);
    if (!buffer) {
        perror("receive_data");
        send(client_socket, HTTP_ERROR_400, sizeof(HTTP_ERROR_400), 0);
        exit(EXIT_FAILURE);
    }

    // write to client socket
    ssize_t valwrite = write(client_socket, HTTP_HELLO_WORLD, strlen(HTTP_HELLO_WORLD));
    if (valwrite < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    free(buffer);
    close(client_socket);
    exit(EXIT_SUCCESS);
}

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
    if (valread < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    // WRITE
    if (strcmp(buffer, "ping\n") == 0) {
        ssize_t valwrite = write(client_socket, "pong\n", 6);
        if (valwrite < 0) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }
    close(client_socket);
    exit(EXIT_SUCCESS);
}