#include <iostream>

#include "logging.hpp"

void print_byte_vector(std::vector<uint8_t> vector) {
  std::cout << "\nVector print begin\n";
  for (std::size_t i{}; i < vector.size(); ++i) {
    printf("(%02x) ", vector[i]);
  }
  for (std::size_t i{}; i < vector.size(); ++i) {
    printf("(%c) ", vector[i]);
  }
  std::cout << "\nVector print end\n";
}

void print_string_vector(std::vector<std::string> vector) {
  std::cout << "\nVector print begin\n";
  for (std::size_t i{}; i < vector.size(); ++i) {
    std::cout << i << "\n";
    std::cout << vector[i] << "\n";
    for (std::size_t j{}; j < vector[i].length(); ++j) {
      printf("(%02x) ", vector[i][j]);
    }
    std::cout << "\n\n";
  }
  std::cout << "\nVector print end\n";
}
