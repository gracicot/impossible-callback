add_library(bench-helper bench-helper.cpp)
add_executable(benchmarks benchmarks.cpp)

find_package(benchmark CONFIG REQUIRED)

target_link_libraries(benchmarks PRIVATE bench-helper impossible-callback::impossible-callback benchmark::benchmark benchmark::benchmark_main)
target_link_libraries(bench-helper PRIVATE impossible-callback::impossible-callback)
target_compile_features(bench-helper PUBLIC cxx_std_17)
