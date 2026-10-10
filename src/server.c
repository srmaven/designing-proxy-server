#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "http.h"
#include "cache.h"
#include "logger.h"

#pragma comment(lib, "ws2_32.lib")

void parse_request(char *request, char *method, char *path, char *host, int *port) {
    char version[16];
    char host_line[256];

    *port = 80;

    sscanf(request, "%15s %2047s %15s", method, path, version);

    host[0] = '\0';

    char *host_ptr = strstr(request, "\r\nHost:");
    if (host_ptr == NULL)
        host_ptr = strstr(request, "\r\nhost:");

    if (host_ptr != NULL) {
        host_ptr += 7;

        while (*host_ptr == ' ')
            host_ptr++;

        sscanf(host_ptr, "%255s", host_line);

        char *colon = strchr(host_line, ':');

        if (colon != NULL) {
            *colon = '\0';
            *port = atoi(colon + 1);
        }

        strcpy(host, host_line);
    }
}

SOCKET connect_to_server(char *host, int port) {
    SOCKET destination_socket;
    struct sockaddr_in server_addr;
    struct hostent *server;

    destination_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (destination_socket == INVALID_SOCKET) {
        printf("Destination socket creation failed\n");
        log_error("Destination socket creation failed");
        return INVALID_SOCKET;
    }

    server = gethostbyname(host);

    if (server == NULL) {
        printf("DNS resolution failed for %s\n", host);
        log_error("DNS resolution failed");
        closesocket(destination_socket);
        return INVALID_SOCKET;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    memcpy(
        &server_addr.sin_addr,
        server->h_addr_list[0],
        server->h_length
    );

    if (connect(
        destination_socket,
        (struct sockaddr*)&server_addr,
        sizeof(server_addr)
    ) == SOCKET_ERROR) {
        printf("Connection to destination server failed\n");
        log_error("Connection to destination server failed");
        closesocket(destination_socket);
        return INVALID_SOCKET;
    }

    return destination_socket;
}

int main() {
    WSADATA wsa;
    SOCKET server_socket;
    SOCKET client_socket;
    SOCKET destination_socket;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    int client_len;

    char buffer[8192];
    char method[16];
    char path[2048];
    char host[256];

    int port;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        log_error("WSAStartup failed");
        return 1;
    }

    if (cache_init() != 0) {
        printf("Cache initialization failed\n");
        log_error("Cache initialization failed");
        WSACleanup();
        return 1;
    }

    printf("Cache initialized.\n");
    log_info("Cache initialized");

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        log_error("Proxy socket creation failed");
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(
        server_socket,
        (struct sockaddr*)&server_addr,
        sizeof(server_addr)
    ) == SOCKET_ERROR) {
        printf("Bind failed\n");
        log_error("Proxy bind failed");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listen failed\n");
        log_error("Proxy listen failed");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Proxy server listening on port 8080...\n");
    log_info("Proxy server listening on port 8080");

    while (1) {

        client_len = sizeof(client_addr);

        client_socket = accept(
            server_socket,
            (struct sockaddr*)&client_addr,
            &client_len
        );

        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed\n");
            log_error("Client accept failed");
            continue;
        }

        printf("Client connected.\n");
        log_info("Client connected");

        int received = recv(
            client_socket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if (received <= 0) {
            printf("Failed to receive HTTP request.\n");
            log_error("Failed to receive HTTP request");
            closesocket(client_socket);
            continue;
        }

        buffer[received] = '\0';

        printf("\nReceived request:\n");
        printf("%s\n", buffer);

        HttpRequest http_request;

        if (http_parse_request(buffer, &http_request) != 0) {
            printf("HTTP request parsing failed.\n");
            log_error("HTTP request parsing failed");
            closesocket(client_socket);
            continue;
        }

        parse_request(
            buffer,
            method,
            path,
            host,
            &port
        );

        log_request(method, path, host);

        char cache_key[2304];

        snprintf(
            cache_key,
            sizeof(cache_key),
            "%s:%d%s",
            host,
            port,
            path
        );

        const char *cached_response = cache_get(cache_key);

        if (cached_response != NULL) {
            printf("Cache HIT\n");
            log_info("Cache HIT");

            send(
                client_socket,
                cached_response,
                (int)strlen(cached_response),
                0
            );

            closesocket(client_socket);
            continue;
        }

        printf("Cache MISS\n");
        log_info("Cache MISS");

        printf("\nParsed Request\n");
        printf("-------------------\n");
        printf("Method : %s\n", method);
        printf("Path   : %s\n", path);
        printf("Host   : %s\n", host);
        printf("Port   : %d\n", port);
        printf("-------------------\n");

        printf("\nConnecting to destination server...\n");

        destination_socket = connect_to_server(host, port);

        if (destination_socket == INVALID_SOCKET) {
            printf("Could not connect to destination server.\n");
            log_error("Could not connect to destination server");

            closesocket(client_socket);
            continue;
        }

        printf("Connected to %s:%d\n", host, port);
        log_info("Connected to destination server");

        char request_to_server[8192];

        snprintf(
            request_to_server,
            sizeof(request_to_server),
            "%s %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Connection: close\r\n"
            "\r\n",
            method,
            path,
            host
        );

        int sent = send(
            destination_socket,
            request_to_server,
            (int)strlen(request_to_server),
            0
        );

        if (sent == SOCKET_ERROR) {
            printf("Failed to send request to destination server.\n");
            log_error("Failed to send request to destination server");

            closesocket(destination_socket);
            closesocket(client_socket);
            continue;
        }

        printf("Request sent to destination server.\n");
        log_info("Request sent to destination server");

        int response_size;

        char cached_data[CACHE_DATA_SIZE];
        int total_response = 0;
        int cacheable = 1;

        while ((response_size = recv(
            destination_socket,
            buffer,
            sizeof(buffer),
            0
        )) > 0) {

            int forwarded = send(
                client_socket,
                buffer,
                response_size,
                0
            );

            if (forwarded == SOCKET_ERROR) {
                printf("Failed to send response to client.\n");
                log_error("Failed to send response to client");
                break;
            }

            if (cacheable) {
                if (total_response + response_size < CACHE_DATA_SIZE) {

                    memcpy(
                        cached_data + total_response,
                        buffer,
                        response_size
                    );

                    total_response += response_size;
                    cached_data[total_response] = '\0';

                } else {
                    cacheable = 0;
                }
            }
        }

        if (cacheable && total_response > 0) {

            if (cache_put(cache_key, cached_data) == 0) {
                printf("Response stored in cache.\n");
                log_info("Response stored in cache");
            } else {
                printf("Failed to store response in cache.\n");
                log_error("Failed to store response in cache");
            }
        }

        printf("Response received from destination server.\n");
        printf("Response sent to client.\n");

        log_info("Response received from destination server");
        log_info("Response sent to client");

        closesocket(destination_socket);
        closesocket(client_socket);

        printf("\nWaiting for next client...\n");
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}