#include "create_response.hpp"

std::vector<uint8_t> build_error_response(int status_code) {
  std::string response_string{"HTTP/1.1 " + std::to_string(status_code) +
                              " Not Found\r\n"
                              "Content-Length: 0\r\n"
                              "\r\n"
                              "404 Not Found\n"};
  std::vector<uint8_t> response{response_string.begin(), response_string.end()};
  return response;
}

std::vector<uint8_t> build_success_response(std::string mime_type,
                                            std::vector<uint8_t> file_buffer) {
  std::string response_string{"HTTP/1.1 200 OK\r\n"
                              "Content-Length: " +
                              std::to_string(file_buffer.size()) +
                              "\r\n"
                              "Content-Type: " +
                              mime_type +
                              "\r\n"
                              "\r\n"};
  std::vector<uint8_t> response{response_string.begin(), response_string.end()};
  response.insert(response.end(), file_buffer.begin(), file_buffer.end());
  return response;
}
