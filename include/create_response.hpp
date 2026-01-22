#ifndef CREATE_RESPONSE_HPP
#define CREATE_RESPONSE_HPP

#include <cstdint>
#include <string>
#include <vector>
std::vector<uint8_t> build_error_response(int status_code);
std::vector<uint8_t> build_success_response(std::string mime_type,
                                            std::vector<uint8_t> file_buffer);
#endif
