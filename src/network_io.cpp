#include <sys/socket.h>

#include "network_io.hpp"

ssize_t recv_request(int client_fd, std::vector<uint8_t> &buffer) {
  // Receive request, store in buffer
  ssize_t bytes_recv{};
  if ((bytes_recv = recv(client_fd, buffer.data(), buffer.size(), 0)) == -1) {
    // std::cerr << "recv error\n";
    return -1;
  }

  // Resize buffer
  if (bytes_recv > 0) {
    buffer.resize(bytes_recv);
  }
  return bytes_recv;
}

ssize_t send_response(int client_fd, const std::vector<uint8_t> &buffer) {
  // Send success response header
  ssize_t bytes_sent{};
  if ((bytes_sent = send(client_fd, buffer.data(), buffer.size(), 0)) == -1) {
    // std::cerr << "send error\n";
    return -1;
  }
  return bytes_sent;
}
