#include "create_response.hpp"

std::string create_response_phrase(int status_code){
  switch(status_code){
    case 400: return "Bad Request";
    case 404: return "Not Found";
    case 405: return "Method Not Allowed";
    case 505: return "HTTP Version Not Supported";
    default: return "Internal Server Error";
  }
}

std::vector<uint8_t> build_error_response(int status_code) {
  std::string response_phrase = create_response_phrase(status_code);
  std::string response_body = std::to_string(status_code) + " " + response_phrase + "\n";
  std::string response_string{"HTTP/1.1 " + std::to_string(status_code) +
                              " " + response_phrase + "\r\n"
                              "Content-Type: text/plain\r\n"
                              "Content-Length: " + std::to_string(response_body.size()) + "\r\n"
                              "\r\n" + response_body
  };
  // Can just return this directly
  // std::vector<uint8_t> response{response_string.begin(), response_string.end()};
  return {response_string.begin(), response_string.end()};
}

std::vector<uint8_t> build_success_response(const std::string &mime_type,
                                            const std::vector<uint8_t> &file_buffer) {
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
