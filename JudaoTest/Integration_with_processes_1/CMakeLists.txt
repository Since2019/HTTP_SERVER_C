cmake_minimum_required(VERSION 3.15)
project(HTTP_SERVER)

set(CMAKE_C_STANDARD 11)

add_library(thread STATIC thread_handler.c)
target_link_libraries(thread pthread)
target_compile_options(thread PRIVATE -W -Wpedantic -Wall -Wextra -g -pg)
add_library(queue STATIC myqueue.c)
target_compile_options(queue PRIVATE -W -Wpedantic -Wall -Wextra -g)
add_library(get_conf STATIC get_configuration.c)
target_link_libraries(get_conf config)
target_compile_options(get_conf PRIVATE -W -Wpedantic -Wall -Wextra -g)
add_library(http_handle STATIC http_handler.c)
target_compile_options(http_handle PRIVATE -W -Wpedantic -Wall -Wextra -g)

include_directories(SYSTEM /usr/local/include)
link_directories(/usr/local/lib)

add_executable(curseform config_editor_dynamic.c)
target_link_libraries(curseform form config ncurses)
target_compile_options(curseform PRIVATE -W -Wpedantic -Wall -Wextra -g)

add_executable(server server.c)
target_link_libraries(server config pthread thread queue get_conf http_handle)
target_compile_options(server PRIVATE -W -Wpedantic -Wall -Wextra -g)
