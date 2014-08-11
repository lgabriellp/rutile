cmake_minimum_required(VERSION 2.8)
project(unsockets)

add_definitions(-Wall -Werror -g -ggdb)
include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)
