//
// Created by marti on 12/23/2024.
//
#include <stdio.h>

#ifdef _WIN32

#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "win_server.h"
#include "server.h"
#include "utils.h"


int win_server() {
    int gai_status, wsa_status;
    struct addrinfo hints, *res;

    WSADATA wsaData;
    if ((wsa_status = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
        printf("WSAStartup failed with error: %d\n", wsa_status);
        exit(ERROR_WSA_STARTUP);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;

    if ((gai_status = getaddrinfo(nullptr, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_status));
        return EXIT_FAILURE;
    }

    // create server socket
    SOCKET serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == INVALID_SOCKET) {
        fprintf(stderr, "socket() failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        exit(ERROR_SOCKET_CREATE);
    }

    // create server address
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // bind socket
    if (bind(serverfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        fprintf(stderr, "bind() failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        exit(ERROR_SOCKET_BIND);
    }

    // listen on socket for incoming connections
    if (listen(serverfd, SOMAXCONN) != 0) {
        fprintf(stderr, "listen() failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        exit(ERROR_SOCKET_LISTEN);
    }

    printf("Server listening on port %s\n", PORT);

    while (true) {
        SOCKADDR_IN client_addr = {0};
        socklen_t client_addr_len = sizeof(client_addr);

        // accept connection and print client addr and port
        SOCKET clientfd = accept(serverfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (clientfd == INVALID_SOCKET) {
            fprintf(stderr, "accept() failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            exit(ERROR_SOCKET_ACCEPT);
        }
        printf("Connection accepted from [%s:%d]\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

        win_http(clientfd);

        closesocket(clientfd);
    }
}

void win_http(SOCKET client_socket) {
    // Read from client socket
    char *buffer = receive_data(client_socket);
    if (!buffer) {
        perror("receive_data");
        send(client_socket, HTTP_ERROR_400, sizeof(HTTP_ERROR_400), 0);
        return;
    }
    printf("%s\n", buffer);

    char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s", method, uri, version);

    // remove leading forward slash from URI
    if (uri[0] == '/') memmove(uri, uri + 1, strlen(uri));
    if (strlen(uri) == 0) memmove(uri, "index.html", 11);

    if (strcmp(method, "GET") == 0) {
        long file_size = 0;
        char *file_contents = read_file(uri, &file_size);
        if (!file_contents) {
            fprintf(stderr, "Error reading file: %s\n", uri);
            send(client_socket, HTTP_ERROR_404, sizeof(HTTP_ERROR_404), 0);
            free(file_contents);
            return;
        }

        char *http_header = get_resp_header(uri);
        if (!http_header) {
            fprintf(stderr, "Error getting response header for: %s\n", uri);
            send(client_socket, HTTP_ERROR_400, strlen(HTTP_ERROR_400), 0);
            free(file_contents);
            return;
        }

        char msg[MAX_BUFFER_SIZE] = {0};
        strcpy(msg, http_header);
        strcat(msg, file_contents);

        // send(client_socket, msg, sizeof msg, 0);
        send(client_socket, http_header, strlen(http_header), 0);
        send(client_socket, file_contents, strlen(file_contents), 0);
        free(file_contents);
    }

    free(buffer);
}

#endif