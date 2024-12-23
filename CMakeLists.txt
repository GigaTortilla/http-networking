cmake_minimum_required(VERSION 3.30)
project(http_server C)

set(CMAKE_C_STANDARD 23)

add_executable(http_server main.c)
add_library(http_server_lib STATIC
        utils.c include/utils.h
        server.c include/utils.h
        win_server.c include/win_server.h)

target_include_directories(http_server_lib PUBLIC include)
target_include_directories(http_server PUBLIC include)

if (WIN32)
    target_link_libraries(http_server LINK_PUBLIC http_server_lib ws2_32)
else ()
    target_link_libraries(http_server LINK_PUBLIC http_server_lib)
endif ()
