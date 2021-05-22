#pragma once

// Include C++ STL headers
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

// Include project headers
#include "macroprocessor.h"
#include "compiler.h"
#include "preprocessor.h"
#include "parser.h"

/**
 * The class that represents the application. It starts the
 * compilation processes and reads/writes programs.
 */
class Application {
public:
	/**
	 * The enum holds the modes in which the app can build
	 * the program
	 */
	enum CompilationMode {
		FULL = 0,
		PREPROCESSING,
		COMPILATION
	};

public :
    /**
     * Constructor
	 * @param input_file
	 * @param output_file
     */
    explicit Application(std::vector<std::string> input_files, std::string output_file);

	/**
	 * Set the 
	 * @param value mode of compilation
	 */
	void set_mode(const CompilationMode& value);

    /**
     * Start the execution
     */
    void start(void);

protected :
	std::shared_ptr<mmix::Compiler> compiler_;						// MMIX compiler
	std::vector<std::string> 		input_files_;					// The file with the original program
	std::string 					output_file_{""};				// The file to write the compiled program to
	CompilationMode 				mode_{CompilationMode::FULL};	//

protected:
	/**
	 * Read program from the given file
	 * @return array of program's lines
	 */
	std::shared_ptr<mmix::parser::RawProgram> read(void);

	/**
	 * Write the compiled program into the given file
	 * @param program the program to write
	 */
	void write(std::shared_ptr<mmix::compiler::CompiledProgram> program);

	/**
	 * Write the preprocessed program program into the given file
	 * @param program the program to write
	 */
	void write(std::shared_ptr<mmix::preprocessor::PreprocessedProgram> program);
};