#include <stdio.h>
#include "http.h"

int main(void)
{
    const char *raw_request = "GET /index.html HTTP/1.1\r\n"
                              "Host: example.com\r\n"
                              "\r\n";

    HttpRequest request;

    int result = http_parse_request(raw_request, &request);

    if (result == 0)
    {
        printf("Method: %s\n", request.method);
        printf("Path: %s\n", request.path);
        printf("Version: %s\n", request.version);
        return 0;
    }

    printf("Failed to parse HTTP request\n");
    return 1;
}