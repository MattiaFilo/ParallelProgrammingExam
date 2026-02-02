cmake_minimum_required(VERSION 3.20)

project(ParallelProgrammingExam LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ParallelProgrammingExam
    src/main.cpp
)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(ParallelProgrammingExam PRIVATE OpenMP::OpenMP_CXX)
    target_compile_definitions(ParallelProgrammingExam PRIVATE USE_OPENMP)
endif()
