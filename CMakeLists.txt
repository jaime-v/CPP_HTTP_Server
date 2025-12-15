cmake_minimum_required(VERSION 3.16)
project(
    http_server
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server
    src/server.cpp
)

add_executable(client
    src/client.cpp
)

target_include_directories(
    server
    PRIVATE
    include
)

target_include_directories(
    client
    PRIVATE
    include
)


target_compile_options(server PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(server PRIVATE -fsanitize=address,undefined)
target_link_options(server PRIVATE -fsanitize=address,undefined)

target_compile_options(client PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(client PRIVATE -fsanitize=address,undefined)
target_link_options(client PRIVATE -fsanitize=address,undefined)
