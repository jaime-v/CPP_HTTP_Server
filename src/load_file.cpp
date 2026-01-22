#include <fstream>
#include <iostream> // for cerr

#include "load_file.hpp"
// Load file into memory

// Takes const path reference
// Loads a file into memory
// Opens up a filestream using path -- binary?
// If we failed to open a file, throw error
// Seek to end
// Get current cursor position (how many bytes are in the file)
// Seek back to beginning
// Allocate buffer based on file size
// Read file data into buffer.data()
//      buffer.data() is a uint8_t *
//      std::ifstream::read expects a char *
//      So we need to reinterpret_cast to char *, taking buffer.data()
//      Then we can successfully read into buffer.data
//
//      Basically, cast buffer.data() into char *, then read size bytes into it
// Return buffer
std::vector<uint8_t> load_file(const std::filesystem::path &path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);

  if (!file) {
    // throw std::runtime_error("failed to open file");
    // Might want to look into std::optional and std::nullopt
    std::cerr << "Failed to open file\n";
    return {};
  }

  std::size_t size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(size);
  file.read(reinterpret_cast<char *>(buffer.data()), size);

  return buffer;
}
