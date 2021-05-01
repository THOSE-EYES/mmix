#Setting CMake
cmake_minimum_required(VERSION 3.15)
project(mmix)

#Setting the compiler
if (WIN32)
	set(CXXFLAGS "")
	set(CMAKE_CXX_STANDARD_LIBRARIES "")
endif (WIN32)

if (UNIX)
	set(CMAKE_CXX_COMPILER "g++")
	set(CXXFLAGS "")
	set(CMAKE_CXX_STANDARD_LIBRARIES "")
endif (UNIX)

#Looking for source files
include_directories(include)

file(GLOB SOURCES "src/*.cpp")

#Command to compile the whole project
add_executable(assembler ${SOURCES})