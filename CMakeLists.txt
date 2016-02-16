cmake_minimum_required(VERSION 3.3)
project(rot13)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CLIENT_FILES client.c)
set(SERVER_FILES server.c)
add_executable(client ${CLIENT_FILES})
add_executable(server ${SERVER_FILES})