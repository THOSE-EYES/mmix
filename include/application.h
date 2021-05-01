#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "compiler.h"
#include "preprocessor.h"
#include "parser.h"

/**
 * 
 */
class Application {
protected :
	std::shared_ptr<mmix::Compiler> compiler_;		//
	std::string 					input_file_;	//
	std::string 					output_file_;	//

protected:
	/**
	 * 
	 * @return 
	 */
	std::shared_ptr<mmix::parser::RawProgram> read_program(void);

	/**
	 * 
	 * @param program 
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
     * 
     */
    void start(void);
};