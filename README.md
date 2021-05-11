# MMIX Compiler
Compiler for D.Knuth's computer architecture MMIX.

## Table of contents
* [Prerequisites](#prerequisites)
* [Building](#building-the-project)
* [Usage](#usage)
	* [How to run](#running-the-app)

## Prerequisites
In order to build use the application you should install the following software:
* `g++`
* `cmake`

## Building the project
To build the project just run the following commands :
```bash
$ mkdir build && cd build
$ cmake ..
$ make
```

Or just build the app in MSVS using clang compiler (if you use Windows).

## Usage
### Running the app
Starting the application is fairly simple :
```bash
$ ./assembler <input_file> <output_file>
```