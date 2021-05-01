#include "preprocessor.h"

using mmix::exceptions::preprocessor::BlockExistsException;
using mmix::exceptions::preprocessor::BlockNotFoundException;
using mmix::exceptions::preprocessor::LabelNotFoundException;

namespace mmix {
	Preprocessor::Preprocessor(std::shared_ptr<parser::ParsedProgram> program) :
	program_{std::make_shared<preprocessor::PreprocessedProgram>()},
	label_table_{std::make_shared<LabelTable>()},
	block_table_{std::make_shared<BlockTable>()} {
		// Copy the elements from the  source
		for (auto element : *program) program_->push_back(element);

		// Preprocess the program
		relocate_instructions();
		fill_tables();
		preprocess();
	}

	void Preprocessor::create_block(std::string label, uint64_t address) {
		try {
			find_block(label);
			throw BlockExistsException(label);
		}
		catch (const BlockNotFoundException& e) {
			block_table_->push_back(Block{label, address});
		}
	}

	Block& Preprocessor::find_block(std::string& label) {
		//  Try to find a block and return it
		for (Block& block : *block_table_)
			if (block.label == label) return block;

		throw BlockNotFoundException(label);
	}

	void Preprocessor::update_block_addresses(Block& block) {
		for (uint32_t index = block.start; index <= block.end; ++index) {
			auto instruction = program_->at(index);

			// If there is no label just get to the next instruction
			if (instruction.label.empty()) continue;
			// Ignore data allocation labels
			if (not instruction.size.empty()) continue;
			
			auto entry = find_label(instruction.label);
			entry.second = std::to_string(block.origin + (index - block.start));
		}
	}

	void Preprocessor::emplace_block(Block& block) {
		auto block_origin 	= program_->begin() + block.origin;
		auto block_iterator = program_->begin() + block.start;
		auto block_end 		= program_->begin() + block.end;

		// Move instructions of the block
		while (block_iterator <= block_end) {
			// Insert an instruction into the right place
			program_->insert(block_origin, *block_iterator);

			// Move to the next position of the program
			++block_origin;
			++block_iterator;

			// Erase the instruction after insertion
			program_->erase(block_iterator);
		}
	}

	void Preprocessor::create_label(std::string& label, std::string& expression) {
		// Insert a new label
		try {
			find_label(label);
		}
		catch (const LabelNotFoundException& e) {
			label_table_->insert(Label(label, expression));
		}
	}

	Preprocessor::Label& Preprocessor::find_label(std::string& label) {
		auto iterator = label_table_->find(label);
		if (iterator != label_table_->end()) return *iterator;
			
		// Throw an exception if the block was not found
		throw LabelNotFoundException(label);
	}

	void Preprocessor::replace_labels(Instruction& instruction) {
		// Iterate over parameters
		for (auto& parameter : instruction.parameters) {
			auto iterator = label_table_->find(parameter);

			// If there is a label, change it to the expression associated with it
			if (iterator != label_table_->end())
				parameter = iterator->second;
		}
	}

	void Preprocessor::fill_tables(void) {
		// Iterate over addresses
		for (uint64_t address = 0; address != program_->size();) {
			auto instruction	= program_->at(address);
			auto directive 		= instruction.directive;
			auto label			= instruction.label;
			auto parameter		= instruction.parameters.at(0);

			// Skip if the directive variable is empty
			if (directive.empty()) {
				// Get to the next address
				++address;
				continue;
			}

			// FIXME : create new block on BLOCK and ENDBLOCK 
			// (when USE directive is used later)
			// FIXME : check block ranges
			// Process directives if it's found
			if (directive == "USE") {
				create_block(parameter, address);
			}
			else if (directive == "BLOCK") {
				auto& block = find_block(parameter);
				block.start = (address);
			}
			else if (directive == "ENDBLOCK") {
				auto& block = find_block(parameter);
				block.end = (address - 1);
			}
			else if (directive == "IS") {
				create_label(label, parameter);
			}
			else {
				// FIXME : throw an exception
			}

			// Remove the preprocessed data
			program_->erase(program_->begin() + address);
		}
	}

	void Preprocessor::preprocess(void) {
		// Iterate over blocks
		for (auto block : *block_table_) {
			emplace_block(block);
			update_block_addresses(block);
		}

		// Change labels to data
		for (Instruction& instruction : *program_) replace_labels(instruction);
	}

	void Preprocessor::relocate_instructions(void) {
		// Iterae over addresses
		for (uint64_t address = 0; address != program_->size(); ++address) {
			auto instruction	= program_->at(address);
			auto directive 		= instruction.directive;
			auto parameter		= instruction.parameters.at(0);

			// Skip if the directive variable is empty
			if (directive.empty()) continue;

			// Relocate a block of memory
			if (directive == "LOC") {
				auto offset = (parameter == "Data_Segment") ? constants::data_segment : stoi(parameter);

				// Fill space to create an offset
				for (auto iterator = program_->begin() + address; 
						iterator != program_->begin() + offset; 
						++iterator) 
					program_->insert(iterator, Instruction());
			}
		}
	}

	std::shared_ptr<preprocessor::PreprocessedProgram> Preprocessor::get(void) {
		return program_;
	}
} // mmix