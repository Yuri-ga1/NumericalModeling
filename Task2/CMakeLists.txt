cmake_minimum_required(VERSION 3.20)
project(Integrals LANGUAGES CXX)

find_package(OpenMP REQUIRED)
set(CMAKE_CXX_STANDART 20)

add_executable(integrals main.cpp integrate.cpp)
target_link_libraries(integrals PRIVATE OpenMP::OpenMP_CXX)
