#set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
#set(CMAKE_CUDA_ARCHITECTURES "native")
cmake_minimum_required(VERSION 3.31)
#project(factorial CUDA)
project(factorial CXX)
#add_compile_options(-G)

#set(CMAKE_CUDA_STANDARD 20)

find_package(OpenMP REQUIRED)

add_executable(factorial main.cpp
        math.hpp
        factorial.hpp
        perf.hpp
        eratosthenes.hpp)

target_link_libraries(factorial OpenMP::OpenMP_CXX)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-long-long -pedantic")

#
#set_target_properties(factorial PROPERTIES
#        CUDA_SEPARABLE_COMPILATION ON)
