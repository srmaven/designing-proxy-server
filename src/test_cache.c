#include <stdio.h>
#include "cache.h"

int main(void)
{
    const char *key = "/index.html";
    const char *data = "Hello from cache";

    cache_init();

    printf("Testing cache MISS...\n");

    if (cache_get(key) == NULL)
    {
        printf("Cache MISS\n");
    }

    printf("\nStoring response in cache...\n");

    if (cache_put(key, data) == 0)
    {
        printf("Response stored successfully\n");
    }

    printf("\nTesting cache HIT...\n");

    const char *cached_data = cache_get(key);

    if (cached_data != NULL)
    {
        printf("Cache HIT\n");
        printf("Cached data: %s\n", cached_data);
    }

    return 0;
}