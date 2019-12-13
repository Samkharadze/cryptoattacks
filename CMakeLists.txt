cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_COVERAGE "Build coverage" OFF)


project(Chopch.cpp)
string(APPEND CMAKE_CXX_FLAGS " -Wnarrowing ")




find_package(Threads)

add_executable(
  ${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/choph.cpp
)





