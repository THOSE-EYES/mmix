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
		replace_macros();
		include_files(iterator->first.first);

		// Return only the main file
		program_ = iterator->second;
	}

	void Macroprocessor::fill_tables(void) {
		for (auto [file, content] : *sources_) {
			const auto filename		= file.first;
			auto table 				= MacroEntries();

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
				auto macro = process_macro(instruction, offset);
				auto [it, is_inserted] = table.insert(macro);
				if (not is_inserted) throw MacroExistsException("");

				// Remove known macros
				content->erase(iterator);
			}

			// Push the file's tables into the global tables
			macro_table_->insert(std::make_pair(filename, table));
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
			auto iterator = std::find_if(sources_->begin(), sources_->end(), 
				[=](const auto& pair) { return pair.first.first == filename; });
			if (iterator == sources_->end()) return;
			auto& content = iterator->second;

			for (auto entry : table) {
				auto macro = std::dynamic_pointer_cast<UseMacro>(entry);
				if (not macro) continue;

				// Expanding the macro 
				auto instruction = expand_macro(macro, filename);

				// Insert the new instruction
				auto insert_iterator = content->begin() + macro->offset;
				content->insert(insert_iterator, instruction);
			} 
		}
	}

	std::shared_ptr<Macroprocessor::MacroEntry> 
	Macroprocessor::process_macro(const std::shared_ptr<Macro>& value, size_t offset) {
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
		else return std::make_shared<MacroEntry>();
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

	bool operator<(const Macroprocessor::IncludeMacro& rhs, const Macroprocessor::IncludeMacro& lhs) {
		return lhs.filename < rhs.filename;
	}

	bool operator<(const Macroprocessor::MacroExpression& lhs, const Macroprocessor::MacroExpression& rhs) {
		return lhs.label < rhs.label;
	}

	bool operator<(const Macroprocessor::UseMacro& lhs, const Macroprocessor::UseMacro& rhs) {
		return lhs.parameters < rhs.parameters;
	}
} // namespace mmix