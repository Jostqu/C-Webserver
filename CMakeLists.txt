cmake_minimum_required(VERSION 3.10.2)
project(pse C)

set(CMAKE_C_STANDARD 11)

add_executable(PSE echo_server.c string.h string.c hash.h hash.c response.h response.c http_request.h http_request.c helpers.h helpers.c)

