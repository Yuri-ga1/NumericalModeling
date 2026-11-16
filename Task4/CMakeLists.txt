cmake_minimum_required(VERSION 3.20)
project(Particles LANGUAGES CXX)

find_package(OpenMP REQUIRED)
find_package(HDF5 REQUIRED COMPONENTS C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

# Рекурсивно ищет .cpp файлы в app/
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "app/*.cpp")

if(APP_SOURCES)
    list(FILTER APP_SOURCES EXCLUDE REGEX [=[\.ipynb_checkpoints]=])
endif()

message(STATUS "CPP FILES FOUNDED IN PROJECT ${APP_SOURCES}")

add_executable(motion main.cpp ${APP_SOURCES})

target_include_directories(motion PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(motion PRIVATE OpenMP::OpenMP_CXX hdf5::hdf5_cpp hdf5::hdf5)

set(CMAKE_VERBOSE_MAKEFILE off)
