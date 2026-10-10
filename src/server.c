#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <winsock2.h>

#include "http.h"
#include "cache.h"
#include "logger.h"
#include "access_control.h"

#pragma comment(lib, "ws2_32.lib")

int parse_request(
    char *request,
    char *method,
    char *path,
    char *host,
    int *port
)
{
    char version[16];
    char host_line[256];

    *port = 80;
    host[0] = '\0';

    if (request == NULL ||
        method == NULL ||
        path == NULL ||
        host == NULL ||
        port == NULL)
    {
        return -1;
    }

    if (sscanf(
        request,
        "%15s %2047s %15s",
        method,
        path,
        version
    ) != 3)
    {
        return -1;
    }

    char *host_ptr = strstr(request, "\r\nHost:");

    if (host_ptr == NULL)
        host_ptr = strstr(request, "\r\nhost:");

    if (host_ptr == NULL)
        return -1;

    host_ptr += 7;

    while (*host_ptr == ' ')
        host_ptr++;

    if (sscanf(host_ptr, "%255s", host_line) != 1)
        return -1;

    /*
     * A Host header must contain a hostname,
     * optionally followed by :port.
     *
     * Reject URL-like values such as:
     * http://example.com
     */

    for (size_t i = 0; host_line[i] != '\0'; i++)
    {
        unsigned char c = (unsigned char)host_line[i];

        if (isspace(c) ||
            host_line[i] == '/' ||
            host_line[i] == '\\' ||
            host_line[i] == '@')
        {
            return -1;
        }
    }

    char *colon = strchr(host_line, ':');

    if (colon != NULL)
    {
        *colon = '\0';

        if (*(colon + 1) == '\0')
            return -1;

        for (char *p = colon + 1; *p != '\0'; p++)
        {
            if (!isdigit((unsigned char)*p))
                return -1;
        }

        *port = atoi(colon + 1);

        if (*port < 1 || *port > 65535)
            return -1;
    }

    if (host_line[0] == '\0')
        return -1;

    strcpy(host, host_line);

    return 0;
}

SOCKET connect_to_server(char *host, int port)
{
    SOCKET destination_socket;
    struct sockaddr_in server_addr;
    struct hostent *server;

    destination_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (destination_socket == INVALID_SOCKET)
    {
        printf("Destination socket creation failed\n");
        return INVALID_SOCKET;
    }

    server = gethostbyname(host);

    if (server == NULL)
    {
        printf("DNS resolution failed for %s\n", host);

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
        (struct sockaddr *)&server_addr,
        sizeof(server_addr)
    ) == SOCKET_ERROR)
    {
        printf("Connection to destination server failed\n");

        closesocket(destination_socket);
        return INVALID_SOCKET;
    }

    return destination_socket;
}

