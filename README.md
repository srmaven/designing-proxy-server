# Designing a Proxy Server

## Team Members

| Name | Roll No. |
|---|---|
| **Swayam Ranjan Mohanty** | **2405167** |
| **Sai Arpit Panda** | **2405145** |
| **Subhra Prakash Sahoo** | **2405164** |
| **Aniket Kumar Dalai** | **24051908** |

## 1. Project Overview

The **Proxy Server** project implements an intermediary server that sits between clients and their intended internet destinations.

The proxy receives client requests, processes them, forwards valid requests to the destination server, receives the response, and sends it back to the client.

The project demonstrates practical concepts of **Computer Networks, HTTP communication, concurrent connections, caching, access control, traffic logging, and performance measurement**.

## 2. Project Objective

The main objective is to design and implement a functional proxy server capable of handling real client requests while providing additional traffic-management features.

The system focuses on:

- Handling multiple clients simultaneously.
- Processing and forwarding HTTP requests.
- Implementing response caching.
- Providing access-control mechanisms.
- Maintaining useful traffic logs.
- Handling malformed requests and unreachable destinations.
- Measuring the performance overhead introduced by the proxy.

## 3. System Architecture

```text
                 Client 1
                    |
                 Client 2
                    |
                 Client 3
                    |
                    v
            +----------------+
            |  Proxy Server  |
            +----------------+
              |     |      |
              |     |      |
          Cache   Filter   Logger
              |
              v
       Destination Server
              |
              v
          HTTP Response
              |
              v
         Proxy Server
              |
              v
            Clients
```

The proxy server acts as an intermediary between clients and destination servers.

## 4. Key Features

- **Multi-Client Support** — Handles multiple simultaneous client connections.
- **HTTP Request Forwarding** — Receives and forwards HTTP requests to the intended destination.
- **Response Caching** — Stores suitable responses and serves them from cache when applicable.
- **Access Control** — Supports rules for restricting specific domains/URLs or controlling client access.
- **Traffic Logging** — Records useful information about requests and responses.
- **Error Handling** — Handles malformed requests, unreachable destinations, and connection failures.
- **Performance Measurement** — Measures latency and throughput with and without the proxy.

## 5. Computer Networks Concepts

The project demonstrates:

- Client-Server Architecture
- Proxy Servers
- HTTP Communication
- TCP/IP Networking
- Socket Programming
- Concurrent Connections
- Request and Response Handling
- Caching
- Access Control
- Traffic Logging
- Network Latency
- Throughput
- Error Handling

## 6. Proxy Workflow

```text
Client
  |
  | HTTP Request
  v
Proxy Server
  |
  +---- Check Access Control
  |
  +---- Check Cache
  |       |
  |       +---- Cache Hit ----> Return Cached Response
  |
  +---- Cache Miss
          |
          v
   Destination Server
          |
          | HTTP Response
          v
      Proxy Server
          |
          +---- Store Response in Cache
          |
          v
        Client
```

## 7. Caching

The proxy maintains a cache of previously received responses.

### Cache Hit
If a requested resource is available and valid in the cache, the proxy can return the cached response without contacting the destination server.

### Cache Miss
If the resource is not available in the cache or needs to be refreshed, the proxy forwards the request to the destination server and may store the received response for future requests.

## 8. Access Control

The proxy includes an access-control mechanism that can restrict access to selected domains or URLs.

```text
Client Request
      |
      v
Access Control Check
      |
   +--+--+
   |     |
Allowed Blocked
   |     |
   v     v
Forward  Reject
```

## 9. Concurrent Connections

The proxy is designed to support multiple clients simultaneously. A client request should not prevent other clients from communicating with the proxy.

Concurrency can be implemented using:

- Threads
- Processes
- Asynchronous I/O

The final implementation will use the approach selected by the team.

## 10. Traffic Logging

The proxy records useful traffic information for monitoring and debugging, such as:

- Client address
- Requested URL
- Request method
- Destination server
- Request status
- Response time
- Cache hit/miss status
- Access-control result

## 11. Error Handling

The proxy should handle common failures without crashing, including:

- Malformed HTTP requests
- Invalid destinations
- Unreachable servers
- Connection timeouts
- Client disconnections
- Invalid requests

## 12. Performance Analysis

The project will compare direct communication with communication through the proxy.

The analysis will consider:

- Request latency
- Response latency
- Throughput
- Cache-hit performance
- Cache-miss performance
- Proxy processing overhead

## 13. Technologies

The final implementation may use:

- C
- Socket Programming
- HTTP
- TCP/IP
- Multithreading or asynchronous networking
- File-based or in-memory caching
- Git & GitHub

The exact implementation technologies and libraries will be finalized during development.

## 14. AI Tools Used

AI tools may be used during development for **learning, debugging, code assistance, documentation, brainstorming, and understanding networking concepts**.

### Tools

- **ChatGPT** — Concept clarification, debugging assistance, code guidance, documentation, and project planning.
- **GitHub Copilot** — Code completion, boilerplate generation, and development assistance.

AI-generated suggestions will be reviewed, tested, and modified by the team before being incorporated into the project.

## 15. Demonstration

The final demonstration will include:

1. Starting the proxy server.
2. Connecting multiple clients.
3. Sending normal HTTP requests.
4. Demonstrating request forwarding.
5. Demonstrating a cache miss.
6. Demonstrating a cache hit.
7. Demonstrating an access-control rule.
8. Showing traffic logs.
9. Demonstrating error handling.
10. Comparing performance with and without the proxy.

## 16. Expected Outcome

The final system will demonstrate a working proxy server capable of acting as an intermediary between clients and destination servers while providing **request forwarding, caching, access control, logging, concurrent connection handling, error handling, and performance analysis**.

## 17. Project Information

**Course:** Computer Networks  
**Project Type:** Mini Project  
**Project Title:** Designing a Proxy Server  
**Team Size:** 4 Members
# designing-proxy-server
Computer Networks Mini Project - Designing a Proxy Server
