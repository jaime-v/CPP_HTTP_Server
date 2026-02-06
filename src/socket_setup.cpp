#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include "socket_setup.hpp"

#define ADDRESS "0.0.0.0"
#define LISTEN_BACKLOG 100

ServerSocket::ServerSocket(int port) {
  create_socket();
  bind_and_listen(port);
  // Just one line, can probably just throw in the setup functions right away
  // instead of putting them all into a helper
}

int ServerSocket::accept_client() {
  int client{accept(fd_, nullptr, nullptr)};
  if (client == -1) {
    std::cerr << "accept error\n";
    throw new std::runtime_error("Accept failed");
  }
  return client;
}

ServerSocket::~ServerSocket() { close(fd_); }

void ServerSocket::create_socket() {
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ == -1) {
    std::cerr << "socket error\n";
    throw new std::runtime_error("Socket failed");
  }
}

void ServerSocket::bind_and_listen(int port) {
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ADDRESS, &server_addr.sin_addr.s_addr) == -1) {
    std::cerr << "inet_pton error\n";
    throw new std::runtime_error("inet_pton failed");
  }
  // server_address.addr.sin_addr.s_addr = htonl(INADDR_ANY);
  // ^ this is a simpler version instead of using inet_pton

  // Bind socket to address
  if (bind(fd_, reinterpret_cast<sockaddr *>(&server_addr),
           sizeof(server_addr)) == -1) {
    std::cerr << "bind error\n";
    throw new std::runtime_error("bind failed");
  }

  // Make socket listen
  if (listen(fd_, LISTEN_BACKLOG) == -1) {
    std::cerr << "listen error\n";
    throw new std::runtime_error("listen failed");
  }
}
