cmake_minimum_required(VERSION 3.16)
project(
    http_server
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Also need to do ln -s build/compile_commands.json . to link it

add_executable(server
    src/server.cpp
    src/network_io.cpp
    src/socket_setup.cpp
    src/create_response.cpp
    src/parse_request.cpp
    src/load_file.cpp
    src/path_building.cpp
)

add_executable(client
    src/client.cpp
)

target_include_directories(
    server
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_include_directories(
    client
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)


target_compile_options(server PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(server PRIVATE -fsanitize=address,undefined)
target_link_options(server PRIVATE -fsanitize=address,undefined)

target_compile_options(client PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(client PRIVATE -fsanitize=address,undefined)
target_link_options(client PRIVATE -fsanitize=address,undefined)
