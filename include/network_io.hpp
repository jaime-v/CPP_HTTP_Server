#ifndef NETWORK_IO_HPP
#define NETWORK_IO_HPP

#include <sys/types.h>
#include <vector>

ssize_t recv_request(int client_fd, std::vector<uint8_t> &buffer);
ssize_t send_response(int client_fd, const std::vector<uint8_t> &buffer);

#endif
