cmake_minimum_required(VERSION 3.10)
project(problem_039_csr_matrix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(code src/main.cpp)
target_include_directories(code PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(code PRIVATE -O2 -pipe -static-libgcc -static-libstdc++)

