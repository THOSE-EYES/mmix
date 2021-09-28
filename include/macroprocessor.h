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
#include <memory>
#include <string>
#include <set>
#include <tuple>
#include <functional>

// Include project headers
#include "instruction.h"
#include "parser.h"

namespace mmix {
	namespace macroprocessor {
		using MacroprocessedProgram = std::vector<std::shared_ptr<Instruction>>;
	} // namespace macroprocessor
	
	/**
	 * Macroprocessor is used to process macros. It
	 * can expand macros an include files into a 
	 * target file. The class returns a single (main)
	 * file with a complete program.
	 */
	class Macroprocessor {
	private:
		/**
		 * This is a superclass for all macro structures.
		 * It holds common fields and is used as a element
		 * type for containers.
		 */
		struct MacroEntry {
		public:
			using Parameters = std::vector<std::string>;

			Parameters	parameters;
			std::string label;

		public:
			/**
			 * Constructor
			 */
			MacroEntry() = default;

			/**
			 * Constructor
			 * @param parameters_ 	macro parameters
			 * @param label_ 		the label on the macro 
			 */
			explicit MacroEntry(std::string label_) : 
				label{label_} {}

			/**
			 * Constructor
			 * @param parameters_ 	macro parameters
			 * @param label_ 		the label on the macro 
			 */
			MacroEntry(Parameters parameters_, std::string label_ = "") : 
				parameters{parameters_}, label{label_} {}

			/**
			 * Destructor
			 */
			virtual ~MacroEntry() = default;
		};

		/**
		 * This structure is used with MacroExpression
		 * structure in order to expand a macro into an
		 * actual instruction which the compiler 
		 * understands.
		 */
		struct UseMacro : MacroEntry {
		public:
			size_t offset;

		public:
			/**
			 * Constructor
			 * @param parameters_	macro parameters
			 * @param offset_		the address of the macro to be expanded
			 */
			UseMacro(Parameters parameters_, size_t offset_ = 0, const std::string& label_ = "") : 
				MacroEntry{parameters_, label_}, offset{offset_} {}

		};

		/**
		 * The class is used to store data about 
		 * macros with "MACRO" in it. It's expanded
		 * by the data in UseMacro object.
		 */
		struct MacroExpression : MacroEntry {
		public:
			using Expression = Instruction;

			std::shared_ptr<Expression>	expression;

		public:
			/**
			 * Constructor
			 * @param label_		the label on the macro 
			 * @param expression_	the expression which is to be changed
			 * @param parameters_	macro parameters
			 */
			MacroExpression(const std::string& label_, std::shared_ptr<Expression> expression_, 
							Parameters parameters_) :
							MacroEntry{parameters_,label_}, expression{expression_}
							{}
		};

		/**
		 * The macro is used when there is a
		 * file to include into a given target
		 * file.
		 */
		struct ConstantMacro : MacroEntry {
		public:
			std::string value;

		public:
			/**
			 * Constructor
			 * @param l the label
			 * @param v the constant expression
			 */
			ConstantMacro(const std::string& label_,
				const std::string& value_) : 
				MacroEntry{label_}, value{value_} {}

		private:
			using MacroEntry::Parameters;
			using MacroEntry::parameters;
		};


		/**
		 * The macro is used when there is a
		 * file to include into a given target
		 * file.
		 */
		struct IncludeMacro : MacroEntry {
		public:
			std::string filename;

		public:
			/**
			 * Constructor
			 * @param filename_ the name of the file to include
			 */
			IncludeMacro(const std::string& filename_) : filename{filename_} {}

		private:
			using MacroEntry::Parameters;
			using MacroEntry::parameters;
			using MacroEntry::label;
		};

		/**
		 * Interface of the branching macros
		 */
		struct IBranchingMacro : MacroEntry {
		public:
			std::string expression;		// The expression to check
		
		public:
			/**
			 * Constructor
			 * @param expr the expression to analyze
			 */
			IBranchingMacro(const std::string& expr) : 
				expression{expr} {}

			/**
			 * Destructor
			 */
			virtual ~IBranchingMacro() {}

		public:
			/**
			 * Start the block
			 * @param t the type of the block
			 * @param addr the address
			 */
			virtual void start(const std::string& t, size_t addr) = 0;

			/**
			 * End a block
			 * @param addr the address
			 */
			virtual void end(size_t addr) = 0;

		private:
			using MacroEntry::Parameters;
			using MacroEntry::parameters;
			using MacroEntry::label;
		};

		/**
		 * Structure which holds data about IFDEF/IFNDEF blocks
		 */
		struct DefineBranchingMacro : IBranchingMacro {
		public:
			/**
			 * Type of the macro
			 */
			enum Type {
				DEF = 0,
				NDEF
			};

		public:
			Type 	type;
			size_t 	start_offset;
			size_t 	end_offset;

