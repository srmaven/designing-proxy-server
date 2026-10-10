# Proxy Server Testing

## Date

2026-10-10

## Objective

The objective of testing was to verify the complete proxy-server request flow, including:

- Proxy server startup
- Client connection
- HTTP request parsing
- Destination server connection
- HTTP request forwarding
- HTTP response forwarding
- Multi-client handling
- Cache MISS and HIT behavior
- Logger integration
- Domain access control
- Invalid request handling
- Destination connection failure handling
- Large-response forwarding
- Cache and HTTP parser unit tests

HTTPS CONNECT tunneling was also checked to identify the current project limitation.

---

## Test Environment

- Platform: Windows
- Proxy Port: 8080
- Proxy Server: `src/server.c`
- HTTP Parser: `src/http.c`
- Cache: `src/cache.c`
- Logger: `src/logger.c`
- Access Control: `src/access_control.c`
- Proxy Client: `src/client.c`
- Compiler: GCC / MinGW
- Test Client: Custom C client and `curl.exe`

---

# Final Test Results

| Test Case | Description | Result |
|---|---|---|
| TC01 | Proxy startup | PASS |
| TC02 | Client connection | PASS |
| TC03 | HTTP GET / destination forwarding | PASS |
| TC04 | Destination connection | PASS |
| TC05 | Response forwarding | PASS |
| TC06 | Invalid destination / destination failure handling | PASS |
| TC07 | Different URL/path handling | PASS |
| TC08 | Cache MISS → HIT | PASS |
| TC09 | Connection failure handling | PASS |
| TC10 | Logger verification | PASS |
| TC11 | Access-control allow/block testing | PASS |
| TC12 | HTTP parser unit tests | PASS |
| TC13 | Cache unit tests | PASS |
| TC14 | Large-response forwarding | PASS |
| TC15 | Multi-client / repeated client requests | PASS |
| TC16 | HTTPS CONNECT tunneling | NOT IMPLEMENTED |

---

# Detailed Test Results

## TC01 — Proxy Startup

### Objective

Verify that the proxy server starts successfully and listens on port `8080`.

### Result

**PASS**

The server successfully initialized Winsock, initialized the cache, and started listening on port `8080`.

Example output:

```text
Cache initialized.
Proxy server listening on port 8080...