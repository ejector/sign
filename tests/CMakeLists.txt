cmake_minimum_required(VERSION 3.5)

add_executable(
    sign_test
    filesigner_tests.cpp
    randomizer.h
    ../src/signer.h
    ../src/signer.cpp
    ../src/hashrot13.h
    ../src/signaturefile.h
)

target_link_libraries(
    sign_test
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(sign_test)
