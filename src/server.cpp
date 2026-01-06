#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
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

void print_byte_vector(std::vector<uint8_t> vector) {
  std::cout << "\nVector print begin\n";
  for (std::size_t i{}; i < vector.size(); ++i) {
    printf("(%02x) ", vector[i]);
  }
  for (std::size_t i{}; i < vector.size(); ++i) {
    printf("(%c) ", vector[i]);
  }
  std::cout << "\nVector print end\n";
}

void print_string_vector(std::vector<std::string> vector) {
  std::cout << "\nVector print begin\n";
  for (std::size_t i{}; i < vector.size(); ++i) {
    std::cout << i << "\n";
    std::cout << vector[i] << "\n";
    for (std::size_t j{}; j < vector[i].length(); ++j) {
      printf("(%02x) ", vector[i][j]);
    }
    std::cout << "\n\n";
  }
  std::cout << "\nVector print end\n";
}

// I assume this modifies string internally? Doesn't look like it actually
// since we are passing it by value, not by reference
std::vector<std::string> tokenize_input(std::string string, std::string delim) {
  std::vector<std::string> tokens;
  std::string token{};
  size_t pos = 0;
  while ((pos = string.find(delim)) != std::string::npos) {
    token = string.substr(0, pos);
    tokens.push_back(token);
    string.erase(0, pos + delim.length());
  }
  tokens.push_back(string);
  return tokens;
}

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
    std::vector<uint8_t> buffer(BUF_SIZE);
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

    // Now, we need to tokenize based on \r\n, then we need to tokenize again
    // based on spaces
    std::cout << "We are starting to tokenize the input\n";
    std::vector<std::string> lines = tokenize_input(request, "\r\n");
    std::cout << "We tokenized the input based on lines\n";
    print_string_vector(lines);

    // Create a 2-D vector
    // Vector contains vectors of strings
    // Vector 0 is line 0 (GET /index.html HTTP/1.1), Vector 1 is line 1, etc.
    // Expect vector 0 to have 3 tokens, vector 1 to have 2 tokens
    std::cout << "Starting to tokenize first line based on spaces\n";
    std::vector<std::string> request_line = tokenize_input(lines[0], " ");
    std::cout << "We tokenized the input based on spaces\n";

    std::cout << "Displaying tokens of request line\n";
    print_string_vector(request_line);
    std::cout << "End of tokens of request line\n";

    std::string method = request_line[0];
    std::string path = request_line[1];
    std::string version = request_line[2];

    if (method != "GET") {
      std::cout << "Respond with 405 - Method Not Allowed\n";
    }
    if (version != "HTTP/1.1" && version != "HTTP/1.0") {
      std::cout << "Respond with 505 - HTTP Version Not Supported\n";
    }
    if (path == "/") {
      std::cout << "Path is just /, swapping to /index.html\n";
      path = "/index.html";
    }

    // Adjust path based on file structure
    std::string doc_root = "./files";
    path.insert(0, doc_root);
    std::cout << "Updated path:";
    std::cout << path << '\n';

    // If file exists, open it and send response
    // If file does not exist, send 404

    // Tokenizing based on spaces
    // std::vector<std::string> tokens = tokenize_input(request, " ");
    // print_string_vector(tokens);

    // Tokens should now contain all tokens
    // We create tokens vector, create a null token, and a delimiter we
    // want Start at pos 0 We find delimiter and as long as its not
    // equal to npos (no position?) We create a substring from pos 0 to
    // the delimiter We then push that substring onto the vector Then we
    // erase the token and delimiter from buffer Then we repeat the
    // cycle until our pos hits npos Once we find npos, that means the
    // delimiter was not found Then buffer only has the last token So we
    // add it to tokens and yeah
    // std::cout << "Displaying tokens\n";
    // for (size_t i{}; i < tokens.size(); ++i) {
    //   std::cout << tokens[i] << "\n";
    // }
    // std::cout << "End of tokens\n";

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
