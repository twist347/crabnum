add_executable(bench_crabnum main.cpp)
target_link_libraries(bench_crabnum crabnum::crabnum benchmark::benchmark_main)
target_compile_options(bench_crabnum PRIVATE -Wall -Wextra -pedantic)