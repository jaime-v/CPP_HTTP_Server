#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "create_response.hpp"
#include "load_file.hpp"
#include "network_io.hpp"
#include "parse_request.hpp"
#include "path_building.hpp"
#include "socket_setup.hpp"

#define ADDRESS "0.0.0.0"
#define PORT 8000
#define LISTEN_BACKLOG 100
#define BUF_SIZE 4096

int main(void) {
  // General flow is:
  // Socket setup
  // Accept loop
  //    Read request
  //    Parse request
  //    Figure out what to do with request
  //    Load file (if necessary)
  //    Make response
  //    Send response

  // Setup server socket and address
  // server_state server{server_setup()};
  ServerSocket server(PORT);

  // Forever accept loop, accept_connection blocks
  for (;;) {
    int client{server.accept_client()};
    std::vector<uint8_t> request_buffer(BUF_SIZE);
    ssize_t recv_result{recv_request(client, request_buffer)};
    if (recv_result == 0) {
      std::cout << "Client closed connection\n";
    } else if (recv_result < 0) {
      std::cerr << "recv_request had error\n";
    }
    http_request request{parse_request(request_buffer)};
    request_result result{validate_request(request)};
    std::vector<uint8_t> response{};
    if (!result.ok) {
      // Request isn't valid, so send response for 400 - bad request
      response = build_error_response(result.status_code);
      continue;
    }
    path_result file_path_result{build_path(request)};
    if (!file_path_result.ok) {
      response = build_error_response(file_path_result.status_code);
      continue;
    }
    std::vector<uint8_t> file_buffer{load_file(file_path_result.file_path)};
    std::string mime_type{get_mime_type(request)};
    response = build_success_response(mime_type, file_buffer);
    ssize_t send_result{send_response(client, response)};
    if (send_result == 0) {
      std::cout << "Client closed connection\n";
    } else if (send_result < 0) {
      std::cerr << "send_response had an error\n";
    }
    if (close(client) == -1) {
      std::cerr << "close client had an error\n";
    }
  }
  // Currently no server cleanup, since we just ctrl+c to end the program
  // if (close(server.fd) == -1) {
  //   std::cerr << "close server had an error\n";
  // }
  return 0;
}

