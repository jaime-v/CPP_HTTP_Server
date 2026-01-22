#include <cstdlib>
#include <iostream>

#include "socket_setup.hpp"

#define ADDRESS "0.0.0.0"
#define PORT 8000
#define LISTEN_BACKLOG 100

int create_socket(void) {
  int socket_fd{socket(AF_INET, SOCK_STREAM, 0)};
  if (socket_fd == -1) {
    std::cerr << "socket error\n";
    std::exit(EXIT_FAILURE);
  }
  return socket_fd;
}

server_address create_and_setup_address(void) {
  server_address server_addr{};

  // memset(&server_address.addr, 0, server_address.addr_len);
  // ^ memset is redundant now because of brace initialization setting
  // everything to 0
  server_addr.addr.sin_family = AF_INET;
  server_addr.addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, ADDRESS, &server_addr.addr.sin_addr.s_addr) == -1) {
    std::cerr << "inet_pton error\n";
    std::exit(EXIT_FAILURE);
  }
  // server_address.addr.sin_addr.s_addr = htonl(INADDR_ANY);
  // ^ this is a simpler version instead of using inet_pton
  return server_addr;
}

void bind_and_listen(int socket_fd, server_address server_addr) {
  // Bind socket to address
  if (bind(socket_fd, reinterpret_cast<sockaddr *>(&server_addr.addr),
           server_addr.addr_len) == -1) {
    std::cerr << "bind error\n";
    std::exit(EXIT_FAILURE);
  }

  // Make socket listen
  if (listen(socket_fd, LISTEN_BACKLOG) == -1) {
    std::cerr << "listen error\n";
    std::exit(EXIT_FAILURE);
  }
}

client_connection accept_connection(int socket_fd) {
  client_connection client{};
  client.fd = accept(socket_fd, reinterpret_cast<sockaddr *>(&client.addr),
                     &client.addr_len);
  if (client.fd == -1) {
    std::cerr << "accept error\n";
    std::exit(EXIT_FAILURE);
  }
  return client;
}

server_state server_setup() {
  server_state state{};
  state.fd = create_socket();
  state.addr = create_and_setup_address();
  bind_and_listen(state.fd, state.addr);
  return state;
}
