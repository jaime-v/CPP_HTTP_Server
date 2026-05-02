#ifndef CREATE_RESPONSE_HPP
#define CREATE_RESPONSE_HPP

#include <cstdint>
#include <string>
#include <vector>
std::vector<uint8_t> build_error_response(int status_code);
std::vector<uint8_t> build_success_response(const std::string &mime_type,
                                            const std::vector<uint8_t> &file_buffer);
std::string create_response_phrase(int status_code);
#endif