// Forever accept loop
// for (;;) {
//   client_fd = accept(socket_fd, NULL, NULL);
//   if (client_fd == -1) {
//     std::cerr << "accept error\n";
//   }
//   // String Safety
//   // buffer[bytes_recv] = '\0';
//   // std::cout << "This is the buffer\n";
//   // std::cout << buffer.data() << '\n';
//
//   // Parse request
//   // We need to take the string
//   // Iterate through the string until we find the delimiter
//   // Create a substring including everything from the starting position, up
//   // until the delimiter.
//   // We use std::string::find and std::string::substr to find a delimiter
//   and
//   // create a substring respectively.
//   // We also use std::vector<std::string> to create a vector of strings
//   which
//   // will represent our tokens.
//   // And we have the std::string::erase method for removing tokens.
//   // This is essentially a strtok_r function, but in C++
//
//   // Now, we need to tokenize based on \r\n, then we need to tokenize again
//   // based on spaces
//   std::vector<std::string> lines = tokenize_input(request, "\r\n");
//   print_string_vector(lines);
//
//   // Create a 2-D vector
//   // Vector contains vectors of strings
//   // Vector 0 is line 0 (GET /index.html HTTP/1.1), Vector 1 is line 1,
//   etc.
//   // Expect vector 0 to have 3 tokens, vector 1 to have 2 tokens
//   std::vector<std::string> request_line = tokenize_input(lines[0], " ");
//   print_string_vector(request_line);
//
//   std::string method = request_line[0];
//   std::string request_path = request_line[1];
//   std::string version = request_line[2];
//
//   // Tokenizing based on spaces
//   // std::vector<std::string> tokens = tokenize_input(request, " ");
//   // print_string_vector(tokens);
//
//   // Tokens should now contain all tokens
//   // We create tokens vector, create a null token, and a delimiter we
//   // want Start at pos 0 We find delimiter and as long as its not
//   // equal to npos (no position?) We create a substring from pos 0 to
//   // the delimiter We then push that substring onto the vector Then we
//   // erase the token and delimiter from buffer Then we repeat the
//   // cycle until our pos hits npos Once we find npos, that means the
//   // delimiter was not found Then buffer only has the last token So we
//   // add it to tokens and yeah
//   // std::cout << "Displaying tokens\n";
//   // for (size_t i{}; i < tokens.size(); ++i) {
//   //   std::cout << tokens[i] << "\n";
//   // }
//   // std::cout << "End of tokens\n";
//
//   if (method != "GET") {
//     std::cout << "Respond with 405 - Method Not Allowed\n";
//   }
//   if (version != "HTTP/1.1" && version != "HTTP/1.0") {
//     std::cout << "Respond with 505 - HTTP Version Not Supported\n";
//   }
//   if (request_path == "/") {
//     std::cout << "Path is just /, swapping to /index.html\n";
//     request_path = "/index.html";
//   }
//   if (request_path.empty() || request_path[0] != '/') {
//     std::cout << "Respond with 400 - Bad Request\n";
//   }
//
//   // If file exists, open it and send response
//   // If file does not exist, send 404
//
//   // Convert string request to file path -- also remove the leading '/'
//   std::filesystem::path file_path{request_path.substr(1)};
//   std::cout << "We have reached the file_path constructor\n";
//   std::cout << file_path << '\n';
//
//   // What we want
//   std::filesystem::path doc_root{std::filesystem::current_path() /
//                                  "../files"};
//   doc_root = std::filesystem::weakly_canonical(doc_root);
//   std::cout << "doc_root: " << doc_root << "\n";
//
//   // Normalzed path
//   // std::filesystem::path normalized_path{
//   //     std::filesystem::weakly_canonical(want_path)};
//   // std::cout << "Normalized: " << normalized_path << "\n";
//
//   // Get full path
//   std::filesystem::path full_path{doc_root / file_path};
//   std::cout << "Final path: " << full_path << "\n";
//
//   // Normalize full path
//   full_path = std::filesystem::weakly_canonical(full_path);
//
//   // Validate path
//   if (full_path.string().find(doc_root.string()) != 0) {
//     std::cout << "Respond with 403 - Forbidden Access\n";
//   }
//
//   // Check if file exists and is a regular file
//   if (!std::filesystem::exists(full_path) ||
//       !std::filesystem::is_regular_file(full_path)) {
//     std::cout << "Respond with 404 - does not exist or not regular file\n";
//   }
//
//   // If file exists open it -- load the entire file into memory
//   std::vector<uint8_t> file_buffer{load_file(full_path)};
//   if (!file_buffer.empty()) {
//     std::cout << "I think we have loaded it in\n";
//   } else {
//     std::cout << "file buffer is empty\n";
//   }
//
//   // Figure out MIME Type for Content-Type
//   // request_path remains unchanged, so we can just tokenize based on '.'
//   // Then map the extension to the proper string and add the corrrect
//   string
//   // while building the response
//
//   std::vector<std::string> req_chunk = tokenize_input(request_path, ".");
//   std::string file_extension = req_chunk.back();
//   std::cout << file_extension << "\n\n";
//   std::cout << mime_map[file_extension] << "\n\n";
//
//   // Construct the HTTP response
//
//   // Success
//   // HTTP/1.1 200 OK\r\n
//   // Content-Length: <bytes>\r\n
//   // Content-Type: text/html\r\n
//   // \r\n
//   // <file bytes>
//
//   std::string success_response = "HTTP/1.1 200 OK\r\n"
//                                  "Content-Length: " +
//                                  std::to_string(file_buffer.size()) +
//                                  "\r\n"
//                                  "Content-Type: text/html\r\n"
//                                  "\r\n";
//   std::cout << "Created success_response:\n";
//   std::cout << success_response;
//
//   // Fail
//   // HTTP/1.1 404 Not Found\r\n
//   // Content-Length: 0\r\n
//   // \r\n
//   // <body>
//   // body is just text, so we can add it on to the response
//   //
//   // std::string fail_response = "HTTP/1.1 404 Not Found\r\n"
//   //                             "Content-Length: 0\r\n"
//   //                             "\r\n"
//   //                             "404 Not Found\n";
//   // std::cout << "Created fail_response:\n";
//   // std::cout << fail_response;
//
//   // Send the HTTP response
//   // Use send
//   // Since we want to send the uint8_t vector directly after the response,
//   we
//   // can just use 2 sends here like we did with the Chat Server (which sent
//   // the header then the payload)
//
//   // Send success response header
//   if (send(client_fd, success_response.data(), success_response.size(), 0)
//   ==
//       -1) {
//     std::cerr << "send success response header error\n";
//   } else {
//     std::cout << "sent success response header to client\n";
//   }
//
//   // Send success response body
//   if (send(client_fd, file_buffer.data(), file_buffer.size(), 0) == -1) {
//     std::cerr << "send response body error\n";
//   } else {
//     std::cout << "sent response body to client\n";
//   }
//
//   // Send fail response (header and body)
//   // if (send(client_fd, fail_response.data(), fail_response.size(), 0) ==
//   -1)
//   // {
//   //   std::cerr << "send fail response header error\n";
//   // } else {
//   //   std::cout << "sent fail response header to client\n";
//   // }
//
//   // if (send(client_fd, "Hello World!\n", strlen("Hello World!\n"), 0) ==
//   -1)
//   // {
//   //   std::cerr << "send error\n";
//   // }
//   // std::cout << "Sent message to client\n";
//
// Clean up
//   if (close(client_fd) == -1) {
//     std::cerr << "close error\n";
//   }
// }
// return 0;
// }
