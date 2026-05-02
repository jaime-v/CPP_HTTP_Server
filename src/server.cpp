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
    // Client connection
    int client{server.accept_client()};
    std::vector<uint8_t> request_buffer(BUF_SIZE);
    ssize_t recv_result{recv_request(client, request_buffer)};
    if (recv_result == 0) {
      std::cout << "Client closed connection\n";
      if (close(client) == -1) {
        std::cerr << "close client had an error\n";
      }
      continue;
    } else if (recv_result < 0) {
      std::cerr << "recv_request had error\n";
      if (close(client) == -1) {
        std::cerr << "close client had an error\n";
      }
      continue;
    }

    // Parse request
    http_request request{parse_request(request_buffer)};
    request_result result{validate_request(request)};
    std::vector<uint8_t> response{};
    if (!result.ok) {
      // Request isn't valid, so send response for 400 - bad request
      response = build_error_response(result.status_code);
      ssize_t send_result{send_response(client, response)};
      if (send_result == 0) {
        std::cout << "Client closed connection\n";
      } else if (send_result < 0) {
        std::cerr << "send_response had an error\n";
      }
      if (close(client) == -1) {
        std::cerr << "close client had an error\n";
      }
      continue;
    }

    // Build path to file
    path_result file_path_result{build_path(request)};
    if (!file_path_result.ok) {
      response = build_error_response(file_path_result.status_code);
      ssize_t send_result{send_response(client, response)};
      if (send_result == 0) {
        std::cout << "Client closed connection\n";
      } else if (send_result < 0) {
        std::cerr << "send_response had an error\n";
      }
      if (close(client) == -1) {
        std::cerr << "close client had an error\n";
      }
      continue;
    }

    // Load file and send
    std::vector<uint8_t> file_buffer{load_file(file_path_result.file_path)};
    if (file_buffer.empty()) {
      // No file, send 404
      response = build_error_response(404);
      ssize_t send_result{send_response(client, response)};
      if (send_result == 0) {
        std::cout << "Client closed connection\n";
      } else if (send_result < 0) {
        std::cerr << "send_response had an error\n";
      }
      if (close(client) == -1) {
        std::cerr << "close client had an error\n";
      }
      continue;
    }
    std::string mime_type{get_mime_type(request)};
    response = build_success_response(mime_type, file_buffer);
    ssize_t send_result{send_response(client, response)};
    if (send_result == 0) {
      std::cout << "Client closed connection\n";
      if (close(client) == -1) {
        std::cerr << "close client had an error\n";
      }
      continue;
    } else if (send_result < 0) {
      std::cerr << "send_response had an error\n";
      if (close(client) == -1) {
        std::cerr << "close client had an error\n";
      }
      continue;
    }

    // Close client if it is still here
    if (close(client) == -1) {
      std::cerr << "close client had an error\n";
    }
  }
  return 0;
}
