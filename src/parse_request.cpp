#include <iostream> // debugging

#include "parse_request.hpp"

// Static keyword here?? because its global
static const std::unordered_map<std::string, std::string> mime_map = {
    {"html", "text/html"},
    {"css", "text/css"},
    {"js", "text/js"},
    {"jpg", "image/jpeg"},
    {"jpeg", "image/jpeg"},
    {"png", "image/png"},
    {"ico", "image/vnd.microsoft.icon"},
    {"json", "application/json"},
    {"mp3", "audio/mpeg"},
    {"mp4", "video/mp4"}};

// It looks like it modifies strings internally, but doesnt
// I guess since we are passing it by value, not by reference
std::vector<std::string> tokenize_string(std::string string,
                                         const std::string &delim) {
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

http_request parse_request(const std::vector<uint8_t> &buffer) {
  http_request request{};
  std::string request_string{buffer.begin(), buffer.end()};
  std::vector<std::string> lines{tokenize_string(request_string, "\r\n")};

  // Check to make sure lines has something
  if (lines.empty()) {
    return request;
  }

  std::vector<std::string> request_line{tokenize_string(lines[0], " ")};

  if (request_line.size() < 3) {
    return request;
  }

  request.method = request_line[0];
  request.request_path = request_line[1];
  request.version = request_line[2];
  return request;
}

// validate request
request_result validate_request(http_request &request) {
  request_result result{};
  if (request.method != "GET") {
    std::cout << "Respond with 405 - Method Not Allowed\n";
    result.ok = false;
    result.status_code = 405;
  }
  if (request.version != "HTTP/1.1" && request.version != "HTTP/1.0") {
    std::cout << "Respond with 505 - HTTP Version Not Supported\n";
    result.ok = false;
    result.status_code = 505;
  }
  if (request.request_path.empty() || request.request_path[0] != '/') {
    std::cout << "Respond with 400 - Bad Request\n";
    result.ok = false;
    result.status_code = 400;
  }

  // Normalize path
  if (request.request_path == "/") {
    std::cout << "Path is just /, swapping to /index.html\n";
    request.request_path = "/index.html";
  }
  return result;
}

std::string get_mime_type(const http_request &request) {
  std::vector<std::string> req_chunk =
      tokenize_string(request.request_path, ".");
  std::string file_extension = req_chunk.back();

  // This is a gross tolower function i found on stack overflow
  std::transform(file_extension.begin(), file_extension.end(),
                 file_extension.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  std::string mime_type{};
  // Mime map should use find (adapted this from cppreference) when map is const
  if (auto search = mime_map.find(file_extension); search != mime_map.end()) {
    // If we found the file extension as a key, we get the value
    mime_type = search->second;
  } else {
    // Otherwise, we default to application/octet-stream
    // and we should return 415 for Unsupported Media Type, but I'm too lazy to
    // do that for now
    mime_type = "application/octet-stream";
  }

  return mime_type;
}
