cmake_minimum_required(VERSION 3.5)

project(sign LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sign
    main.cpp
    commandlineapp.cpp
    commandlineapp.h
    commandlineparser.h
    commandlineparser.cpp
    filesigner.h
    filesigner.cpp)

install(TARGETS sign
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
