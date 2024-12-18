//
// Created by Martin Appel on 17.12.24.
//

#ifndef UTILS_H
#define UTILS_H

#define PORT "8080"
#define BUFFER_SIZE 1024
#define MSG_BUFFER_SIZE 2048
#define MAX_BUFFER_SIZE 10240
#define BACKLOG 5

// handler function for child processes
void sigchld_handler(int sig);

/**
 * gets the socket address struct (IPv4 or IPv6)
 * @param sa socket address
 * @return the address in IPv4 or IPv6 sockaddr_in[6]
 */
void *get_in_addr(struct sockaddr *sa);

void *get_in_port(struct sockaddr *sa);

/**
 * Create a socket and bind an address to it.
 * Closes the process if the bind failes.
 * @param addr address info struct for the socket to bind to
 * @return socket file descriptor
 */
int get_socket_bind(struct addrinfo *addr);

/**
 * Receiving data from the specified socket.
 * Returns a heap allocated pointer which the process calling this function has to free.
 * @param sockfd file descriptor of the reading socket
 * @return heap allocated pointer with the message content
 */
char *receive_data(int sockfd);

char *read_file(const char *path, long *size);

#endif //UTILS_H
