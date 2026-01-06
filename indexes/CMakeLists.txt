add_library(vdb_indexes
    linear_scan.cpp
    kd_tree.cpp
    # ivf.cpp
)

target_link_libraries(vdb_indexes PUBLIC vdb_core)
target_include_directories(vdb_indexes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
