//
// Created by Martin Appel on 17.12.24.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#ifdef _WIN32

#include <WinSock2.h>

#elif defined(__linux__) || defined(__unix__) || defined(__unix) || defined(unix) \
    || defined(__APPLE__) && defined(__MACH__)

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#endif

// additional project specific header files
#include "include/utils.h"
#include "include/server.h"

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void *get_in_port(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_port);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_port);
}

void sigchld_handler(int sig) {
    int saved_errno = errno;
    while (waitpid(-1, nullptr, WNOHANG) > 0);
    errno = saved_errno;
}

char *get_resp_header(const char *uri) {
    // HTTP header specific to requested file extension
    char *file_extension = strrchr(uri, '.');
    if (!file_extension)
        return nullptr;

    if (strcmp(file_extension, ".html") == 0)
        return HTTP_HEADER_HTML;
    if (strcmp(file_extension, ".css") == 0)
        return HTTP_HEADER_CSS;
    if (strcmp(file_extension, ".png") == 0)
        return HTTP_HEADER_PNG;
    if (strcmp(file_extension, ".jpeg") == 0)
        return HTTP_HEADER_JPEG;
    if (strcmp(file_extension, ".gif") == 0)
        return HTTP_HEADER_GIF;
    if (strcmp(file_extension, ".ico") == 0)
        return HTTP_HEADER_ICO;
    if (strcmp(file_extension, ".js") == 0)
        return HTTP_HEADER_JS;
    return nullptr;
}

void *check_mem(void *ptr) {
    if (!ptr) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

int get_socket_bind(struct addrinfo *addr) {
    struct addrinfo *p;
    int server_socket;
    int yes = 1;

    for (p = addr; p != NULL; p = p->ai_next) {
        if ((server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(server_socket);
            perror("bind");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Unable to bind\n");
        exit(EXIT_FAILURE);
    }

    return server_socket;
}

char *receive_data(int sockfd) {
    char *buf;
    for (int attempt = 1; MSG_BUFFER_SIZE * attempt <= MAX_BUFFER_SIZE; attempt++) {
        buf = check_mem(malloc(sizeof(char) * MSG_BUFFER_SIZE));
        ssize_t bytes_received = recv(sockfd, buf, MSG_BUFFER_SIZE, 0);
        if (bytes_received >= 0) {
            buf[bytes_received] = '\0';
            break;
        }
        free(buf);
        buf = nullptr;
    }
    return buf;
}

char *read_file(const char *path, long *size) {
    FILE *file = fopen(path, "rb");
    if (file == nullptr) return nullptr;
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);
    char *data = check_mem(malloc(sizeof(char) * (*size + 1)));
    fread(data, 1, *size, file);
    data[*size] = '\0';
    fclose(file);
    return data;
}