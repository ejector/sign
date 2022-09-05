cmake_minimum_required(VERSION 3.5)

add_executable(
    sign_test
    filesigner_tests.cpp
    randomizer.h
    ../src/filesigner.h
    ../src/filesigner.cpp
)

target_link_libraries(
    sign_test
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(sign_test)
