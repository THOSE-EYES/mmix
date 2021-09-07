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

	void Compiler::convert(std::shared_ptr<Mnemonic>& instruction) {
		uint64_t code = 0;

		// Convert parameters into digital representation
		for (auto parameter : instruction->parameters) {
			if (parameter.front() == '$') code |= stoi(parameter.substr(1));
			else code |= stoi(parameter);

			// Shift the code to store the next parameter
			code <<= 8;
		} 

		// Add the code of the mnemonic into the code
		code |= static_cast<uint64_t>(mnemonics.find(instruction->mnemonic)->second) << (7 * 8);

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
			auto instruction	= std::dynamic_pointer_cast<Allocator>(program_->at(address));
			if (not instruction) continue;

			// If there is a label on the data, push it to the table
			if (not instruction->label.empty()) data_table_->insert(AllocatedData(instruction->label, address + offset));

			// FIXME : use lexer to determine the type of the data
			// Calculate an offset to an actual address after allocation
			offset += sizes.find(instruction->size)->second;
		}
	}

	void Compiler::replace_labels(Instruction::Parameters& parameters) {
		// Iterate over parameters
		for (auto& parameter : parameters) {
			auto iterator = data_table_->find(parameter);

			// If there is a label, change it to the address associated with it
			if (iterator != data_table_->end())
				parameter = std::to_string(iterator->second);
		}
	}

	void Compiler::compile(void) {
		for (auto& base_instruction : *program_) {
			auto& parameters = base_instruction->parameters;
		
			// Replace labels with the address associated with it
			replace_labels(parameters);

			// If the instruction contains mnemonics, compile it
			if (auto instruction = std::dynamic_pointer_cast<Mnemonic>(base_instruction)) {
				convert(instruction);
			}
			// If the instruction means to allocate memory, allocate it and save the label of the data
			else if (auto instruction = std::dynamic_pointer_cast<Allocator>(base_instruction)){
				auto parameter 	= parameters.at(0);
				auto size		= instruction->size;

				if (parameter.front() == '$') 
					allocate(size, std::stoi(parameter.substr(1)));
				else if (parameter.front() == '\"' and parameter.back() == '\"')
					allocate(size, parameter.substr(1, parameter.size() - 2));
				else
					allocate(size, std::stoi(parameter));
			}
		}
	}
}