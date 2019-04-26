cmake_minimum_required(VERSION 3.10.2)
project(PSE C)

set(CMAKE_C_STANDARD 11)

add_executable(PSE echo_server.c string.h string.c hash.h hash.c)