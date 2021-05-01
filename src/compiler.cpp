#include "compiler.h"

using mmix::compiler::sizes;
using mmix::compiler::mnemonics;

namespace mmix {
	Compiler::Compiler(std::shared_ptr<preprocessor::PreprocessedProgram> program) :
	program_{std::make_shared<preprocessor::PreprocessedProgram>(*program)},
	compiled_{std::make_shared<compiler::CompiledProgram>()},
	data_table_{std::make_shared<DataTable>()} {
		// Compile the program
		fill_table();
		compile();
	}

	void Compiler::convert(Instruction& instruction) {
		uint64_t code = 0;

		// Convert parameters into digital representation
		for (auto parameter : instruction.parameters) {
			if (parameter.front() == '$') code |= stoi(parameter.substr(1));
			else code |= stoi(parameter);

			// Shift the code to store the next parameter
			code <<= 8;
		} 

		// Add the code of the mnemonic into the code
		code |= static_cast<uint64_t>(mnemonics.find(instruction.mnemonic)->second) << (7 * 8);

		// Push a new value into the vector
		compiled_->push_back(code);
	}

	void Compiler::allocate(std::string size, std::string value) {
		// Store each symbol of the string separately
		for (auto character : value) {
			if (character != '\"');
				compiled_->push_back(static_cast<uint8_t>(character));
		}
	}

	void Compiler::allocate(std::string size, uint32_t value) {
		// Store each 8 bits of the value separately
		for (int8_t iteration = sizes.find(size)->second - 1; iteration >= 0; --iteration)
			compiled_->push_back((value >> (iteration * 8)) & 0xFF);
	}

	std::shared_ptr<compiler::CompiledProgram> Compiler::get(void) {
		return compiled_;
	}

	void Compiler::fill_table(void) {
		uint32_t offset = 0;

		// Iterate over addresses
		for (uint64_t address = 0; address != program_->size(); ++address) {
			auto instruction	= program_->at(address);
			auto size 			= instruction.size;
			auto label			= instruction.label;

			// Skip if the directive variable is empty
			if (size.empty()) continue;

			// If there is a label on the data, push it to the table
			if (not label.empty()) data_table_->insert(AllocatedData(label, address + offset));

			// FIXME : use lexer to determine the type of the data
			// Calculate an offset to an actual address after allocation
			offset += sizes.find(size)->second;
		}
	}

	void Compiler::replace_labels(Instruction& instruction) {
		// Iterate over parameters
		for (std::string& parameter : instruction.parameters) {
			auto iterator = data_table_->find(parameter);

			// If there is a label, change it to the address associated with it
			if (iterator != data_table_->end())
				parameter = std::to_string(iterator->second);
		}
	}

	void Compiler::compile(void) {
		for (Instruction& instruction : *program_) {
			auto mnemonic 		= instruction.mnemonic;
			auto size			= instruction.size;
			auto parameter		= instruction.parameters.at(0);
		
			replace_labels(instruction);

			if (!instruction.mnemonic.empty())
				convert(instruction);
			// FIXME : use lexer to determine the type of the data
			else if (!instruction.size.empty()) {
				if (parameter.front() == '$') 
					allocate(size, stoi(parameter.substr(1)));
				else if (parameter.front() == '\"' and parameter.back() == '\"')
					allocate(size, parameter.substr(1, parameter.size() - 2));
				else
					allocate(size, stoi(parameter));
			}
		}
	}
}