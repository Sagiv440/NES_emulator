cmake_minimum_required(VERSION 3.16.3)

project(Emulator)

file(GLOB_RECURSE SOURCE_FILE
        ${CMAKE_SOURCE_DIR}/*.c
        ${CMAKE_SOURCE_DIR}/*.cpp)

file(GLOB_RECURSE HEADER_FILES 
        ${CMAKE_SOURCE_DIR}/*.h
        ${CMAKE_SOURCE_DIR}/*.hpp)

add_library(CPU6502 CPU6502.cpp)
add_library(PPU PPU.cpp)
add_library(cartridge cartridge.cpp)
add_library(mapper mapper.cpp)

add_executable(${PROJECT_NAME} main.cpp )

target_include_directories(${PROJECT_NAME}
    PUBLIC ${SOURCE_DIR}
    PUBLIC SFML/include
)

target_link_directories(${PROJECT_NAME}
    PUBLIC SFML/lib
)

target_link_libraries(${PROJECT_NAME} 
	cartridge 
	CPU6502 PPU 
	mapper
	sfml-graphics
    	sfml-window
    	sfml-system
)
