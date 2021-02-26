cmake_minimum_required(VERSION 3.0.0)
project(PuhuLang VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CTest)
enable_testing()

file(GLOB_RECURSE src_files 
    "src/*.h" 
    "src/*.hpp"
    "src/*.cpp")

add_executable(puhuc ${src_files})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
