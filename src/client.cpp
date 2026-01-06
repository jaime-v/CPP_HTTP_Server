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

void print_vector(std::vector<uint8_t> vector) {
  std::cout << "\nVector print begin\n";
  for (std::size_t i{}; i < vector.size(); ++i) {
    printf("(%02x) ", vector[i]);
  }
  for (std::size_t i{}; i < vector.size(); ++i) {
    printf("(%c) ", vector[i]);
  }
  std::cout << "\nVector print end\n";
}

int main(void) {
  // Typical Socket API stuff
  struct sockaddr_in addr;
  socklen_t addr_len{sizeof(addr)};
  int socket_fd;
  // ssize_t bytes_read;
  ssize_t bytes_recv;

  // Create buffers for reading and writing
  // These are vectors of uint8_t, we will use these for networking
  // std::vector<uint8_t> read_buffer(BUF_SIZE);
  // std::vector<uint8_t> write_buffer(BUF_SIZE);

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

  /*
  // Read from user input into the write buffer's data
  // C++ Style involving string for input, then converting to uint8_t
  // push_back the null terminator for string safety
  std::string input;
  std::getline(std::cin, input);
  std::vector<uint8_t> write_buffer(input.begin(), input.end());
  write_buffer.push_back('\0');
  print_vector(write_buffer);
  std::cout << "Got user input\n";

  // C Style that we used before
  // bytes_read = read(STDIN_FILENO, write_buffer.data(), BUF_SIZE);
  // if (bytes_read == 0) {
  //   std::cout << "Read in EOF\n";
  // }
  // if (bytes_read == -1) {
  //   std::cout << "Read from stdin error\n";
  // }

  // Send user input through socket
  if (send(socket_fd, write_buffer.data(), write_buffer.size(), 0) == -1) {
    std::cerr << "send error\n";
  }
  std::cout << "Sent user input to server\n";
  */

  // Sending valid request
  std::string message_string =
      "GET /index.html HTTP/1.0\r\nHost: localhost\r\n\r\n";
  std::vector<uint8_t> message_vector(message_string.begin(),
                                      message_string.end());
  if (send(socket_fd, message_vector.data(), message_vector.size(), 0) == -1) {
    std::cerr << "send error\n";
  }
  print_vector(message_vector);

  // Sending valid request with altered path
  // std::string message_string = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
  // std::vector<uint8_t> message_vector(message_string.begin(),
  //                                     message_string.end());
  // if (send(socket_fd, message_vector.data(), message_vector.size(), 0) == -1)
  // {
  //   std::cerr << "send error\n";
  // }
  // print_vector(message_vector);

  // Sending invalid method
  // std::string message_string =
  //     "DELETE /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
  // std::vector<uint8_t> message_vector(message_string.begin(),
  //                                     message_string.end());
  // if (send(socket_fd, message_vector.data(), message_vector.size(), 0) == -1)
  // {
  //   std::cerr << "send error\n";
  // }
  // print_vector(message_vector);

  // Sending invalid version
  // std::string message_string =
  //     "GET /index.html HTTP/6.7\r\nHost: localhost\r\n\r\n";
  // std::vector<uint8_t> message_vector(message_string.begin(),
  //                                     message_string.end());
  // if (send(socket_fd, message_vector.data(), message_vector.size(), 0) == -1)
  // {
  //   std::cerr << "send error\n";
  // }
  // print_vector(message_vector);

  // Read server response
  std::vector<uint8_t> read_buffer(BUF_SIZE);
  if ((bytes_recv =
           recv(socket_fd, read_buffer.data(), read_buffer.size(), 0)) == -1) {
    std::cerr << "recv error\n";
  }
  std::cout << "Received from server\n";
  return 0;
}
