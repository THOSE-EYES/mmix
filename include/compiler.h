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
#include <map>
#include <string>

// Include project headers
#include "instruction.h"
#include "mnemonics.h"
#include "sizes.h"
#include "preprocessor.h"

namespace mmix {
	namespace compiler {
		using CompiledProgram = std::vector<uint64_t>;
	}

	/**
	 * The compiler of MMIX instructions
	 */
	 // FIXME : addresses should be instruction_number * obj_entry_size
	class Compiler {
	protected :		
		using AllocatedData = std::pair<const std::string, uint64_t>;
		using DataTable 	= std::map<std::string, uint64_t>;

		// FIXME : store an instance of Lexer class

		std::shared_ptr<preprocessor::PreprocessedProgram> 	program_;		// The preprocessed program
		std::shared_ptr<compiler::CompiledProgram> 			compiled_;		// The compiled sources
		std::shared_ptr<DataTable>							data_table_;	// Table of addresses of the allocated data

	protected :
		/**
		 * Allocate data for the value
		 * @param size the size of the data
		 * @param value the value to store in the allocated space
		 */
		void allocate(std::string size, std::string value);

		/**
		 * Allocate data for the value
		 * @param size the size of the data
		 * @param value the value to store in the allocated space
		 */
		void allocate(std::string size, uint32_t value);

		/**
		 * Convert instruction into digital representation
		 * @param instruction the instruction to convert
		 */
		void convert(std::shared_ptr<Mnemonic>& instruction);

		/**
		 * Fill the table of addresses
		 */
		void fill_table(void);

		/**
		 * Replace labels with the addresses from the table
		 * @param instruction the instruction to process
		 */
		void replace_labels(Instruction::Parameters& parameters);

		/**
		 * Compile the program
		 */
		void compile(void);

	public :
		/**
		 * Constructor
		 * @param program the program to compile
		 */
		Compiler(std::shared_ptr<preprocessor::PreprocessedProgram> program);

		/**
		 * Get the compiled program
		 * @return a compiled version of the program
		 */
		std::shared_ptr<compiler::CompiledProgram> get(void);
	};
}