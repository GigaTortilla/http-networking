//
// Created by Martin Appel on 17.12.24.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "include/utils.h"

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void sigchld_handler(int sig) {
    int saved_errno = errno;
    while (waitpid(-1, nullptr, WNOHANG) > 0);
    errno = saved_errno;
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