add_executable(bench_linear bench_linear.cpp)
add_executable(bench_ktree bench_ktree.cpp)

target_link_libraries(bench_linear PRIVATE vdb_indexes vdb_core)
target_link_libraries(bench_ktree  PRIVATE vdb_indexes vdb_core)
