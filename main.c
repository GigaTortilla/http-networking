#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/errno.h>

#define PORT "8080"
#define BUFSIZE 1024
#define BACKLOG 5

void sigint_handler(int sig);
void *get_in_addr(struct sockaddr *sa);

int main(void) {
    int server_fd, conn_fd, status;
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    struct sigaction action;
    char s[INET6_ADDRSTRLEN];
    int yes = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(nullptr, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return EXIT_FAILURE;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if ((server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
            perror("setsockopt");
            return EXIT_FAILURE;
        }
        if (bind(server_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(server_fd);
            perror("bind");
            continue;
        }
        break;
    }
    freeaddrinfo(res);

    if (p == NULL) {
        fprintf(stderr, "Unable to bind\n");
        return EXIT_FAILURE;
    }
    if (listen(server_fd, BACKLOG) == -1) {
        perror("listen");
        return EXIT_FAILURE;
    }

    action.sa_handler = sigint_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &action, nullptr) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    while (1) {
        sin_size = sizeof(client_addr);
        if ((conn_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size)) == -1) {
            perror("accept");
            continue;
        }

        printf("Connection accepted\n");
        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr*)&client_addr), s, sizeof s);
        printf("%s\n", s);

        if (!fork()) {
            close(server_fd);
            printf("%ld\n", (long)getpid());
            // stdout is copied to connection socket file descriptor
            // dup2(conn_fd, STDOUT_FILENO);
            if (send(conn_fd, "Hello World!\n", 13, 0) == -1)
                perror("send");
            close(conn_fd);
            return EXIT_SUCCESS;
        }
        close(conn_fd);
    }
}

void sigint_handler(int sig) {
    int saved_errno = errno;
    while (waitpid(-1, nullptr, WNOHANG) > 0);
    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}