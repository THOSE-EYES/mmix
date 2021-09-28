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
#include <string>
#include <vector>
#include <iostream>
#include <memory>

namespace mmix {
	/**
	 * The main structure in the program. It is used
	 * to store the complete information about the
	 * instruction. It is filled in Parser class
	 */
	struct Instruction {
		using Parameters = std::vector<std::string>;

		std::string label;
		Parameters	parameters;

		/**
		 * Destructor
		 */
		virtual ~Instruction() = default;

		/**
		 * Write the object into a stream
		 * @param stream the output stream to put the data into
		 */
		virtual void write(std::ostream& stream) const noexcept = 0;
	};

	/**
	 * Mnemonic instruction, which is compiled by the
	 * compiler is stored in the structure
	 */
	struct Mnemonic : Instruction {
		std::string mnemonic;

		/**
		 * Write the object into a stream
		 * @param stream the output stream to put the data into
		 */
		void write(std::ostream& stream) const noexcept;
	};

	/**
	 * Macro data is stored in the structure
	 */
	struct Macro : Instruction {
		std::string 					type;
		std::shared_ptr<Instruction> 	expression;

		/**
		 * Write the object into a stream
		 * @param stream the output stream to put the data into
		 */
		void write(std::ostream& stream) const noexcept;
	};

	/**
	 * Allocator is used when the instruction allocates
	 * memory for data
	 */
	struct Allocator : Instruction {
		std::string size;

		/**
		 * Write the object into a stream
		 * @param stream the output stream to put the data into
		 */
		void write(std::ostream& stream) const noexcept;
	};
	
	/**
	 * Directive is used when the instruction 
	 * contains a directive in it
	 */
	struct Directive : Instruction {
		std::string	directive;

		/**
		 * Write the object into a stream
		 * @param stream the output stream to put the data into
		 */
		void write(std::ostream& stream) const noexcept;
	};

	/**
	 * Factory class for instructions
	 */
	class InstructionFactory {
	public:
		std::shared_ptr<Macro> create_macro();
		std::shared_ptr<Mnemonic> create_mnemonic();
		std::shared_ptr<Allocator> create_allocator();
		std::shared_ptr<Directive> create_directive();
	};
} // mmix