#ifndef PARSE_REQUEST_HPP
#define PARSE_REQUEST_HPP

#include <string>
#include <vector>

#include "http_types.hpp"

struct request_result {
  bool ok{true};
  int status_code{};
};

std::vector<std::string> tokenize_string(std::string string,
                                         const std::string &delim);
http_request parse_request(const std::vector<uint8_t> &buffer);
request_result validate_request(http_request &request);
std::string get_mime_type(const http_request &request);

#endif
