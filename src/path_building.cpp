#include <iostream>

#include "path_building.hpp"
// struct path_result {
//   bool ok{};
//   int status_code{};
// };

path_result build_path(const http_request &request) {
  path_result result{};
  // Convert string request to file path -- also remove the leading '/'
  std::filesystem::path file_path{request.request_path.substr(1)};
  std::cout << "We have reached the file_path constructor\n";
  std::cout << file_path << '\n';

  // What we want
  std::filesystem::path doc_root{std::filesystem::current_path() / "../files"};
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
  result.file_path = full_path;

  // Validate path
  if (full_path.string().find(doc_root.string()) != 0) {
    std::cout << "Respond with 403 - Forbidden Access\n";
    result.ok = false;
    result.status_code = 403;
  }

  // Check if file exists and is a regular file
  if (!std::filesystem::exists(full_path) ||
      !std::filesystem::is_regular_file(full_path)) {
    std::cout << "Respond with 404 - does not exist or not regular file\n";
    result.ok = false;
    result.status_code = 404;
  }
  return result;
}
