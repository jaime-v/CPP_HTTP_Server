#ifndef PATH_BUILDING_HPP
#define PATH_BUILDING_HPP

#include "http_types.hpp"
#include <filesystem>

struct path_result {
  bool ok{true};
  int status_code{};
  std::filesystem::path file_path{};
};

path_result build_path(const http_request &request);

#endif
