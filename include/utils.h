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


#endif //UTILS_H
