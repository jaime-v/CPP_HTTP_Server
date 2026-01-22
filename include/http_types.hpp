#ifndef HTTP_TYPES_HPP
#define HTTP_TYPES_HPP

#include <string>
#include <vector>

struct http_request {
  std::string method{};
  std::string request_path{};
  std::string version{};
};

struct http_response {
  std::string header{};
  std::vector<uint8_t> body{};
};

#endif
