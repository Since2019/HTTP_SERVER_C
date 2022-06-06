cmake_minimum_required(VERSION 3.15)
project(HTTP_SERVER)

set(CMAKE_C_STANDARD 11)
add_compile_options(-W -Wpedantic -Wall -Wextra -g)


# 
add_executable(http_handler http_handler.c http_handler.h)
add_executable(server server.h server.c  config_defaults.h)

# link libraries together
target_link_libraries(http_handler init_config config pthread)
