#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

void parse_request(char *request) {
    char method[16];
    char path[2048];
    char version[16];
    char host[256];
    int port = 80;

    method[0] = '\0';
    path[0] = '\0';
    version[0] = '\0';
    host[0] = '\0';

    sscanf(request, "%15s %2047s %15s", method, path, version);

    char *host_line = strstr(request, "\nHost:");

    if (host_line == NULL)
        host_line = strstr(request, "\nhost:");

    if (host_line != NULL) {
        host_line += 6;

        while (*host_line == ' ')
            host_line++;

        sscanf(host_line, "%255s", host);

        char *colon = strchr(host, ':');

        if (colon != NULL) {
            *colon = '\0';
            port = atoi(colon + 1);
        }
    }

    printf("\nParsed HTTP Request\n");
    printf("-------------------\n");
    printf("Method : %s\n", method);
    printf("Path   : %s\n", path);
    printf("Host   : %s\n", host);
    printf("Port   : %d\n", port);
    printf("-------------------\n");
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char buffer[4096];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Proxy server listening on port 8080...\n");

    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Client connected.\n");

    int received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    if (received > 0) {
        buffer[received] = '\0';

        printf("\nReceived request:\n");
        printf("%s\n", buffer);

        parse_request(buffer);

        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 21\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Proxy Server Working!";

        send(client_socket, response, (int)strlen(response), 0);
    }

    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}