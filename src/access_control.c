
#include "access_control.h"
#include <ctype.h>
#include <stddef.h>
#include <string.h>

#define DOMAIN_COUNT 2

static const char *blocked_domains[DOMAIN_COUNT] = {
    "blocked.example",
    "malware.test"};

static int domain_matches(const char *host, const char *domain)
{
    size_t host_len;
    size_t domain_len;

    if (host == NULL || domain == NULL)
    {
        return 0;
    }

    host_len = strlen(host);
    domain_len = strlen(domain);

    if (host_len < domain_len)
    {
        return 0;
    }

    if (_strnicmp(host + host_len - domain_len,
                  domain, domain_len) != 0)
    {
        return 0;
    }

    if (host_len == domain_len ||
        host[host_len - domain_len - 1] == '.')
    {
        return 1;
    }

    return 0;
}

int is_domain_allowed(const char *host)
{
    size_t i;
    size_t len;

    if (host == NULL || host[0] == '\0')
    {
        return -1;
    }

    len = strlen(host);

    for (i = 0; i < len; i++)
    {
        if (isspace((unsigned char)host[i]) ||
            host[i] == '/' || host[i] == '\\' ||
            host[i] == ':' || host[i] == '@')
        {
            return -1;
        }
    }

    if (len > 0 && host[len - 1] == '.')
    {
        char normalized[256];

        if (len >= sizeof(normalized))
        {
            return -1;
        }

        memcpy(normalized, host, len - 1);
        normalized[len - 1] = '\0';
        return is_domain_allowed(normalized);
    }

    for (i = 0; i < DOMAIN_COUNT; i++)
    {
        if (domain_matches(host, blocked_domains[i]))
        {
            return 0;
        }
    }

    return 1;
}
