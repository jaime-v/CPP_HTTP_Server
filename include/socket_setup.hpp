#ifndef SOCKET_SETUP_HPP
#define SOCKET_SETUP_HPP

#include <arpa/inet.h>
#include <sys/socket.h>

class ServerSocket {
public:
  explicit ServerSocket(int port);
  ServerSocket(const ServerSocket &) = delete;
  ServerSocket &operator=(const ServerSocket &) = delete;
  int accept_client();
  ~ServerSocket();

private:
  int fd_{-1};

  void create_socket();
  void bind_and_listen(int port);
};

#endif
