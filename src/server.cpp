#include <arpa/inet.h>
#include <filesystem>
#include <fstream>
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
#define BUF_SIZE 4096

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

// It looks like it modifies strings internally, but doesnt
// I guess since we are passing it by value, not by reference
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

// Takes const path reference
// Loads a file into memory
// Opens up a filestream using path -- binary?
// If we failed to open a file, throw error
// Seek to end
// Get current cursor position (how many bytes are in the file)
// Seek back to beginning
// Allocate buffer based on file size
// Read file data into buffer.data()
//      buffer.data() is a uint8_t *
//      std::ifstream::read expects a char *
//      So we need to reinterpret_cast to char *, taking buffer.data()
//      Then we can successfully read into buffer.data
//
//      Basically, cast buffer.data() into char *, then read size bytes into it
// Return buffer
std::vector<uint8_t> load_file(const std::filesystem::path &path) {
  std::ifstream file(path, std::ios::binary);

  if (!file) {
    throw std::runtime_error("failed to open file");
  }

  file.seekg(0, std::ios::end);
  std::size_t size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(size);
  file.read(reinterpret_cast<char *>(buffer.data()), size);

  return buffer;
}

int main(void) {
  struct sockaddr_in addr{};
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
  addr.sin_port = htons(PORT);
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
    std::cout << "\n\n\n~~~~waiting for client~~~~\n\n\n";
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
    std::string request_path = request_line[1];
    std::string version = request_line[2];

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

    if (method != "GET") {
      std::cout << "Respond with 405 - Method Not Allowed\n";
    }
    if (version != "HTTP/1.1" && version != "HTTP/1.0") {
      std::cout << "Respond with 505 - HTTP Version Not Supported\n";
    }
    if (request_path == "/") {
      std::cout << "Path is just /, swapping to /index.html\n";
      request_path = "/index.html";
    }
    if (request_path.empty() || request_path[0] != '/') {
      std::cout << "Respond with 400 - Bad Request\n";
    }

    // Adjust path based on file structure
    // std::string doc_root = "./files";
    // path.insert(0, doc_root);
    // std::cout << "Updated path:";
    // std::cout << path << '\n';

    // If file exists, open it and send response
    // If file does not exist, send 404

    // Convert string request to file path -- also remove the leading '/'
    std::filesystem::path file_path{request_path.substr(1)};
    std::cout << "We have reached the file_path constructor\n";
    std::cout << file_path << '\n';

    // Current pwd
    // std::cout << "CWD: " << std::filesystem::current_path() << "\n";

    // What we want
    std::filesystem::path doc_root{std::filesystem::current_path() /
                                   "../files"};
    doc_root = std::filesystem::weakly_canonical(doc_root);
    std::cout << "doc_root: " << doc_root << "\n";

    // Normalzed path
    // std::filesystem::path normalized_path{
    //     std::filesystem::weakly_canonical(want_path)};
    // std::cout << "Normalized: " << normalized_path << "\n";

    // Get full path
    std::filesystem::path full_path{doc_root / file_path};
    std::cout << "Final path: " << full_path << "\n";

    // Normalize full path
    full_path = std::filesystem::weakly_canonical(full_path);

    // Validate path
    if (full_path.string().find(doc_root.string()) != 0) {
      std::cout << "Respond with 403 - Forbidden Access\n";
    }

    // Check if file exists and is a regular file
    if (!std::filesystem::exists(full_path) ||
        !std::filesystem::is_regular_file(full_path)) {
      std::cout << "Respond with 404 - does not exist or not regular file\n";
    }

    // If file exists open it -- load the entire file into memory
    std::vector<uint8_t> file_buffer{load_file(full_path)};
    if (!file_buffer.empty()) {
      std::cout << "I think we have loaded it in\n";
    } else {
      std::cout << "file buffer is empty\n";
    }

    // Construct the HTTP response
    // -- How do we make this response?
    // -- Just hardcode i guess
    // -- For now at least
    // -- Maybe eventually we will add different paths based on the request
    // method
    // -- But for now, just hardcode the GET and 200 / 404

    // Success
    // HTTP/1.1 200 OK\r\n
    // Content-Length: <bytes>\r\n
    // Content-Type: text/html\r\n
    // \r\n
    // <file bytes>
    std::string success_response = "HTTP/1.1 200 OK\r\n"
                                   "Content-Length: " +
                                   std::to_string(file_buffer.size()) +
                                   "\r\n"
                                   "Content-Type: text/html\r\n"
                                   "\r\n";
    std::cout << "Created success_response:\n";
    std::cout << success_response;

    // Fail
    // HTTP/1.1 404 Not Found\r\n
    // Content-Length: 0\r\n
    // \r\n
    // <body>
    // body is just text, so we can add it on to the response
    //
    // std::string fail_response = "HTTP/1.1 404 Not Found\r\n"
    //                             "Content-Length: 0\r\n"
    //                             "\r\n"
    //                             "404 Not Found\n";
    // std::cout << "Created fail_response:\n";
    // std::cout << fail_response;

    // Send the HTTP response
    // Use send
    // Since we want to send the uint8_t vector directly after the response, we
    // can just use 2 sends here like we did with the Chat Server (which sent
    // the header then the payload)

    // Send success response header
    if (send(client_fd, success_response.data(), success_response.size(), 0) ==
        -1) {
      std::cerr << "send success response header error\n";
    } else {
      std::cout << "sent success response header to client\n";
    }

    // Send success response body
    if (send(client_fd, file_buffer.data(), file_buffer.size(), 0) == -1) {
      std::cerr << "send response body error\n";
    } else {
      std::cout << "sent response body to client\n";
    }

    // Send fail response (header and body)
    // if (send(client_fd, fail_response.data(), fail_response.size(), 0) == -1)
    // {
    //   std::cerr << "send fail response header error\n";
    // } else {
    //   std::cout << "sent fail response header to client\n";
    // }

    // if (send(client_fd, "Hello World!\n", strlen("Hello World!\n"), 0) == -1)
    // {
    //   std::cerr << "send error\n";
    // }
    // std::cout << "Sent message to client\n";

    // Clean up
    if (close(client_fd) == -1) {
      std::cerr << "close error\n";
    }
  }
  return 0;
}
