cmake_minimum_required(VERSION 3.15)
project(HTTP_Server C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wpedantic -Wall -Wextra -g)

add_executable(HTTP_Server server.c config_defaults.h)
add_executable(http_handler http_handler.c http_handler.h)
add_executable(init_config init_config.c config_defaults.h create_configuration.c create_configuration.h get_configuration.c get_configuration.h)
target_link_libraries(http_handler init_config config pthread)

add_executable(get_config config_defaults.h get_configuration.c  get_configuration.h)
target_link_libraries(get_config config)