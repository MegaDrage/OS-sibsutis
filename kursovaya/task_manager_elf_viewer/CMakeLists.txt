cmake_minimum_required(VERSION 3.11)

project(TaskManagerAndElfViewer LANGUAGES CXX VERSION 1.0.0)

find_package(PkgConfig REQUIRED)
find_package(ftxui 5 REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(TaskManager main.cpp)
target_link_libraries(TaskManager PRIVATE ftxui::screen
PRIVATE ftxui::component
PRIVATE ftxui::dom
)
