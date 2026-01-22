#ifndef LOAD_FILE_HPP
#define LOAD_FILE_HPP

#include <filesystem>
#include <vector>

std::vector<uint8_t> load_file(const std::filesystem::path &path);

#endif
