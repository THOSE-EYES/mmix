#include "preprocessor.h"

using mmix::exceptions::preprocessor::BlockExistsException;
using mmix::exceptions::preprocessor::BadBlockException;
using mmix::exceptions::preprocessor::UnknownDirectiveException;
using mmix::exceptions::preprocessor::BlockNotFoundException;
using mmix::exceptions::preprocessor::LabelNotFoundException;
using mmix::macroprocessor::MacroprocessedProgram;

namespace mmix {
	Preprocessor::Preprocessor(std::shared_ptr<MacroprocessedProgram> program) :
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

	Preprocessor::Block& Preprocessor::find_block(std::string& label) {
		//  Try to find a block and return it
		for (Block& block : *block_table_)
			if (block.label == label) return block;

		throw BlockNotFoundException(label);
	}

	void Preprocessor::update_block_addresses(Block& block) {
		for (uint32_t index = block.start; index <= block.end; ++index) {
			auto instruction = std::dynamic_pointer_cast<Allocator>(program_->at(index));

			// If there is no label just get to the next instruction
			if (instruction->label.empty()) continue;
			// Ignore data allocation labels
			if (not instruction->size.empty()) continue;
			
			auto entry = find_label(instruction->label);
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

	void Preprocessor::replace_labels(std::shared_ptr<Instruction>& instruction) {
		// Iterate over parameters
		for (auto& parameter : instruction->parameters) {
			auto iterator = label_table_->find(parameter);

			// If there is a label, change it to the expression associated with it
			if (iterator != label_table_->end())
				parameter = iterator->second;
		}
	}

	void Preprocessor::fill_tables(void) {
		// Iterate over addresses
		for (uint64_t address = 0; address != program_->size();) {
			auto instruction = std::dynamic_pointer_cast<Directive>(program_->at(address));

			// Skip if the directive variable is empty
			if (not instruction) {
				++address;
				continue;
			}

			auto directive 		= instruction->directive;
			auto label			= instruction->label;
			auto parameter		= instruction->parameters.at(0);

			// Process directives if it's found
			if (directive == "USE") {
				try {
					auto& block = find_block(parameter);
					block.label = label;
					block.origin = address;
				}
				catch (const BlockNotFoundException& e){
					create_block(parameter, address);
				}
			}
			else if (directive == "BLOCK") {
				try {
					find_block(parameter);
				}
				catch (const BlockNotFoundException& e){
					create_block(parameter, address);
				}

				auto& block = find_block(parameter);
				block.start = (address);
			}
			else if (directive == "ENDBLOCK") {
				auto& block = find_block(parameter);
				block.end = (address - 1);

				if (block.start > block.end) 
					throw BadBlockException(block.label);
			}
			else if (directive == "IS")
				create_label(label, parameter);
			else 
				throw UnknownDirectiveException(directive);

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
		for (auto& instruction : *program_) replace_labels(instruction);
	}

	void Preprocessor::relocate_instructions(void) {
		// Iterate over addresses
		for (uint64_t address = 0; address < program_->size(); ++address) {
			auto instruction = std::dynamic_pointer_cast<Directive>(program_->at(address));

			// Skip if the directive variable is empty
			if (not instruction) {
				++address;
				continue;
			}

			// Relocate a block of memory
			if (instruction->directive == "LOC") {
				// FIXME : throw an exception when a size of a parameter vector is != 1
				auto parameter	= instruction->parameters.at(0);
				auto offset = (parameter == "Data_Segment") ? constants::data_segment : stoi(parameter);

				// Fill space to create an offset
				for (auto iterator = program_->begin() + address; 
						iterator != program_->begin() + offset; 
						++iterator) 
					program_->insert(iterator, std::make_shared<Mnemonic>());
			}
		}
	}

	std::shared_ptr<preprocessor::PreprocessedProgram> Preprocessor::get(void) {
		return program_;
	}
} // namespace mmix