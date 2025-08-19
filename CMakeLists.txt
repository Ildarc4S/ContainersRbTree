cmake_minimum_required(VERSION 3.20)
project(S21Containers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(src/tests)

