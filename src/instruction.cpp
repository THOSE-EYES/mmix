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

#include "instruction.h"

namespace mmix {
	std::shared_ptr<Macro> InstructionFactory::create_macro() {
		return std::make_shared<Macro>();
	}

	std::shared_ptr<Mnemonic> InstructionFactory::create_mnemonic() {
		return std::make_shared<Mnemonic>();
	}

	std::shared_ptr<Allocator> InstructionFactory::create_allocator() {
		return std::make_shared<Allocator>();
	}

	std::shared_ptr<Directive> InstructionFactory::create_directive() {
		return std::make_shared<Directive>();
	}

	void Mnemonic::write(std::ostream& stream) const noexcept {
		stream << (label.empty() ? "" : (label + " "));
		stream << mnemonic << " ";

		// Pass every parameter to the stream
		for (auto it = parameters.begin(); it != (parameters.end() - 1); ++it)
			stream << *it << ",";
		stream << parameters.back();

		// Get to the next line
		stream << std::endl;
	}

	void Macro::write(std::ostream& stream) const noexcept {
		stream << (label.empty() ? "" : (label + " "));
		stream << '\b' << " ";

		// Pass every parameter to the stream
		for (auto it = parameters.begin(); it != (parameters.end() - 1); ++it)
			stream << *it << ",";
		stream << parameters.back();

		// If there is an instruction print it
		if (expression) expression->write(stream);

		// Get to the next line
		stream << std::endl;
	}

	void Allocator::write(std::ostream& stream) const noexcept {
		stream << (label.empty() ? "" : (label + " "));
		stream << size << " ";

		// Pass every parameter to the stream
		for (auto it = parameters.begin(); it != (parameters.end() - 1); ++it)
			stream << *it << ",";
		stream << parameters.back();

		// Get to the next line
		stream << std::endl;
	}

	void Directive::write(std::ostream& stream) const noexcept {
		stream << (label.empty() ? "" : (label + " "));
		stream << directive << " ";

		// Pass every parameter to the stream
		for (auto it = parameters.begin(); it != (parameters.end() - 1); ++it)
			stream << *it << ",";
		stream << parameters.back();

		// Get to the next line
		stream << std::endl;
	}
} // namespace mmix