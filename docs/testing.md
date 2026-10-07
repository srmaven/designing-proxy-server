# Proxy Server Testing

## Date

2026-10-07

## Objective

The objective of this testing was to verify proxy startup, client connections, HTTP request parsing, destination server connections, response forwarding, error handling, URL/path handling, and cache behavior.

## Test Environment

- Platform: Windows
- Proxy Port: 8080
- Proxy: `src/server.c`
- HTTP parser: `src/http.c`
- Cache: `src/cache.c`
- Logger: `src/logger.c`

## Test Results

| Test Case | Description | Result |
|---|---|---|
| TC01 | Proxy startup | PASS |
| TC02 | Client connection | PASS |
| TC03 | HTTP GET / destination forwarding | FAIL |
| TC04 | Destination connection | PASS |
| TC05 | Response forwarding | PASS |
| TC06 | Invalid destination | PASS |
| TC07 | Different URL/path | PASS |
| TC08 | Cache HIT/MISS | FAIL / NOT VERIFIED |
| TC09 | Connection failure | PASS |

## Observations

### TC03 — HTTP GET / Destination Forwarding

The proxy successfully received and parsed the HTTP GET request. However, the destination connection failed during the initial test. Direct access to `example.com` without the proxy succeeded, indicating that the failure occurred in the proxy-side destination connection path.

A later test using `/test` successfully connected to `example.com:80` and forwarded the response.

### TC08 — Cache HIT/MISS

The same URL was requested twice. Both requests connected to the destination server and fetched the response. No explicit cache HIT/MISS behavior was observed in the server output.

Therefore, cache HIT/MISS behavior could not be verified successfully.

## Conclusion

Seven test cases passed successfully. TC03 exposed a destination connection issue during the initial HTTP GET test, while TC08 could not verify cache HIT/MISS behavior.

The failed/not-verified cases should be investigated during further integration and debugging.