#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define ADDRESS "0.0.0.0"
#define PORT 8000
#define LISTEN_BACKLOG 100
#define BUF_SIZE 256

int main(void) {
  struct sockaddr_in addr {};
  socklen_t addr_len{sizeof(addr)};
  int socket_fd{-1};
  int client_fd{-1};
  ssize_t bytes_recv{};

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

  // Bind socket to address
  if (bind(socket_fd, (struct sockaddr *)&addr, addr_len) == -1) {
    std::cerr << "bind error\n";
  }

  // Make socket listen
  if (listen(socket_fd, LISTEN_BACKLOG) == -1) {
    std::cerr << "listen error\n";
  }

  // Forever accept loop
  for (;;) {
    client_fd = accept(socket_fd, NULL, NULL);
    if (client_fd == -1) {
      std::cerr << "accept error\n";
    }
    std::cout << "Connected a client! " << client_fd << '\n';

    // Receive request, store in buffer
    std::vector<uint8_t> buffer(4096);
    if ((bytes_recv = recv(client_fd, buffer.data(), buffer.size(), 0)) == -1) {
      std::cerr << "recv error\n";
    }
    std::cout << "Received message from client\n";

    // Resize buffer
    if (bytes_recv > 0) {
      buffer.resize(bytes_recv);
    }

    // Convert uint8_t vector to string
    std::string request(buffer.begin(), buffer.end());
    std::cout << "We converted the uint8_t to string\n";
    std::cout << request << '\n';

    // String Safety
    // buffer[bytes_recv] = '\0';
    // std::cout << "This is the buffer\n";
    // std::cout << buffer.data() << '\n';

    // Parse request
    // We need to take the string
    // Iterate through the string until we find the delimiter
    // Create a substring including everything from the starting position, up
    // until the delimiter.
    // We use std::string::find and std::string::substr to find a delimiter and
    // create a substring respectively.
    // We also use std::vector<std::string> to create a vector of strings which
    // will represent our tokens.
    // And we have the std::string::erase method for removing tokens.
    // This is essentially a strtok_r function, but in C++
    std::cout << "We are starting to tokenize the input\n";
    std::vector<std::string> tokens;
    std::string token{};
    std::string delimiter{" "};
    size_t pos = 0;
    while ((pos = request.find(delimiter)) != std::string::npos) {
      token = request.substr(0, pos);
      tokens.push_back(token);
      request.erase(0, pos + delimiter.length());
    }
    tokens.push_back(request);
    std::cout << "We tokenized the input\n";
    // Tokens should now contain all tokens
    // We create tokens vector, create a null token, and a delimiter we want
    // Start at pos 0
    // We find delimiter and as long as its not equal to npos (no position?)
    // We create a substring from pos 0 to the delimiter
    // We then push that substring onto the vector
    // Then we erase the token and delimiter from buffer
    // Then we repeat the cycle until our pos hits npos
    // Once we find npos, that means the delimiter was not found
    // Then buffer only has the last token
    // So we add it to tokens and yeah
    std::cout << "Displaying tokens\n";
    for (size_t i{}; i < tokens.size(); ++i) {
      std::cout << tokens[i] << '\n';
    }
    std::cout << "End of tokens\n";

    // Process request
    // Send response
    // Handle errors

    if (send(client_fd, "Hello World!\n", strlen("Hello World!\n"), 0) == -1) {
      std::cerr << "send error\n";
    }
    std::cout << "Sent message to client\n";

    // Clean up
    if (close(client_fd) == -1) {
      std::cerr << "close error\n";
    }
  }
  return 0;
}
