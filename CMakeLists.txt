project (emuc)
cmake_minimum_required (VERSION 3.18)

find_package (SDL2 REQUIRED)

option (BUILD_SHARED_LIBS "Build shared libraries" ON)

add_subdirectory (src)
