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

#include "application.h"

using mmix::parser::RawFile;
using mmix::parser::RawProgram;
using mmix::parser::ParsedFile;
using mmix::parser::ParsedProgram;
using mmix::macroprocessor::MacroprocessedProgram;
using mmix::compiler::CompiledProgram;
using mmix::preprocessor::PreprocessedProgram;

Application::Application(std::vector<std::string> input_files, std::string output_file) :
	input_files_{input_files},
	output_file_{output_file} {}

void Application::start(void) {
	mmix::Parser parser(read());
	mmix::Macroprocessor macroprocessor(parser.get());
	mmix::Preprocessor preprocessor(macroprocessor.get());

	// Process the program according to the mode
	switch (mode_) {
		// Write the result of preprocessing
		case PREPROCESSING:
			write(preprocessor.get());
			break;

		// Compile the program and write it to the file
		case FULL:
		case COMPILATION:
			compiler_ = std::make_shared<mmix::Compiler>(preprocessor.get());
			write(compiler_->get());
			break;
	}
}

std::shared_ptr<RawProgram> Application::read(void) {
	std::string line;
	std::ifstream input_stream;
	auto program = std::make_shared<RawProgram>();

	for (auto file : input_files_) {
		input_stream.open(file);
		auto source = std::make_shared<RawFile>();

		// Check if the file was opened
		if (!input_stream.is_open())
			throw std::ifstream::failure("File was not opened!");

		// Store every line of the program
		while (!input_stream.eof()) {
			std::getline(input_stream, line);
			if (not line.empty()) source->push_back(line);
		}

		// Close the stream
		input_stream.close();

		// Push the file to the map
		program->insert(std::make_pair(file, source));
	}

	return program;
}

void Application::write(std::shared_ptr<CompiledProgram> program) {
	std::ofstream output_stream(output_file_);
	std::stringstream hex_stream;

	// Check if the file was opened
    if (!output_stream.is_open())
        throw std::invalid_argument("The output file is not correct!");

	// Store every value into the file
	for (auto code : *program) {
		std::stringstream hex_stream;
		hex_stream << std::setfill('0') << std::setw(16) << std::left << std::hex << code; 
		output_stream << hex_stream.str() << std::endl;
	}

	// Close the stream
    output_stream.close();
}

void Application::write(std::shared_ptr<mmix::preprocessor::PreprocessedProgram> program) {
	std::ofstream output_stream(output_file_);

	// Check if the file was opened
    if (!output_stream.is_open())
        throw std::invalid_argument("The output file is not correct!");

	// Store every value into the file
	for (auto instruction : *program) { 
		instruction->write(output_stream);
		output_stream << std::endl;
	}

	// Close the stream
    output_stream.close();
}

void Application::set_mode(const Application::CompilationMode& value) {
	mode_ = value;
}