#include "macroprocessor.h"

using mmix::parser::ParsedFile;
using mmix::parser::ParsedProgram;
using mmix::macroprocessor::MacroprocessedProgram;
using mmix::exceptions::macroprocessor::MacroExistsException;
using mmix::exceptions::macroprocessor::UnknownMacroException;
using mmix::exceptions::macroprocessor::NoMainFileException;
using mmix::exceptions::macroprocessor::MacroNotFoundException;
using mmix::exceptions::macroprocessor::FileNotFoundException;

namespace mmix {
	Macroprocessor::Macroprocessor(std::shared_ptr<ParsedProgram> sources) :
		sources_{std::make_shared<ParsedProgram>(*sources)},
		program_{std::make_shared<MacroprocessedProgram>()},
		macro_table_{std::make_shared<MacroTable>()} {
		// Find the main file
		auto iterator = std::find_if(sources_->begin(), sources_->end(), 
			[](const auto& pair) { return pair.first.second; });
		if (iterator == sources_->end()) throw NoMainFileException();

		// Process the program
		fill_tables();
		process_branching();
		replace_macros();
		include_files(iterator->first.first);

		// Return only the main file
		program_ = iterator->second;
	}

	void Macroprocessor::fill_tables(void) {
		for (auto [file, content] : *sources_) {
			const auto 		filename = file.first;

			// Push the file's tables into the global tables
			macro_table_->insert(std::make_pair(filename, MacroEntries()));

			// Iterate over addresses
			auto iterator = content->begin();
			while (iterator != content->end()) {
				auto instruction = std::dynamic_pointer_cast<Macro>(*iterator);

				// Skip if instruction is macro
				if (not instruction) {
					++iterator;
					continue;
				}

				// Insert a new macro
				auto offset = std::distance(content->begin(), iterator);
				auto macro = process_macro(instruction, offset, filename);
				macro_table_->at(filename).push_back(macro);

				// Remove known macros
				content->erase(iterator);
			}
		}
	}

	void Macroprocessor::include_files(const std::string& target_filename) {
		const auto& target_file = std::find_if(sources_->begin(), sources_->end(), 
			[=](const auto& pair) { return pair.first.first == target_filename; })->second;
		const auto& target_table = macro_table_->at(target_filename);

		// Iterate over the include table for the given file
		for (const auto& entry : target_table) {	
			// Sanity check
			auto macro = std::dynamic_pointer_cast<IncludeMacro>(entry);
			if (not macro) continue;

			// Get include file content
			const auto& source_iterator = std::find_if(sources_->begin(), sources_->end(), 
				[=](const auto& pair) { return pair.first.first == macro->filename; });
			if (source_iterator == sources_->end()) throw FileNotFoundException(macro->filename);

			// FIXME : don't include already included files

			// Recursion goes brrrrr...
			include_files(macro->filename);

			// Merge files
			target_file->insert(target_file->begin(), 
				source_iterator->second->begin(), 
				source_iterator->second->end());
		}
	}

	std::shared_ptr<MacroprocessedProgram> Macroprocessor::get() {
		return program_;
	}

	void Macroprocessor::replace_macros(void) {
		for (auto& [filename, table] : *macro_table_) {
			for (auto entry : table) {
				auto macro = std::dynamic_pointer_cast<UseMacro>(entry);
				if (not macro) continue;

				// Expanding the macro 
				auto instruction = expand_macro(macro, filename);

				// Insert the new instruction
				auto content = get_content(filename);
				auto insert_iterator = content->begin() + macro->offset;
				content->insert(insert_iterator, instruction);
			} 
		}
	}

	void Macroprocessor::process_branching(void) {
		for (auto& [filename, table] : *macro_table_) {
			for (auto entry : table) {
				auto base_macro = 
					std::dynamic_pointer_cast<IBranchingMacro>(entry);
				if (not base_macro) continue;

				// If the macro depends on the definition, check if the macro was defined
				if (auto macro = 
					std::dynamic_pointer_cast<DefineBranchingMacro>(base_macro)) {
					// Check the type of the macro and find the expression
					if (not macro->type xor exists(filename, base_macro->expression)) {
						// Erases unneeded instructions
						clear(filename, macro->start_offset, macro->end_offset);
					}
				}

				// If the macro depends on the macro content, check it
				if (auto macro = 
					std::dynamic_pointer_cast<ExprBranchingMacro>(base_macro)) {
					if (check(filename, base_macro->expression)) {
						if (macro->else_block.end != 0) 
							clear(filename, macro->else_block.start, macro->else_block.end);	
					}
					else {
						clear(filename, macro->if_block.start, macro->if_block.end);	
					}
				}
			}
		}
	}

