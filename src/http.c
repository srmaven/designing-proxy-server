#include "http.h"
#include <stdio.h>
#include <string.h>

int http_parse_request(const char *raw_request, HttpRequest *request)
{
    if (raw_request == NULL || request == NULL)
    {
        return -1;
    }

    if (sscanf(raw_request, "%15s %255s %15s",
               request->method,
               request->path,
               request->version) != 3)
    {
        return -1;
    }

    return 0;
}