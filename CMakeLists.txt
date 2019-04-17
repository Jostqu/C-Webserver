cmake_minimum_required(VERSION 3.13)
project(PSE C)

set(CMAKE_C_STANDARD 11)

add_executable(PSE echo_server.c string.h string.c)