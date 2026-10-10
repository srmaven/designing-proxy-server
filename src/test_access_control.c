
#include <stdio.h>
#include "access_control.h"

static int run_test(const char *host, int expected, const char *description)
{
    int result = is_domain_allowed(host);

    if (result == expected)
    {
        printf("PASS: %s\n", description);
        return 0;
    }

    printf("FAIL: %s (expected %d, got %d)\n",
           description, expected, result);
    return 1;
}

int main(void)
{
    int failures = 0;

    failures += run_test("example.com", 1, "Allow normal domain");
    failures += run_test("blocked.example", 0, "Block listed domain");
    failures += run_test("sub.blocked.example", 0, "Block subdomain");
    failures += run_test("malware.test", 0, "Block second listed domain");
    failures += run_test("notblocked.example", 1, "Do not block similar suffix");
    failures += run_test("BLOCKED.EXAMPLE", 0, "Case-insensitive block");
    failures += run_test("example.com.", 1, "Allow trailing dot");
    failures += run_test("", -1, "Reject empty hostname");
    failures += run_test("http://example.com", -1, "Reject URL instead of hostname");
    failures += run_test(NULL, -1, "Reject NULL hostname");

    printf("\nAccess control tests: %d failure(s)\n", failures);

    return failures == 0 ? 0 : 1;
}