int main()
{
    WSADATA wsa;
    SOCKET server_socket;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    int client_len;

    char buffer[8192];
    char method[16];
    char path[2048];
    char host[256];

    int port;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed\n");
        return 1;
    }

    if (cache_init() != 0)
    {
        printf("Cache initialization failed\n");
        WSACleanup();
        return 1;
    }

    printf("Cache initialized.\n");
    log_info("Cache initialized");

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == INVALID_SOCKET)
    {
        printf("Socket creation failed\n");
        log_error("Socket creation failed");

        WSACleanup();
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(
        server_socket,
        (struct sockaddr *)&server_addr,
        sizeof(server_addr)
    ) == SOCKET_ERROR)
    {
        printf("Bind failed\n");
        log_error("Bind failed");

        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 5) == SOCKET_ERROR)
    {
        printf("Listen failed\n");
        log_error("Listen failed");

        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Proxy server listening on port 8080...\n");
    log_info("Proxy server listening on port 8080");

    while (1)
    {
        client_len = sizeof(client_addr);

        SOCKET client_socket = accept(
            server_socket,
            (struct sockaddr *)&client_addr,
            &client_len
        );

        if (client_socket == INVALID_SOCKET)
        {
            printf("Accept failed\n");
            log_error("Accept failed");
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

        if (received <= 0)
        {
            if (received == SOCKET_ERROR)
            {
                printf("Failed to receive request.\n");
                log_error("Failed to receive request");
            }
            else
            {
                log_info("Client closed connection without sending request");
            }

            closesocket(client_socket);
            continue;
        }

        buffer[received] = '\0';

        printf("\nReceived request:\n");
        printf("%s\n", buffer);

        HttpRequest http_request;

        if (http_parse_request(buffer, &http_request) != 0)
        {
            printf("HTTP request parsing failed.\n");
            log_error("HTTP request parsing failed");

            const char *bad_request_response =
                "HTTP/1.1 400 Bad Request\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 12\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Bad Request\n";

            send(
                client_socket,
                bad_request_response,
                (int)strlen(bad_request_response),
                0
            );

            closesocket(client_socket);
            continue;
        }

        if (parse_request(
            buffer,
            method,
            path,
            host,
            &port
        ) != 0)
        {
            printf("Invalid HTTP Host header.\n");
            log_error("Invalid HTTP Host header");

            const char *invalid_response =
                "HTTP/1.1 400 Bad Request\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 13\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Invalid Host\n";

            send(
                client_socket,
                invalid_response,
                (int)strlen(invalid_response),
                0
            );

            closesocket(client_socket);
            continue;
        }

        printf("\nParsed Request\n");
        printf("-------------------\n");
        printf("Method : %s\n", method);
        printf("Path   : %s\n", path);
        printf("Host   : %s\n", host);
        printf("Port   : %d\n", port);
        printf("-------------------\n");

        log_request(method, path, host);

        /*
         * Access control happens before cache lookup.
         */

        int access_result = is_domain_allowed(host);

        if (access_result == 0)
        {
            printf("Access denied for domain: %s\n", host);
            log_info("Access denied by access control");

            const char *blocked_response =
                "HTTP/1.1 403 Forbidden\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 13\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Access Denied\n";

            send(
                client_socket,
                blocked_response,
                (int)strlen(blocked_response),
                0
            );

            closesocket(client_socket);
            continue;
        }

        if (access_result == -1)
        {
            printf("Invalid hostname: %s\n", host);
            log_error("Invalid hostname");

            const char *invalid_response =
                "HTTP/1.1 400 Bad Request\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 13\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Invalid Host\n";

            send(
                client_socket,
                invalid_response,
                (int)strlen(invalid_response),
                0
            );

            closesocket(client_socket);
            continue;
        }

        printf("Access allowed for domain: %s\n", host);
        log_info("Access allowed");

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

        if (cached_response != NULL)
        {
            printf("Cache HIT\n");
            log_info("Cache HIT");

            send(
                client_socket,
                cached_response,
                (int)strlen(cached_response),
                0
            );

            log_info("Cached response sent to client");

            closesocket(client_socket);
            continue;
        }

        printf("Cache MISS\n");
        log_info("Cache MISS");

        printf("\nConnecting to destination server...\n");

        SOCKET destination_socket = connect_to_server(host, port);

        if (destination_socket == INVALID_SOCKET)
        {
            printf("Could not connect to destination server.\n");
            log_error("Could not connect to destination server");

            const char *server_error_response =
                "HTTP/1.1 502 Bad Gateway\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 12\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Bad Gateway\n";

            send(
                client_socket,
                server_error_response,
                (int)strlen(server_error_response),
                0
            );

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

        int request_sent = send(
            destination_socket,
            request_to_server,
            (int)strlen(request_to_server),
            0
        );

        if (request_sent == SOCKET_ERROR)
        {
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
        )) > 0)
        {
            int bytes_sent_to_client = send(
                client_socket,
                buffer,
                response_size,
                0
            );

            if (bytes_sent_to_client == SOCKET_ERROR)
            {
                printf("Failed to send response to client.\n");
                log_error("Failed to send response to client");
                break;
            }

            if (cacheable)
            {
                if (total_response + response_size < CACHE_DATA_SIZE)
                {
                    memcpy(
                        cached_data + total_response,
                        buffer,
                        response_size
                    );

                    total_response += response_size;
                    cached_data[total_response] = '\0';
                }
                else
                {
                    cacheable = 0;
                    log_info("Response too large to store in cache");
                }
            }
        }

        if (response_size == SOCKET_ERROR)
        {
            printf("Failed to receive response from destination server.\n");
            log_error("Failed to receive response from destination server");
        }

        if (cacheable && total_response > 0)
        {
            if (cache_put(cache_key, cached_data) == 0)
            {
                printf("Response stored in cache.\n");
                log_info("Response stored in cache");
            }
            else
            {
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
    }

    closesocket(server_socket);

    WSACleanup();

    return 0;
}