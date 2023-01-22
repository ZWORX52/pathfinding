cmake_minimum_required(VERSION 3.20)

project(pathfinding)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(pathfinding main.cpp grid.cpp astar.cpp)

target_link_libraries(pathfinding ncurses)
