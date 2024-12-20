//
// Created by Martin Appel on 17.12.24.
//

#ifndef SERVER_H
#define SERVER_H

// HTTP header macros
#define HTTP_HELLO_WORLD "HTTP/1.1 200 OK\r\n" \
                         "Server: http-server\r\n" \
                         "Content-type: text/html\r\n\r\n" \
                         "<html>Hello World!</html>\r\n"
#define HTTP_HEADER_HTML "HTTP/1.1 200 OK\r\n" \
                         "Content-type: text/html\r\n\r\n"
#define HTTP_HEADER_CSS  "HTTP/1.1 200 OK\r\n" \
                         "Content-type: text/css\r\n\r\n"
#define HTTP_HEADER_PNG  "HTTP/1.1 200 OK\r\n" \
                         "Content-type: image/png\r\n\r\n"
#define HTTP_HEADER_GIF  "HTTP/1.1 200 OK\r\n" \
                         "Content-type: image/gif\r\n\r\n"
#define HTTP_HEADER_JPEG "HTTP/1.1 200 OK\r\n" \
                         "Content-type: image/jpeg\r\n\r\n"
#define HTTP_HEADER_ICO  "HTTP/1.1 200 OK\r\n" \
                         "Content-type: image/x-icon\r\n\r\n"
#define HTTP_HEADER_JS   "HTTP/1.1 200 OK\r\n" \
                         "Content-type: text/javascript\r\n\r\n"

// HTTP error message macros
#define HTTP_ERROR_400   "HTTP/1.1 400 Bad Request\r\n" \
                         "Content-type: text/html\r\n" \
                         "<html>400 Bad Request</html>\r\n"
#define HTTP_ERROR_404   "HTTP/1.1 404 Not Found\r\n" \
                         "Content-type: text/html\r\n" \
                         "<html>404 Not Found</html>\r\n"
#define HTTP_ERROR_5ßß   "HTTP/1.1 500 Internal Server Error\r\n" \
                         "Content-type: text/html\r\n" \
                         "<html>500 Internal Server Error</html>\r\n"

void http_hello_world(int client_socket);

void simple_http(int client_socket);

// simple hello world response to a successful connected client
// child process to the server main loop
void hello_world_stream(int client_socket);

// Simple telnet ping pong
// if you send "ping\n" it will respond with "pong"
void ping_pong(int client_socket);

#endif //SERVER_H
