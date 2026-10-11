#ifndef CACHE_H
#define CACHE_H

#define CACHE_KEY_SIZE 256
#define CACHE_DATA_SIZE 4096

typedef struct
{
    char key[CACHE_KEY_SIZE];
    char data[CACHE_DATA_SIZE];
} CacheEntry;

int cache_init(void);
int cache_put(const char *key, const char *data);
const char *cache_get(const char *key);

#endif