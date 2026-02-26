# Minimal HTTP Server (C++)

This project is a multi-client chat server implemented in C using Linux epoll for scalable event-driven I/O.  
It supports a custom binary messaging protocol, partial read/write handling, and stress testing with fuzzing clients.

This project is a minimal HTTP/1.0-1.1 server implemented in C++ using TCP sockets.
It handles HTTP GET requests, MIME type detection, and static file serving.

---

## Features

- TCP socket server (POSIX sockets)
- Serves static files from a server directory
- Basic HTTP request parsing
- MIME type detection


---

## Build

```
cmake -S . -B build/
cmake --build build/
cd build
./server
```


