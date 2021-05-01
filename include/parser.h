#pragma once

// Include C++ STL headers
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <algorithm>

// FIXME : remove
#include <iostream>

// Include project headers
#include "instruction.h"
#include "directives.h"
#include "mnemonics.h"
#include "sizes.h"
#include "exceptions.h"

namespace mmix {
	namespace parser {
		using RawProgram		= std::vector<std::string>;
		using ParsedProgram		= std::vector<mmix::Instruction>;
	}

	/**
	 * Class reads the original program and converts it into a
	 * vector of instructions 
	 */
	class Parser {
	protected : 
		using SplittedLine = std::vector<std::string>;		

		std::shared_ptr<parser::RawProgram> 	raw_;			// The raw strings of the program
		std::shared_ptr<parser::ParsedProgram> 	parsed_;		// The parsed version of the program

	protected :
		/**
		 * Remove comments from the line
		 * @param line the line to clear
		 * @return a clear line
		 */
		std::string remove_comments(std::string line);

		/**
		 * Split the line into a vector of tokens
		 * @param line the line to split
		 * @param delimiter the delimeter to use to split the line
		 * @return the split line
		 */
		SplittedLine split_line(std::string line, std::string delimiter);

		/**
		 * Fill an Instruction struct using the data from the string
		 * @param line 
		 * @return 
		 */
		void parse_line(std::string line);

		/**
		 * Parse the given program into a vector of structs
		 */
		void parse(void);

	public :
		/**
		 * Constructor
		 * @param program
		 */
		Parser(std::shared_ptr<mmix::parser::RawProgram> program);

		/**
		 * Get the parsed program
		 * @return the parsed program
		 */
		std::shared_ptr<parser::ParsedProgram> get(void);
	};
}