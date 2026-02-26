# Minimal HTTP Server (C++)

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

## Testing

```
Browser
Open http://localhost:8000

Terminal
curl -iv http://localhost:8000/index.html

Phone
ifconfig | grep inet
http://<ip>:8000
```

