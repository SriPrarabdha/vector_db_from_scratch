cmake_minimum_required(VERSION 3.16)
project(vector_db LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(OpenMP)
# if(OpenMP_CXX_FOUND)
#     target_link_libraries(vdb PUBLIC OpenMP::OpenMP_CXX)
# endif()
# target_compile_options(vdb PRIVATE -mavx2 -mfma)

add_subdirectory(core)
add_subdirectory(indexes)
add_subdirectory(bench)
add_subdirectory(tools)
