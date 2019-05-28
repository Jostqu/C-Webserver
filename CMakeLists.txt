cmake_minimum_required(VERSION 3.10.2)
project(pse20 C)

set(CMAKE_C_STANDARD 11)

add_executable(pse20 echo_server.c string.h string.c hash.h hash.c response.h response.c request.h request.c helpers.h helpers.c tests.c base64.c base64.h authorization.c authorization.h)
