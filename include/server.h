//
// Created by Martin Appel on 17.12.24.
//

#ifndef SERVER_H
#define SERVER_H

#define HTTP_HELLO_WORLD "HTTP/1.1 200 OK\r\n" \
                         "Server: http-server\r\n" \
                         "Content-type: text/html\r\n\r\n" \
                         "<html>Hello World!</html>\r\n"

void http_hello_world(int client_socket);

// simple hello world response to a successful connected client
// child process to the server main loop
void hello_world_stream(int client_socket);

// Simple telnet ping pong
// if you send "ping\n" it will respond with "pong"
void ping_pong(int client_socket);

#endif //SERVER_H
