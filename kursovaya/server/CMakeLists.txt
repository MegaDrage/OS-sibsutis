
cmake_minimum_required(VERSION 3.22.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(server LANGUAGES CXX)

add_executable(server server.cpp) 

target_compile_features(server PRIVATE cxx_std_17)

find_package(Boost 1.74.0 REQUIRED)
if (Boost_FOUND)
  target_include_directories(server PUBLIC ${Boost_INCLUDE_DIRS})
endif()

