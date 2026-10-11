#include <stdio.h>
#include "../src/logger.h"

int main() {
    printf("Testing log_info...\n");
    log_info("Test info message");

    printf("Testing log_error...\n");
    log_error("Test error message");

    printf("Testing log_request...\n");
    log_request("GET", "/", "example.com");

    printf("\nAll logger tests completed!\n");
    return 0;
}