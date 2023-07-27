cmake_minimum_required(VERSION 3.27.0)

project(SerialCPP VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Add An Option To Build The Examples
option(BUILD_EXAMPLES "Build The Examples" ON)

add_library(SerialCPP STATIC)

target_sources(SerialCPP PRIVATE
    "src/SerialCPP.cpp"
)

target_include_directories(SerialCPP PUBLIC
    "include"
)

# Build Examples If Enabled
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
