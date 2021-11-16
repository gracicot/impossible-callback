add_library(bench-common INTERFACE)
add_executable(benchmarks benchmarks.cpp)

find_package(benchmark CONFIG REQUIRED)

target_link_libraries(bench-common INTERFACE impossible-callback::impossible-callback benchmark::benchmark benchmark::benchmark_main)
target_include_directories(bench-common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(benchmarks PRIVATE bench-common)
