cmake_minimum_required(VERSION 3.10)
project(find_hidden_treasure C)
set(CMAKE_C_STANDARD 99)

find_package(Curses REQUIRED)
find_package(Threads REQUIRED)

add_executable(client client.c client_screen.h client_socket.h)
target_include_directories(client PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(client PRIVATE ${CURSES_LIBRARIES})

add_executable(server server.c server_socket.h)
target_link_libraries(server PRIVATE Threads::Threads)
