cmake_minimum_required(VERSION 3.10)

project(TaskManagerServerClient)

# Устанавливаем стандарт C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем подпроекты
add_subdirectory(server)
add_subdirectory(client)
