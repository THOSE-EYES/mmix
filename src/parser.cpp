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

#include "parser.h"

using mmix::parser::RawProgram;
using mmix::parser::RawFile;
using mmix::parser::ParsedFile;
using mmix::parser::ParsedProgram;
using mmix::preprocessor::directives;
using mmix::compiler::mnemonics;
using mmix::compiler::sizes;
using mmix::macroprocessor::macros;
using mmix::exceptions::parser::WrongLineException;
using mmix::exceptions::parser::UnexpectedTokenException;

namespace mmix {
	Parser::Parser(std::shared_ptr<RawProgram> program) :
		raw_{program},
		parsed_{std::make_shared<parser::ParsedProgram>()} {
		parse();
	}

	Parser::SplittedLine Parser::split_line(std::string line, std::string delimiter, uint64_t count) {
		std::vector<std::string>	result;
		size_t 						pos = 0;

		// Look up for the delimiter
		while (((pos = line.find(delimiter)) != std::string::npos) and (count != 1)) {
			// If the token is found, push it
			if (pos != 0) {
				result.push_back(line.substr(0, pos));
				--count;
			}

			// Erase the added element
			line.erase(0, pos + delimiter.length());
		}

		// Push the last token to the result
		if (pos != 0 and not line.empty()) result.push_back(line);

		return result;
	}

	// FIXME : rewrite using data from Lexer
	std::shared_ptr<Instruction> Parser::parse_line(std::string line) {
		std::vector<std::string> 		split = split_line(line, " ", 4);
		std::vector<std::string> 		parameters;

		// Sanity check
		if (split.size() < 1) throw WrongLineException(line);

		// If the size is > 2, then the instruction contains a label
		std::string label = (split.size() > 2) ? split.at(0) : "";
		std::string token = (split.size() > 2) ? split.at(1) : split.at(0);

		// Create an object
		auto instruction = create_instruction(token);
		if (not instruction) throw WrongLineException(line);

		// Parse the parameters
		if (split.size() == 4) {
			parameters = split_line(split.at(split.size() - 2), ",");

			// Save macro-specific info (only for "MACRO"'s)
			auto& expression = std::dynamic_pointer_cast<Macro>(instruction)->expression;
			expression = parse_line(split.back());
		}
		else parameters = split_line(split.at(split.size() - 1), ",");
		
		// Save the common variables
		instruction->parameters = parameters;
		instruction->label 		= label;

		return instruction;
	}

	void Parser::parse(void) {
		for (auto file : *raw_) {
			auto filename		= file.first;
			auto parsed_file	= std::make_shared<ParsedFile>();
			bool is_main 		= false;

			for (auto& line : *file.second) {
				// Remove unnecessary tabs, spaces and comments
				//remove_comments(line);

				// Parse the line if it's not empty
				if (not line.empty()) {
					auto instruction = parse_line(line);
					if (instruction->label == "Main") {
						is_main = true;
						instruction->label.clear();
					} 

					// Save a new parsed instruction
					parsed_file->push_back(instruction);
				}
			}

			// Store a new filled file
			parsed_->insert(std::make_pair(std::make_pair(filename,is_main), parsed_file));
		}
	}

	std::shared_ptr<parser::ParsedProgram> Parser::get(void) {
		return parsed_;
	}

	std::shared_ptr<Instruction> Parser::create_instruction(const std::string& token) {
		InstructionFactory 	factory;

		// Create an object depending on the token
		if (std::find(macros.begin(), macros.end(), token) != macros.end()) {
			auto concrete_instruction 	= factory.create_macro();
			concrete_instruction->type 	= token;
				
			return concrete_instruction;
		}
		else if (std::find(directives.begin(), directives.end(), token) != directives.end()) {
			auto concrete_instruction = factory.create_directive();
			concrete_instruction->directive = token;

			return concrete_instruction;
		}
		else if (mnemonics.find(token) != mnemonics.end()) {
			auto concrete_instruction = factory.create_mnemonic(); 
			concrete_instruction->mnemonic = token;
	
			return concrete_instruction;
		}
		else if (sizes.find(token) != sizes.end()) {
			auto concrete_instruction = factory.create_allocator();
			concrete_instruction->size = token;

			return concrete_instruction;
		}
		else return std::shared_ptr<Instruction>();
	}

	std::string Parser::replace_substr(std::string str, 
			const std::string& f_sbstr, 
			const std::string& r_sbstr) {
		size_t index = 0;

		while (true) {
     		// Locate the substring to replace
     		index = str.find(f_sbstr, index);
     		if (index == std::string::npos) break;

 			// Replace the substring
			str.erase(index, f_sbstr.size());
			str.insert(index, r_sbstr);

			// Get to the next occurence
 			index += r_sbstr.size();
		}

		return str;
	}
}