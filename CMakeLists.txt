cmake_minimum_required(VERSION 3.6)
project(Moment_of_Inertia)

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c11")

set(SOURCE_FILES src/main.c)
add_executable(Moment_of_Inertia ${SOURCE_FILES})