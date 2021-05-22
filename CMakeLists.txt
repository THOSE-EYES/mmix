#Setting CMake
cmake_minimum_required(VERSION 3.15)
project(mmix)

set (CMAKE_CXX_STANDARD 17)

#Setting the compiler
if (WIN32)
	set(CXXFLAGS "")
	set(CMAKE_CXX_STANDARD_LIBRARIES "")
endif (WIN32)

if (UNIX)
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
	set(CMAKE_CXX_COMPILER "g++")
	set(CXXFLAGS "-g")
	set(CMAKE_CXX_STANDARD_LIBRARIES "-lboost_program_options")
endif (UNIX)

#Looking for source files
include_directories(include)

file(GLOB SOURCES "src/*.cpp")

#Command to compile the whole project
add_executable(assembler ${SOURCES})