add_executable(test_crabnum main.cpp)
target_link_libraries(test_crabnum crabnum::crabnum GTest::gtest_main)
target_compile_options(test_crabnum PRIVATE -Wall -Wextra -pedantic)
add_test(NAME test_crabnum COMMAND test_crabnum)
