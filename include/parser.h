/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#pragma once

// Include C++ STL headers
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <algorithm>
#include <sstream>

// FIXME : remove
#include <iostream>

// Include project headers
#include "instruction.h"
#include "directives.h"
#include "mnemonics.h"
#include "macros.h"
#include "sizes.h"
#include "exceptions.h"

namespace mmix {
	namespace parser {
		using RawFile		= std::vector<std::string>;
		using RawProgram 	= std::map<std::string, std::shared_ptr<RawFile>>;
		using ParsedFile	= std::vector<std::shared_ptr<Instruction>>;
		using ParsedProgram	= std::map<std::pair<std::string, bool>, std::shared_ptr<ParsedFile>>;
	}

	/**
	 * Class reads the original program and converts it into a
	 * vector of instructions 
	 */
	class Parser {
	protected : 
		using SplittedLine = std::vector<std::string>;		

		std::shared_ptr<parser::RawProgram> 	raw_;		// The raw strings of the program
		std::shared_ptr<parser::ParsedProgram> 	parsed_;	// The parsed version of the program

	protected :
		/**
		 * Remove comments from the line
		 * @param line the line to clear
		 * @return a clear line
		 */
		std::string remove_comments(std::string line);

		/**
		 * Fill an Instruction struct using the data from the string
		 * @param line the line to parse
		 * @return instruction
		 */
		std::shared_ptr<Instruction> parse_line(std::string line);

		/**
		 * Parse the given program into a vector of structs
		 */
		void parse(void);

		/**
		 * Create an object of a concrete type
		 * @param token an unknown token 
		 * @return a new object
		 */
		std::shared_ptr<Instruction> create_instruction(const std::string& token);

	public :
		/**
		 * Constructor
		 * @param program the program to parse
		 */
		Parser(std::shared_ptr<mmix::parser::RawProgram> program);

		/**
		 * Get the parsed program
		 * @return the parsed program
		 */
		std::shared_ptr<parser::ParsedProgram> get(void);

		/**
		 * Split the line into a vector of tokens
		 * @param line the line to split
		 * @param delimiter the delimeter to use to split the line
		 * @param count the last element to be split
		 * @return the split line
		 */
		static SplittedLine split_line(std::string line, std::string delimiter = " ", uint64_t count = 0);

		/**
		 * Replace a substring with another
		 * @param str the string where to replace the substring
		 * @param f_sbstr the substring to replace
		 * @param r_sbstr the substring which will replace the original one
		 * @return a string with replaced substrings
		 */
		static std::string replace_substr(std::string str, 
			const std::string& f_sbstr, 
			const std::string& r_sbstr);
	};
} // namespace mmix