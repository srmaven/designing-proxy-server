#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

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

    /* Close connection */
    closesocket(client_socket);

    /* Clean up Winsock */
    WSACleanup();

    return 0;
}