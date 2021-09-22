cmake_minimum_required(VERSION 3.16.3)

project(Emulator)

file(GLOB_RECURSE SOURCE_FILE
        ${CMAKE_SOURCE_DIR}/*.c
        ${CMAKE_SOURCE_DIR}/*.cpp)

file(GLOB_RECURSE HEADER_FILES 
        ${CMAKE_SOURCE_DIR}/*.h
        ${CMAKE_SOURCE_DIR}/*.hpp)

add_executable(${PROJECT_NAME} main.cpp)