	bool Macroprocessor::exists(const std::string& filename, 
		const std::string& expr) {
		// Find the expression
		auto& table = macro_table_->at(filename);
		auto expr_it = std::find_if(table.begin(), 
			table.end(), [=](const auto& macro) { 
				return macro->label == expr; 
		});
		return expr_it != table.end();
	}

	bool Macroprocessor::check(const std::string& filename, 
		const std::string& expr) {

		// FIXME : create functions to determine ">", "<" and so on
		// FIXME : throw an exception
		auto split = Parser::split_line(expr, "==");
		auto& table = macro_table_->at(filename);
		auto iterator = std::find_if(table.begin(), 
			table.end(), [=](const auto& macro) { 
				return macro->label == split[0]; 
		});
		auto constant = std::dynamic_pointer_cast<ConstantMacro>(*iterator);

		return constant->value == split[1];
	}
	
	void Macroprocessor::clear(const std::string& filename, 
		size_t start, 
		size_t end) {
		auto content = get_content(filename);

		// Erase the instructions
		for (auto it = content->begin() + start;
			it != content->begin() + end; ++it)
				it->reset();
	}

	std::shared_ptr<mmix::parser::ParsedFile> 
	Macroprocessor::get_content(const std::string& filename) {
		// Get the content of the file
		auto iterator = std::find_if(sources_->begin(), sources_->end(), 
			[=](const auto& pair) { return pair.first.first == filename; });
		if (iterator == sources_->end()) throw FileNotFoundException(filename);
		return iterator->second;
	}

	std::shared_ptr<Macroprocessor::MacroEntry> 
	Macroprocessor::process_macro(const std::shared_ptr<Macro>& value, 
		size_t offset, 
		const std::string& filename) {
		const auto type 		= value->type;
		const auto label 		= value->label;
		const auto parameters	= value->parameters;

		// Process macros and push it into the corresponding tables
		if (type == "MACRO") {
			return std::make_shared<MacroExpression>(label, value->expression, parameters);
		}
		else if (type == "INCLUDE") {
			// FIXME : throw an exception when there are more parameters
			return std::make_shared<IncludeMacro>(parameters.at(0));
		}
		else if (type == "USEMACRO") {
			return std::make_shared<UseMacro>(parameters, offset);
		}
		else if (type == "DEFINE") {
			// FIXME : throw an exception when there are more parameters
			return std::make_shared<ConstantMacro>(label, parameters.at(0));
		}
		else if (type == "IFDEF" or type == "IFNDEF") {
			// FIXME : throw an exception when there are more parameters
			return std::make_shared<DefineBranchingMacro>(parameters.at(0), offset, type);
		}
		// FIXME : should also process "ELSE" and "ELSEIF"
		else if (type == "IF") {
			// FIXME : throw an exception when there are more parameters
			return std::make_shared<ExprBranchingMacro>(parameters.at(0), offset);
		}
		else if (type == "ENDIF") {
			// Store the end address
			auto& macro = (macro_table_->at(filename)).back();
			auto br_makro = std::dynamic_pointer_cast<IBranchingMacro>(macro);
			br_makro->end(offset);
		}
		
		return std::make_shared<MacroEntry>();
	}

	std::shared_ptr<Instruction> 
	Macroprocessor::expand_macro(std::shared_ptr<UseMacro>& value, const std::string& filename) {
		auto& 		parameters 	= value->parameters;
		auto 		label 		= parameters.front();

		// Remove the label from the parmaameters 
		parameters.erase(parameters.begin());

		// Get the corresponding macro table entry
		auto macro = std::dynamic_pointer_cast<MacroExpression>(find_label(label, filename));
		if (not macro) throw UnknownMacroException(label);

		// Get necessary data out of the entry
		auto& expression 			= macro->expression;
		auto& param_list 			= macro->parameters;
		auto& expression_parameters	= expression->parameters;

		// Replace occurences in the instruction
		for (auto& expression_param : expression_parameters)
			for (uint64_t index = 0; index < parameters.size(); ++index) 
				expression_param = Parser::replace_substr(expression_param, 
					("&" + param_list.at(0)), 
					parameters.at(index));

		return expression;
	}

	std::shared_ptr<Macroprocessor::MacroEntry> 
	Macroprocessor::find_label(const std::string& label, const std::string& filename) {
		const auto& table = macro_table_->at(filename);

		// Look for the entry
		for (auto entry : table) 
			if (entry->label == label) 
				return entry;

		// Throw an exception if the entry was not found
		throw MacroNotFoundException(label);
	}
} // namespace mmix