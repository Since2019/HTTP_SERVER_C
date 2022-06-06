cmake_minimum_required(VERSION 3.15)
project(HTTP_SERVER)

set(CMAKE_C_STANDARD 11)
add_compile_options(-W -Wpedantic -Wall -Wextra -g)


# 
add_executable(http_handler http_handler.c)
add_executable(init_config init_config.c config_defaults.h create_configuration.c create_configuration.h get_configuration.c get_configuration.h)
add_executable(server server.h server.c  config_defaults.h)

# link libraries together
target_link_libraries(http_handler init_config config pthread)
