cmake_minimum_required(VERSION 3.26)
project(Prog4MiniginProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Minigin)
add_subdirectory(Game)
