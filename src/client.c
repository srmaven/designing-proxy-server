#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {

    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in proxy_address;

    /* Initialize Windows socket library */
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    /* Create TCP socket */
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    printf("TCP socket created successfully.\n");

    /* Configure proxy server address */
    proxy_address.sin_family = AF_INET;
    proxy_address.sin_port = htons(8080);

    /*
       For now, use localhost.
       Later, when testing with Swayam's PC,
       replace this with Swayam's IP address.
    */
    proxy_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (proxy_address.sin_addr.s_addr == INADDR_NONE) {
        printf("Invalid proxy address.\n");

        closesocket(client_socket);
        WSACleanup();

        return 1;
    }

    /* Connect to proxy server */
    printf("Connecting to proxy server...\n");

    if (connect(client_socket,
                (struct sockaddr *)&proxy_address,
                sizeof(proxy_address)) == SOCKET_ERROR) {

        printf("Connection to proxy failed.\n");

        closesocket(client_socket);
        WSACleanup();

        return 1;
    }

    printf("Connected to proxy server successfully.\n");

    /* Create HTTP request */
    const char *http_request =
        "GET http://example.com/ HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Connection: close\r\n"
        "\r\n";

    /* Send HTTP request to proxy */
    int bytes_sent = send(
        client_socket,
        http_request,
        (int)strlen(http_request),
        0
    );

    if (bytes_sent == SOCKET_ERROR) {
        printf("Failed to send HTTP request.\n");

        closesocket(client_socket);
        WSACleanup();

        return 1;
    }

    printf("HTTP request sent successfully.\n");

    /* Receive HTTP response from proxy */

    char buffer[4096];
    int bytes_received;

    printf("Waiting for HTTP response...\n");

    while ((bytes_received = recv(
        client_socket,
        buffer,
        sizeof(buffer) - 1,
        0
    )) > 0) {

        /* Add string terminator */
        buffer[bytes_received] = '\0';

        /* Display received response */
        printf("%s", buffer);
    }

    /* Check why recv() stopped */
    if (bytes_received == 0) {

        printf("\n\nHTTP response received successfully.\n");

    } else if (bytes_received == SOCKET_ERROR) {

        printf("\n\nFailed to receive HTTP response.\n");
    }

    /* Close connection */
    closesocket(client_socket);

    /* Clean up Winsock */
    WSACleanup();

    return 0;
}