		public:
			/**
			 * Constructor
			 * @param expr the expression to analyze
			 * @param addr the address of the start
			 * @param t the type of the macro
			 */
			DefineBranchingMacro(const std::string& expr, 
				size_t addr, 
				const std::string& t) : 
				IBranchingMacro{expr} {
					start(t, addr);
				}

		public:
			/**
			 * Start the block
			 * @param t the type of the block
			 * @param addr the address
			 */
			void start(const std::string& t, size_t addr) override {
				// Set the type
				if (t == "IFDEF") type = Type::DEF;
				else if (t == "IFNDEF") type = Type::NDEF;

				// Set the address
				start_offset = addr;
			}

			/**
			 * End a block
			 * @param addr the address
			 */
			void end(size_t addr) override {
				end_offset = addr;
			}			
		};

		/**
		 * A structure that represents "IF", "ELIF" and
		 * "ELSE" branching macros
		 */
		struct ExprBranchingMacro : IBranchingMacro {
		public:
			/**
			 * A block of data to paste/remove
			 */
			struct Block {
				size_t 	start;	// Start of the block
				size_t 	end;	// End of the block
			};	

		public:
			Block 				if_block;		// The only "if" block
			Block 				else_block;		// The only "else" block
			std::vector<Block> 	elif_blocks;	// Vector of "elif"'s

		public:
			/**
			 * Constructor
			 * @param expr the expression to analyze
			 */
			// FIXME : hardcodded value
			ExprBranchingMacro(const std::string& expr, size_t addr) :
				IBranchingMacro{expr} { start("IF", addr); }

		public:
			/**
			 * Start the block
			 * @param t the type of the block
			 * @param addr the address
			 */
			void start(const std::string& t, size_t addr) override {
				if (t == "IF") if_block.start = addr;
				else if (t == "ELSE") {
					if_block.end = addr - 1;
					else_block.start = addr;
				}
			}

			/**
			 * End a block
			 * @param addr the address
			 */
			void end(size_t addr) override {
				if (if_block.end == 0) if_block.end = addr;
				else else_block.end = addr;
			}
		};

		using MacroEntries	= std::vector<std::shared_ptr<MacroEntry>>;
		using MacroTable 	= std::map<std::string, MacroEntries>;

	protected:
		std::shared_ptr<parser::ParsedProgram> 					sources_;		// The sources of the program
		std::shared_ptr<macroprocessor::MacroprocessedProgram>	program_;		// The result of processing macros
		std::shared_ptr<MacroTable> 							macro_table_;	// The table of macro's to process

	protected:
		/**
		 * Fill the macro table with data
		 */
		virtual void fill_tables(void);

		/**
		 * Include files to the given file
		 * @param target_filename the file to which include other files
		 */
		void include_files(const std::string& target_filename);

		/**
		 * Replace all the macros with actual data
		 */
		void replace_macros(void);

		/**
		 * Find branching macros and process the code
		 */
		void process_branching(void);

		/**
		 * Clear positions of the file
		 * @param filename the file to lookup instructions in
		 * @param start the starting position
		 * @param end the ending position (excluded)
		 */
		void clear(const std::string& filename, size_t start, size_t end);

		/**
		 * Check if the expression existst
		 * @param filename the file to lookup expression in
		 * @param expr the expression to check
		 * @return true if there is such an expression
		 */
		bool exists(const std::string& filename, const std::string& expr);

		/**
		 * Check if the expression is logically correct
		 * @param filename the file to lookup expression in
		 * @param expr the expression to check
		 * @return true if the expression is correct (e.g. VALUE == true)
		 */
		bool check(const std::string& filename, const std::string& expr);

		/**
		 * 
		 * @param filename the file to check
		 * @return the content
		 */
		std::shared_ptr<mmix::parser::ParsedFile> get_content(const std::string& filename);

		/**
		 * Expand a macro into an actual instruction
		 * @param value 	data for macro expanding
		 * @param filename 	the file where the instruction residents
		 * @return 			the instruction from the expanded macro
		 */
		std::shared_ptr<Instruction> expand_macro(std::shared_ptr<UseMacro>& value, const std::string& filename);

		/**
		 * Extract data from the macro and differentiate macro types
		 * @param value 	the macro to process
		 * @param offset 	macro's offset in the memory
		 * @return 			a macro with a specific type
		 */
		std::shared_ptr<MacroEntry> 
		process_macro(const std::shared_ptr<Macro>& value, 
			size_t offset, 
			const std::string& filename);

		/**
		 * Find a macro with a given label
		 * @param label 	macro's label
		 * @param filename 	the name of the file where the macro is stored
		 * @return 			the macro (if it exists)
		 */
		std::shared_ptr<MacroEntry> find_label(const std::string& label, const std::string& filename);

	public:
		/**
		 * Constructor
		 * @param sources the source files of the 
		 */
		Macroprocessor(std::shared_ptr<parser::ParsedProgram> program);

	public:
		/**
		 * Get the result of the processing operation
		 * @return the processed program
		 */
		std::shared_ptr<macroprocessor::MacroprocessedProgram> get();

	};
} // namespace mmix 