#include "cache.h"
#include <string.h>

static CacheEntry cache_entry;

int cache_init(void)
{
    memset(&cache_entry, 0, sizeof(cache_entry));
    return 0;
}

int cache_put(const char *key, const char *data)
{
    if (key == NULL || data == NULL)
    {
        return -1;
    }

    strncpy(cache_entry.key, key, CACHE_KEY_SIZE - 1);
    cache_entry.key[CACHE_KEY_SIZE - 1] = '\0';

    strncpy(cache_entry.data, data, CACHE_DATA_SIZE - 1);
    cache_entry.data[CACHE_DATA_SIZE - 1] = '\0';

    return 0;
}

const char *cache_get(const char *key)
{
    if (key == NULL)
    {
        return NULL;
    }

    if (strcmp(cache_entry.key, key) == 0)
    {
        return cache_entry.data;
    }

    return NULL;
}