//
// Created by Martin Appel on 17.12.24.
//

#ifndef SERVER_H
#define SERVER_H

// simple hello world response to a successful connected client
// child process to the server main loop
void hello_world_stream(int client_socket);

void ping_pong(int client_socket);

#endif //SERVER_H
