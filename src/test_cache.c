#include <stdio.h>
#include <string.h>
#include "cache.h"

int main(void)
{
    const char *key1 = "/index.html";
    const char *data1 = "Hello from index";

    const char *key2 = "/about.html";
    const char *data2 = "Hello from about";

    cache_init();

    printf("========================================\n");
    printf("TEST 1: Initial Cache MISS\n");
    printf("========================================\n");

    if (cache_get(key1) == NULL)
    {
        printf("PASS: Key1 MISS\n");
    }
    else
    {
        printf("FAIL: Key1 should be MISS\n");
    }

    printf("\n========================================\n");
    printf("TEST 2: Store Key1\n");
    printf("========================================\n");

    if (cache_put(key1, data1) == 0)
    {
        printf("PASS: Key1 stored successfully\n");
    }
    else
    {
        printf("FAIL: Key1 could not be stored\n");
    }

    printf("\n========================================\n");
    printf("TEST 3: Key1 Cache HIT\n");
    printf("========================================\n");

    const char *cached_data1 = cache_get(key1);

    if (cached_data1 != NULL)
    {
        printf("PASS: Key1 HIT\n");
        printf("Cached data: %s\n", cached_data1);
    }
    else
    {
        printf("FAIL: Key1 should be HIT\n");
    }

    printf("\n========================================\n");
    printf("TEST 4: Key2 Initial MISS\n");
    printf("========================================\n");

    if (cache_get(key2) == NULL)
    {
        printf("PASS: Key2 MISS\n");
    }
    else
    {
        printf("FAIL: Key2 should be MISS\n");
    }

    printf("\n========================================\n");
    printf("TEST 5: Store Key2\n");
    printf("========================================\n");

    if (cache_put(key2, data2) == 0)
    {
        printf("PASS: Key2 stored successfully\n");
    }
    else
    {
        printf("FAIL: Key2 could not be stored\n");
    }

    printf("\n========================================\n");
    printf("TEST 6: Key2 Cache HIT\n");
    printf("========================================\n");

    const char *cached_data2 = cache_get(key2);

    if (cached_data2 != NULL)
    {
        printf("PASS: Key2 HIT\n");
        printf("Cached data: %s\n", cached_data2);
    }
    else
    {
        printf("FAIL: Key2 should be HIT\n");
    }

    printf("\n========================================\n");
    printf("TEST 7: Key1 After Key2 Storage\n");
    printf("========================================\n");

    cached_data1 = cache_get(key1);

    if (cached_data1 != NULL)
    {
        printf("Key1 HIT\n");
        printf("Key1 data: %s\n", cached_data1);
    }
    else
    {
        printf("Key1 MISS\n");
        printf("INFO: Key1 was replaced when Key2 was stored.\n");
    }

    printf("\n========================================\n");
    printf("TEST 8: Oversized Data\n");
    printf("========================================\n");

    char oversized_data[CACHE_DATA_SIZE + 100];

    memset(oversized_data, 'A', sizeof(oversized_data) - 1);
    oversized_data[sizeof(oversized_data) - 1] = '\0';

    printf("Input data length: %u\n",
           (unsigned int)strlen(oversized_data));
    printf("Cache data limit: %d\n", CACHE_DATA_SIZE - 1);

    if (cache_put("/large.html", oversized_data) == 0)
    {
        printf("cache_put() accepted oversized data.\n");

        const char *cached_large_data = cache_get("/large.html");

        if (cached_large_data != NULL)
        {
            size_t stored_length = strlen(cached_large_data);

            printf("Stored data length: %u\n",
                   (unsigned int)stored_length);

            if (stored_length == CACHE_DATA_SIZE - 1)
            {
                printf("PASS: Data was safely truncated to cache limit.\n");
            }
            else if (stored_length < CACHE_DATA_SIZE)
            {
                printf("PASS: Data was stored within cache limit.\n");
            }
            else
            {
                printf("FAIL: Stored data exceeds cache limit.\n");
            }
        }
        else
        {
            printf("FAIL: Oversized data could not be retrieved.\n");
        }
    }
    else
    {
        printf("cache_put() rejected oversized data.\n");
        printf("This is also a safe behavior.\n");
    }

    printf("\n========================================\n");
    printf("CACHE VERIFICATION COMPLETE\n");
    printf("========================================\n");

    return 0;
}