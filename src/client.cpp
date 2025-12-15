#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define ADDRESS "127.0.0.1"
#define PORT 8000
#define LISTEN_BACKLOG 100
#define BUF_SIZE 256

int main(void) {
  struct sockaddr_in addr;
  socklen_t addr_len{sizeof(addr)};
  int socket_fd;
  uint8_t *write_buffer = (uint8_t *)malloc(BUF_SIZE);
  uint8_t *read_buffer = (uint8_t *)malloc(BUF_SIZE);
  ssize_t bytes_read;
  ssize_t bytes_recv;
  // Create socket
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    std::cerr << "socket error\n";
  }

  // Set up address
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = PORT;
  if (inet_pton(AF_INET, ADDRESS, &addr.sin_addr.s_addr) == -1) {
    std::cerr << "inet_pton error\n";
  }

  // Connect to address if there is a listening socket
  if (connect(socket_fd, (struct sockaddr *)&addr, addr_len) == -1) {
    std::cerr << "connect error\n";
  }

  // Read from user input
  bytes_read = read(STDIN_FILENO, write_buffer, BUF_SIZE);
  if (bytes_read == 0) {
    std::cout << "Read in EOF\n";
  }
  if (bytes_read == -1) {
    std::cout << "Read from stdin error\n";
  }
  // String Safety
  write_buffer[bytes_read] = '\0';
  std::cout << "Got message\n";
  std::cout << write_buffer << '\n';

  // Sanitize input

  // Send user input through socket
  if (send(socket_fd, write_buffer, bytes_read, 0) == -1) {
    std::cerr << "send error\n";
  }
  std::cout << "Sent to server\n";

  // Read server response
  if ((bytes_recv = recv(socket_fd, read_buffer, BUF_SIZE, 0)) == -1) {
    std::cerr << "recv error\n";
  }
  read_buffer[bytes_recv] = '\0';
  std::cout << "Received from server\n";
  std::cout << read_buffer << '\n';

  free(write_buffer);
  free(read_buffer);
  return 0;
}
