#ifndef SOCKET_SETUP_HPP
#define SOCKET_SETUP_HPP

#include <arpa/inet.h>
#include <sys/socket.h>

struct server_address {
  sockaddr_in addr{};
  socklen_t addr_len{sizeof(addr)};
};

struct client_connection {
  int fd{-1};
  sockaddr_in addr{};
  socklen_t addr_len{sizeof(addr)};
};

struct server_state {
  int fd{-1};
  server_address addr{};
};

int create_socket();
server_address create_and_setup_address();
void bind_and_listen(int socket_fd, server_address server_addr);
client_connection accept_connection(int socket_fd);
server_state server_setup();

#endif
