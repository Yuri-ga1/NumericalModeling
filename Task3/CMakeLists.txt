cmake_minimum_required(VERSION 3.20)
project(Particles LANGUAGES CXX)

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")


add_executable(motion main.cpp particle.cpp clsSort.cpp clsWorld.cpp)

target_link_libraries(motion PRIVATE OpenMP::OpenMP_CXX)

set(CMAKE_VERBOSE_MAKEFILE off)
