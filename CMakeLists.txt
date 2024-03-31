cmake_minimum_required(VERSION 3.20)

project(SearchEngine VERSION 0.2)

set(CMAKE_CXX_STANDARD 20)

set(SE_INCLUDE ${CMAKE_SOURCE_DIR}/include)

set(SE_SRC ${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)

add_subdirectory(tests)