//
// Created by Martin Appel on 17.12.24.
//

#ifndef UTILS_H
#define UTILS_H

#define PORT "8080"
#define BUFSIZE 1024
#define BACKLOG 5

// handler function for child processes
void sigchld_handler(int sig);

/**
 * gets the socket address struct (IPv4 or IPv6)
 * @param sa socket address
 * @return the address in IPv4 or IPv6 sockaddr_in[6]
 */
void *get_in_addr(struct sockaddr *sa);

// simple hello world response to a successful connected client
void hello_world_stream(int server_socket, int client_socket);

/**
 * Create a socket and bind an address to it.
 * Closes the process if the bind failes.
 * @param addr address info struct for the socket to bind to
 * @return socket file descriptor
 */
int get_socket_bind(struct addrinfo *addr);

#endif //UTILS_H
