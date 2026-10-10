# Designing a Proxy Server

## Team Members

| **Name**                  | **Roll No.** |
| ------------------------- | ------------ |
| **Swayam Ranjan Mohanty** | **2405167**  |
| **Sai Arpit Panda**       | **2405145**  |
| **Subhra Prakash Sahoo**  | **2405164**  |
| **Aniket Kumar Dalai**    | **24051908** |

## 1. Project Overview

The **Designing a Proxy Server** project implements an HTTP proxy server that acts as an intermediary between a client and a destination server.

The proxy receives an HTTP request from a client, parses and validates the request, applies access-control rules, checks the cache, forwards valid cache-miss requests to the destination server, receives the response, and forwards the response back to the client.

The project demonstrates practical concepts of **Computer Networks, TCP socket programming, HTTP communication, request and response forwarding, caching, access control, traffic logging, and error handling**.

## 2. Project Objective

The main objective of the project is to design and implement a functional HTTP proxy server using C and socket programming.

The system focuses on:

- Accepting client connections.
- Processing HTTP requests.
- Validating HTTP request information.
- Applying domain access-control rules.
- Forwarding HTTP requests to destination servers.
- Receiving and forwarding HTTP responses.
- Implementing response caching.
- Maintaining traffic logs.
- Handling invalid requests and connection failures.
- Supporting repeated client connections.

## 3. System Architecture

```text
                 Client
                   |
                   | HTTP Request
                   v
          +-------------------+
          |   Proxy Server    |
          +-------------------+
             |      |      |
             |      |      |
          Access   Cache  Logger
          Control    |
             |       |
             +-------+
                 |
           Cache Miss
                 |
                 v
        +-------------------+
        | Destination Server|
        +-------------------+
                 |
                 | HTTP Response
                 v
          +-------------------+
          |   Proxy Server    |
          +-------------------+
                 |
                 | HTTP Response
                 v
               Client
```

The proxy server acts as an intermediary between the client and the destination server.

## 4. Key Features

### HTTP Request Forwarding

The proxy receives HTTP requests from clients, parses the request information, connects to the destination server, and forwards the request.

### Response Caching

The proxy stores suitable destination responses in memory. A repeated request for the same resource can be served directly from the cache.

### Access Control

The proxy checks requested domains against access-control rules and blocks configured domains.

### Traffic Logging

The proxy records important runtime events such as client connections, requests, cache activity, destination connections, and response forwarding.

### Error Handling

The proxy handles invalid requests, invalid hosts, blocked domains, destination connection failures, and other connection errors without terminating the server unexpectedly.

### Multiple Client Connections

The proxy remains active after completing a client request and can accept subsequent client connections.

## 5. Computer Networks Concepts

The project demonstrates the following Computer Networks concepts:

- Client-Server Architecture
- Proxy Servers
- HTTP Communication
- TCP/IP Networking
- Socket Programming
- Request and Response Handling
- Caching
- Access Control
- Traffic Logging
- Error Handling
- DNS Resolution
- TCP Connections

## 6. Proxy Workflow

```text
Client
  |
  | HTTP Request
  v
Proxy Server
  |
  +---- Validate HTTP Request
  |
  +---- Check Access Control
  |       |
  |       +---- Blocked ----> HTTP 403 Response
  |
  +---- Check Cache
  |       |
  |       +---- Cache Hit ----> Return Cached Response
  |
  +---- Cache Miss
          |
          v
   Connect to Destination
          |
          v
   Forward HTTP Request
          |
          v
   Receive HTTP Response
          |
          +---- Store Response in Cache
          |
          v
   Forward Response to Client
```

## 7. HTTP Request Parsing

The project contains an HTTP parser module that extracts and validates information from incoming HTTP requests.

The parser is used to process request information such as:

- HTTP method
- Request target
- Host information
- Port information
- HTTP request format

Invalid or malformed requests are rejected instead of being forwarded to the destination server.

## 8. Caching

The proxy implements an in-memory response cache.

### Cache Miss

When a requested resource is not available in the cache:

```text
Client
  |
  v
Proxy
  |
  | Cache MISS
  v
Destination Server
  |
  v
Response
  |
  +---- Store in Cache
  |
  v
Client
```

### Cache Hit

When the requested resource is already available in the cache:

```text
Client
  |
  v
Proxy
  |
  | Cache HIT
  v
Cached Response
  |
  v
Client
```

The implementation also handles cache-size limitations for responses that are too large to store.

## 9. Access Control

The proxy includes a domain-based access-control mechanism.

Configured blocked domains are rejected before cache lookup and destination-server communication.

```text
Client Request
      |
      v
Access Control Check
      |
   +--+------+
   |         |
Allowed    Blocked
   |         |
   v         v
Continue   HTTP 403
Processing Response
```

