cmake_minimum_required(VERSION 3.25)

project(pathfinding)

# if (NOT CMAKE_BUILD_TYPE)
#     set(CMAKE_BUILD_TYPE Debug)
# endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(pathfinding main.cpp grid.cpp astar.cpp render.cpp)

target_compile_options(pathfinding PRIVATE -Wall -Wextra -Werror -fdiagnostics-color)
# target_compile_options(pathfinding PRIVATE $<$<CONFIG:Debug>:
#     -Og
#   >)

target_link_libraries(pathfinding ncurses fmt)
