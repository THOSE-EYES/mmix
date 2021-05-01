#include "parser.h"

using mmix::preprocessor::directives;
using mmix::compiler::mnemonics;
using mmix::compiler::sizes;
using mmix::exceptions::parser::WrongLineException;
using mmix::exceptions::parser::UnexpectedTokenException;

namespace mmix {
	Parser::Parser(std::shared_ptr<parser::RawProgram> program) :
		raw_{program},
		parsed_{std::make_shared<parser::ParsedProgram>()} {
		parse();
	}

	Parser::SplittedLine Parser::split_line(std::string line, std::string delimiter) {
		std::vector<std::string>	result;
		size_t 						pos = 0;

		// Look up for the delimiter
		while ((pos = line.find(delimiter)) != std::string::npos) {
			// If the token is found, push it
			if (pos != 0) result.push_back(line.substr(0, pos));

			// Erase the added element
			line.erase(0, pos + delimiter.length());
		}

		// Push the last token to the result
		if (pos != 0) result.push_back(line);

		return result;
	}

	// FIXME : remove unnecessary tabs, spaces and comments in the line before calling
	// FIXME : rewrite using data from Lexer
	void Parser::parse_line(std::string line) {
		std::vector<std::string> split =  split_line(line, " ");
		mmix::Instruction instruction;

		// Check if the size of the solit line is OK
		if ((split.size() > 3) or (split.size() < 2)) 
			throw WrongLineException(line);
			
		// If the size of the split line is 3, the first element is a label
		if (split.size() == 3) instruction.label = split[0];

		// Define the following statement
		if (std::find(directives.begin(), directives.end(), split[split.size() - 2]) != directives.end())
			instruction.directive = split[split.size() - 2];
		else if (mnemonics.find(split[split.size() - 2]) != mnemonics.end())
			instruction.mnemonic = split[split.size() - 2];
		else if (sizes.find(split[split.size() - 2]) != sizes.end())
			instruction.size = split[split.size() - 2];
		else throw UnexpectedTokenException(split[split.size() - 2]);

		// Parse the parameters
		instruction.parameters = split_line(split[split.size() - 1], ",");


/*		// FIXME : wtf???
		if (split[split.size() - 1].find("%") != std::string::npos) {
			instruction.parameters.resize(instruction.parameters.size() - 1);
		}*/

		// FIXME : is it necessary?
/*		while (instruction.parameters.size() != 3) {
			instruction.parameters.push_back("0");
		}*/

		// Push the instruction to the vector
		parsed_->push_back(instruction);
	}

	void Parser::parse(void) {
		for (auto line : *raw_) {
			// FIXME : remove unnecessary tabs, spaces and comments in the line before calling

			// Parse the line if it's not empty
			if (not line.empty()) parse_line(line);
		}
	}

	std::shared_ptr<parser::ParsedProgram> Parser::get(void) {
		return parsed_;
	}
}