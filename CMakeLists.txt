cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(tinyServer LANGUAGES CXX)

add_subdirectory(app)
add_subdirectory(tinyServerCore)
# add_subdirectory(tinyServerUserModule)
