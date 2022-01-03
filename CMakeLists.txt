cmake_minimum_required(VERSION 3.21)
project(Final C)
project(test C)
set(CMAKE_C_STANDARD 99)

add_executable(Final client.c  client_screen.h client_socket.h server_socket.h)
add_executable(test test.c)