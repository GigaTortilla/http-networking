//
// Created by Martin Appel on 17.12.24.
//

#include <stdlib.h>
#include <signal.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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