The access-control module also handles cases such as case differences, subdomains, trailing dots, and invalid input.

## 10. Multi-Client Support

The proxy server uses a persistent server loop so that it can continue accepting client connections after completing a request.

```text
Start Proxy
     |
     v
Accept Client 1
     |
     v
Process Request
     |
     v
Close Client 1
     |
     v
Accept Client 2
     |
     v
Process Request
     |
     v
Continue...
```

The current implementation supports **repeated client connections**. It does not currently implement thread-based or process-based simultaneous client handling.

## 11. Traffic Logging

The project includes a logging module for recording important runtime events.

The logs can include information related to:

- Server startup
- Client connections
- HTTP requests
- Cache MISS events
- Cache HIT events
- Destination connections
- Request forwarding
- Response forwarding
- Cache storage
- Access-control decisions
- Errors

Logging is useful for debugging, monitoring, and understanding the proxy's execution flow.

## 12. Error Handling

The proxy handles common errors without unexpectedly terminating the server.

The implementation includes handling for:

- Malformed HTTP requests
- Invalid Host information
- Blocked domains
- Invalid destination hosts
- Destination connection failures
- Client connection failures
- Socket errors

The proxy returns appropriate HTTP error responses for supported error conditions, including:

- **400 Bad Request**
- **403 Forbidden**
- **502 Bad Gateway**

## 13. Technologies Used

The project uses:

- **C**
- **TCP Socket Programming**
- **Winsock2**
- **HTTP**
- **TCP/IP**
- **Git**
- **GitHub**
- **GCC / MinGW**

The main project modules include:

```text
src/
├── server.c
├── client.c
├── http.c
├── http.h
├── cache.c
├── cache.h
├── logger.c
├── logger.h
├── access_control.c
└── access_control.h
```

## 14. Testing

The project includes both module-level and integrated proxy testing.

Testing covers:

- Proxy startup
- Client connection
- HTTP request forwarding
- Destination-server connection
- Response forwarding
- Invalid destinations
- Different URL paths
- Cache MISS
- Cache HIT
- Connection failure handling
- Logger integration
- Access-control rules
- Invalid Host handling
- HTTP parser tests
- Cache tests
- Access-control tests
- Large-response forwarding
- Repeated client connections

The detailed test results are available in:

```text
tests/test_results.txt
```

Additional testing information is available in:

```text
docs/testing.md
```

## 15. Demonstrated Cache Behavior

The integrated proxy was tested using repeated requests for the same resource.

The first request resulted in:

```text
Cache MISS
```

The response was received from the destination server and stored in the cache.

A subsequent request for the same resource resulted in:

```text
Cache HIT
```

and the cached response was returned directly to the client.

## 16. Project Limitations

The current implementation has the following limitations:

- The proxy currently supports HTTP request forwarding.
- HTTPS `CONNECT` tunneling is not implemented.
- The current multi-client implementation accepts repeated client connections but does not provide true simultaneous thread-based or process-based concurrency.
- The cache is an in-memory cache with a limited storage capacity.
- The current access-control rules contain a predefined set of blocked domains.

## 17. AI Tools Used

AI tools were used during development for:

- Learning networking concepts
- Understanding socket programming
- Debugging
- Code assistance
- Understanding project requirements
- Documentation assistance
- Test planning
- Project organization

### Tools Used

- **ChatGPT** — Concept clarification, debugging assistance, code guidance, documentation, testing guidance, and project planning.
- **GitHub Copilot** — Code completion and development assistance.

AI-generated suggestions were reviewed, tested, and modified by the team before being incorporated into the project.

## 18. Project Demonstration

The final demonstration can include:

1. Starting the proxy server.
2. Connecting a client to the proxy.
3. Sending an HTTP request.
4. Demonstrating destination-server connection.
5. Demonstrating HTTP request and response forwarding.
6. Demonstrating a cache MISS.
7. Demonstrating a cache HIT.
8. Demonstrating an allowed domain.
9. Demonstrating a blocked domain.
10. Showing proxy logs.
11. Demonstrating invalid-request handling.
12. Demonstrating destination connection failure handling.
13. Demonstrating repeated client connections.

## 19. Expected Outcome

The completed project demonstrates a functional HTTP proxy server capable of acting as an intermediary between clients and destination servers.

The implemented system provides:

- HTTP request parsing
- HTTP request forwarding
- HTTP response forwarding
- Response caching
- Cache HIT/MISS handling
- Domain access control
- Traffic logging
- Error handling
- Repeated client connection support
- TCP socket-based communication

## 20. Project Information

**Course:** Computer Networks  
**Project Type:** Mini Project  
**Project Title:** Designing a Proxy Server  
**Team Size:** 4 Members