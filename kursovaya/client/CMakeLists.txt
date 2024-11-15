cmake_minimum_required(VERSION 3.22.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(client LANGUAGES CXX)

add_executable(client client.cpp)

target_compile_features(client PRIVATE cxx_std_17)

find_package(Boost 1.74.0 REQUIRED)
find_package(ftxui 5 REQUIRED)
target_link_libraries(client PRIVATE ftxui::screen
PRIVATE ftxui::component
PRIVATE ftxui::dom
)
if (Boost_FOUND)
  target_include_directories(client PUBLIC ${Boost_INCLUDE_DIRS})
endif()

