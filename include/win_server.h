//
// Created by marti on 12/23/2024.
//

#ifndef WIN_SERVER_H
#define WIN_SERVER_H

#ifdef _WIN32

// Windows server exit codes
#define ERROR_WSA_STARTUP    1
#define ERROR_SOCKET_CREATE  2
#define ERROR_SOCKET_BIND    3
#define ERROR_SOCKET_LISTEN  4
#define ERROR_SOCKET_ACCEPT  5

int win_server();

void win_http(SOCKET client_socket);

#endif //_WIN32

#endif //WIN_SERVER_H
