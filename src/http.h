#ifndef HTTP_H
#define HTTP_H

typedef struct
{
    char method[16];
    char path[256];
    char version[16];
} HttpRequest;

int http_parse_request(const char *raw_request, HttpRequest *request);

#endif