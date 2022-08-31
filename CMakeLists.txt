cmake_minimum_required(VERSION 3.5)

project(sign LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sign
    src/main.cpp
    src/commandlineapp.cpp
    src/commandlineapp.h
    src/commandlineparser.h
    src/commandlineparser.cpp
    src/filesigner.h
    src/filesigner.cpp)

install(TARGETS sign
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory(tests)
