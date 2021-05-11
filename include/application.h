#pragma once

// Include C++ STL headers
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

// Include project headers
#include "compiler.h"
#include "preprocessor.h"
#include "parser.h"

/**
 * The class that represents the application. It starts the
 * compilation processes and reads/writes programs.
 */
class Application {
protected :
	std::shared_ptr<mmix::Compiler> compiler_;		// MMIX compiler
	std::string 					input_file_;	// The file with the original program
	std::string 					output_file_;	// The file to write the compiled program to

protected:
	/**
	 * Read program from the given file
	 * @return array of program's lines
	 */
	std::shared_ptr<mmix::parser::RawProgram> read_program(void);

	/**
	 * Write the compiled program into the given file
	 * @param program the program to write
	 */
	void write_program(std::shared_ptr<mmix::compiler::CompiledProgram> program);

public :
    /**
     * Constructor
	 * @param input_file
	 * @param output_file
     */
    explicit Application(std::string input_file, std::string output_file);

    /**
     * Start teh execution
     */
    void start(void);
};