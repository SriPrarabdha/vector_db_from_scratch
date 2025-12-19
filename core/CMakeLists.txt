add_library(vdb_core
    vector.cpp
    distance.cpp
)

target_include_directories(vdb_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
